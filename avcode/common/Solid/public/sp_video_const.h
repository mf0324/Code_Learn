/**
 * @file sp_video_const.h  SP Video Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.17
 *
 *
 */

#ifndef __SP_VIDEO_CONST_H__
#define __SP_VIDEO_CONST_H__



/**
 * @name Video Render Mode
 *
 * @{
 */

/** main wnd */
#define SP_VIDEO_RENDER_MODE_MAIN_WND                      (1)
/** multi wnd */
#define SP_VIDEO_RENDER_MODE_MULTI_WND                     (2)

/**
 * @}
 */

/**
 * @name Video Wnd Mode
 *
 * @{
 */

/** origin size */
#define SP_VIDEO_WND_MODE_ORIGIN_SIZE                      (1)
/** dest size */
#define SP_VIDEO_WND_MODE_DEST_SIZE                        (2)
/** dest size with origin aspect */
#define SP_VIDEO_WND_MODE_DEST_SIZE_AND_ORIGIN_ASPECT      (3)
/** fix aspect */
#define SP_VIDEO_WND_MODE_FIX_ASPECT                       (4)
/** origin aspect fullfill back ground */
#define SP_VIDEO_WND_MODE_ORIGIN_ASPECT_FULLFILL_BACKGROUND  (5)
/** custom */
#define SP_VIDEO_WND_MODE_CUSTOM                           (0xFF)


/**
 * @}
 */

/**
 * @name 视图扫描命令
 *
 * @{
 */

/** 上 */
#define SP_VIDEO_VIEW_SCAN_UP                              (1)
/** 下 */
#define SP_VIDEO_VIEW_SCAN_DOWN                            (2)
/** 左 */
#define SP_VIDEO_VIEW_SCAN_LEFT                            (3)
/** 右 */
#define SP_VIDEO_VIEW_SCAN_RIGHT                           (4)
/** 左上 */
#define SP_VIDEO_VIEW_SCAN_LEFT_UP                         (5)
/** 左下 */
#define SP_VIDEO_VIEW_SCAN_LEFT_DOWN                       (6)
/** 右上 */
#define SP_VIDEO_VIEW_SCAN_RIGHT_UP                        (7)
/** 右下 */
#define SP_VIDEO_VIEW_SCAN_RIGHT_DOWN                      (8)
/** 放大 */
#define SP_VIDEO_VIEW_SCAN_ZOOM_IN                         (9)
/** 缩小 */
#define SP_VIDEO_VIEW_SCAN_ZOOM_OUT                        (10)

/**
 * @}
 */

/** 
 * @name Deinterlace Mode
 * @{
 */

/** not deinterlace */
#define SP_VIDEO_DEINTERLACE_MODE_NULL                     (0)
/** discard */
#define SP_VIDEO_DEINTERLACE_MODE_DISCARD                  (1)
/** blend */
#define SP_VIDEO_DEINTERLACE_MODE_BLEND                    (2)
/** bob */
#define SP_VIDEO_DEINTERLACE_MODE_BOB                      (3)

/** 
 * @}
 */


#endif ///__SP_VIDEO_CONST_H__
