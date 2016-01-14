/**
 * @file sp_video_render_public.h  SP Video Render Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.16
 *
 *
 */

#ifndef __SP_VIDEO_RENDER_PUBLIC_H__
#define __SP_VIDEO_RENDER_PUBLIC_H__

#include <public/gen_int.h>

/** ddraw render capability */
typedef struct tag_ddraw_render_cap
{
	/**	capability */
	uint32 cap;
	/**	capability2 */
	uint32 cap2;

	/** yuv capability */
	uint32* p_yuv_cap;
	/** yuv capability count */
	int32   yuv_cap_count;

} DDRAW_RENDER_CAP, *PDDRAW_RENDER_CAP;

/** d3d9 render capability */
typedef struct tag_d3d9_render_cap
{
	/**	capability */
	uint32 cap;
	/**	capability2 */
	uint32 cap2;

} D3D9_RENDER_CAP, *PD3D9_RENDER_CAP;

/** video render capability */
typedef struct tag_video_render_cap
{
	/**	render type */
	int32  render_type;

	/** render */
	union
	{
		/** ddraw */
		DDRAW_RENDER_CAP ddraw;

		/** d3d9 */
		D3D9_RENDER_CAP d3d9;

	} render;

} VIDEO_RENDER_CAP, *PVIDEO_RENDER_CAP;


#endif /// __SP_VIDEO_RENDER_PUBLIC_H__
