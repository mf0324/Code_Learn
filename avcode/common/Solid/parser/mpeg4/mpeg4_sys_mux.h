/**
 * @file mpeg4_muxer.h   MPEG4 System Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.11
 *
 *
 */


#ifndef __MPEG4_SYS_MUX_H__
#define __MPEG4_SYS_MUX_H__

#include <public/gen_int.h>
#include "mpeg4_def.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算Base Descriptor长度
 * @param [in] p_desc Descriptor句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   size.bit_size
 */
int32 mpeg4_calc_base_desc_size(MPEG4_BASE_DESCRIPTOR *p_desc, uint32 *p_size);

/**  
 * @brief 生成Base Descriptor
 * @param [in] p_desc Descriptor句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括:
 *   tag
 *   size.value
 *   size.bit_siz
 */
int32 mpeg4_base_desc_mux(MPEG4_BASE_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size);

/**  
 * @brief 计算SL Config Descriptor长度
 * @param [in] p_desc Descriptor句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   base.size.bit_size
 */
int32 mpeg4_calc_sl_config_desc_size(MPEG4_SL_CONFIG_DESCRIPTOR *p_desc, uint32 *p_size);

/**  
 * @brief 生成SL Descriptor
 * @param [in] p_desc Descriptor句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括:
 *   base
 *   predefine
 *   其中base的相应字段也要填写，包括
 *   base.size.bit_siz
 */
int32 mpeg4_sl_config_desc_mux(MPEG4_SL_CONFIG_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size);

/**  
 * @brief 计算Video Decoder Specific Info长度
 * @param [in] p_info Info句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   vos
 *   visual_obj
 *   vol
 *   其中各子部分的相关字段也要填写，包括
 *   vos.profile
 *   visual_obj.is_visual_obj
 *   visual_obj.type
 *   visual_obj.signal_type
 *   visual_obj.color_desc
 *   vol.video_obj_type_indication
 *   vol.is_obj_layer
 *   vol.aspect_ratio
 *   vol.vol_ctrl_param
 *   vol.vbv_param
 *   vol.shape
 */
int32 mpeg4_calc_vdec_spec_info_size(MPEG4_VDEC_SPEC_INFO *p_info, uint32 *p_size);

/**  
 * @brief 计算Decoder Config Descriptor长度
 * @param [in] p_desc Descriptor句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   base
 *   vdec_spec_info_size
 *   其中base相关字段也要填写，包括
 *   base.size.bit_size
 */
int32 mpeg4_calc_dec_config_desc_size(MPEG4_DEC_CONFIG_DESCRIPTOR *p_desc, uint32 *p_size);

/**  
 * @brief 生成Decoder Config Descriptor
 * @param [in] p_desc Descriptor句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   base
 *   obj_type
 *   stream_type
 *   buf_size
 *   max_bitrate
 *   avg_bitrate
 *   p_vdec_spec_info
 *   vdec_spec_info_size
 *   其中base相关字段也要填写，包括
 *   base.size.bit_size
 */
int32 mpeg4_dec_config_desc_mux(MPEG4_DEC_CONFIG_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size);

/**  
 * @brief 计算ES Descriptor长度
 * @param [in] p_desc Descriptor句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   base.size.bit_size
 *   dec_config.base
 *   dec_config.vdec_spec_info_size
 *   sl_config.base
 * 2、返回时会修改base.size.value字段
 */
int32 mpeg4_calc_es_desc_size(MPEG4_ES_DESCRIPTOR *p_desc, uint32 *p_size);

/**  
 * @brief 生成ES Descriptor
 * @param [in] p_desc Descriptor句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   base.size.bit_size
 *   es_id
 *   stream_depend_flag
 *   url_flag
 *   ocr_stream_flag
 *   stream_priority
 *   dec_config.base.size.bit_size
 *   dec_config.obj_type
 *   dec_config.stream_type
 *   dec_config.buf_size
 *   dec_config.max_bitrate
 *   dec_config.avg_bitrate
 *   dec_config.p_vdec_spec_info
 *   dec_config.vdec_spec_info_size
 *   sl_config.base.size.bit_size
 *   sl_config.predefine
 */
int32 mpeg4_es_desc_mux(MPEG4_ES_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG4_SYS_MUX_H__
