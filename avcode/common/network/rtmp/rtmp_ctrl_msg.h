/**
 * @file rtmp_ctrl_msg.h  RTMP Control Msg
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.07.14
 *
 *
 */


#ifndef __RTMP_CTRL_MSG_H__
#define __RTMP_CTRL_MSG_H__

#include <public/gen_int.h>
#include "rtmp_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief set chunk size
 * @param [in] chunk_size, chunk size
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 */
int32 rtmp_ctrl_mux_set_chunk_size(uint32 chunk_size, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief window acknowledgement size
 * @param [in] wnd_ack_size, window ack size
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means require size
 */
int32 rtmp_ctrl_mux_wnd_ack_size(uint32 wnd_ack_size, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief set peer bandwidth
 * @param [in] wnd_ack_size, window ack size
 * @param [in] limit_type, limite_type
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means require size
 */
int32 rtmp_ctrl_mux_set_peer_bandwidth(uint32 wnd_ack_size, int32 limit_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief mux event
 * @param [in] event_type, event type
 * @param [in] event_data, event data
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means require size
 */
int32 rtmp_ctrl_mux_event(uint32 event_type, uint32 event_data, uint8* p_buf, uint32* p_buf_size);



#ifdef __cplusplus
}
#endif

#endif ///__RTMP_CTRL_MSG_H__
