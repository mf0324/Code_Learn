/**
 * @file sp_media_frame.h  SP Media Frame
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.10
 *
 *
 */

#ifndef __SP_MEDIA_FRAME_H__
#define __SP_MEDIA_FRAME_H__

#include <public/gen_int.h>
#include <Solid/public/sp_video_frame.h>
#include <Solid/public/sp_audio_frame.h>

/** media frame */
typedef struct tag_sp_media_frame
{
	/** id */
	int32   id;

	/** format */
	int32   format;
	/** flag */
	uint32  flag;
	/** flag2 */
	uint32  flag2;
	/** resv */
	uint32  resv;

	/** timescale */
	uint32  timescale;
	/** ts, unit: timescale */
	int64   ts;
	/** dts, unit: timescale */
	int64   dts;

	/** data */
	uint8*  p_data;
	/** data size */
	int32   data_size;

	/** payload */
	union
	{
		/** video */
		SP_VIDEO_FRAME video;

		/** audio */
		SP_AUDIO_FRAME audio;

	} payload;

	/** custom data */
	void*  p_custom_data;
	/** custom data size */
	uint32 custom_data_size;

} SP_MEDIA_FRAME, *PSP_MEDIA_FRAME;


#endif /// __SP_MEDIA_FRAME_H__
