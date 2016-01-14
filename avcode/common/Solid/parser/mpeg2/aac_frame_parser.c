
#include <string.h>
#include <stdlib.h>

#include "mpeg2_aac_parser.h"
#include "aac_frame_parser.h"

#include "../../util/log_debug.h"


/** 音频解析缓冲区大小 */
#define AAC_FRAME_PARSER_MAX_BUF_SIZE                      (16*1024)


/** AAC帧解析器 */
typedef struct tag_aac_parser
{
	/** 初始化标志 */
	int32  b_init;

	/** 是否音频序列开始 */
	int32  b_audio_start;
	/** 是否已获取前一帧音频数据 */
	int32  b_get_audio;
	/** 当前音频帧 */
	ADTS_PACK audio1;
	/** 下一音频帧 */
	ADTS_PACK audio2;

	/** 音频数据缓冲区 */
	uint8 *p_audio_buf;
	/** 音频缓冲区总长度 */
	int32 abuf_total_size;
	/** 音频缓冲区已使用长度 */
	int32 abuf_use_size;
	/** 音频缓冲区剩余长度 */
	int32 abuf_left_size;
	/** 音频缓冲区帧长度 */
	int32 abuf_frame_size;
	/** 音频缓冲区已解析长度 */
	int32 abuf_parse_size;
	/** 音频缓冲区未解析长度 */
	int32 abuf_unparse_size;

	/** 音频帧头长度 */
	int32 audio_header_size;

} AAC_PARSER, *PAAC_PARSER;


static int32 aac_update_audio(AAC_PARSER *p_parser);
static int32 aac_submit_audio(AAC_PARSER *p_parser, MEDIA_FRAME *p_frame);

/**  
 * @brief 解析AAC音频流
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
static int32 aac_parse_audio(AAC_PARSER *p_parser, MEDIA_FRAME *p_frame, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32  parse_len;
	uint8* p_parse_pos = NULL;
	uint8* p_head_pos;
	int32  head_len = 0;
	int32  b_find_next_frame;

	int32  ret;

	if( buf_len > p_parser->abuf_total_size )
	{
		*p_parse_len = 0;
		log_warn(_T("[aac_frame_parser::aac_parse_audio] audio input size = %d too large!\n"), buf_len);
		return -1;
	}

	if( buf_len > p_parser->abuf_left_size )
	{
		/** 帧长度溢出 */
		p_parser->abuf_use_size = 0;
		p_parser->abuf_left_size = p_parser->abuf_total_size;
		p_parser->abuf_frame_size = 0;
		p_parser->abuf_unparse_size = 0;
		log_warn(_T("[aac_frame_parser::aac_parse_audio] audio frame too large!\n"));
	}

	/** 保存音频数据 */
	memcpy(p_parser->p_audio_buf + p_parser->abuf_use_size, p_buf, buf_len);
	p_parser->abuf_use_size += buf_len;
	p_parser->abuf_left_size -= buf_len;
	p_parser->abuf_unparse_size += buf_len;

	/** 解析音频帧 */
	b_find_next_frame = 0;
	p_parse_pos = p_parser->p_audio_buf + p_parser->abuf_frame_size;
	while( 1 )
	{
		ret = aac_adts_parse(&p_parser->audio2, p_parse_pos, p_parser->abuf_unparse_size, &parse_len);
		p_parse_pos += parse_len;
		p_parser->abuf_frame_size += parse_len;
		p_parser->abuf_unparse_size -= parse_len;

		if( ret )
		{
			break;
		}

		b_find_next_frame = 1;

		/** 计算下一帧的帧头长度 */
		p_head_pos = p_parser->audio2.p_start;
		head_len = p_parse_pos - p_parser->audio2.p_start;

		p_parser->audio_header_size = head_len;

		/** 计算当前帧实际长度 */
		p_parser->abuf_frame_size -= head_len;
		p_parser->abuf_unparse_size += head_len;
		p_parser->abuf_parse_size = head_len;

		break;
	}

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
 * @brief 更新当前音频帧状态
 * @param [in] p_parser 解析器句柄
 * @return
 * 0：成功
 */
static int32 aac_update_audio(AAC_PARSER *p_parser)
{
	if( p_parser->abuf_frame_size > 0 && p_parser->abuf_unparse_size > 0 )
	{
		/** 保存下一帧数据 */
		memcpy(p_parser->p_audio_buf, p_parser->p_audio_buf + p_parser->abuf_frame_size, p_parser->abuf_unparse_size);
	}

	p_parser->abuf_use_size = p_parser->abuf_unparse_size;
	p_parser->abuf_left_size = p_parser->abuf_total_size - p_parser->abuf_use_size;
	p_parser->abuf_frame_size = p_parser->abuf_parse_size;
	p_parser->abuf_unparse_size -= p_parser->abuf_parse_size;
	p_parser->audio_header_size = 0;

	memcpy(&p_parser->audio1, &p_parser->audio2, sizeof(ADTS_PACK));

	return 0;
}

/**  
 * @brief 提交视频帧
 * @param [in] p_parser 解析器句柄
 * @param [in] p_frame 帧句柄
 * @return
 * 0：成功
 */
static int32 aac_submit_audio(AAC_PARSER *p_parser, MEDIA_FRAME *p_frame)
{
	p_parser->b_get_audio = 1;

	p_frame->type = MP_DATA_TYPE_AUDIO;
	p_frame->p_frame = p_parser->p_audio_buf;
	p_frame->frame_size = p_parser->abuf_frame_size;

	p_frame->frame_flag = 0;
	p_frame->frame_flag |= MP_FRAME_VALID;
	p_frame->frame_ts = 0;

	p_frame->param.audio.algo = MP_AUDIO_AAC;
	p_frame->param.audio.chn = MP_AUDIO_DUAL;
	p_frame->param.audio.sample_rate = p_parser->audio2.fix_header.sample_freq;
	p_frame->param.audio.sample_bits = MP_AUDIO_SAMPLE_16BIT;
	p_frame->param.audio.b_padding = 0;
	p_frame->param.audio.elapse = 30;

	p_frame->p_user_data = NULL;
	p_frame->user_data_size = 0;

	return 0;
}


///////////////////////////////////外部接口////////////////////////////////////
AAC_FRAME_PARSER aac_frame_parser_create()
{
	AAC_PARSER *p_parser = NULL;
	p_parser = (AAC_PARSER*)malloc( sizeof(AAC_PARSER) );

	if( p_parser == NULL )
	{
		return NULL;
	}

	memset(p_parser, 0, sizeof(AAC_PARSER));

	return p_parser;
}

int32 aac_frame_parser_destroy(AAC_FRAME_PARSER frame_parser)
{
	AAC_PARSER *p_parser = (AAC_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->b_init )
	{
		aac_frame_parser_deinit(p_parser);
	}

	free(p_parser);

	return 0;
}

int32 aac_frame_parser_init(AAC_FRAME_PARSER frame_parser)
{
	AAC_PARSER *p_parser = (AAC_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->b_init )
	{
		return 0;
	}

	p_parser->b_audio_start = 0;
	p_parser->b_get_audio = 0;
	aac_adts_init(&p_parser->audio1);
	aac_adts_init(&p_parser->audio2);

	p_parser->p_audio_buf = (uint8*)malloc( AAC_FRAME_PARSER_MAX_BUF_SIZE );
	if( p_parser->p_audio_buf == NULL )
	{
		aac_frame_parser_deinit(p_parser);
		return -2;
	}
	p_parser->abuf_total_size = AAC_FRAME_PARSER_MAX_BUF_SIZE;
	p_parser->abuf_use_size = 0;
	p_parser->abuf_left_size = AAC_FRAME_PARSER_MAX_BUF_SIZE;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->audio_header_size = 0;

	p_parser->b_init = 1;

	return 0;
}

int32 aac_frame_parser_deinit(AAC_FRAME_PARSER frame_parser)
{
	AAC_PARSER *p_parser = (AAC_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( p_parser->p_audio_buf )
	{
		free(p_parser->p_audio_buf);
		p_parser->p_audio_buf = NULL;
	}

	p_parser->b_init = 0;

	return 0;
}

int32 aac_frame_parser_reset(AAC_FRAME_PARSER frame_parser)
{
	AAC_PARSER *p_parser = (AAC_PARSER*)frame_parser;

	if( p_parser == NULL )
	{
		return -1;
	}

	if( !p_parser->b_init )
	{
		return -3;
	}

	p_parser->b_audio_start = 0;
	p_parser->b_get_audio = 0;
	aac_adts_init(&p_parser->audio1);
	aac_adts_init(&p_parser->audio2);

	p_parser->abuf_total_size = AAC_FRAME_PARSER_MAX_BUF_SIZE;
	p_parser->abuf_use_size = 0;
	p_parser->abuf_left_size = AAC_FRAME_PARSER_MAX_BUF_SIZE;
	p_parser->abuf_frame_size = 0;
	p_parser->abuf_parse_size = 0;
	p_parser->abuf_unparse_size = 0;
	p_parser->audio_header_size = 0;

	return 0;
}

int32 aac_frame_parse(AAC_FRAME_PARSER frame_parser, MEDIA_FRAME *p_frame, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	AAC_PARSER *p_parser = (AAC_PARSER*)frame_parser;

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
		return -1;
	}

	if( p_parser->b_get_audio )
	{
		aac_update_audio(p_parser);
		p_parser->b_get_audio = 0;
	}

	while( !b_get_frame )
	{
		ret = aac_parse_audio(p_parser, p_frame, p_parse_buf, unparse_len, &parse_len);
		p_parse_buf += parse_len;
		total_parse_len += parse_len;
		unparse_len -= parse_len;

		if( ret )
		{
			break;
		}

		if( p_parser->b_audio_start )
		{
			/** 提交当前音频帧 */
			b_get_frame = 1;
			aac_submit_audio(p_parser, p_frame);

		}else
		{
			aac_update_audio(p_parser);
		}

		p_parser->b_audio_start = 1;
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
