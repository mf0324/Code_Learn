
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_writer.h>
#include "mpeg2_public.h"
#include "mpeg2_ps_muxer.h"
#include "mpeg2_ps_frame_muxer.h"

#include <util/log_debug.h>


/** 默认视频stream id */
#define MPEG2_PS_FRAME_MUXER_VIDEO_STREAM_ID       (0xE0)
/** 默认音频stream id */
#define MPEG2_PS_FRAME_MUXER_AUDIO_STREAM_ID       (0xC0)


/** PS帧复用器 */
typedef struct tag_ps_frame_muxer
{
	/** 初始化标志 */
	int32  b_init;

	/** 视频参数 */
	MPEG2_PS_STREAM_PARAM  video_param;
	/** 音频参数 */
	MPEG2_PS_STREAM_PARAM  audio_param;

} PS_FRAME_MUXER, *PPS_FRAME_MUXER;

/////////////////////////////////////外部接口//////////////////////////////////
MPEG2_PS_FRAME_MUXER mpeg2_ps_frame_muxer_create()
{
	PS_FRAME_MUXER *p_muxer = NULL;
	p_muxer = (PS_FRAME_MUXER*)malloc( sizeof(PS_FRAME_MUXER) );

	if( p_muxer == NULL )
	{
		return NULL;
	}

	memset(p_muxer, 0, sizeof(PS_FRAME_MUXER));

	return p_muxer;
}

int32 mpeg2_ps_frame_muxer_destroy(MPEG2_PS_FRAME_MUXER frame_muxer)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL )
	{
		return -1;
	}

	if( p_muxer->b_init )
	{
		mpeg2_ps_frame_muxer_deinit(p_muxer);
	}

	free(p_muxer);

	return 0;
}

int32 mpeg2_ps_frame_muxer_init(MPEG2_PS_FRAME_MUXER frame_muxer)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL )
	{
		return -1;
	}

	if( p_muxer->b_init )
	{
		return 0;
	}

	p_muxer->video_param.stream_id = MPEG2_PS_FRAME_MUXER_VIDEO_STREAM_ID;
	p_muxer->audio_param.stream_id = MPEG2_PS_FRAME_MUXER_AUDIO_STREAM_ID;

	p_muxer->b_init = 1;

	return 0;
}

int32 mpeg2_ps_frame_muxer_deinit(MPEG2_PS_FRAME_MUXER frame_muxer)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL )
	{
		return -1;
	}

	p_muxer->b_init = 0;

	return 0;
}

int32 mpeg2_ps_frame_muxer_reset(MPEG2_PS_FRAME_MUXER frame_muxer)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL )
	{
		return -1;
	}

	if( !p_muxer->b_init )
	{
		return -1;
	}

	return 0;
}

int32 mpeg2_ps_set_ves_param(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_STREAM_PARAM *p_param)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL || p_param == NULL )
	{
		return -1;
	}

	if( !p_muxer->b_init )
	{
		return -1;
	}

	p_muxer->video_param.stream_id = p_param->stream_id;

	return 0;
}

int32 mpeg2_ps_set_aes_param(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_STREAM_PARAM *p_param)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	if( p_muxer == NULL || p_param == NULL )
	{
		return -1;
	}

	if( !p_muxer->b_init )
	{
		return -1;
	}

	p_muxer->audio_param.stream_id = p_param->stream_id;

	return 0;
}

int32 mpeg2_ps_make_ps_header_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	int32 ret;

	if( p_muxer == NULL )
	{
		return -1;
	}

	if( !p_muxer->b_init )
	{
		return -1;
	}

	ret = mpeg2_ps_header_mux(p_param, p_buf, p_buf_size);

	return ret;
}

int32 mpeg2_ps_make_sys_header_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_SYS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	int32 ret;

	if( p_muxer == NULL )
	{
		return -1;
	}

	if( !p_muxer->b_init )
	{
		return -1;
	}

	ret = mpeg2_sys_header_mux(p_param, p_buf, p_buf_size);

	return ret;
}

int32 mpeg2_ps_make_frame_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_FRAME_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	PS_FRAME_MUXER *p_muxer = (PS_FRAME_MUXER*)frame_muxer;

	int32  ret;

	if( p_muxer == NULL || p_param == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	*p_buf_size = 0;

	return 0;
}
