/**
 * @file mpeg2_util.h   MPEG2 Utility
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.21
 *
 *
 */


#ifndef __MPEG2_UTIL_H__
#define __MPEG2_UTIL_H__

#include <public/gen_int.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 将MPEG2系统时间转换为流时间
 *
 * @param [in/out] p_frame_time MPEG2系统时间/流时间
 * @return
 * 0：成功
 * -1：参数错误
 * @remark
 * 1、MPEG2系统时间单位：clock(1秒 = 90000 clock)，流时间单位：毫秒(1秒 = 1000毫秒)
 */
int32 mpeg2_sys2stream_time(int64 *p_frame_time);

/**
 * @brief 将MPEG2流时间转换为系统时间
 *
 * @param [in/out] p_frame_time MPEG2流时间/系统时间
 * @return
 * 0：成功
 * -1：参数错误
 * @remark
 * 1、MPEG2系统时间单位：clock(1秒 = 90000 clock)，流时间单位：毫秒(1秒 = 1000毫秒)
 */
int32 mpeg2_stream2sys_time(int64 *p_frame_time);

#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_UTIL_H__
