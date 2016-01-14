/**
 * @file mp4_file_muxer.h   MP4 File Muxer
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.12
 *
 *
 */

#ifndef __MP4_FILE_MUXER_H__
#define __MP4_FILE_MUXER_H__

#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <Solid/public/sp_media_frame.h>

#include "mp4_file_muxer_callback.h"

/** MP4 File Muxer Handle */
typedef void*  MP4_FILE_MUXER_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not NULL: success
 * NULL: fail
 */
MP4_FILE_MUXER_H mp4_file_muxer_create();

/**  
 * @brief destroy
 * @param [in] muxer_h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_destroy(MP4_FILE_MUXER_H muxer_h);

/**  
 * @brief init
 * @param [in] muxer_h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_init(MP4_FILE_MUXER_H muxer_h);

/**  
 * @brief deinit
 * @param [in] muxer_h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_deinit(MP4_FILE_MUXER_H muxer_h);

/**  
 * @brief reset
 * @param [in] muxer_h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_reset(MP4_FILE_MUXER_H muxer_h);

/**  
 * @brief set file type
 * @param [in] muxer_h, muxer handle
 * @param [in] file_type, file type
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_file_type(MP4_FILE_MUXER_H muxer_h, int32 file_type);

/**  
 * @brief set callback
 * @param [in] muxer_h, muxer handle
 * @param [in] p_callback, callback
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_callback(MP4_FILE_MUXER_H muxer_h, MP4_FILE_MUXER_CALLBACK* p_callback);

/**  
 * @brief enable video
 * @param [in] muxer_h, muxer handle
 * @param [in] b_enable, enable or not
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_enable_video(MP4_FILE_MUXER_H muxer_h, int32 b_enable);

/**  
 * @brief enable audio
 * @param [in] muxer_h, muxer handle
 * @param [in] b_enable, enable or not
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_enable_audio(MP4_FILE_MUXER_H muxer_h, int32 b_enable);

/**  
 * @brief set reserve audio
 * @param [in] muxer_h, muxer handle
 * @param [in] b_reserve, reserve or not
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_reserve_audio(MP4_FILE_MUXER_H muxer_h, int32 b_reserve);

/**  
 * @brief set force sync
 * @param [in] muxer_h, muxer handle
 * @param [in] b_force_sync, force sync or not
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_force_sync(MP4_FILE_MUXER_H muxer_h, int32 b_force_sync);

/**  
 * @brief set time scale
 * @param [in] muxer_h, muxer handle
 * @param [in] time_scale, time scale
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_time_scale(MP4_FILE_MUXER_H muxer_h, uint32 time_scale);

/**  
 * @brief set avc dec config
 * @param [in] muxer_h, muxer handle
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_muxer_set_avc_dec_config(MP4_FILE_MUXER_H muxer_h, uint8* p_data, int32 data_size);

/**  
 * @brief startA
 * @param [in] muxer_h, muxer handle
 * @param [in] p_file_path, file path
 * @param [in] file_path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_muxer_startA(MP4_FILE_MUXER_H muxer_h, int8* p_file_path, int32 file_path_size);

/**  
 * @brief startW
 * @param [in] muxer_h, muxer handle
 * @param [in] p_file_path, file path
 * @param [in] file_path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_muxer_startW(MP4_FILE_MUXER_H muxer_h, int16* p_file_path, int32 file_path_size);

/**  
 * @brief stop
 * @param [in] muxer_h, muxer handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_muxer_stop(MP4_FILE_MUXER_H muxer_h);

/**  
 * @brief input frame
 * @param [in] muxer_h, muxer handle
 * @param [in] p_frame, frame
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_muxer_input_frame(MP4_FILE_MUXER_H muxer_h, SP_MEDIA_FRAME* p_frame);


#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define mp4_file_muxer_start    mp4_file_muxer_startW

#else

#define mp4_file_muxer_start    mp4_file_muxer_startA

#endif//unicode

#else

/** linux */
#define mp4_file_muxer_start    mp4_file_muxer_startA

#endif//if RUN_OS_WINDOWS



#ifdef __cplusplus
}
#endif

#endif ///__MP4_FILE_MUXER_H__
