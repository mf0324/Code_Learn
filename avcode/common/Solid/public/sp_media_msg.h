/**
 * @file sp_media_msg.h  SP Media Msg
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.03.09
 *
 *
 */

#ifndef __SP_MEDIA_MSG_H__
#define __SP_MEDIA_MSG_H__

/**
 * @name media msg
 *
 * @{
 */

/** base */
#define SP_MEDIA_MSG_BASE                                  (0x0)
/** make media msg */
#define SP_MAKE_MEDIA_MSG(value)                           (SP_MEDIA_MSG_BASE + value)

/** exit */
#define SP_MEDIA_MSG_EXIT                                  SP_MAKE_MEDIA_MSG(1)
/** play */
#define SP_MEDIA_MSG_PLAY                                  SP_MAKE_MEDIA_MSG(100)
/** pause */
#define SP_MEDIA_MSG_PAUSE                                 SP_MAKE_MEDIA_MSG(101)
/** stop */
#define SP_MEDIA_MSG_STOP                                  SP_MAKE_MEDIA_MSG(102)
/** reset */
#define SP_MEDIA_MSG_RESET                                 SP_MAKE_MEDIA_MSG(103)
/** set play mode */
#define SP_MEDIA_MSG_SET_PLAY_MODE                         SP_MAKE_MEDIA_MSG(130)
/** set sync mode */
#define SP_MEDIA_MSG_SET_SYNC_MODE                         SP_MAKE_MEDIA_MSG(131)
/** set sync param */
#define SP_MEDIA_MSG_SET_SYNC_PARAM                        SP_MAKE_MEDIA_MSG(132)
/** set main wnd */
#define SP_MEDIA_MSG_SET_MAIN_WND                          SP_MAKE_MEDIA_MSG(140)
/** set tolerance strategy */
#define SP_MEDIA_MSG_SET_TOLERANCE_STRATEGY                SP_MAKE_MEDIA_MSG(141)
/** set audio specific config */
#define SP_MEDIA_MSG_SET_AUDIO_SPEC_CONFIG                 SP_MAKE_MEDIA_MSG(160)

/** enable video decode */
#define SP_MEDIA_MSG_ENABLE_VIDEO_DECODE                   SP_MAKE_MEDIA_MSG(200)
/** set video decode callback */
#define SP_MEDIA_MSG_SET_VIDEO_DECODE_CALLBACK             SP_MAKE_MEDIA_MSG(201)

/** enable video render */
#define SP_MEDIA_MSG_ENABLE_VIDEO_RENDER                   SP_MAKE_MEDIA_MSG(300)
/** set video render callback */
#define SP_MEDIA_MSG_SET_VIDEO_RENDER_CALLBACK             SP_MAKE_MEDIA_MSG(301)
/** video sync */
#define SP_MEDIA_MSG_VIDEO_SYNC                            SP_MAKE_MEDIA_MSG(302)
/** set src wnd mode */
#define SP_MEDIA_MSG_SET_SRC_WND_MODE                      SP_MAKE_MEDIA_MSG(303)
/** set dest wnd mode */
#define SP_MEDIA_MSG_SET_DEST_WND_MODE                     SP_MAKE_MEDIA_MSG(304)

/** enable audio decode */
#define SP_MEDIA_MSG_ENABLE_AUDIO_DECODE                   SP_MAKE_MEDIA_MSG(400)
/** set audio render callback */
#define SP_MEDIA_MSG_SET_AUDIO_RENDER_CALLBACK             SP_MAKE_MEDIA_MSG(401)
/** audio sync */
#define SP_MEDIA_MSG_AUDIO_SYNC                            SP_MAKE_MEDIA_MSG(402)
/** set audio decoder callback */
#define SP_MEDIA_MSG_SET_AUDIO_DECODER_CALLBACK            SP_MAKE_MEDIA_MSG(403)


/**
 * @}
 */

#endif ///__SP_MEDIA_MSG_H__
