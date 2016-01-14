/**
 * @file mpeg2_aac_mux.h   MPEG2 AAC Mux
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.11.21
 *
 *
 */

#ifndef __MPEG2_AAC_MUX_H__
#define __MPEG2_AAC_MUX_H__

#include <public/gen_int.h>
#include "mpeg2_aac_mux_public.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief mux adts header
 * @param [in] p_header, adts header
 * @param [in] p_buf, output buf
 * @param [in/out] p_buf_size, output buf total size/actually mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_BUF, *p_buf_size means actually need size
 */
int32 mpeg2_aac_adts_header_mux(MPEG2_AAC_ADTS_HEADER* p_header, uint8* p_buf, int32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_AAC_MUX_H__
