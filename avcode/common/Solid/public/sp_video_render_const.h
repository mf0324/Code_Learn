/**
 * @file sp_video_render_const.h  SP Video Render Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.24
 *
 *
 */

#ifndef __SP_VIDEO_RENDER_CONST_H__
#define __SP_VIDEO_RENDER_CONST_H__

/**
 * @name Video Render Type
 *
 * @{
 */

/** invalid */
#define SP_VIDEO_RENDER_TYPE_INVALID                       (0)
/** ddraw7 */
#define SP_VIDEO_RENDER_TYPE_DDRAW7                        (1)
/** d3d9 */
#define SP_VIDEO_RENDER_TYPE_D3D9                          (2)

/**
 * @}
 */

/**
 * @name DDraw Capability
 *
 * @{
 */

/** yuv blt */
#define SP_VIDEO_RENDER_DDRAW_CAP_YUV_BLT                  (0x1)


/**
 * @}
 */


#endif ///__SP_VIDEO_RENDER_CONST_H__
