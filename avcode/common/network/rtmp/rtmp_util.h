/**
 * @file rtmp_util.h  RTMP Util
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.16
 *
 *
 */

#ifndef __RTMP_UTIL_H__
#define __RTMP_UTIL_H__

#include <public/gen_int.h>
#include "rtmp_public.h"

extern uint8 rtmp_fms_key[68];
extern uint8 rtmp_fp_key[62];


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief calc digest random size
 * @param [in] p_digest, digest
 * @param [out] p_random_size, random data size
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_calc_digest_random_size(RTMP_C1S1_DIGEST* p_digest, int32* p_random_size);

/**  
 * @brief calc key random size
 * @param [in] p_key, key
 * @param [out] p_random_size, random data size
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_calc_key_random_size(RTMP_C1S1_KEY* p_key, int32* p_random_size);


#ifdef __cplusplus
}
#endif

#endif ///__RTMP_UTIL_H__
