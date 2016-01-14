/**
 * @file log_agent.h  Log Agent
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.04.03
 *
 *
 */

#ifndef __LOG_AGENT_H__
#define __LOG_AGENT_H__

//#define LOG_AGENT_NO_DEBUG

#ifndef LOG_AGENT_NO_DEBUG

#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <public/gen_char_public.h>

#include "log_agent_public.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern LOG_AGENT  g_log_agent;
extern LOG_AGENT* g_p_log_agent;


/** 
 * @brief set global agent
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done in exclusive context!
 */
int32 log_agent_set_global_agent(LOG_AGENT* p_agent);

/** 
 * @brief setup
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done in exclusive context!
 */
int32 log_agent_setup(LOG_AGENT* p_agent);

/** 
 * @brief cleanup
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done in exclusive context!
 */
int32 log_agent_cleanup(LOG_AGENT* p_agent);

/** 
 * @brief init
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done in exclusive context!
 */
int32 log_agent_init(LOG_AGENT* p_agent);

/** 
 * @brief deinit
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done in exclusive context!
 */
int32 log_agent_deinit(LOG_AGENT* p_agent);

/** 
 * @brief set log level
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. log level can be mix together
 * e.g.
 *    LA_LOG_LEVEL_ERROR | LA_LOG_LEVEL_DEBUG
 */
int32 log_agent_set_log_level(LOG_AGENT* p_agent, uint32 level);

/** 
 * @brief set log type
 * @param [in] p_agent, agent
 * @param [in] type, log type
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. log type can be mix together
 * e.g.
 *    LA_LOG_TYPE_STDOUT | LA_LOG_TYPE_FILE
 */
int32 log_agent_set_log_type(LOG_AGENT* p_agent, uint32 type);

/** 
 * @brief set log file dir
 * @param [in] p_agent, agent
 * @param [in] p_dir, log file dir
 * @param [in] dir_size, log file dir size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. log file dir should be absolute path
 */
int32 log_agent_set_log_file_dirA(LOG_AGENT* p_agent, int8* p_dir, int32 dir_size);

#if RUN_OS_WINDOWS
/** 
 * @brief set log file dir(unicode)
 * @param [in] p_agent, agent
 * @param [in] p_dir, log file dir
 * @param [in] dir_size, log file dir size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. NOTICE: the log file dir size count in bytes, not in wchar_t!!!
 * 2. log file dir should be absolute path
 */
int32 log_agent_set_log_file_dirW(LOG_AGENT* p_agent, wchar_t* p_dir, int32 dir_size);
#endif

/**  
 * @brief set split file rule
 * @param [in] p_agent, agent
 * @param [in] split rule, split rule
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. default rule is split by time
 */
int32 log_agent_set_split_file_rule(LOG_AGENT* p_agent, int32 split_rule);

/**  
 * @brief set log file split time
 * @param [in] p_agent, agent
 * @param [in] split time, split time, unit: second
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. split time unit is second, 0 means never split, default value is 60 x 60(1 hour)
 */
int32 log_agent_set_log_file_split_time(LOG_AGENT* p_agent, int32 split_time);

/**  
 * @brief set log file split size
 * @param [in] p_agent, agent
 * @param [in] split size, split size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. split size unit is byte
 */
int32 log_agent_set_log_file_split_size(LOG_AGENT* p_agent, uint32 split_size);

#if LA_ENABLE_NETWORK
/**  
 * @brief set log sock info
 * @param [in] p_agent, agent
 * @param [in] p_info, sock info
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. sock info's local and remote info must be specify
 */
int32 log_agent_set_log_sock_info(LOG_AGENT* p_agent, NET_SOCK_INFO* p_info);
#endif

/**  
 * @brief trace
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. only the log which specify by preset level of agent would be record
 */
int32 log_agent_traceA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_format, ...);

/**  
 * @brief trace to buf
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_buf, output buf
 * @param [in/out] p_buf_size, output buf size/actual use size
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. only the log which specify by preset level of agent would be record
 * 2. buf size count in bytes!!!
 */
int32 log_agent_trace2bufA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, int8* p_format, ...);

/**  
 * @brief debug
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_debugA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...);

/**  
 * @brief info
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_infoA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...);

/**  
 * @brief warn
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_warnA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...);

/**  
 * @brief error
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_errorA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...);

#if RUN_OS_WINDOWS

/**  
 * @brief trace(unicode)
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. only the log which specify by preset level of agent would be record
 */
int32 log_agent_traceW(LOG_AGENT* p_agent, int32 level, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...);

/**  
 * @brief trace to buf(unicode)
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_buf, output buf
 * @param [in/out] p_buf_size, output buf size/actual use size
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. only the log which specify by preset level of agent would be record
 * 2. buf size count in bytes!!!
 */
int32 log_agent_trace2bufW(LOG_AGENT* p_agent, int32 level, wchar_t* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, wchar_t* p_format, ...);

/**  
 * @brief debug(unicode)
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_debugW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...);

/**  
 * @brief info(unicode)
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_infoW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...);

/**  
 * @brief warn(unicode)
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_warnW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t *p_format, ...);

/**  
 * @brief error(unicode)
 * @param [in] p_agent, agent
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @return
 * 0: success
 * other: fail
 */
int32 log_agent_errorW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t *p_format, ...);

#endif

#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define log_agent_set_log_file_dir                         log_agent_set_log_file_dirW
#define log_agent_trace                                    log_agent_traceW
#define log_agent_trace2buf                                log_agent_trace2bufW
#define log_agent_debug                                    log_agent_debugW
#define log_agent_info                                     log_agent_infoW
#define log_agent_warn                                     log_agent_warnW
#define log_agent_error                                    log_agent_errorW

#else

#define log_agent_set_log_file_dir                         log_agent_set_log_file_dirA
#define log_agent_trace                                    log_agent_traceA
#define log_agent_trace2buf                                log_agent_trace2bufA
#define log_agent_debug                                    log_agent_debugA
#define log_agent_info                                     log_agent_infoA
#define log_agent_warn                                     log_agent_warnA
#define log_agent_error                                    log_agent_errorA

#endif//unicode

#else

/** linux */
#define log_agent_set_log_file_dir                         log_agent_set_log_file_dirA
#define log_agent_trace                                    log_agent_traceA
#define log_agent_trace2buf                                log_agent_trace2bufA
#define log_agent_debug                                    log_agent_debugA
#define log_agent_info                                     log_agent_infoA
#define log_agent_warn                                     log_agent_warnA
#define log_agent_error                                    log_agent_errorA

#endif//RUN_OS_WINDOWS



#ifdef __cplusplus
}
#endif


#else

/** LOG_AGENT_NO_DEBUG */
#define  log_agent_set_global_agent(x)                     
#define  log_agent_setup(x)                                
#define  log_agent_cleanup(x)                              
#define  log_agent_init(x)                                 
#define  log_agent_deinit(x)                               
#define  log_agent_set_log_level(x)                        
#define  log_agent_set_log_type(x)                         
#define  log_agent_set_log_file_dir(x)                     
//#define  log_agent_set_log_file_dirW(x)                    ()
#define  log_agent_set_split_file_rule(x)                  
#define  log_agent_set_log_file_split_time(x)              
#define  log_agent_set_log_file_split_size(x)              
#define  log_agent_set_log_sock_info(x)                    
#define  log_agent_trace(x)                                
#define  log_agent_trace2buf(x)                            
#define  log_agent_debug(x)                                
#define  log_agent_info(x)                                 
#define  log_agent_warn(x)
#define  log_agent_error(x)


#endif// LOG_AGENT_NO_DEBUG


#endif ///__LOG_AGENT_H__
