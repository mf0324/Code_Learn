/**
 * @file log_agent_util.h  Log Agent Utility
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.04.16
 *
 *
 */

#ifndef __LOG_AGENT_UTIL_H__
#define __LOG_AGENT_UTIL_H__

#include <public/gen_int.h>


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief translate string to level
 * @param [in] p_str, log level in string form
 * @param [in] str_size, string size
 * @param [out] p_level, log level in digi form
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. string size unit is byte
 */
int32 log_agent_trans_str_to_level(int8* p_str, int32 str_size, uint32* p_level);





#ifdef __cplusplus
}
#endif

#endif ///__LOG_AGENT_UTIL_H__
