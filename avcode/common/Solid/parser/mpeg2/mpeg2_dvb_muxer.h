/**
 * @file mpeg2_dvb_muxer.h   MPEG2 DVB Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.04.06
 *
 *
 */


#ifndef __MPEG2_DVB_MUXER_H__
#define __MPEG2_DVB_MUXER_H__

#include <public/gen_int.h>
#include "mpeg2_dvb_param.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算SIT数据长度
 * @param [in] p_param SIT参数句柄
 * @param [out] p_buf_size 所需长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 */
int32 mpeg2_dvb_calc_sit_size(MPEG2_DVB_SIT_PARAM *p_param, int32 *p_buf_size);

/**  
 * @brief 生成SIT字段
 * @param [in] p_param SIT参数句柄
 * @param [in] p_buf 数据缓冲地址
 * @param [in/out] p_buf_size 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mpeg2_dvb_mux_sit(MPEG2_DVB_SIT_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_DVB_MUXER_H__
