/**
 * @file rtmp_handshake_demux.h  RTMP Handshake Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.19
 *
 *
 */

#ifndef __RTMP_HANDSHAKE_DEMUX_H__
#define __RTMP_HANDSHAKE_DEMUX_H__

#include <public/gen_int.h>
#include "rtmp_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief demux c1
 * @param [out] p_c1, c1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
int32 rtmp_demux_c1(RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size);

/**  
 * @brief demux c2
 * @param [out] p_c2, c2
 * @param [in] p_s1, s1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
int32 rtmp_demux_c2(RTMP_C2* p_c2, RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size);

/**  
 * @brief demux s1
 * @param [out] p_s1, s1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
int32 rtmp_demux_s1(RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size);

/**  
 * @brief demux s2
 * @param [out] p_s2, s2
 * @param [in] p_c1, c1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
int32 rtmp_demux_s2(RTMP_S2* p_s2, RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__RTMP_HANDSHAKE_DEMUX_H__
