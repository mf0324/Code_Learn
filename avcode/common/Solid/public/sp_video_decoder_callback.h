/**
 * @file sp_video_decoder_callback.h  SP Video Decoder Callback
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.17
 *
 *
 */

#ifndef __SP_VIDEO_DECODER_CALLBACK_H__
#define __SP_VIDEO_DECODER_CALLBACK_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

/**  
 * @brief decode notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*sp_video_decode_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);


/** video decode callback */
typedef struct tag_sp_video_decode_callback
{
	/**	decode */
	sp_video_decode_notify decode_notify;

	/** custom param */
	void* p_custom_param;

} SP_VIDEO_DECODE_CALLBACK, *PSP_VIDEO_DECODE_CALLBACK;


#endif /// __SP_VIDEO_RENDER_CALLBACK_H__
