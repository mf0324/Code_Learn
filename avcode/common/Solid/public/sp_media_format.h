/**
 * @file sp_media_format.h  SP Media Format
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.10
 *
 *
 */

#ifndef __SP_MEDIA_FORMAT_H__
#define __SP_MEDIA_FORMAT_H__

/**
 * @name play mode
 *
 * @{
 */

/** unknown mode */
#define SP_MEDIA_MODE_UNKNOWN                              (0)
/** live */
#define SP_MEDIA_MODE_LIVE                                 (1)
/** replay */
#define SP_MEDIA_MODE_REPLAY                               (2)
/** replay fast */
#define SP_MEDIA_MODE_REPLAY_FAST                          (3)
/** replay reverse */
#define SP_MEDIA_MODE_REPLAY_REVERSE                       (4)

/**
 * @}
 */

/**
 * @name tolerance strategy
 *
 * @{
 */

/** pass through */
#define SP_MEDIA_STRATEGY_PASS_THROUGH                     (0)
/** discard */
#define SP_MEDIA_STRATEGY_DISCARD                          (1)

/**
 * @}
 */


/**
 * @name media format
 *
 * @{
 */

/** unknown */
#define SP_MEDIA_FORMAT_UNKNOWN                            (0)
/** Video */
#define SP_MEDIA_FORMAT_VIDEO                              (1)
/** Audio */
#define SP_MEDIA_FORMAT_AUDIO                              (2)
/** TS */
#define SP_MEDIA_FORMAT_TS                                 (3)
/** PS */
#define SP_MEDIA_FORMAT_PS                                 (4)
/** custom */
#define SP_MEDIA_FORMAT_CUSTOM                             (0xFFFF)

/**
 * @}
 */

/**
 * @name media flag
 *
 * @{
 */

/** error */
#define SP_MEDIA_FLAG_ERROR                                (0x1)
/** segment frame */
#define SP_MEDIA_FLAG_SEG_FRAME                            (0x2)
/** timestamp */
#define SP_MEDIA_FLAG_TIMESTAMP                            (0x4)


/**
 * @}
 */


#endif /// __SP_MEDIA_FORMAT_H__
