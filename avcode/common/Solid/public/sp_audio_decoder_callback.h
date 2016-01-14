/**
 * @file sp_audio_decoder_callback.h  SP Audio Decoder Callback
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.05.09
 *
 *
 */

#ifndef __SP_AUDIO_DECODER_CALLBACK_H__
#define __SP_AUDIO_DECODER_CALLBACK_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

/**  
 * @brief audio decode notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*sp_audio_decode_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);

/** audio decoder callback */
typedef struct tag_sp_audio_decoder_callback
{
	/**	decode */
	sp_audio_decode_notify  decode_notify;

	/** custom param */
	void* p_custom_param;

} SP_AUDIO_DECODER_CALLBACK, *PSP_AUDIO_DECODER_CALLBACK;


#endif /// __SP_AUDIO_DECODER_CALLBACK_H__
