/**
 * @file sp_audio_render_callback.h  SP Audio Render Callback
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.05.03
 *
 *
 */

#ifndef __SP_AUDIO_RENDER_CALLBACK_H__
#define __SP_AUDIO_RENDER_CALLBACK_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

/**  
 * @brief pre render notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*sp_audio_pre_render_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);

/**  
 * @brief post render notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*sp_audio_post_render_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);


/** audio render callback */
typedef struct tag_sp_audio_render_callback
{
	/**	pre render */
	sp_audio_pre_render_notify pre_render;
	/**	post render */
	sp_audio_post_render_notify post_render;

	/** custom param */
	void* p_custom_param;

} SP_AUDIO_RENDER_CALLBACK, *PSP_AUDIO_RENDER_CALLBACK;


#endif /// __SP_AUDIO_RENDER_CALLBACK_H__
