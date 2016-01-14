/**
 * @file sp_video_signal.h  SP Video Signal
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.09
 *
 *
 */

#ifndef __SP_VIDEO_SIGNAL_H__
#define __SP_VIDEO_SIGNAL_H__


/**
 * @name video signal
 *
 * @{
 */

/** unknown */
#define SP_VIDEO_SIGNAL_UNKNOWN                            (0)
/** PAL */
#define SP_VIDEO_PAL                                       (1)
/** NTSC */
#define SP_VIDEO_NTSC                                      (2)
/** SECAM */
#define SP_VIDEO_SECAM                                     (3)
/** 720P */
#define SP_VIDEO_720P                                      (10)
/** 1080P */
#define SP_VIDEO_1080P                                     (11)

/** PAL CIF width */
#define SP_VIDEO_PAL_CIF_WIDTH                             (352)
/** PAL CIF height */
#define SP_VIDEO_PAL_CIF_HEIGHT                            (288)
/** PAL CIF pixel */
#define SP_VIDEO_PAL_CIF_PIXEL                             (SP_VIDEO_PAL_CIF_WIDTH * SP_VIDEO_PAL_CIF_HEIGHT)
/** PAL D1 width */
#define SP_VIDEO_PAL_D1_WIDTH                              (720)
/** PAL D1 height */
#define SP_VIDEO_PAL_D1_HEIGHT                             (576)
/** PAL D1 pixel */
#define SP_VIDEO_PAL_D1_PIXEL                              (SP_VIDEO_PAL_D1_WIDTH * SP_VIDEO_PAL_D1_HEIGHT)
/** NTSC CIF width */
#define SP_VIDEO_NTSC_CIF_WIDTH                            (352)
/** NTSC CIF height */
#define SP_VIDEO_NTSC_CIF_HEIGHT                           (240)
/** NTSC CIF pixel */
#define SP_VIDEO_NTSC_CIF_PIXEL                            (SP_VIDEO_NTSC_CIF_WIDTH * SP_VIDEO_NTSC_CIF_HEIGHT)
/** NTSC D1 width */
#define SP_VIDEO_NTSC_D1_WIDTH                             (720)
/** NTSC D1 height */
#define SP_VIDEO_NTSC_D1_HEIGHT                            (480)
/** NTSC D1 pixel */
#define SP_VIDEO_NTSC_D1_PIXEL                             (SP_VIDEO_NTSC_D1_WIDTH * SP_VIDEO_NTSC_D1_HEIGHT)
/** 720P width */
#define SP_VIDEO_720P_WIDTH                                (1280)
/** 720P height */
#define SP_VIDEO_720P_HEIGHT                               (720)
/** 720P pixel */
#define SP_VIDEO_720P_PIXEL                                (SP_VIDEO_720P_WIDTH * SP_VIDEO_720P_HEIGHT)
/** 1080P width */
#define SP_VIDEO_1080P_WIDTH                               (1920)
/** 1080P height */
#define SP_VIDEO_1080P_HEIGHT                              (1080)
/** 1080P pixel */
#define SP_VIDEO_1080P_PIXEL                               (SP_VIDEO_1080P_WIDTH * SP_VIDEO_1080P_HEIGHT)

/**
 * @}
 */


#endif ///__SP_VIDEO_SIGNAL_H__
