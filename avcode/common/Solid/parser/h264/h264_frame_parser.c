
#include <string.h>
#include <stdlib.h>

#include "h264_bs_parser.h"

#include "h264_frame_parser.h"

#include "../../util/log_debug.h"

/** 视频帧最大长度 */
#define H264_VIDEO_FRAME_MAX_SIZE                  (512*1024)
/** 视频帧头最大长度 */
#define H264_VIDEO_HEADER_MAX_SIZE                 (1024)

/** 视频帧 */
typedef struct tag_h264_video
{
	/** 视频算法 */
	int32  algo;
	/** 是否已发现sps */
	int32  b_find_sps;
	/** 是否已发现idr */
	int32  b_find_idr;
	/** 是否VCL(h264) */
	int32  b_vcl;
	/** 是否帧已开始 */
	int32  b_frame_start;
	/** 帧类型 */
	int32  frame_type;
	/** 帧模式 */
	int32  frame_mode;
	/** 帧编号 */
	int32  frame_num;
	/** 时间戳 */
	int64  pts;

	/** H264 */
	H264_BS_PACK bs_pack;

} H264_VIDEO, *PH264_VIDEO;


/** H264帧解析器 */
typedef struct tag_h264_parser
{
	/** 初始化标志 */
	int32  b_init;

	/** 是否视频序列开始 */
	int32  b_video_start;
	/** 是否已获取前一帧视频数据 */
	int32  b_get_video;
	/** 当前视频帧 */
	H264_VIDEO video1;
	/** 下一视频帧 */
	H264_VIDEO video2;

	/** 视频数据缓冲区 */
	uint8 *p_video_buf;
	/** 视频缓冲区总长度 */
	int32 vbuf_total_size;
	/** 视频缓冲区已使用长度(vbuf_use_size = vbuf_frame_size) */
	int32 vbuf_use_size;
	/** 视频缓冲区剩余长度 */
	int32 vbuf_left_size;
	/** 视频缓冲区帧长度 */
	int32 vbuf_frame_size;
	/** 视频缓冲区已解析长度 */
	int32 vbuf_parse_size;
	/** 视频缓冲区未解析长度 */
	int32 vbuf_unparse_size;
	/** 视频缓冲区起始偏移 */
	int32 vbuf_offset;

	/** 视频帧头数据缓冲区 */
	uint8 *p_video_header_buf;
	/** 视频帧头长度 */
	int32 video_header_size;

} H264_PARSER, *PH264_PARSER;

static int32 h264_update_video(H264_PARSER *p_parser);
static int32 h264_submit_video(H264_PARSER *p_parser, MEDIA_FRAME *p_frame);

/**  
 * @brief 解析H264 BS流
 * @param [in] p_parser 解析器句柄
 * @param [out] p_frame 帧句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_len 输入数据长度
 * @param [out] p_parse_len 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入数据不足以解析
 */
static int32 h264_parse_video(
							  H264_PARSER *p_parser,
							  MEDIA_FRAME *p_frame,
							  uint8 *p_buf,
							  int32 buf_len,
							  int32 *p_parse_len)
{
	int32  parse_len;
	int32  total_parse_len = 0;
	uint8* p_parse_pos = NULL;
	uint8* p_head_pos;
	int32  head_len = 0;
	int32  b_find_next_frame;

	int32  ret;

	if( buf_len > (p_parser->vbuf_total_size - p_parser->vbuf_offset) )
	{
		*p_parse_len = 0;
		log_warn(_T("[h264_frame_parser::h264_parse_video] video input size = %d too large!\n"), buf_len);
		return -1;
	}

	if( buf_len > p_parser->vbuf_left_size )
	{
		/** 帧长度溢出 */
		p_parser->vbuf_use_size = p_parser->vbuf_offset;
		p_parser->vbuf_left_size = p_parser->vbuf_total_size - p_parser->vbuf_use_size;
		p_parser->vbuf_frame_size = 0;
		p_parser->vbuf_unparse_size = 0;
		log_warn(_T("[h264_frame_parser::h264_parse_video] video frame too large!\n"));
	}

	/** 保存视频数据 */
	memcpy(p_parser->p_video_buf + p_parser->vbuf_use_size, p_buf, buf_len);
	p_parser->vbuf_use_size += buf_len;
	p_parser->vbuf_left_size -= buf_len;
	p_parser->vbuf_unparse_size += buf_len;

	/** 解析视频帧 */
	b_find_next_frame = 0;
	p_parse_pos = p_parser->p_video_buf + p_parser->vbuf_offset + p_parser->vbuf_frame_size;
	while( 1 )
	{
		ret = h264_parse_bs(&p_parser->video2.bs_pack, p_parse_pos, p_parser->vbuf_unparse_size, &parse_len);
		p_parse_pos += parse_len;
		p_parser->vbuf_frame_size += parse_len;
		p_parser->vbuf_unparse_size -= parse_len;

		if( ret )
		{
			break;
		}

		/** 分析nalu类型 */
		switch( p_parser->video2.bs_pack.nalu_type )
		{
		case H264_NALU_SEI:
		case H264_NALU_SPS:
		case H264_NALU_PPS:
			{
				if( p_parser->video2.bs_pack.nalu_type == H264_NALU_SPS )
				{
					/** I帧 */
					if( !p_parser->video2.b_find_sps )
					{
						p_parser->video2.b_find_sps = 1;
					}

					p_parser->video2.b_vcl = 0;
					b_find_next_frame = 1;

				}else if( p_parser->video1.b_vcl )
				{
					/** VCL之后第一个非VCL NAL单元 */
					p_parser->video2.b_vcl = 0;
					b_find_next_frame = 1;
				}

				p_parser->video1.b_vcl = 0;
			}
			break;

		case H264_NALU_IDR_SLICE:
			{
				p_parser->video2.b_find_idr = 1;
				p_parser->video1.b_vcl = 1;
				p_parser->video1.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;

				p_parser->video1.frame_type = MP_VIDEO_I_FRAME;
			}
			break;

		case H264_NALU_NON_IDR_SLICE:
			{
				if( p_parser->video1.b_vcl )
				{
					if( p_parser->video1.frame_num != p_parser->video2.bs_pack.slice_header.frame_num )
					{
						b_find_next_frame = 1;

						p_parser->video2.b_vcl = 1;
						p_parser->video2.b_find_idr = 0;

						if( p_parser->video2.bs_pack.slice_header.type == H264_P_SLICE
							|| p_parser->video2.bs_pack.slice_header.type == H264_DUP_P_SLICE )
						{
							p_parser->video2.frame_type = MP_VIDEO_P_FRAME;

						}else
						{
							p_parser->video2.frame_type = MP_VIDEO_B_FRAME;
						}

						p_parser->video2.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
						break;
					}
				}

				p_parser->video1.b_vcl = 1;

				if( p_parser->video2.bs_pack.slice_header.type == H264_P_SLICE
					|| p_parser->video2.bs_pack.slice_header.type == H264_DUP_P_SLICE )
				{
					p_parser->video1.frame_type = MP_VIDEO_P_FRAME;

				}else
				{
					p_parser->video1.frame_type = MP_VIDEO_B_FRAME;
				}

				p_parser->video1.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
				p_parser->video2.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
				p_parser->video2.b_find_idr = 0;
			}
			break;

		default:
			{
			}
			break;
		}

		if( b_find_next_frame )
		{
			/** 计算下一帧的帧头长度 */
			p_head_pos = p_parser->video2.bs_pack.p_start;
			head_len = p_parse_pos - p_parser->video2.bs_pack.p_start;

			p_parser->video_header_size = head_len;

			/** 计算当前帧实际长度 */
			p_parser->vbuf_frame_size -= head_len;
			p_parser->vbuf_unparse_size += head_len;
			p_parser->vbuf_parse_size = head_len;

			break;
		}

	}//end while(1)

	*p_parse_len = buf_len;

	if( b_find_next_frame )
	{
		return 0;

	}else
	{
		return ret;
	}
}

/**  
 * @brief 解析H264 BS流
 * @param [in] p_parser 解析器句柄
 * @param [in] p_frame 帧句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_len 输入数据长度
 * @param [out] p_parse_len 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入数据不足以解析
 */
static int32 h264_parse_video2(
							   H264_PARSER *p_parser,
							   MEDIA_FRAME *p_frame,
							   uint8 *p_buf,
							   int32 buf_len,
							   int32 *p_parse_len)
{
	int32  parse_len;
	uint8* p_parse_pos = NULL;
	int32  head_len = 0;
	int32  b_find_next_frame;

	int32  ret;

	/** 解析视频帧 */
	ret = h264_parse_bs(&p_parser->video2.bs_pack, p_buf, buf_len, &parse_len);
	*p_parse_len = parse_len;
	if( ret )
	{
		return ret;
	}

	p_frame->p_frame = p_parser->video2.bs_pack.p_start;
	p_frame->frame_size = buf_len;
	p_frame->type = MP_DATA_TYPE_VIDEO;
	p_frame->frame_flag = MP_FRAME_VALID;
	p_frame->frame_flag |= MP_FRAME_DATA;
	p_frame->frame_ts = 0;
	p_frame->param.video.algo = MP_VIDEO_H264;

	b_find_next_frame = 0;
	/** 分析nalu类型 */
	switch( p_parser->video2.bs_pack.nalu_type )
	{
	case H264_NALU_SEI:
	case H264_NALU_SPS:
	case H264_NALU_PPS:
		{
			if( p_parser->video2.bs_pack.nalu_type == H264_NALU_SPS )
			{
				/** I帧 */
				if( !p_parser->video2.b_find_sps )
				{
					p_parser->video2.b_find_sps = 1;
				}

				p_parser->video2.b_vcl = 0;
				b_find_next_frame = 1;

				p_frame->frame_flag |= MP_FRAME_HEAD;
				p_frame->param.video.frame_type = MP_VIDEO_I_FRAME;

			}else if( p_parser->video1.b_vcl )
			{
				/** VCL之后第一个非VCL NAL单元 */
				p_parser->video2.b_vcl = 0;
				b_find_next_frame = 1;

				p_frame->frame_flag |= MP_FRAME_HEAD;
				p_frame->param.video.frame_type = MP_VIDEO_P_FRAME;
			}

			p_parser->video1.b_vcl = 0;
		}
		break;

	case H264_NALU_IDR_SLICE:
		{
			p_parser->video2.b_find_idr = 1;
			p_parser->video1.b_vcl = 1;
			p_parser->video1.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;

			p_parser->video1.frame_type = MP_VIDEO_I_FRAME;
		}
		break;

	case H264_NALU_NON_IDR_SLICE:
		{
			if( p_parser->video1.b_vcl )
			{
				if( p_parser->video1.frame_num != p_parser->video2.bs_pack.slice_header.frame_num )
				{
					b_find_next_frame = 1;
					p_frame->frame_flag |= MP_FRAME_HEAD;

					p_parser->video2.b_vcl = 1;
					p_parser->video2.b_find_idr = 0;

					if( p_parser->video2.bs_pack.slice_header.type == H264_P_SLICE
						|| p_parser->video2.bs_pack.slice_header.type == H264_DUP_P_SLICE )
					{
						p_parser->video2.frame_type = MP_VIDEO_P_FRAME;
						p_frame->param.video.frame_type = MP_VIDEO_P_FRAME;

					}else
					{
						p_parser->video2.frame_type = MP_VIDEO_B_FRAME;
						p_frame->param.video.frame_type = MP_VIDEO_B_FRAME;
					}

					p_parser->video2.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
					break;
				}
			}

			p_parser->video1.b_vcl = 1;

			if( p_parser->video2.bs_pack.slice_header.type == H264_P_SLICE
				|| p_parser->video2.bs_pack.slice_header.type == H264_DUP_P_SLICE )
			{
				p_parser->video1.frame_type = MP_VIDEO_P_FRAME;

			}else
			{
				p_parser->video1.frame_type = MP_VIDEO_B_FRAME;
			}

			p_parser->video1.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
			p_parser->video2.frame_num = p_parser->video2.bs_pack.slice_header.frame_num;
			p_parser->video2.b_find_idr = 0;
		}
		break;

	default:
		{
		}
		break;
	}

	if( b_find_next_frame )
	{
		return 0;

	}else
	{
		return -2;
	}
}

/**  
 * @brief 更新当前视频帧状态
 * @param [in] p_parser 解析器句柄
 * @return
 * 0：成功
 */
static int32 h264_update_video(H264_PARSER *p_parser)
{
	if( p_parser->vbuf_frame_size > 0 && p_parser->vbuf_unparse_size > 0 )
	{
		/** 保存下一帧数据 */
		memcpy(p_parser->p_video_buf + p_parser->vbuf_offset, p_parser->p_video_buf + p_parser->vbuf_offset + p_parser->vbuf_frame_size, p_parser->vbuf_unparse_size);
	}

	p_parser->vbuf_use_size = p_parser->vbuf_offset + p_parser->vbuf_unparse_size;
	p_parser->vbuf_left_size = p_parser->vbuf_total_size - p_parser->vbuf_use_size;
	p_parser->vbuf_frame_size = p_parser->vbuf_parse_size;
	p_parser->vbuf_unparse_size -= p_parser->vbuf_parse_size;
	p_parser->video_header_size = 0;

	memcpy(&p_parser->video1, &p_parser->video2, sizeof(H264_VIDEO));

	return 0;
}

/**  
 * @brief 提交视频帧
 *
 * @param [in] p_parser 解析器句柄
 * @param [in] p_frame TS帧句柄
 * @return
 * 0：成功
 */
static int32 h264_submit_video(H264_PARSER *p_parser, MEDIA_FRAME *p_frame)
{
	p_parser->b_get_video = 1;

	p_frame->type = MP_DATA_TYPE_VIDEO;
	p_frame->p_frame = p_parser->p_video_buf + p_parser->vbuf_offset;
	p_frame->frame_size = p_parser->vbuf_frame_size;

	p_frame->frame_flag = 0;
	p_frame->frame_flag |= MP_FRAME_VALID;
	p_frame->frame_ts = 0;

	p_frame->param.video.algo = MP_VIDEO_H264;
	p_frame->param.video.frame_type = p_parser->video1.frame_type;

	p_frame->param.video.field_flag = 0;
	p_frame->param.video.field_flag |= MP_VIDEO_FRAME;

	p_frame->param.video.width = p_parser->video1.bs_pack.sps.width;
	p_frame->param.video.height = p_parser->video1.bs_pack.sps.height;

	p_frame->p_user_data = NULL;
	p_frame->user_data_size = 0;

	return 0;
}

///////////////////////////////////外部接口////////////////////////////////////
H264_FRAME_PARSER h264_frame_parser_create()
{
	H264_PARSER *p_parser = NULL;

	p_parser = (H264_PARSER*)malloc( sizeof(H264_PARSER) );
	if( p_parser == NULL )
	{
		return NULL;
	}

	memset(p_parser, 0, sizeof(H264_PARSER));

	return p_parser;
}

int32 h264_frame_parser_destroy(H264_FRAME_PARSER frame_parser)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->b_init )
	{
		h264_frame_parser_deinit(p_parser);
	}

	free(p_parser);

	return 0;
}

int32 h264_frame_parser_init(H264_FRAME_PARSER frame_parser)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->b_init )
	{
		return 0;
	}

	memset(p_parser, 0, sizeof(H264_PARSER));

	h264_bs_init(&p_parser->video1.bs_pack);
	h264_bs_init(&p_parser->video2.bs_pack);

	p_parser->video2.algo = MP_VIDEO_H264;
	p_parser->video2.b_find_sps = 0;
	p_parser->video2.b_find_idr = 0;
	p_parser->video2.b_vcl = 0;
	p_parser->video2.frame_mode = H264_FRAME_MODE;
	p_parser->video2.frame_num = 0;

	p_parser->video1.b_vcl = 0;
	p_parser->video1.frame_mode = H264_FRAME_MODE;
	p_parser->video1.algo = p_parser->video2.algo;

	p_parser->b_video_start = 0;
	p_parser->b_get_video = 0;
	p_parser->p_video_buf = (uint8*)malloc(H264_VIDEO_FRAME_MAX_SIZE);
	if( p_parser->p_video_buf == NULL )
	{
		h264_frame_parser_deinit(p_parser);
		return -2;
	}
	p_parser->vbuf_total_size = H264_VIDEO_FRAME_MAX_SIZE;
	p_parser->vbuf_use_size = 0;
	p_parser->vbuf_left_size = H264_VIDEO_FRAME_MAX_SIZE;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vbuf_offset = 0;

	p_parser->p_video_header_buf = (uint8*)malloc(H264_VIDEO_HEADER_MAX_SIZE);
	if( p_parser->p_video_header_buf == NULL )
	{
		h264_frame_parser_deinit(p_parser);
		return -2;
	}
	p_parser->video_header_size = 0;

	p_parser->b_init = 1;

	return 0;
}

int32 h264_frame_parser_deinit(H264_FRAME_PARSER frame_parser)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->p_video_buf )
	{
		free(p_parser->p_video_buf);
		p_parser->p_video_buf = NULL;
	}

	if( p_parser->p_video_header_buf )
	{
		free(p_parser->p_video_header_buf);
		p_parser->p_video_header_buf = NULL;
	}

	p_parser->b_init = 0;

	return 0;
}

int32 h264_frame_parser_reset(H264_FRAME_PARSER frame_parser)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( !p_parser->b_init )
	{
		return -3;
	}

	p_parser->b_video_start = 0;
	p_parser->b_get_video = 0;
	memset(&p_parser->video1, 0, sizeof(H264_VIDEO));
	memset(&p_parser->video2, 0, sizeof(H264_VIDEO));

	h264_bs_init(&p_parser->video1.bs_pack);
	h264_bs_init(&p_parser->video2.bs_pack);

	p_parser->video2.algo = MP_VIDEO_H264;
	p_parser->video2.b_find_sps = 0;
	p_parser->video2.b_find_idr = 0;
	p_parser->video2.b_vcl = 0;
	p_parser->video2.frame_mode = H264_FRAME_MODE;
	p_parser->video2.frame_num = 0;

	p_parser->video1.b_vcl = 0;
	p_parser->video1.frame_mode = H264_FRAME_MODE;
	p_parser->video1.algo = p_parser->video2.algo;

	p_parser->vbuf_use_size = 0;
	p_parser->vbuf_left_size = p_parser->vbuf_total_size - p_parser->vbuf_use_size;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->video_header_size = 0;

	return 0;
}

int32 h264_frame_parse(
					   H264_FRAME_PARSER frame_parser,
					   MEDIA_FRAME *p_frame,
					   uint8 *p_buf,
					   int32 buf_len,
					   int32 *p_parse_len)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	int32  parse_len;
	int32  total_parse_len = 0;
	int32  unparse_len = buf_len;
	uint8* p_parse_buf = p_buf;
	int32  b_get_frame = 0;

	int32  ret;

	if( p_parser == NULL || p_frame == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	if( !p_parser->b_init )
	{
		return -3;
	}

	if( p_parser->b_get_video )
	{
		h264_update_video(p_parser);
		p_parser->b_get_video = 0;
	}

	while( !b_get_frame )
	{
		ret = h264_parse_video(p_parser, p_frame, p_parse_buf, unparse_len, &parse_len);
		p_parse_buf += parse_len;
		total_parse_len += parse_len;
		unparse_len -= parse_len;
		
		if( ret )
		{
			break;
		}

		if( p_parser->b_video_start )
		{
			/** 提交当前视频帧 */
			b_get_frame = 1;
			h264_submit_video(p_parser, p_frame);

		}else
		{
			h264_update_video(p_parser);
		}

		p_parser->b_video_start = 1;
	}

	*p_parse_len = total_parse_len;

	if( b_get_frame )
	{
		return 0;

	}else
	{
		return -2;
	}
}

int32 h264_frame_parse2(
						H264_FRAME_PARSER frame_parser,
						MEDIA_FRAME *p_frame,
						uint8 *p_buf,
						int32 buf_len,
						int32 *p_parse_len)
{
	H264_PARSER *p_parser = (H264_PARSER*)frame_parser;

	int32  ret;

	if( p_parser == NULL || p_frame == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	if( !p_parser->b_init )
	{
		return -3;
	}

	ret = h264_parse_video2(p_parser, p_frame, p_buf, buf_len, p_parse_len);
	if( ret == 0 )
	{
		p_parser->b_video_start = 1;
	}

	return ret;
}
