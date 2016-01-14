/**
 * @file gen_graphic.h  General Graphic
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.09
 *
 *
 */

#ifndef __GEN_GRAPHIC_H__
#define __GEN_GRAPHIC_H__

#include <public/gen_int.h>

/** point */
typedef struct tag_gen_point
{
	/** x */
	int32  x;
	/** y */
	int32  y;

} GEN_POINT, *PGEN_POINT;

/** point64 */
typedef struct tag_gen_point64
{
	/** x */
	int64  x;
	/** y */
	int64  y;

} GEN_POINT64, *PGEN_POINT64;

/** 3d point */
typedef struct tag_gen_3d_point
{
	/** x */
	int32  x;
	/** y */
	int32  y;
	/** z */
	int32  z;

} GEN_3D_POINT, *PGEN_3D_POINT;

/** 3d point64 */
typedef struct tag_gen_3d_point64
{
	/** x */
	int64  x;
	/** y */
	int64  y;
	/** z */
	int64  z;

} GEN_3D_POINT64, *PGEN_3D_POINT64;

/** rect */
typedef struct tag_gen_rect
{
	/** left */
	int32  left;
	/** top */
	int32  top;
	/** right */
	int32  right;
	/** bottom */
	int32  bottom;

} GEN_RECT, *PGEN_RECT;

/** rect64 */
typedef struct tag_gen_rect64
{
	/** left */
	int64  left;
	/** top */
	int64  top;
	/** right */
	int64  right;
	/** bottom */
	int64  bottom;

} GEN_RECT64, *PGEN_RECT64;



#endif ///__GEN_GRAPHIC_H__
