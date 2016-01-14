/**
 * @file sp_audio_frame.h  SP Audio Frame
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.10
 *
 *
 */

#ifndef __SP_AUDIO_FRAME_H__
#define __SP_AUDIO_FRAME_H__

#include <public/gen_int.h>


/** audio param */
typedef struct tag_sp_audio_param
{
	/** chn */
	int32  chn;
	/** sample bit */
	int32  sample_bit;
	/** sample rate */
	int32  sample_rate;

} SP_AUDIO_PARAM, *PSP_AUDIO_PARAM;

/** audio frame */
typedef struct tag_sp_audio_frame
{
	/** format */
	int32   format;
	/** fourcc */
	uint32  fourcc;

	/** flag */
	uint32  flag;
	/** resv */
	uint32  resv;

	/** timescale */
	uint32  timescale;
	/** duration, unit: timescale */
	int32   duration;
	/** ts, unit: timescale */
	int64   ts;

	/** param */
	SP_AUDIO_PARAM param;
	/** sample count */
	int32   sample_count;

	/** data */
	uint8*  p_data;
	/** data size */
	int32   data_size;

} SP_AUDIO_FRAME, *PSP_AUDIO_FRAME;


#endif ///__SP_AUDIO_FRAME_H__
