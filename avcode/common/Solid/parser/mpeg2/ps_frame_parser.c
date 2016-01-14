
#include <stdlib.h>
#include <string.h>

#include <public/gen_buf.h>
#include <public/gen_fourcc.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include <Solid/public/sp_video_format.h>
#include <Solid/public/sp_audio_format.h>
#include <Solid/public/sp_media_format.h>

#include "ps_stream_public.h"
#include "mpeg2_ps_parser.h"
#include "mpeg2_pes_parser.h"
#include "mpeg2_ves_parser.h"
#include "mpeg2_audio_demux.h"
#include "mpeg2_aac_parser.h"
#include "../mpeg4/mpeg4_ves_parser.h"
#include "../h264/h264_bs_parser.h"

#include "ps_frame_parser.h"


/** max video frame size */
#define PS_FRAME_PARSER_MAX_VIDEO_FRAME_SIZE               (512 * 1024)
/** max video frame header size */
#define PS_FRAME_PARSER_MAX_VIDEO_FRAME_HEADER_SIZE        (1024)
/** max audio frame size */
#define PS_FRAME_PARSER_MAX_AUDIO_FRAME_SIZE               (32 * 1024)
/** max audio frame header size */
#define PS_FRAME_PARSER_MAX_AUDIO_FRAME_HEADER_SIZE        (1024)
/** max private data size */
#define PS_FRAME_PARSER_MAX_PRIVATE_DATA_SIZE              (1024)



/** video stat */
typedef struct tag_ts_video_stat
{
	/** format */
	int32  format;
	/** find head */
	int32  b_find_head;
	/** frame begin */
	int32  b_frame_begin;

	/** frame type */
	int32  frame_type;
	/** has pts */
	int32  b_pts;
	/** pts */
	int64  pts;

	union
	{
		/** mpeg2 */
		struct
		{
			/** state */
			MPEG2_VES_STATE state;

		} mpeg2;

		/** mpeg4 */
		struct
		{
			/** state */
			MPEG4_VES_STATE state;

		} mpeg4;

		/** h264 */
		struct
		{
			/** find sps */
			int32  b_find_sps;
			/** find idr */
			int32  b_find_idr;
			/** vcl */
			int32  b_vcl;
			/** frame mode */
			int32  frame_mode;
			/** frame number */
			int32  frame_num;

			/** bs */
			H264_BS_PACK bs;

		} h264;

	} stat;

} PS_VIDEO_STAT, *PPS_VIDEO_STAT;

/** audio stat */
typedef struct tag_ts_audio_stat
{
	/** format */
	int32  format;
	/** has pts */
	int32  b_pts;
	/** pts */
	int64  pts;

	union
	{
		/** mpeg2 */
		struct
		{
			/** avg frame size */
			int32  avg_frame_size;

			/** state */
			MPEG2_AUDIO_HEADER state;

		} mpeg2;

		/** aac */
		struct
		{
			/** frame size */
			int32  frame_size;

			/** adts */
			ADTS_PACK adts;

		} aac;

	} stat;

} PS_AUDIO_STAT, *PPS_AUDIO_STAT;

/** parser */
typedef struct tag_ps_frame_parser
{
	/** init */
	int32  b_init;

	/** ps stream */
	PS_STREAM  stream;

	/** auto detect */
	int32  b_auto_detect;
	/** auto adapt video */
	int32  b_auto_adapt_video;
	/** auto adapt audio */
	int32  b_auto_adapt_audio;
	/** get video stream */
	int32  b_get_video_stream;
	/** video stream id */
	int32  video_stream_id;
	/** get audio stream */
	int32  b_get_audio_stream;
	/** audio stream id */
	int32  audio_stream_id;

	/** video begin */
	int32  b_video_begin;
	/** get video */
	int32  b_get_video;
	/** video error */
	int32  b_video_error;
	/** current video status */
	PS_VIDEO_STAT video1;
	/** next video status */
	PS_VIDEO_STAT video2;

	/** video buf */
	GEN_BUF  video_buf;
	/** video buf frame size */
	int32    vbuf_frame_size;
	/** video buf parse size */
	int32    vbuf_parse_size;
	/** video buf unparse size */
	int32    vbuf_unparse_size;
	/** video buf frame offset */
	int32    vbuf_frame_offset;

	/** video header buf */
	GEN_BUF  vh_buf;

	/** audio begin */
	int32  b_audio_begin;
	/** get audio */
	int32  b_get_audio;
	/** audio error */
	int32  b_audio_error;
	/** current audio status */
	PS_AUDIO_STAT audio1;
	/** next audio status */
	PS_AUDIO_STAT audio2;

	/** audio buf */
	GEN_BUF  audio_buf;
	/** audio buf frame size */
	int32    abuf_frame_size;
	/** audio buf parse size */
	int32    abuf_parse_size;
	/** audio buf unparse size */
	int32    abuf_unparse_size;
	/** audio buf frame offset */
	int32    abuf_frame_offset;

	/** audio header buf */
	GEN_BUF  ah_buf;

} PS_FRAME_PARSER, *PPS_FRAME_PARSER;



/////////////////////////////// Inner Interface  //////////////////////////////
/**  
 * @brief init stream
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_init_stream(PS_STREAM* p_stream)
{
	p_stream->b_get_ps_map = 0;
	p_stream->es_stream_count = 0;

	return GEN_S_OK;
}

/**  
 * @brief init av meta
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_init_av_meta(PS_FRAME_PARSER *p_parser)
{
	p_parser->b_auto_detect = 1;
	p_parser->b_auto_adapt_video = 0;
	p_parser->b_auto_adapt_audio = 0;
	p_parser->b_get_video_stream = 0;
	p_parser->b_get_audio_stream = 0;

	return GEN_S_OK;
}

/**  
 * @brief init video
 * @param [in] p_parser, parser
 * @param [in] video_format, video format
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_init_video(PS_FRAME_PARSER *p_parser, int32 video_format)
{

	switch( video_format )
	{
	case SP_VIDEO_FORMAT_UNKNOWN:
		{
		}
		break;

	case SP_VIDEO_FORMAT_MPEG2:
		{
			mpeg2_ves_init(&p_parser->video1.stat.mpeg2.state);
			mpeg2_ves_init(&p_parser->video2.stat.mpeg2.state);
		}
		break;

	case SP_VIDEO_FORMAT_MPEG4:
		{
			mpeg4_ves_init(&p_parser->video1.stat.mpeg4.state);
			mpeg4_ves_init(&p_parser->video2.stat.mpeg4.state);
		}
		break;

	case SP_VIDEO_FORMAT_H264:
		{
			p_parser->video2.stat.h264.b_find_sps = 0;
			p_parser->video2.stat.h264.b_find_idr = 0;
			p_parser->video2.stat.h264.b_vcl = 0;
			p_parser->video2.stat.h264.frame_mode = H264_FRAME_MODE;
			p_parser->video2.stat.h264.frame_num = 0;

			p_parser->video1.stat.h264.b_find_sps = 0;
			p_parser->video1.stat.h264.b_find_idr = 0;
			p_parser->video1.stat.h264.b_vcl = 0;
			p_parser->video1.stat.h264.frame_mode = H264_FRAME_MODE;
			p_parser->video1.stat.h264.frame_num = 0;

			h264_bs_init(&p_parser->video1.stat.h264.bs);
			h264_bs_init(&p_parser->video2.stat.h264.bs);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	/** video status */
	p_parser->b_video_begin = 0;
	p_parser->b_get_video = 0;
	p_parser->b_video_error = 0;

	p_parser->video2.format = video_format;
	p_parser->video2.b_find_head = 0;
	p_parser->video2.b_pts = 0;

	p_parser->video1.format = video_format;
	p_parser->video1.b_find_head = 0;
	p_parser->video1.b_pts = 0;

	/** video buf */
	p_parser->video_buf.buf_use_size = p_parser->vbuf_frame_offset;
	p_parser->video_buf.buf_left_size = p_parser->video_buf.buf_valid_size - p_parser->video_buf.buf_use_size;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vh_buf.buf_use_size = 0;

	return GEN_S_OK;
}

/**  
 * @brief init audio
 * @param [in] p_parser, parser
 * @param [in] audio_format, audio format
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_init_audio(PS_FRAME_PARSER *p_parser, int32 audio_format)
{

	switch( audio_format )
	{
	case SP_AUDIO_FORMAT_UNKNOWN:
		{
		}
		break;

	case SP_AUDIO_FORMAT_MPEG1:
	case SP_AUDIO_FORMAT_MPEG2:
		{
			mpeg2_audio_header_init(&p_parser->audio1.stat.mpeg2.state);
			mpeg2_audio_header_init(&p_parser->audio2.stat.mpeg2.state);
		}
		break;

	case SP_AUDIO_FORMAT_AAC_RAW:
	case SP_AUDIO_FORMAT_AAC_ADTS:
		{
			aac_adts_init(&p_parser->audio1.stat.aac.adts);
			aac_adts_init(&p_parser->audio2.stat.aac.adts);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	/** audio status */
	p_parser->b_audio_begin = 0;
	p_parser->b_get_audio = 0;
	p_parser->b_audio_error = 0;

	p_parser->audio1.format = audio_format;
	p_parser->audio1.b_pts = 0;
	p_parser->audio2.format = audio_format;
	p_parser->audio2.b_pts = 0;

	/** audio buf */
	p_parser->audio_buf.buf_use_size = p_parser->abuf_frame_offset;
	p_parser->audio_buf.buf_left_size = p_parser->audio_buf.buf_valid_size - p_parser->audio_buf.buf_use_size;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->ah_buf.buf_use_size = 0;

	return GEN_S_OK;
}

/**  
 * @brief update es stream
 * @param [in] p_parser, parser
 * @param [in] p_stream, es stream
 * @param [in] stream_count, es stream count
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_update_stream(PS_FRAME_PARSER* p_parser, MPEG2_ES_STREAM* p_stream, int32 stream_count)
{
	int32  video_format;
	int32  audio_format;

	int32   i;
	//int32  ret;

	p_parser->stream.b_get_ps_map = 1;
	p_parser->stream.es_stream_count = stream_count;

	for( i = 0; i < stream_count; i++ )
	{
		p_parser->stream.es_stream[i].type = p_stream[i].stream_type;
		p_parser->stream.es_stream[i].id = p_stream[i].es_id;

		//log_debug(_T("[ts_frame_parser::ts_frame_parser_update_pmt_stream] pmt program number = %d, stream type = 0x%x, pid = %d\n"),
		//	p_pmt_stream->program_number, p_sub_sec->stream_type, p_sub_sec->pid);
		switch( p_stream[i].stream_type )
		{
		case MPEG_STREAM_MPEG2_VIDEO:
		case MPEG_STREAM_H264:
			{
				if( p_stream[i].stream_type == MPEG_STREAM_MPEG2_VIDEO )
				{
					/** mpeg2 */
					video_format = SP_VIDEO_FORMAT_MPEG2;

				}else if( p_stream[i].stream_type == MPEG_STREAM_H264 )
				{
					/** h264 */
					video_format = SP_VIDEO_FORMAT_H264;

				}else
				{
					/** mpeg2 */
					video_format = SP_VIDEO_FORMAT_MPEG2;
				}

				if( p_parser->b_auto_detect )
				{
					if( !p_parser->b_get_video_stream )
					{
						ps_frame_parser_init_video(p_parser, video_format);

						p_parser->video_stream_id = p_stream[i].es_id;
						p_parser->b_get_video_stream = 1;
					}

				}else if( p_parser->b_get_video_stream && p_parser->video_stream_id == p_stream[i].es_id )
				{
					if( p_parser->video2.format == SP_VIDEO_FORMAT_UNKNOWN )
					{
						/** get video algo */
						ps_frame_parser_init_video(p_parser, video_format);
					}
				}
			}
			break;

		case MPEG_STREAM_MPEG1_AUDIO:
		case MPEG_STREAM_MPEG2_AUDIO:
		case MPEG_STREAM_MPEG2_AAC:
			{
				if( p_stream[i].stream_type == MPEG_STREAM_MPEG1_AUDIO )
				{
					audio_format = SP_AUDIO_FORMAT_MPEG1;

				}else if( p_stream[i].stream_type == MPEG_STREAM_MPEG2_AUDIO )
				{
					audio_format = SP_AUDIO_FORMAT_MPEG2;

				}else
				{
					audio_format = SP_AUDIO_FORMAT_AAC_ADTS;
				}

				if( p_parser->b_auto_detect )
				{
					if( !p_parser->b_get_audio_stream )
					{
						ps_frame_parser_init_audio(p_parser, audio_format);

						p_parser->audio_stream_id = p_stream[i].es_id;
						p_parser->b_get_audio_stream = 1;
					}

				}else if( p_parser->b_get_audio_stream && p_parser->audio_stream_id == p_stream[i].es_id )
				{
					if( p_parser->audio2.format == SP_AUDIO_FORMAT_UNKNOWN )
					{
						/** get audio algo */
						ps_frame_parser_init_audio(p_parser, audio_format);
					}
				}
			}
			break;

		default:
			{
				//log_debug(_T("[ts_frame_parser::ts_frame_parser_update_pmt_stream] unknown stream type = 0x%x\n"), p_sub_sec->stream_type);
			}
			break;
		}
	}


	return GEN_S_OK;
}

/**  
 * @brief update video data
 * @param [in] p_parser, parser
 * @param [in] p_pack, ps pack
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_update_video_data(PS_FRAME_PARSER *p_parser, MPEG2_PS_PACK *p_pack)
{

	int32  parse_len;

	uint8* p_payload = NULL;
	int32  payload_size;

	int32  ret;

	if( p_pack->pes_header.p_pack_data == NULL )
	{
		return GEN_S_OK;
	}

	p_payload = p_pack->pes_header.p_pack_data;
	payload_size = p_pack->pes_header.pack_data_len;

	if( p_pack->pes_header.pts_dts_flag == 0x2 || p_pack->pes_header.pts_dts_flag == 0x3 )
	{
		/** save video timestamp */
		p_parser->video2.b_pts = 1;
		p_parser->video2.pts = p_pack->pes_header.pts;
		//log_debug(_T("[ts_frame_parser::ts_parse_video] video pts = %I64d\n"), pes_header.pts);

		if( !p_parser->video1.b_pts )
		{
			p_parser->video1.b_pts = 1;
			p_parser->video1.pts = p_pack->pes_header.pts;
		}
	}

	if( payload_size > p_parser->video_buf.buf_left_size )
	{
		/** frame overflow */
		p_parser->b_video_error = 1;
		p_parser->video_buf.buf_use_size = p_parser->vbuf_frame_offset;
		p_parser->video_buf.buf_left_size = p_parser->video_buf.buf_valid_size - p_parser->video_buf.buf_use_size;
		p_parser->vbuf_frame_size = 0;
		p_parser->vbuf_parse_size = 0;
		p_parser->vbuf_unparse_size = 0;
		//log_warn(_T("[ts_frame_parser::ts_frame_parser_update_video_data] video frame too large!\n"));
	}

	memcpy(p_parser->video_buf.p_buf + p_parser->video_buf.buf_use_size, p_payload, payload_size);

	p_parser->video_buf.buf_use_size += payload_size;
	p_parser->video_buf.buf_left_size -= payload_size;
	p_parser->vbuf_unparse_size += payload_size;

	return GEN_S_OK;
}

/**  
 * @brief demux video
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_demux_video(PS_FRAME_PARSER *p_parser)
{
	int32  parse_len;
	uint8* p_parse_pos = NULL;
	uint8* p_head_pos;
	int32  head_len = 0;
	int32  b_find_next_frame;

	int32  ret;

	b_find_next_frame = 0;
	p_parse_pos = p_parser->video_buf.p_buf + p_parser->vbuf_frame_offset + p_parser->vbuf_frame_size;
	while( 1 )
	{
		if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG2 )
		{
			/** mpeg2 */
			ret = mpeg2_ves_parse(&p_parser->video2.stat.mpeg2.state, p_parse_pos, p_parser->vbuf_unparse_size, &parse_len);

		}else if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG4 )
		{
			/** mpeg4 */
			ret = mpeg4_ves_parse(&p_parser->video2.stat.mpeg4.state, p_parse_pos, p_parser->vbuf_unparse_size, &parse_len);

		}else if( p_parser->video1.format == SP_VIDEO_FORMAT_H264 )
		{
			/** h264 */
			ret = h264_parse_bs(&p_parser->video2.stat.h264.bs, p_parse_pos, p_parser->vbuf_unparse_size, &parse_len);

		}else
		{
			//log_debug(_T("[ts_frame_parser::ts_frame_parse_video] unknown video format = %d\n"), p_parser->video1.format);
			break;
		}

		p_parse_pos += parse_len;
		p_parser->vbuf_frame_size += parse_len;
		p_parser->vbuf_unparse_size -= parse_len;

		if( ret )
		{
			break;
		}

		if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG2 )
		{
			/** mpeg2 */
			switch( p_parser->video2.stat.mpeg2.state.start_code )
			{
			case MPEG2_SEQ_START_CODE:
				{
					/** I frame */
					b_find_next_frame = 1;
					p_parser->video2.b_find_head = 1;
				}
				break;

			case MPEG2_PICTURE_START_CODE:
				{
					if( p_parser->video2.b_find_head )
					{
						/** I frame */
						p_parser->video1.stat.mpeg2.state.pic.type = p_parser->video2.stat.mpeg2.state.pic.type;

					}else
					{
						/** P frame */
						if( p_parser->b_video_begin )
						{
							b_find_next_frame = 1;
						}
					}

					p_parser->video2.b_find_head = 0;
				}
				break;

			case MPEG2_EXT_START_CODE:
				{
					if( p_parser->video2.stat.mpeg2.state.ext_id == MPEG2_SEQ_EXT_ID )
					{
						p_parser->video1.stat.mpeg2.state.seq.width = p_parser->video2.stat.mpeg2.state.seq.width;
						p_parser->video1.stat.mpeg2.state.seq.height = p_parser->video2.stat.mpeg2.state.seq.height;
					}
				}
				break;

			default:
				{
				}
				break;
			}

		}else if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG4 )
		{
			/** mpeg4 */
			switch( p_parser->video2.stat.mpeg4.state.start_code )
			{
			case MPEG4_VOP_START_CODE:
				{
					if( p_parser->video2.b_find_head )
					{
						/** I frame */
						p_parser->video1.stat.mpeg4.state.vop.frame_type = p_parser->video2.stat.mpeg4.state.vop.frame_type;

					}else
					{
						/** P frame */
						b_find_next_frame = 1;
					}

					p_parser->video2.b_find_head = 0;
				}
				break;

			default:
				{
					if( p_parser->video2.stat.mpeg4.state.start_code >= MPEG4_VOL_START_CODE_BEGIN
						&& p_parser->video2.stat.mpeg4.state.start_code <= MPEG4_VOL_START_CODE_END )
					{
						/** VOL */

						/** I frame */
						b_find_next_frame = 1;
						p_parser->video2.b_find_head = 1;
					}
				}
				break;
			}

		}else
		{
			/** h264 */
			switch( p_parser->video2.stat.h264.bs.nalu_type )
			{
			case H264_NALU_AUD:
				{
					//if( p_parser->video1.b_vcl )
					{
						/** non vcl after fore vcl */
						p_parser->video2.stat.h264.b_vcl = 0;
						b_find_next_frame = 1;

						p_parser->video2.frame_type = SP_VIDEO_INVALID_FRAME;
					}

					p_parser->video1.stat.h264.b_vcl = 0;
				}
				break;

			case H264_NALU_SEI:
			case H264_NALU_SPS:
			case H264_NALU_PPS:
				{
					if( p_parser->video1.stat.h264.b_vcl )
					{
						/** non vcl after fore vcl */
						p_parser->video2.stat.h264.b_vcl = 0;
						b_find_next_frame = 1;

						p_parser->video2.frame_type = SP_VIDEO_INVALID_FRAME;
					}

					if( p_parser->video2.stat.h264.bs.nalu_type == H264_NALU_SPS )
					{
						if( !p_parser->video1.stat.h264.b_find_sps )
						{
							p_parser->video1.stat.h264.b_find_sps = 1;
							p_parser->video1.stat.h264.bs.sps = p_parser->video2.stat.h264.bs.sps;
						}

						if( !p_parser->video2.stat.h264.b_find_sps )
						{
							p_parser->video2.stat.h264.b_find_sps = 1;

							if( !p_parser->b_video_begin )
							{
								p_parser->b_video_begin = 1;
							}
						}
					}

					p_parser->video1.stat.h264.b_vcl = 0;
				}
				break;

			case H264_NALU_IDR_SLICE:
				{
					p_parser->video2.stat.h264.b_find_idr = 1;
					p_parser->video1.stat.h264.b_vcl = 1;
					p_parser->video1.stat.h264.frame_num = p_parser->video2.stat.h264.bs.slice_header.frame_num;

					if( p_parser->video1.frame_type == SP_VIDEO_INVALID_FRAME )
					{
						p_parser->video1.frame_type = SP_VIDEO_I_FRAME;
					}
				}
				break;

			case H264_NALU_NON_IDR_SLICE:
				{
					if( p_parser->video1.stat.h264.b_vcl )
					{
						if( p_parser->video1.stat.h264.frame_num != p_parser->video2.stat.h264.bs.slice_header.frame_num )
						{
							b_find_next_frame = 1;

							p_parser->video2.stat.h264.b_vcl = 1;
							p_parser->video2.stat.h264.b_find_idr = 0;

							if( p_parser->video2.stat.h264.bs.slice_header.type == H264_P_SLICE
								|| p_parser->video2.stat.h264.bs.slice_header.type == H264_DUP_P_SLICE )
							{
								p_parser->video2.frame_type = SP_VIDEO_P_FRAME;

							}else
							{
								p_parser->video2.frame_type = SP_VIDEO_B_FRAME;
							}

							p_parser->video2.stat.h264.frame_num = p_parser->video2.stat.h264.bs.slice_header.frame_num;
							break;
						}
					}

					p_parser->video1.stat.h264.b_vcl = 1;

					if( p_parser->video2.stat.h264.bs.slice_header.type == H264_P_SLICE
						|| p_parser->video2.stat.h264.bs.slice_header.type == H264_DUP_P_SLICE )
					{
						p_parser->video1.frame_type = SP_VIDEO_P_FRAME;

					}else
					{
						p_parser->video1.frame_type = SP_VIDEO_B_FRAME;
					}

					p_parser->video1.stat.h264.frame_num = p_parser->video2.stat.h264.bs.slice_header.frame_num;
					p_parser->video2.stat.h264.frame_num = p_parser->video2.stat.h264.bs.slice_header.frame_num;
					p_parser->video2.stat.h264.b_find_idr = 0;
				}
				break;

			default:
				{
				}
				break;
			}
		}

		if( b_find_next_frame )
		{
			/** calc next frame header size */
			if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG2 )
			{
				p_head_pos = p_parser->video2.stat.mpeg2.state.p_start;
				head_len = p_parse_pos - p_parser->video2.stat.mpeg2.state.p_start;

			}else if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG4 )
			{
				p_head_pos = p_parser->video2.stat.mpeg4.state.p_start;
				head_len = p_parse_pos - p_parser->video2.stat.mpeg4.state.p_start;

			}else
			{
				p_head_pos = p_parser->video2.stat.h264.bs.p_start;
				head_len = p_parse_pos - p_parser->video2.stat.h264.bs.p_start;
			}

			p_parser->vh_buf.buf_use_size = head_len;

			/** calc current frame actual size */
			p_parser->vbuf_frame_size -= head_len;
			p_parser->vbuf_parse_size = head_len;
			p_parser->vbuf_unparse_size += head_len;

			break;
		}

	}//end while(1)

	if( b_find_next_frame )
	{
		return GEN_S_OK;
	}

	return GEN_E_NEED_MORE_DATA;
}


/**  
 * @brief update video frame
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_update_video_frame(PS_FRAME_PARSER *p_parser)
{
	p_parser->b_video_error = 0;

	if( p_parser->vbuf_frame_size > 0 && p_parser->vbuf_unparse_size > 0 )
	{
		/** copy next frame data */
		memcpy(p_parser->video_buf.p_buf + p_parser->vbuf_frame_offset, p_parser->video_buf.p_buf + p_parser->vbuf_frame_offset + p_parser->vbuf_frame_size, p_parser->vbuf_unparse_size);
	}

	p_parser->video_buf.buf_use_size = p_parser->vbuf_frame_offset + p_parser->vbuf_unparse_size;
	p_parser->video_buf.buf_left_size = p_parser->video_buf.buf_valid_size - p_parser->video_buf.buf_use_size;
	p_parser->vbuf_frame_size = p_parser->vh_buf.buf_use_size;
	p_parser->vbuf_unparse_size -= p_parser->vh_buf.buf_use_size;
	p_parser->vh_buf.buf_use_size = 0;

	memcpy(&p_parser->video1, &p_parser->video2, sizeof(PS_VIDEO_STAT));
	p_parser->video2.b_pts = 0;

	return GEN_S_OK;
}

/**  
 * @brief submit video frame
 * @param [in] p_parser, parser
 * @param [out] p_frame, frame
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_submit_video_frame(PS_FRAME_PARSER* p_parser, SP_MEDIA_FRAME* p_frame)
{
	p_parser->b_get_video = 1;

	p_frame->format = SP_MEDIA_FORMAT_VIDEO;
	p_frame->payload.video.p_data = p_parser->video_buf.p_buf + p_parser->vbuf_frame_offset;
	p_frame->payload.video.data_size = p_parser->vbuf_frame_size;

	p_frame->p_data = p_frame->payload.video.p_data;
	p_frame->data_size = p_frame->payload.video.data_size;

	p_frame->flag = 0;
	if( p_parser->b_video_error )
	{
		p_frame->flag |= SP_MEDIA_FLAG_ERROR;
	}

	if( p_parser->video1.b_pts )
	{
		p_frame->flag |= SP_MEDIA_FLAG_TIMESTAMP;
		p_frame->timescale = MPEG2_SYS_TICK_PER_SECOND;
		p_frame->ts = p_parser->video1.pts;
		p_frame->payload.video.ts = p_parser->video1.pts;
	}

	p_frame->payload.video.format = p_parser->video1.format;
	p_frame->payload.video.flag = 0;
	if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG2 )
	{
		/** mpeg2 */
		p_frame->payload.video.fourcc = GEN_FOURCC('m', 'p', '2', 'v');
		switch( p_parser->video1.stat.mpeg2.state.pic.type )
		{
		case MPEG2_I_FRAME:
			{
				p_frame->payload.video.type = SP_VIDEO_I_FRAME;
			}
			break;

		case MPEG2_P_FRAME:
			{
				p_frame->payload.video.type = SP_VIDEO_P_FRAME;
			}
			break;

		default:
			{
				p_frame->payload.video.type = SP_VIDEO_B_FRAME;
			}
			break;
		}

		switch( p_parser->video1.stat.mpeg2.state.pic.pic_struct )
		{
		case MPEG2_VIDEO_PIC_TOP_FIELD:
			{
				//p_frame->payload.video.flag |= SP_VIDEO_TOP_FIELD;
			}
			break;

		case MPEG2_VIDEO_PIC_BOTTOM_FIELD:
			{
				p_frame->payload.video.flag |= SP_VIDEO_FLAG_BOTTOM;
			}
			break;

		case MPEG2_VIDEO_PIC_FRAME:
			{
				p_frame->payload.video.flag |= SP_VIDEO_FLAG_FRAME;
			}
			break;

		default:
			{
				p_frame->payload.video.flag |= SP_VIDEO_FLAG_FRAME;
			}
			break;
		}

		p_frame->payload.video.width = p_parser->video1.stat.mpeg2.state.seq.width;
		p_frame->payload.video.height = p_parser->video1.stat.mpeg2.state.seq.height;

	}else if( p_parser->video1.format == SP_VIDEO_FORMAT_MPEG4 )
	{
		/** mpeg4 */
		p_frame->payload.video.flag |= SP_VIDEO_FLAG_FRAME;
		switch( p_parser->video1.stat.mpeg4.state.vop.frame_type )
		{
		case MPEG4_I_FRAME:
			{
				p_frame->payload.video.type = SP_VIDEO_I_FRAME;
			}
			break;

		case MPEG4_P_FRAME:
			{
				p_frame->payload.video.type = SP_VIDEO_P_FRAME;
			}
			break;

		case MPEG4_B_FRAME:
			{
				p_frame->payload.video.type = SP_VIDEO_B_FRAME;
			}
			break;

		default:
			{
				p_frame->payload.video.type = SP_VIDEO_SPRITE_FRAME;
			}
			break;
		}

		if( p_parser->video1.stat.mpeg4.state.vol.b_interlace )
		{

		}else
		{
			p_frame->payload.video.flag |= SP_VIDEO_FLAG_FRAME;
		}

		p_frame->payload.video.width = p_parser->video1.stat.mpeg4.state.vol.width;
		p_frame->payload.video.height = p_parser->video1.stat.mpeg4.state.vol.height;

	}else
	{
		/** h264 */
		p_frame->payload.video.flag |= SP_VIDEO_FLAG_FRAME;
		p_frame->payload.video.type = p_parser->video1.frame_type;
		p_frame->payload.video.width = p_parser->video1.stat.h264.bs.sps.width;
		p_frame->payload.video.height = p_parser->video1.stat.h264.bs.sps.height;
	}

	return GEN_S_OK;
}

/**  
 * @brief update audio data
 * @param [in] p_parser, parser
 * @param [in] p_pack, ps pack
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_update_audio_data(PS_FRAME_PARSER* p_parser, MPEG2_PS_PACK* p_pack)
{
	//int32  parse_len;

	uint8* p_payload = NULL;
	int32  payload_size;

	int32  ret;

	if( p_pack->pes_header.p_pack_data == NULL )
	{
		return GEN_S_OK;
	}

	p_payload = p_pack->pes_header.p_pack_data;
	payload_size = p_pack->pes_header.pack_data_len;

	if( p_pack->pes_header.pts_dts_flag == 0x2 || p_pack->pes_header.pts_dts_flag == 0x3 )
	{
		/** save timestamp */
		p_parser->audio2.b_pts = 1;
		p_parser->audio2.pts = p_pack->pes_header.pts;
		//log_debug(_T("[ts_frame_parser::ts_parse_audio] audio pts = %I64d\n"), pes_header.pts);
	}

	/** update payload */
	if( payload_size > p_parser->audio_buf.buf_left_size )
	{
		/** frame overflow */
		p_parser->b_audio_error = 1;
		p_parser->audio_buf.buf_use_size = p_parser->abuf_frame_offset;
		p_parser->audio_buf.buf_left_size = p_parser->audio_buf.buf_valid_size - p_parser->audio_buf.buf_use_size;
		p_parser->abuf_frame_size = 0;
		p_parser->abuf_parse_size = 0;
		p_parser->abuf_unparse_size = 0;
		//log_warn(_T("[ts_frame_parser::ts_parse_audio] audio frame too large!\n"));
	}

	memcpy(p_parser->audio_buf.p_buf + p_parser->audio_buf.buf_use_size, p_payload, payload_size);

	p_parser->audio_buf.buf_use_size += payload_size;
	p_parser->audio_buf.buf_left_size -= payload_size;
	p_parser->abuf_unparse_size += payload_size;

	return GEN_S_OK;
}

/**  
 * @brief parse audio
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_demux_audio(PS_FRAME_PARSER *p_parser)
{
	int32  parse_len;
	uint8* p_parse_pos = NULL;
	uint8* p_head_pos;
	int32  head_len;
	int32  b_find_next_frame;

	//int32  avg_frame_size;

	int32  ret;

	b_find_next_frame = 0;
	p_parse_pos = p_parser->audio_buf.p_buf + p_parser->abuf_frame_offset + p_parser->abuf_frame_size;
	while( 1 )
	{
		if( p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG1
			|| p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG2 )
		{
			/** mpeg2 */
			ret = mpeg2_audio_header_demux(&p_parser->audio2.stat.mpeg2.state, p_parse_pos, p_parser->abuf_unparse_size, &parse_len);

		}else if( p_parser->audio1.format == SP_AUDIO_FORMAT_AAC_ADTS )
		{
			/** AAC */
			ret = aac_adts_parse(&p_parser->audio2.stat.aac.adts, p_parse_pos, p_parser->abuf_unparse_size, &parse_len);

		}else
		{
			ret = GEN_E_NOT_SUPPORT;
			parse_len = p_parser->abuf_unparse_size;
			//log_debug(_T("[ts_frame_parser::ts_parse_audio] unknown audio format = %d\n"), p_parser->audio1.format);
		}

		p_parse_pos += parse_len;
		p_parser->abuf_frame_size += parse_len;
		p_parser->abuf_unparse_size -= parse_len;

		if( ret )
		{
			break;
		}

		/** calc next frame header size */
		if( p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG1
			|| p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG2 )
		{
			/** mpeg2 */
#if 1
			if( p_parser->audio2.stat.mpeg2.state.version == MPEG2_AUDIO_VERSION_MPEG2_5 )
			{
				//log_debug(_T("[ts_frame_parser::ts_parse_audio] mpeg version = %d!\n"), p_parser->audio2.aes.mpeg2.state.version);
				continue;
			}
#endif

			if( p_parser->b_audio_begin )
			{
				if( p_parser->audio1.stat.mpeg2.state.sample_rate != p_parser->audio2.stat.mpeg2.state.sample_rate || p_parser->audio1.stat.mpeg2.state.chn != p_parser->audio2.stat.mpeg2.state.chn )
				{
					if( !p_parser->b_auto_adapt_audio )
					{
						continue;
					}

					//log_debug(_T("[ts_frame_parser::ts_parse_audio] new audio chn = %d, sample rate = %d\n"), p_parser->audio2.stat.mpeg2.state.chn, p_parser->audio2.stat.mpeg2.state.sample_rate);
				}

				if( p_parser->abuf_frame_size < p_parser->audio1.stat.mpeg2.avg_frame_size )
				{
					continue;
				}

			}else
			{
				if( p_parser->audio2.stat.mpeg2.state.sample_rate != 0 && p_parser->audio2.stat.mpeg2.state.bit_rate > 0 )
				{
					p_parser->audio2.stat.mpeg2.avg_frame_size = ((p_parser->audio2.stat.mpeg2.state.bit_rate * 1000) >> 3) * 1152 / p_parser->audio2.stat.mpeg2.state.sample_rate;

				}else
				{
					p_parser->audio2.stat.mpeg2.avg_frame_size = 128;
				}

				//log_debug(_T("[ts_frame_parser::ts_parse_audio] audio chn = %d, sample rate = %d, bit rate = %dkbps, avg frame size = %d\n"), p_parser->audio2.stat.mpeg2.state.chn, p_parser->audio2.stat.mpeg2.state.sample_rate, p_parser->audio2.stat.mpeg2.state.bit_rate, p_parser->audio2.stat.mpeg2.avg_frame_size);
			}

			p_head_pos = p_parser->audio2.stat.mpeg2.state.p_start;
			head_len = p_parse_pos - (uint8*)p_parser->audio2.stat.mpeg2.state.p_start;

		}else
		{
			/** AAC */
			if( p_parser->b_audio_begin )
			{
				if( p_parser->abuf_frame_size < p_parser->audio1.stat.aac.frame_size )
				{
					continue;
				}

			}

			p_parser->audio2.stat.aac.frame_size = p_parser->audio2.stat.aac.adts.var_header.frame_len;
			//log_debug(_T("[ts_frame_parser::ts_parse_audio] aac chn = %d, sample rate = %d, frame size = %d\n"), p_parser->audio2.aes.aac.adts_pack.fix_header.chn_config, p_parser->audio2.aes.aac.adts_pack.fix_header.sample_freq, p_parser->audio2.aes.aac.adts_pack.var_header.frame_len);

			p_head_pos = p_parser->audio2.stat.aac.adts.p_start;
			head_len = p_parse_pos - p_parser->audio2.stat.aac.adts.p_start;
		}

		p_parser->ah_buf.buf_use_size = head_len;

		/** calc current frame actual size */
		p_parser->abuf_frame_size -= head_len;
		p_parser->abuf_unparse_size += head_len;
		p_parser->abuf_parse_size = head_len;

		b_find_next_frame = 1;

		break;
	}

	if( b_find_next_frame )
	{
		return GEN_S_OK;
	}

	return GEN_E_NEED_MORE_DATA;
}


/**  
 * @brief update audio frame
 * @param [in] p_parser, parser
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_update_audio_frame(PS_FRAME_PARSER* p_parser)
{
	p_parser->b_audio_error = 0;

	if( p_parser->abuf_frame_size > 0 && p_parser->abuf_unparse_size > 0 )
	{
		/** copy next frame data */
		memcpy(p_parser->audio_buf.p_buf + p_parser->abuf_frame_offset, p_parser->audio_buf.p_buf + p_parser->abuf_frame_offset + p_parser->abuf_frame_size, p_parser->abuf_unparse_size);
	}

	p_parser->audio_buf.buf_use_size = p_parser->abuf_frame_offset + p_parser->abuf_unparse_size;
	p_parser->audio_buf.buf_left_size = p_parser->audio_buf.buf_valid_size - p_parser->audio_buf.buf_use_size;
	p_parser->abuf_frame_size = p_parser->ah_buf.buf_use_size;
	p_parser->abuf_unparse_size -= p_parser->ah_buf.buf_use_size;
	p_parser->ah_buf.buf_use_size = 0;

	memcpy(&p_parser->audio1, &p_parser->audio2, sizeof(PS_AUDIO_STAT));
	p_parser->audio2.b_pts = 0;

	return GEN_S_OK;
}

/**  
 * @brief submit audio frame
 * @param [in] p_parser, parser
 * @param [out] p_frame, frame
 * @return
 * 0: success
 * other: fail
 */
static int32 ps_frame_parser_submit_audio_frame(PS_FRAME_PARSER* p_parser, SP_MEDIA_FRAME* p_frame)
{
	p_parser->b_get_audio = 1;

	p_frame->format = SP_MEDIA_FORMAT_AUDIO;
	p_frame->payload.audio.p_data = p_parser->audio_buf.p_buf + p_parser->abuf_frame_offset;
	p_frame->payload.audio.data_size = p_parser->abuf_frame_size;

	p_frame->flag = 0;
	if( p_parser->b_audio_error )
	{
		p_frame->flag |= SP_MEDIA_FLAG_ERROR;
	}

	if( p_parser->audio1.b_pts )
	{
		p_frame->flag |= SP_MEDIA_FLAG_TIMESTAMP;
		p_frame->timescale = MPEG2_SYS_TICK_PER_SECOND;
		p_frame->ts = p_parser->audio1.pts;
	}

	p_frame->payload.audio.format = p_parser->audio1.format;
	p_frame->payload.audio.flag = 0x0;
	p_frame->payload.audio.resv = 0x0;

	if( p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG1
		|| p_parser->audio1.format == SP_AUDIO_FORMAT_MPEG2 )
	{
		/** mpeg2 */
		p_frame->payload.audio.timescale = p_parser->audio1.stat.mpeg2.state.sample_rate;
		p_frame->payload.audio.duration = 1152;
		p_frame->payload.audio.ts = 0;

		p_frame->payload.audio.param.chn = p_parser->audio1.stat.mpeg2.state.chn;
		p_frame->payload.audio.param.sample_rate = p_parser->audio1.stat.mpeg2.state.sample_rate;
		p_frame->payload.audio.param.sample_bit = p_parser->audio1.stat.mpeg2.state.sample_bit;

	}else
	{
		/** aac */
		p_frame->payload.audio.timescale = p_parser->audio1.stat.aac.adts.fix_header.sample_freq;
		p_frame->payload.audio.duration = 1024;
		p_frame->payload.audio.ts = 0;

		if( p_parser->audio1.stat.aac.adts.fix_header.chn_config == 1 )
		{
			p_frame->payload.audio.param.chn = SP_AUDIO_CHN_MONO;

		}else
		{
			p_frame->payload.audio.param.chn = SP_AUDIO_CHN_STEREO;
		}

		p_frame->payload.audio.param.sample_rate = p_parser->audio1.stat.aac.adts.fix_header.sample_freq;
		p_frame->payload.audio.param.sample_bit = SP_AUDIO_SAMPLE_SIZE_16BIT;
	}

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
PS_FRAME_PARSER_H ps_frame_parser_create()
{
	PS_FRAME_PARSER* p_parser = NULL;
	p_parser = (PS_FRAME_PARSER*)malloc( sizeof(PS_FRAME_PARSER) );

	if( p_parser == NULL )
	{
		return NULL;
	}

	memset(p_parser, 0, sizeof(PS_FRAME_PARSER));

	return p_parser;
}

int32 ps_frame_parser_destroy(PS_FRAME_PARSER_H h)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_parser->b_init )
	{
		ps_frame_parser_deinit(p_parser);
	}

	free(p_parser);

	return GEN_S_OK;
}

int32 ps_frame_parser_init(PS_FRAME_PARSER_H h)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	int32   ret;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_parser->b_init )
	{
		return GEN_S_OK;
	}

	ps_frame_parser_init_stream(&p_parser->stream);

	ps_frame_parser_init_av_meta(p_parser);

	p_parser->b_video_begin = 0;
	p_parser->b_get_video = 0;
	p_parser->b_video_error = 0;

	ret = gen_buf_init(&p_parser->video_buf, PS_FRAME_PARSER_MAX_VIDEO_FRAME_SIZE);
	if( ret )
	{
		ps_frame_parser_deinit(p_parser);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vbuf_frame_offset = 0;

	ret = gen_buf_init(&p_parser->vh_buf, PS_FRAME_PARSER_MAX_VIDEO_FRAME_HEADER_SIZE);
	if( ret )
	{
		ps_frame_parser_deinit(p_parser);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_parser->b_audio_begin = 0;
	p_parser->b_get_audio = 0;
	p_parser->b_audio_error = 0;

	ret = gen_buf_init(&p_parser->audio_buf, PS_FRAME_PARSER_MAX_AUDIO_FRAME_SIZE);
	if( ret )
	{
		ps_frame_parser_deinit(p_parser);
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->abuf_frame_offset = 0;

	ret = gen_buf_init(&p_parser->ah_buf, PS_FRAME_PARSER_MAX_AUDIO_FRAME_HEADER_SIZE);
	if( ret )
	{
		ps_frame_parser_deinit(p_parser);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_parser->b_init = 1;

	return GEN_S_OK;
}

int32 ps_frame_parser_deinit(PS_FRAME_PARSER_H h)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	gen_buf_deinit(&p_parser->video_buf);
	gen_buf_deinit(&p_parser->vh_buf);

	gen_buf_deinit(&p_parser->audio_buf);
	gen_buf_deinit(&p_parser->ah_buf);

	p_parser->b_init = 0;

	return GEN_S_OK;
}

int32 ps_frame_parser_reset(PS_FRAME_PARSER_H h)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	//int32  i;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ps_frame_parser_init_stream(&p_parser->stream);

	ps_frame_parser_init_av_meta(p_parser);

	p_parser->b_video_begin = 0;
	p_parser->b_get_video = 0;
	p_parser->b_video_error = 0;
	memset(&p_parser->video1, 0, sizeof(PS_VIDEO_STAT));
	memset(&p_parser->video2, 0, sizeof(PS_VIDEO_STAT));

	p_parser->video_buf.buf_use_size = p_parser->vbuf_frame_offset;
	p_parser->video_buf.buf_left_size = p_parser->video_buf.buf_valid_size - p_parser->video_buf.buf_use_size;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vh_buf.buf_use_size = 0;
	p_parser->vh_buf.buf_left_size = PS_FRAME_PARSER_MAX_VIDEO_FRAME_HEADER_SIZE;

	p_parser->b_audio_begin = 0;
	p_parser->b_get_audio = 0;
	p_parser->b_audio_error = 0;
	memset(&p_parser->audio1, 0, sizeof(PS_AUDIO_STAT));
	memset(&p_parser->audio2, 0, sizeof(PS_AUDIO_STAT));

	p_parser->audio_buf.buf_use_size = p_parser->abuf_frame_offset;
	p_parser->audio_buf.buf_left_size = p_parser->audio_buf.buf_valid_size - p_parser->audio_buf.buf_use_size;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->ah_buf.buf_use_size = 0;

	return GEN_S_OK;
}

int32 ps_frame_parser_set_auto_detect(PS_FRAME_PARSER_H h, int32 b_auto_detect)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	//int32  ret;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( b_auto_detect )
	{
		if( p_parser->b_auto_detect )
		{
			return GEN_S_OK;
		}

		ps_frame_parser_reset(h);

	}else
	{
		if( !p_parser->b_auto_detect )
		{
			return GEN_S_OK;
		}

		p_parser->b_get_video_stream = 0;
		p_parser->b_get_audio_stream = 0;
	}

	p_parser->b_auto_detect = b_auto_detect;

	return GEN_S_OK;
}

int32 ps_frame_parser_set_max_video_frame_size(PS_FRAME_PARSER_H h, int32 max_size)
{
	PS_FRAME_PARSER *p_parser = (PS_FRAME_PARSER*)h;

	int32   ret;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_parser->video_buf.buf_total_size != max_size )
	{
		gen_buf_deinit(&p_parser->video_buf);

		ret = gen_buf_init(&p_parser->video_buf, max_size);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	p_parser->video_buf.buf_total_size = max_size;
	p_parser->video_buf.buf_valid_size = max_size;
	p_parser->video_buf.buf_use_size = p_parser->vbuf_frame_offset;
	p_parser->video_buf.buf_left_size = max_size - p_parser->video_buf.buf_use_size;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vh_buf.buf_use_size = 0;

	return GEN_S_OK;
}

int32 ps_frame_parser_set_video_frame_offset(PS_FRAME_PARSER_H h, int32 offset)
{
	PS_FRAME_PARSER *p_parser = (PS_FRAME_PARSER*)h;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_parser->video_buf.buf_use_size = offset;
	p_parser->video_buf.buf_left_size = p_parser->video_buf.buf_valid_size - p_parser->video_buf.buf_use_size;
	p_parser->vbuf_frame_size = 0;
	p_parser->vbuf_parse_size = 0;
	p_parser->vbuf_unparse_size = 0;
	p_parser->vh_buf.buf_use_size = 0;
	p_parser->vbuf_frame_offset = offset;

	return GEN_S_OK;
}

int32 ps_frame_parser_set_max_audio_frame_size(PS_FRAME_PARSER_H h, int32 max_size)
{
	PS_FRAME_PARSER *p_parser = (PS_FRAME_PARSER*)h;

	int32   ret;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_parser->audio_buf.buf_total_size != max_size )
	{
		gen_buf_deinit(&p_parser->audio_buf);

		ret = gen_buf_init(&p_parser->audio_buf, max_size);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	p_parser->audio_buf.buf_total_size = max_size;
	p_parser->audio_buf.buf_valid_size = max_size;
	p_parser->audio_buf.buf_use_size = p_parser->abuf_frame_offset;
	p_parser->audio_buf.buf_left_size = max_size - p_parser->audio_buf.buf_use_size;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->ah_buf.buf_use_size = 0;

	return GEN_S_OK;
}

int32 ps_frame_parser_set_audio_frame_offset(PS_FRAME_PARSER_H h, int32 offset)
{
	PS_FRAME_PARSER *p_parser = (PS_FRAME_PARSER*)h;

	if( p_parser == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_parser->audio_buf.buf_use_size = offset;
	p_parser->audio_buf.buf_left_size = p_parser->audio_buf.buf_valid_size - p_parser->audio_buf.buf_use_size;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->ah_buf.buf_use_size = 0;
	p_parser->abuf_frame_offset = offset;

	return GEN_S_OK;
}

int32 ps_frame_parser_demux_frame(PS_FRAME_PARSER_H h, uint8* p_data, int32 data_size, SP_MEDIA_FRAME* p_frame, int32* p_demux_size)
{
	PS_FRAME_PARSER* p_parser = (PS_FRAME_PARSER*)h;

	MPEG2_PS_PACK  ps_pack;

	MPEG2_ES_STREAM  es_stream[PS_STREAM_MAX_ES_STREAM_NUM];
	int32   es_stream_count;

	int32   parse_size;
	int32   total_parse_size;
	int32   unparse_size;
	uint8*  p_parse_buf;
	int32   b_get_frame;

	int32   demux_size;

	int32   i;
	int32   ret;

	if( p_parser == NULL || p_frame == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_parser->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_parser->b_get_video )
	{
		ps_frame_parser_update_video_frame(p_parser);
		p_parser->b_get_video = 0;
	}

	if( p_parser->b_get_audio )
	{
		ps_frame_parser_update_audio_frame(p_parser);
		p_parser->b_get_audio = 0;
	}

	if( data_size == 0 )
	{
		ret = ps_frame_parser_demux_video(p_parser);
		if( ret == 0 )
		{
			if( p_parser->vbuf_frame_size > 0 )
			{
				ps_frame_parser_submit_video_frame(p_parser, p_frame);
				return GEN_S_OK;

			}else
			{
				ps_frame_parser_update_video_frame(p_parser);
			}
		}

		ret = ps_frame_parser_demux_audio(p_parser);
		if( ret == 0 )
		{
			if( p_parser->abuf_frame_size > 0 )
			{
				ps_frame_parser_submit_audio_frame(p_parser, p_frame);
				return GEN_S_OK;

			}else
			{
				ps_frame_parser_update_audio_frame(p_parser);
			}
		}

		return GEN_E_NEED_MORE_DATA;
	}

	total_parse_size = 0;
	p_parse_buf = p_data;
	unparse_size = data_size;
	b_get_frame = 0;

	while( !b_get_frame )
	{
		ret = mpeg2_ps_demux(&ps_pack, p_parse_buf, unparse_size, &parse_size);
		p_parse_buf += parse_size;
		total_parse_size += parse_size;
		unparse_size -= parse_size;

		if( ret )
		{
			/** need more data */
			break;
		}

		switch( ps_pack.start_code )
		{
		case MPEG2_PS_START_CODE:
			{
				/** ps start */
			}
			break;

		case MPEG2_SYSTEM_HEADER_START_CODE:
			{
				/** sys header */
			}
			break;

		case MPEG2_PS_MAP_START_CODE:
			{
				/** ps map */
				es_stream_count = PS_STREAM_MAX_ES_STREAM_NUM;
				demux_size = ps_pack.ps_map.es_map_len;
				ret = mpeg2_pes_demux_es_map(es_stream, &es_stream_count, ps_pack.ps_map.p_es_map, &demux_size);
				if( ret == 0 )
				{
					ret = ps_frame_parser_update_stream(p_parser, es_stream, es_stream_count);
				}
			}
			break;

		default:
			{
				if( (ps_pack.start_code >= MPEG2_PES_VIDEO_SID_START) && (ps_pack.start_code <= MPEG2_PES_VIDEO_SID_END) )
				{
					/** video */
					ps_frame_parser_update_video_data(p_parser, &ps_pack);

					ret = ps_frame_parser_demux_video(p_parser);
					if( ret == 0 )
					{
						if( p_parser->b_video_begin )
						{
							/** submit */
							if( p_parser->vbuf_frame_size > 0 )
							{
								b_get_frame = 1;
								ps_frame_parser_submit_video_frame(p_parser, p_frame);

							}else
							{
								ps_frame_parser_update_video_frame(p_parser);
							}

						}else
						{
							/** update */
							ps_frame_parser_update_video_frame(p_parser);
						}

						p_parser->b_video_begin = 1;
					}

				}else if( (ps_pack.start_code >= MPEG2_PES_AUDIO_SID_START) && (ps_pack.start_code <= MPEG2_PES_AUDIO_SID_END) )
				{
					/** audio */
					ps_frame_parser_update_audio_data(p_parser, &ps_pack);

					ret = ps_frame_parser_demux_audio(p_parser);
					if( ret == 0 )
					{
						if( p_parser->b_audio_begin )
						{
							if( p_parser->abuf_frame_size > 0 )
							{
								b_get_frame = 1;
								ps_frame_parser_submit_audio_frame(p_parser, p_frame);

							}else
							{
								ps_frame_parser_update_audio_frame(p_parser);
							}

						}else
						{
							ps_frame_parser_update_audio_frame(p_parser);
						}

						p_parser->b_audio_begin = 1;
					}
				}

			}//end default
			break;
		}

	}//end while

	*p_demux_size = total_parse_size;

	if( b_get_frame )
	{
		return GEN_S_OK;
	}

	return GEN_E_NEED_MORE_DATA;
}

