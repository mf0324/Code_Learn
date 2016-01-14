/**
 * @file sp_video_frame.h  SP Video Frame
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.09
 *
 *
 */


#ifndef __SP_VIDEO_FRAME_H__
#define __SP_VIDEO_FRAME_H__

#include <public/gen_int.h>
#include <public/gen_graphic.h>

/** Video Frame */
typedef struct tag_sp_video_frame
{
	/** id */
	int32   id;
	/** format */
	int32   format;
	/** fourcc */
	uint32  fourcc;
	/** type */
	int32   type;

	/** width */
	int32   width;
	/** height */
	int32   height;

	/** src */
	GEN_RECT src;
	/** dest */
	GEN_RECT dest;

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

	/** img */
	union
	{
		struct
		{
			/** rgb */
			uint8*  p_rgb;
			/** rgb stride, unit: byte */
			int32   rgb_stride;

		} rgb;

		struct
		{
			/** y */
			uint8*  p_y;
			/** u */
			uint8*  p_u;
			/** v */
			uint8*  p_v;
			/** y stride, unit: byte */
			int32   y_stride;
			/** uv stride, unit: byte */
			int32   uv_stride;

		} yuv;

	} img;

	/** custom data */
	void*  p_custom_data;
	/** custom data size */
	uint32 custom_data_size;

} SP_VIDEO_FRAME, *PSP_VIDEO_FRAME;


#endif ///__SP_VIDEO_FRAME_H__
