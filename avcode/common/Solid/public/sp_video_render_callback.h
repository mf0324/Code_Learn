/**
 * @file sp_video_render_callback.h  SP Video Render Callback
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.16
 *
 *
 */

#ifndef __SP_VIDEO_RENDER_CALLBACK_H__
#define __SP_VIDEO_RENDER_CALLBACK_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

/**  
 * @brief pre render notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*sp_video_pre_render_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);

/**  
 * @brief post render notify
 * @param [in] p_frame, frame
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*sp_video_post_render_notify)(SP_MEDIA_FRAME* p_frame, void* p_custom_param);


/** video render callback */
typedef struct tag_sp_video_render_callback
{
	/**	pre render */
	sp_video_pre_render_notify pre_render;
	/**	post render */
	sp_video_post_render_notify post_render;

	/** custom param */
	void* p_custom_param;

} SP_VIDEO_RENDER_CALLBACK, *PSP_VIDEO_RENDER_CALLBACK;


#endif /// __SP_VIDEO_RENDER_CALLBACK_H__
