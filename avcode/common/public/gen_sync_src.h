/**
 * @file gen_sync_src.h   General Sync Source
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.26
 *
 *
 */


#ifndef __GEN_SYNC_SRC_H__
#define __GEN_SYNC_SRC_H__

#include <public/gen_int.h>
#include <public/gen_time_public.h>

/** Gen Sync Src Handle */
typedef void*  GEN_SYNC_SRC_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not NULL: success
 * NULL: fail
 */
GEN_SYNC_SRC_H gen_sync_src_create();

/**  
 * @brief destroy
 * @param [in] src_h, sync src handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sync_src_destroy(GEN_SYNC_SRC_H src_h);

/**  
 * @brief init
 * @param [in] src_h, sync src handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sync_src_init(GEN_SYNC_SRC_H src_h);

/**  
 * @brief deinit
 * @param [in] src_h, sync src handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_sync_src_deinit(GEN_SYNC_SRC_H src_h);

/**  
 * @brief get sys tick
 * @param [in] src_h, sync src handle
 * @param [in/out] p_tick, fore sys tick / current sys tick(unit: ms)
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. sys tick means at the moment the value retrieved from sys clock, in milisecond
 * 2. warning: the elapse time between two calls MUST not exceed 49.7 days!
 * 3. first call when input must init p_tick to 0!
 * 4. if multithread call this func with same variable, any synchronization way should be used
 */
int32 gen_sync_src_get_sys_tick(GEN_SYNC_SRC_H src_h, int64* p_tick);

/**  
 * @brief get utc time(in second)
 * @param [in] src_h, sync src handle
 * @param [out] p_second, utc time in second
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. UTC origin is 1970.01.01 00:00:00
 */
int32 gen_sync_src_get_utc_time(GEN_SYNC_SRC_H src_h, int64* p_second);

/**  
 * @brief get utc time(in millisecond)
 * @param [in] src_h, sync src handle
 * @param [out] p_ms, utc time in millisecond
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. UTC origin is 1970.01.01 00:00:00
 */
int32 gen_sync_src_get_utc_time_ms(GEN_SYNC_SRC_H src_h, int64* p_ms);

/**  
 * @brief get utc date time
 * @param [in] src_h, sync src handle
 * @param [out] p_time, utc time
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. UTC origin is 1970.01.01 00:00:00
 */
int32 gen_sync_src_get_utc_date_time(GEN_SYNC_SRC_H src_h, GEN_DATE_TIME* p_date_time);

/**  
 * @brief conver utc time to date
 * @param [in] src_h, sync src handle
 * @param [in] p_second, utc time in second
 * @param [out] p_date_time, utc date time
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. UTC origin is 1970.01.01 00:00:00
 */
int32 gen_sync_src_convert_utc_time_to_date(GEN_SYNC_SRC_H src_h, int64* p_second, GEN_DATE_TIME* p_date_time);

/**  
 * @brief conver utc date to time
 * @param [in] src_h, sync src handle
 * @param [in] p_date_time, utc date time
 * @param [out] p_second, utc time in second
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. UTC origin is 1970.01.01 00:00:00
 */
int32 gen_sync_src_convert_utc_date_to_time(GEN_SYNC_SRC_H src_h, GEN_DATE_TIME* p_date_time, int64* p_second);


#ifdef __cplusplus
}
#endif

#endif ///__GEN_SYNC_SRC_H__
