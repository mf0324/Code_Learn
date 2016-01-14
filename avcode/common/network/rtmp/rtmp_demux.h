/**
 * @file rtmp_demux.h   RTMP Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.20
 *
 *
 */

#ifndef __RTMP_DEMUX_H__
#define __RTMP_DEMUX_H__

#include <public/gen_int.h>
#include "rtmp_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief demux s1
 * @param [out] p_s1, s1
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
//int32 rtmp_demux_s1(RTMP_S1* p_s1, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief demux s2
 * @param [out] p_s2, s2
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
//int32 rtmp_demux_s2(RTMP_S2* p_s2, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief demux chunk header
 * @param [out] p_header, chunk header
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
int32 rtmp_demux_chunk_header(RTMP_CHUNK_HEADER* p_header, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);



#ifdef __cplusplus
}
#endif

#endif ///__RTMP_DEMUX_H__
