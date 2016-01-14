/**
 * @file sp_render_area.h  SP Render Area
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.16
 *
 *
 */

#ifndef __SP_RENDER_AREA_H__
#define __SP_RENDER_AREA_H__

#include <public/gen_int.h>



/**
 * @name coordinate
 *
 * @{
 */

/** absolute */
#define SP_COORDINATE_ABS                                  (1)
/** relative */
#define SP_COORDINATE_RLT                                  (2)
/** relative2 */
#define SP_COORDINATE_RLT2                                 (3)

/**
 * @}
 */

/**
 * @name relative2 pos
 *
 * @{
 */

/** left top */
#define SP_RLT2_POS_LEFT_TOP                               (1)
/** middle top */
#define SP_RLT2_POS_MIDDLE_TOP                             (2)
/** right top */
#define SP_RLT2_POS_RIGHT_TOP                              (3)
/** left middle */
#define SP_RLT2_POS_LEFT_MIDDLE                            (4)
/** middle */
#define SP_RLT2_POS_MIDDLE                                 (5)
/** right middle */
#define SP_RLT2_POS_RIGHT_MIDDLE                           (6)
/** left bottom */
#define SP_RLT2_POS_LEFT_BOTTOM                            (7)
/** middle bottom */
#define SP_RLT2_POS_MIDDLE_BOTTOM                          (8)
/** right bottom */
#define SP_RLT2_POS_RIGHT_BOTTOM                           (9)

/**
 * @}
 */


/** area */
typedef struct tag_sp_render_area
{
	/** width, unit: pixel */
	int32   width;
	/** height, unit: pixel */
	int32   height;

	/** align */
	int32   align;
	/** coordinate */
	int32   coordinate;

	/** begin */
	union
	{
		/** absolute */
		struct
		{
			/** x */
			int32 x;
			/** y */
			int32 y;

		} abs;

		/** relative */
		struct
		{
			/** x */
			float x;
			/** y */
			float y;

		} rlt;

		/** relative2 */
		struct
		{
			/** x */
			int32 x;
			/** y */
			int32 y;

		} rlt2;

	} begin;

	/** end */
	union
	{
		/** absolute */
		struct
		{
			/** x */
			int32 x;
			/** y */
			int32 y;

		} abs;

		/** relative */
		struct
		{
			/** x */
			float x;
			/** y */
			float y;

		} rlt;

		/** relative2 */
		struct
		{
			/** x */
			int32 x;
			/** y */
			int32 y;

		} rlt2;

	} end;

} SP_RENDER_AREA, *PSP_RENDER_AREA;


#endif ///__SP_RENDER_AREA_H__
