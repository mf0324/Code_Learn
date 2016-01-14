/**
 * @file h264_nalu_demux.h   H264 Nalu Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.19
 *
 *
 */

#ifndef __H264_NALU_DEMUX_H__
#define __H264_NALU_DEMUX_H__


#include "h264_nalu_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief init nalu pack
 * @param [in] p_pack, nalu pack
 * @return
 * 0: success
 * other: fail
 */
int32 h264_nalu_pack_init(H264_NALU_PACK* p_pack);

/**  
 * @brief find header
 * @param [out] p_nalu, nalu
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size would always update
 */
int32 h264_nalu_find_header(H264_NALU* p_nalu, uint8* p_data, int32 data_size, int32* p_demux_size);

/**  
 * @brief demux nalu
 * @param [out] p_pack, pack
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_parse_size, actual parse size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_parse_len always update
 */
int32 h264_nalu_demux(H264_NALU_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_parse_size);

/**
 * @brief demux sps
 * @param [in] p_pack, pack
 * @param [in] p_sps, input data
 * @param [in/out] p_sps_len, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. input data should not begin from start code
 */
int32 h264_nalu_demux_sps(H264_NALU_PACK *p_pack, uint8 *p_sps, int32 *p_sps_len);

/**
 * @brief demux pps
 * @param [in] p_pack, pack
 * @param [in] p_pps, input data
 * @param [in/out] p_pps_len, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. input data should not begin from start code
 */
int32 h264_nalu_demux_pps(H264_NALU_PACK *p_pack, uint8 *p_pps, int32 *p_pps_len);


#ifdef __cplusplus
}
#endif

#endif ///__H264_NALU_DEMUX_H__
