/**
 * @file mpeg4_aac_demux.h   MPEG4 AAC Demux
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.06.26
 *
 *
 */

#ifndef __MPEG4_AAC_DEMUX_H__
#define __MPEG4_AAC_DEMUX_H__

#include <public/gen_int.h>
#include "mpeg4_aac_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief demux audio spec config
 * @param [out] p_config, audio spec config
 * @param [in] p_data, input data
 * @param [in/out] p_data_size input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_audio_spec_config_demux(MPEG4_AUDIO_SPEC_CONFIG* p_config, uint8* p_data, int32* p_data_size);

/**  
 * @brief demux audio decoder spec info
 * @param [out] p_info, audio decoder spec info
 * @param [in] p_data, input data
 * @param [in/out] p_data_size input data size/actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_DATA, *p_data_size means actually need size
 */
int32 mpeg4_adec_spec_info_demux(MPEG4_ADEC_SPEC_INFO* p_info, uint8* p_data, int32* p_data_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG4_SYS_MUX_H__
