/**
 * @file mpeg2_ps_muxer.h   MPEG2 PS Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.16
 *
 *
 */


#ifndef __MPEG2_PS_MUXER_H__
#define __MPEG2_PS_MUXER_H__

#include <public/gen_int.h>
#include "mpeg2_ps_param.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算PS头部长度
 *
 * @param [in] p_param PS头部参数
 * @param [out] p_header_size 头部长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 */
int32 mpeg2_calc_ps_header_size(MPEG2_PS_HEADER_PARAM *p_param, int32 *p_header_size);

/**  
 * @brief 生成PS头部
 *
 * @param [in] p_param PS头部参数
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mpeg2_ps_header_mux(MPEG2_PS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算系统头部长度
 *
 * @param [in] p_param 系统头部参数
 * @param [out] p_header_size 系统头部长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 */
int32 mpeg2_calc_sys_header_size(MPEG2_SYS_HEADER_PARAM *p_param, int32 *p_header_size);

/**  
 * @brief 生成系统头部
 *
 * @param [in] p_param 系统头部参数
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mpeg2_sys_header_mux(MPEG2_SYS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成PS尾部
 *
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mpeg2_ps_end_mux(uint8 *p_buf, int32 *p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_PS_MUXER_H__
