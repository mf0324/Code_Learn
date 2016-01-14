/**
 * @file mpeg2_ps_frame_muxer.h   MPEG2 PS Frame Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.16
 *
 *
 */


#ifndef __MPEG2_PS_FRAME_MUXER_H__
#define __MPEG2_PS_FRAME_MUXER_H__

#include <public/gen_int.h>
#include "mpeg2_ps_param.h"
#include "mpeg2_frame_param.h"

/** PS复用器 */
typedef void* MPEG2_PS_FRAME_MUXER;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建帧复用器
 *
 * @return
 * 非空值：成功
 * NULL：失败
 */
MPEG2_PS_FRAME_MUXER mpeg2_ps_frame_muxer_create();

/**  
 * @brief 销毁帧复用器
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 mpeg2_ps_frame_muxer_destroy(MPEG2_PS_FRAME_MUXER frame_muxer);

/**  
 * @brief 初始化帧复用器
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 mpeg2_ps_frame_muxer_init(MPEG2_PS_FRAME_MUXER frame_muxer);

/**  
 * @brief 反初始化帧复用器
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 mpeg2_ps_frame_muxer_deinit(MPEG2_PS_FRAME_MUXER frame_muxer);

/**  
 * @brief 重置帧复用器
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 mpeg2_ps_frame_muxer_reset(MPEG2_PS_FRAME_MUXER frame_muxer);

/**  
 * @brief 设置视频es参数
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @param [in] p_param 视频es参数
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、相关参数参见mpeg2_public.h
 * 2、默认视频es参数为：stream id = 0xe0
 */
int32 mpeg2_ps_set_ves_param(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_STREAM_PARAM *p_param);

/**  
 * @brief 设置音频es参数
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @param [in] p_param 音频es参数
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、相关参数参见mpeg2_public.h
 * 2、默认音频es参数为：stream id = 0xc0
 */
int32 mpeg2_ps_set_aes_param(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_STREAM_PARAM *p_param);

/**  
 * @brief 生成PS头部
 *
 * @param [in] frame_muxer 帧复用器句柄
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
int32 mpeg2_ps_make_ps_header_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_PS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成系统头部
 *
 * @param [in] frame_muxer 帧复用器句柄
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
int32 mpeg2_ps_make_sys_header_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_SYS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成PS帧
 *
 * @param [in] frame_muxer 帧复用器句柄
 * @param [in] p_param 帧参数
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、要打包的帧数据必须是es！
 */
int32 mpeg2_ps_make_frame_pack(MPEG2_PS_FRAME_MUXER frame_muxer, MPEG2_FRAME_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_PS_FRAME_MUXER_H__
