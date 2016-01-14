/**
 * @file mpeg2_frame_param.h   MPEG2 Frame Param
 * @author ŒÃ—Â <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.09.01
 *
 *
 */


#ifndef __MPEG2_FRAME_PARAM_H__
#define __MPEG2_FRAME_PARAM_H__

#include <public/gen_int.h>

/**  ”∆µ */
#define MPEG2_FRAME_VIDEO                          (1)
/** “Ù∆µ */
#define MPEG2_FRAME_AUDIO                          (2)

/** ÷°≤Œ ˝ */
typedef struct tag_mpeg2_frame_param
{
	/** frame type */
	int32  frame_type;
	/** frame flag */
	uint32 frame_flag;

	/** frame data */
	uint8* p_frame_data;
	/** frame size */
	int32  frame_size;

	/** frame_ts(low 32 bit) */
	uint32 frame_ts_low;
	/** frame_ts(high 32 bit) */
	uint32 frame_ts_high;

} MPEG2_FRAME_PARAM, *PMPEG2_FRAME_PARAM;


#endif ///__MPEG2_FRAME_PARAM_H__
