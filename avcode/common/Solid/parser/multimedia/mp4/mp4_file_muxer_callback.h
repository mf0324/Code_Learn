/**
 * @file mp4_file_muxer_callback.h  MP4 File Muxer Callback
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.05.30
 *
 *
 */

#ifndef __MP4_FILE_MUXER_CALLBACK_H__
#define __MP4_FILE_MUXER_CALLBACK_H__

#include <public/gen_int.h>
//#include <public/sp_media_frame.h>

/**  
 * @brief meta data notify
 * @param [in] p_data, meta data
 * @param [in] data_size, meta data size
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*mp4_file_muxer_meta_data_notify)(uint8* p_data, uint32 data_size, void* p_custom_param);

/**  
 * @brief moof data notify
 * @param [in] p_data, moof data
 * @param [in] data_size, moof data size
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*mp4_file_muxer_moof_data_notify)(uint8* p_data, uint32 data_size, void* p_custom_param);

/**  
 * @brief frag data notify
 * @param [in] p_data, frag data
 * @param [in] data_size, frag data size
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*mp4_file_muxer_frag_data_notify)(uint8* p_data, uint32 data_size, void* p_custom_param);


/** mp4 file muxer callback */
typedef struct tag_mp4_file_muxer_callback
{
	/**	meta data notify */
	mp4_file_muxer_meta_data_notify meta_data;
	/**	moof data */
	mp4_file_muxer_moof_data_notify moof_data;
	/**	video data */
	mp4_file_muxer_frag_data_notify video_data;
	/**	audio data */
	mp4_file_muxer_frag_data_notify audio_data;

	/** custom param */
	void* p_custom_param;

} MP4_FILE_MUXER_CALLBACK, *PMP4_FILE_MUXER_CALLBACK;


#endif /// __MP4_FILE_MUXER_CALLBACK_H__
