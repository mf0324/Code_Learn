/**
 * @file gen_sha256.h   SHA256
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.12
 *
 *
 */

#ifndef __GEN_SHA256_H__
#define __GEN_SHA256_H__

#include "gen_sha_public.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief init
 * @param [in] p_state, sha256 state
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sha256_init(GEN_SHA256_STATE* p_state);

/**  
 * @brief process
 * @param [in] p_state, sha256 state
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sha256_process(GEN_SHA256_STATE* p_state, uint8* p_data, int32 data_size);

/**  
 * @brief done
 * @param [in] p_state, sha256 state
 * @param [out] p_buf, output buf
 * @param [in/out] p_buf_size, total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sha256_done(GEN_SHA256_STATE* p_state, uint8* p_buf, int32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif //__GEN_SHA256_H__
