/**
 * @file log_debug.h  Log Debugger
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.03.05
 *
 *
 */

#ifndef __LOG_DEBUG_H__
#define __LOG_DEBUG_H__

#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <public/gen_char_public.h>

/** 输出到屏幕 */
#define LOG_OUTPUT_PRINT                                   (0x1)
/** 输出到sock */
#define LOG_OUTPUT_SOCK                                    (0x2)
/** 输出到日志文件 */
#define LOG_OUTPUT_FILE                                    (0x4)
/** 全部输出 */
#define LOG_OUTPUT_ALL                                     (LOG_OUTPUT_PRINT | LOG_OUTPUT_FILE | LOG_OUTPUT_SOCK)

/** 致命错误 */
#define LOG_FATAL_LEVEL                                    (0)
/** 一般错误 */
#define LOG_ERROR_LEVEL                                    (1)
/** 警告 */
#define LOG_WARN_LEVEL                                     (2)
/** 正常 */
#define LOG_NORMAL_LEVEL                                   (3)
/** 调试 */
#define LOG_DEBUG_LEVEL                                    (4)



#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief 初始化
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、该接口非多线程安全!
 */
int32 log_init();

/** 
 * @brief 反初始化
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、该接口非多线程安全!
 */
int32 log_deinit();

/** 
 * @brief 设置日志输出级别
 * @param [in] level 输出级别
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_set_level(int32 level);

/** 
 * @brief 设置是否显示
 * @param [in] b_output 是否显示
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_set_output(int32 b_output);

/** 
 * @brief 设置远程参数
 * @param [in] ip 远程IP，网络字节序
 * @param [in] port 远程端口，网络字节序
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、该接口非多线程安全!
 */
int32 log_set_remote(uint32 ip, uint16 port);

/** 
 * @brief 设置日志文件
 * @param [in] p_path 日志文件路径
 * @param [in] path_len 路径长度
 * @param [in] file_len 日志文件长度
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、当日志文件长度超过指定长度时，会自动清空日志
 */
int32 log_set_fileA(int8 *p_path, int32 path_len, uint32 file_len);

#if RUN_OS_WINDOWS
/** 
 * @brief 设置日志文件
 * @param [in] p_path 日志文件路径
 * @param [in] path_len 路径长度
 * @param [in] file_len 日志文件长度
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、当日志文件长度超过指定长度时，会自动清空日志
 */
int32 log_set_fileW(wchar_t *p_path, int32 path_len, uint32 file_len);
#endif

/**  
 * @brief 输出指定缓冲区内容
 * @param [in] p_buf 指定地址
 * @param [in] buf_size 缓冲区长度
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、按行输出，每行最多输出16个
 */
int32 log_output_buf_data(int8* p_buf, int32 buf_size);

/**  
 * @brief 写日志到指定缓冲区
 * @param [in] level 日志级别
 * @param [in] p_buf 指定地址
 * @param [in] p_format 格式化字符串
 * @return
 * >=0：成功，返回值表示实际写入长度
 * -1：失败
 * @remark
 * 1、只有当日志级别小于等于已设置的日志级别时才会被写入
 * 2、实际写入长度不包括末尾的'\0'
 */
int32 log_to_bufA(int32 level, int8* p_buf, int8 *p_format, ...);

/**  
 * @brief 写日志
 * @param [in] level 日志级别
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、只有当日志级别小于等于已设置的日志级别时才会被写入
 */
int32 log_traceA(int32 level, int8 *p_format, ...);

/**  
 * @brief 写调试日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_debugA(int8 *p_format, ...);

/**  
 * @brief 写警告日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_warnA(int8 *p_format, ...);

/**  
 * @brief 写错误日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_errorA(int8 *p_format, ...);

#if RUN_OS_WINDOWS

/**  
 * @brief 写日志到指定缓冲区
 * @param [in] level 日志级别
 * @param [in] p_buf 指定地址
 * @param [in] p_format 格式化字符串
 * @return
 * >=0：成功，返回值表示实际写入长度
 * -1：失败
 * @remark
 * 1、只有当日志级别小于等于已设置的日志级别时才会被写入
 * 2、实际写入长度不包括末尾的'\0'
 */
int32 log_to_bufW(int32 level, wchar_t *p_buf, const wchar_t *p_format, ...);

/**  
 * @brief 写日志
 * @param [in] level 日志级别
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、只有当日志级别小于等于已设置的日志级别时才会被写入
 * 2、如无特别说明，日志长度不能超过2 * 1024!
 */
int32 log_traceW(int32 level, const wchar_t *p_format, ...);

/**  
 * @brief 写调试日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_debugW(const wchar_t *p_format, ...);

/**  
 * @brief 写警告日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_warnW(const wchar_t *p_format, ...);

/**  
 * @brief 写错误日志
 * @param [in] p_format 格式化字符串
 * @return
 * 0：成功
 * -1：失败
 */
int32 log_errorW(const wchar_t *p_format, ...);

#endif

#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define log_set_file  log_set_fileW
#define log_to_buf    log_to_bufW
#define log_trace     log_traceW
#define log_debug     log_debugW
#define log_warn      log_warnW
#define log_error     log_errorW

#else

#define log_set_file  log_set_fileA
#define log_to_buf    log_to_bufA
#define log_trace     log_traceA
#define log_debug     log_debugA
#define log_warn      log_warnA
#define log_error     log_errorA

#endif//unicode

#else

/** linux */
#define log_set_file  log_set_fileA
#define log_to_buf    log_to_bufA
#define log_trace     log_traceA
#define log_debug     log_debugA
#define log_warn      log_warnA
#define log_error     log_errorA

#endif//if RUN_OS_WINDOWS



#ifdef __cplusplus
}
#endif

#endif ///__LOG_DEBUG_H__
