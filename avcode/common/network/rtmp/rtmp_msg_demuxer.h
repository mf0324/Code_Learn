/**
 * @file rtmp_msg_demuxer.h   RTMP Msg Demuxer
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.27
 *
 *
 */

#ifndef __RTMP_MSG_DEMUXER_H__
#define __RTMP_MSG_DEMUXER_H__


#include <public/gen_int.h>


#include "rtmp_public.h"

/** rtmp msg demuxer handle */
typedef void*  RTMP_MSG_DEMUXER_H;


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
RTMP_MSG_DEMUXER_H rtmp_msg_demuxer_create();

/**  
 * @brief destroy
 * @param [in] h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_msg_demuxer_destroy(RTMP_MSG_DEMUXER_H h);

/**  
 * @brief init
 * @param [in] h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_msg_demuxer_init(RTMP_MSG_DEMUXER_H h);

/**  
 * @brief deinit
 * @param [in] h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_msg_demuxer_deinit(RTMP_MSG_DEMUXER_H  h);

/**  
 * @brief reset
 * @param [in] h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_msg_demuxer_reset(RTMP_MSG_DEMUXER_H  h);

/**  
 * @brief set chunk size
 * @param [in] h, demuxer handle
 * @param [in] chunk_size chunk size
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_msg_demuxer_set_chunk_size(RTMP_MSG_DEMUXER_H h, uint32 chunk_size);

/**  
 * @brief parse msg
 * @param [in] h, demuxer handle
 * @param [out] p_msg, msg
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_parse_size, actual parse size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_parse_size would always update
 */
int32 rtmp_parse_msg(RTMP_MSG_DEMUXER_H h, RTMP_MSG* p_msg, uint8* p_data, uint32 data_size, uint32* p_parse_size);



#ifdef __cplusplus
}
#endif

#endif ///__RTMP_MSG_DEMUXER_H__

