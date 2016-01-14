/**
 * @file mpeg4_sys_demux.h   MPEG4 System Demux
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.06.26
 *
 *
 */

#ifndef __MPEG4_SYS_DEMUX_H__
#define __MPEG4_SYS_DEMUX_H__

#include <public/gen_int.h>
#include "mpeg4_def.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief demux size encoding
 * @param [out] p_encoding, size encoding
 * @param [in] p_data, input data
 * @param [in/out] p_data_size input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_size_encoding_demux(MPEG4_SIZE_ENCODING* p_encoding, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux base descriptor
 * @param [out] p_desc, base descriptor
 * @param [in] p_data, input data
 * @param [in/out] p_data_size input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_base_desc_demux(MPEG4_BASE_DESCRIPTOR* p_desc, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux decoder config descriptor
 * @param [out] p_desc, descriptor
 * @param [in] p_data, input data
 * @param [in/out] p_data_size input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_dec_config_desc_demux(MPEG4_DEC_CONFIG_DESCRIPTOR* p_desc, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux es descriptor
 * @param [in] p_desc, descriptor
 * @param [in] p_data, input data
 * @param [in/out] p_data_size, input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_es_desc_demux(MPEG4_ES_DESCRIPTOR *p_desc, uint8* p_data, uint32* p_data_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG4_SYS_DEMUX_H__
