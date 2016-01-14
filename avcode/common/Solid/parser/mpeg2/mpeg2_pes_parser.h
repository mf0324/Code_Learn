/**
 * @file mpeg2_pes_parser.h  MPEG2 PES Parser
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.12.19
 *
 *
 */

#ifndef __MPEG2_PES_PARSER_H__
#define __MPEG2_PES_PARSER_H__

#include <public/gen_int.h>

#include "mpeg2_public.h"
#include "mpeg2_pes_public.h"



#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief find pes header
 * @param [out] p_pack, pes pack
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always hold actual demux size
 * 2. BE CAREFUL, p_demux_size not including pes header size(6 byte)!
 */
int32 mpeg2_pes_find_pes_header(MPEG2_PES_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_demux_size);

/**  
 * @brief demux pes header
 * @param [in] p_header, pes header
 * @param [in] p_data, input data
 * @param [in/out] p_data_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_pes_demux_pes_header(MPEG2_PES_HEADER* p_header, uint8* p_data, int32 *p_data_size);

/**  
 * @brief demux ps map
 * @param [out] p_ps_map, ps map
 * @param [in] p_data, input data
 * @param [in/out] p_data_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_pes_demux_ps_map(MPEG2_PS_MAP* p_ps_map, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux es map
 * @param [out] p_stream, es stream array
 * @param [in/out] p_stream_size, total es count/actual demux count
 * @param [in] p_data, input data
 * @param [in/out] p_data_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_pes_demux_es_map(MPEG2_ES_STREAM* p_stream, int32* p_stream_size, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux ps directory
 * @param [in] p_ps_dir, ps dir
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_len, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_demux_ps_dir(MPEG2_PS_DIR *p_ps_dir, uint8 *p_buf, int32 *p_buf_len);

/**
 * @brief demux descriptor
 * @param [out] p_descriptor, descriptor
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_len, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_demux_descriptor(MPEG2_DESCRIPTOR *p_descriptor, uint8 *p_buf, int32 *p_buf_len);

#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_PES_PARSER_H__
