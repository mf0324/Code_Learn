/**
 * @file rtmp_handshake_mux.h  RTMP Handshake Mux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.16
 *
 *
 */

#ifndef __RTMP_HANDSHAKE_MUX_H__
#define __RTMP_HANDSHAKE_MUX_H__

#include <public/gen_int.h>
#include "rtmp_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief mux c1
 * @param [in] p_c1, c1
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, total input buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill following member:
 *   time
 *   version
 */
int32 rtmp_mux_c1(RTMP_C1* p_c1, uint8* p_buf, int32* p_buf_size);

/**  
 * @brief mux c2
 * @param [in] p_c2, c2
 * @param [in] p_s1, s1
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, total input buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill s1 following member:
 *   digest.digest
 */
int32 rtmp_mux_c2(RTMP_C2* p_c2, RTMP_S1* p_s1, uint8* p_buf, int32* p_buf_size);

/**  
 * @brief mux s1
 * @param [in] p_s1, s1
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, total input buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill following member:
 *   time
 *   version
 */
int32 rtmp_mux_s1(RTMP_S1* p_s1, uint8* p_buf, int32* p_buf_size);

/**  
 * @brief mux s2
 * @param [in] p_s2, s2
 * @param [in] p_c1, c1
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, total input buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill c1 following member:
 *   digest.digest
 */
int32 rtmp_mux_s2(RTMP_S2* p_s2, RTMP_C1* p_c1, uint8* p_buf, int32* p_buf_size);

#ifdef __cplusplus
}
#endif

#endif ///__RTMP_HANDSHAKE_MUX_H__
