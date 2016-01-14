/**
 * @file gen_hmac.h   HMAC
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.15
 *
 *
 */

#ifndef __GEN_HMAC_H__
#define __GEN_HMAC_H__

#include "gen_hmac_public.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief setup
 * @param [in] p_state, hmac state
 * @return
 * 0: success
 * other: fail
 */
int32 gen_hmac_setup(GEN_HMAC_STATE* p_state);

/**  
 * @brief init
 * @param [in] p_state, hmac state
 * @param [in] p_key, key
 * @param [in] key_size, key size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. p_state following member must be filled
 *  sha_type
 */
int32 gen_hmac_init(GEN_HMAC_STATE* p_state, uint8* p_key, int32 key_size);

/**  
 * @brief process
 * @param [in] p_state, hmac state
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_hmac_process(GEN_HMAC_STATE* p_state, uint8* p_data, int32 data_size);

/**  
 * @brief done
 * @param [in] p_state, hmac state
 * @param [out] p_buf, output buf
 * @param [in/out] p_buf_size, total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_hmac_done(GEN_HMAC_STATE* p_state, uint8* p_buf, int32* p_buf_size);

/**  
 * @brief calc
 * @param [in] p_state, hmac state
 * @param [in] p_key, key
 * @param [in] key_size, key size
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_buf, output buf
 * @param [in/out] p_buf_size, total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. p_state following member must be filled
 *  sha_type
 */
int32 gen_hmac_calc(GEN_HMAC_STATE* p_state, uint8* p_key, int32 key_size, uint8* p_data, int32 data_size, uint8* p_buf, int32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif //__GEN_HMAC_H__
