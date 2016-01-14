/**
 * @file rtmp_mux.h  RTMP Mux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.20
 *
 *
 */


#ifndef __RTMP_MUX_H__
#define __RTMP_MUX_H__

#include <public/gen_int.h>
#include "rtmp_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief calc chunk header
 * @param [in] p_header, chunk header
 * @param [out] p_size, actual require size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. caller must fill following param:
 *   format
 *   chunk_stream_id
 *   b_ext_ts
 */
int32 rtmp_calc_chunk_header_size(RTMP_CHUNK_HEADER* p_header, uint32* p_size);

/**  
 * @brief mux chunk header
 * @param [in] p_header, chunk header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill following param:
 *   format
 *   chunk_stream_id
 *   msg_header
 *   b_ext_ts
 *   ext_ts
 */
int32 rtmp_mux_chunk_header(RTMP_CHUNK_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief calc msg size
 * @param [in] p_header, chunk header
 * @param [in] p_payload, chunk payload
 * @param [out] p_buf_size, actual require size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. caller must fill following param:
 *   p_header
 *   p_payload
 */
int32 rtmp_calc_msg_size(RTMP_CHUNK_HEADER* p_header, RTMP_CHUNK_PAYLOAD* p_payload, uint32 chunk_size, uint32* p_size);

/**  
 * @brief mux chunk msg
 * @param [in] p_header, chunk header
 * @param [in] p_payload, chunk payload
 * @param [in] chunk_size, chunk size
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means require size
 * 2. caller must fill following param:
 *   p_header
 *   p_payload
 */
int32 rtmp_mux_msg(RTMP_CHUNK_HEADER* p_header, RTMP_CHUNK_PAYLOAD* p_payload, uint32 chunk_size, uint8* p_buf, uint32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__RTMP_MUX_H__
