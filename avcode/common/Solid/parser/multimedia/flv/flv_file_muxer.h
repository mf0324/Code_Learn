/**
 * @file flv_file_muxer.h   FLV File Muxer
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.20
 *
 *
 */

#ifndef __FLV_FILE_MUXER_H__
#define __FLV_FILE_MUXER_H__

#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <Solid/public/sp_media_frame.h>

//#include "mp4_file_muxer_callback.h"

/** FLV File Muxer Handle */
typedef void*  FLV_FILE_MUXER_H;


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
FLV_FILE_MUXER_H flv_file_muxer_create();

/**  
 * @brief destroy
 * @param [in] h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_destroy(FLV_FILE_MUXER_H h);

/**  
 * @brief init
 * @param [in] h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_init(FLV_FILE_MUXER_H h);

/**  
 * @brief deinit
 * @param [in] h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_deinit(FLV_FILE_MUXER_H h);

/**  
 * @brief reset
 * @param [in] h, muxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_reset(FLV_FILE_MUXER_H h);

/**  
 * @brief enable video
 * @param [in] h, muxer handle
 * @param [in] b_enable, enable or not
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_enable_video(FLV_FILE_MUXER_H h, int32 b_enable);

/**  
 * @brief enable audio
 * @param [in] h, muxer handle
 * @param [in] b_enable, enable or not
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_enable_audio(FLV_FILE_MUXER_H h, int32 b_enable);

/**  
 * @brief set avc dec config
 * @param [in] h, muxer handle
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 */
int32 flv_file_muxer_set_avc_dec_config(FLV_FILE_MUXER_H h, uint8* p_data, int32 data_size);

/**  
 * @brief startA
 * @param [in] h, muxer handle
 * @param [in] p_file_path, file path
 * @param [in] file_path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_file_muxer_startA(FLV_FILE_MUXER_H h, int8* p_file_path, int32 file_path_size);

/**  
 * @brief startW
 * @param [in] h, muxer handle
 * @param [in] p_file_path, file path
 * @param [in] file_path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_file_muxer_startW(FLV_FILE_MUXER_H h, int16* p_file_path, int32 file_path_size);

/**  
 * @brief stop
 * @param [in] h, muxer handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_file_muxer_stop(FLV_FILE_MUXER_H h);

/**  
 * @brief input frame
 * @param [in] h, muxer handle
 * @param [in] p_frame, frame
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_file_muxer_input_frame(FLV_FILE_MUXER_H h, SP_MEDIA_FRAME* p_frame);


#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define flv_file_muxer_start    flv_file_muxer_startW

#else

#define flv_file_muxer_start    flv_file_muxer_startA

#endif//unicode

#else

/** linux */
#define flv_file_muxer_start    flv_file_muxer_startA

#endif//if RUN_OS_WINDOWS



#ifdef __cplusplus
}
#endif

#endif ///__FLV_FILE_MUXER_H__
