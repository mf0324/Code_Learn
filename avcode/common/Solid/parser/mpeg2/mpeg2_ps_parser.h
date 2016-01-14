/**
 * @file mpeg2_ps_parser.h   MPEG2 PS Parser
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.09.29
 *
 *
 */

#ifndef __MPEG2_PS_PARSER_H__
#define __MPEG2_PS_PARSER_H__

#include <public/gen_int.h>

#include "mpeg2_public.h"
//#include "mpeg2_pes_parser.h"
#include "mpeg2_ps_public.h"



#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief demux ps
 * @param [out] p_pack, ps pack
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always return actual demux size.
 */
int32 mpeg2_ps_demux(MPEG2_PS_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_demux_size);

/**  
 * @brief demux ps stream
 * @param [in] p_stream, stream array
 * @param [in/out] p_stream_num, total stream count/actual stream count
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if array not enough, p_stream_num hold actual need stream count
 */
int32 mpeg2_ps_demux_stream_info(MPEG2_PS_STREAM* p_stream, int32* p_stream_num, uint8* p_data, int32 data_size, int32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_PS_PARSER_H__
