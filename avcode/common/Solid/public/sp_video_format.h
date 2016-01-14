/**
 * @file sp_video_format.h  SP Video Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.09
 *
 *
 */

#ifndef __SP_VIDEO_FORMAT_H__
#define __SP_VIDEO_FORMAT_H__


/**
 * @name video format
 *
 * @{
 */

/** Unknown */
#define SP_VIDEO_FORMAT_UNKNOWN                            (0)
/** RGB24 */
#define SP_VIDEO_FORMAT_RGB24                              (1)
/** BGR24 */
#define SP_VIDEO_FORMAT_BGR24                              (2)
/** RGB32 */
#define SP_VIDEO_FORMAT_RGB32                              (3)
/** BGR32 */
#define SP_VIDEO_FORMAT_BGR32                              (4)
/** ARGB32 */
#define SP_VIDEO_FORMAT_ARGB32                             (5)
/** ABGR32 */
#define SP_VIDEO_FORMAT_ABGR32                             (6)
/** RGBA32 */
#define SP_VIDEO_FORMAT_RGBA32                             (7)
/** BGRA32 */
#define SP_VIDEO_FORMAT_BGRA32                             (8)
/** RGB555 */
#define SP_VIDEO_FORMAT_RGB555                             (9)
/** RGB565 */
#define SP_VIDEO_FORMAT_RGB565                             (10)

/** YUYV */
#define SP_VIDEO_FORMAT_YUYV                               (101)
/** YUY2 */
#define SP_VIDEO_FORMAT_YUY2                               (102)
/** UYVY */
#define SP_VIDEO_FORMAT_UYVY                               (103)
/** I420 */
#define SP_VIDEO_FORMAT_I420                               (104)
/** YV12 */
#define SP_VIDEO_FORMAT_YV12                               (105)
/** YVU9 */
#define SP_VIDEO_FORMAT_YVU9                               (109)

/** JPEG */
#define SP_VIDEO_FORMAT_JPEG                               (1000)
/** JPEG2000 */
#define SP_VIDEO_FORMAT_JPEG2000                           (1001)
/** MJPEG */
#define SP_VIDEO_FORMAT_MJPEG                              (1002)
/** PNG */
#define SP_VIDEO_FORMAT_PNG                                (1005)
/** MPEG1 */
#define SP_VIDEO_FORMAT_MPEG1                              (1101)
/** MPEG2 */
#define SP_VIDEO_FORMAT_MPEG2                              (1102)
/** MPEG4 */
#define SP_VIDEO_FORMAT_MPEG4                              (1104)
/** H264 */
#define SP_VIDEO_FORMAT_H264                               (1110)
/** H264 MP4 */
#define SP_VIDEO_FORMAT_H264_MP4                           (1111)


/** RGB16 avg pixel bit */
#define SP_VIDEO_RGB16_AVG_PIXEL_BIT                       (16)
/** RGB16 avg pixel byte */
#define SP_VIDEO_RGB16_AVG_PIXEL_BYTE                      (SP_VIDEO_RGB16_AVG_PIXEL_BIT / 8)
/** RGB24 avg pixel bit */
#define SP_VIDEO_RGB24_AVG_PIXEL_BIT                       (24)
/** RGB24 avg pixel byte */
#define SP_VIDEO_RGB24_AVG_PIXEL_BYTE                      (SP_VIDEO_RGB24_AVG_PIXEL_BIT / 8)
/** RGB32 avg pixel bit */
#define SP_VIDEO_RGB32_AVG_PIXEL_BIT                       (32)
/** RGB32 avg pixel byte */
#define SP_VIDEO_RGB32_AVG_PIXEL_BYTE                      (SP_VIDEO_RGB32_AVG_PIXEL_BIT / 8)
/** YUYV avg pixel bit */
#define SP_VIDEO_YUYV_AVG_PIXEL_BIT                        (16)
/** YUYV avg pixel byte */
#define SP_VIDEO_YUYV_AVG_PIXEL_BYTE                       (SP_VIDEO_YUYV_AVG_PIXEL_BIT / 8)
/** UYVY avg pixel bit */
#define SP_VIDEO_UYVY_AVG_PIXEL_BIT                        (16)
/** UYVY avg pixel byte */
#define SP_VIDEO_UYVY_AVG_PIXEL_BYTE                       (SP_VIDEO_UYVY_AVG_PIXEL_BIT / 8)
/** YV12 avg pixel bit */
#define SP_VIDEO_YV12_AVG_PIXEL_BIT                        (12)

/**
 * @}
 */



/** 
 * @name video frame type
 * @{
 */

/** Invalid frame */
#define SP_VIDEO_INVALID_FRAME                             (0)
/** I frame */
#define SP_VIDEO_I_FRAME                                   (1)
/** P frame */
#define SP_VIDEO_P_FRAME                                   (2)
/** B frame */
#define SP_VIDEO_B_FRAME                                   (3)
/** Sprite frame */
#define SP_VIDEO_SPRITE_FRAME                              (4)

/** 
 * @}
 */

/**
 * @name video flag
 * @remark
 * 1. bit meaning as follow:
 * bit0: 0 -- field, 1 -- frame
 * bit1: 0 -- interlace, 1 -- progressive
 * bit2: 0 -- odd, 1 -- even
 * bit3: 0 -- top, 1 -- bottom
 * bit4: 0 -- top down, 1 -- bottom up
 * bit5: 0 -- ctts invalid, 1 -- ctts valid
 * bit6: 0 -- sd video, 1 -- hd video
 * bit7: 0 -- not modify, 1 -- modify
 * bit8 - bit31: resv
 *
 * @{
 */

/** frame */
#define SP_VIDEO_FLAG_FRAME                                (0x1)
/** progressive */
#define SP_VIDEO_FLAG_PROGRESSIVE                          (0x2)
/** even */
#define SP_VIDEO_FLAG_EVEN                                 (0x4)
/** bottom */
#define SP_VIDEO_FLAG_BOTTOM                               (0x8)
/** bottom up */
#define SP_VIDEO_FLAG_BOTTOM_UP                            (0x10)
/** ctts */
#define SP_VIDEO_FLAG_CTTS                                 (0x20)
/** hd */
#define SP_VIDEO_FLAG_HD_VIDEO                             (0x40)
/** modify */
#define SP_VIDEO_FLAG_MODIFY                               (0x80)

/**
 * @}
 */

#endif ///__SP_VIDEO_FORMAT_H__
