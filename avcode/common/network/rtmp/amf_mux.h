/**
 * @file amf_mux.h  AMF Mux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.19
 *
 *
 */


#ifndef __AMF_MUX_H__
#define __AMF_MUX_H__

#include <public/gen_int.h>
#include "amf_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief number
 * @param [in] p_amf_type, type
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if buf size not enough, p_buf_size means actual need size
 * 2. caller must fill:
 *   value
 */
int32 amf_mux_number(AMF_NUMBER* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成bool
 * @param [in] p_amf_type type句柄
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
 *   b_true
 */
int32 amf_mux_bool(AMF_BOOL* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成string
 * @param [in] p_amf_type type句柄
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
 *   data_size
 *   p_data
 */
int32 amf_mux_string(AMF_STRING* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 计算obj所需长度
 * @param [in] p_amf_type type句柄
 * @param [out] p_size 所需长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   prop_count
 *   p_prop
 */
int32 amf_calc_obj_size(AMF_OBJ* p_amf_type, uint32* p_size);

/**  
 * @brief 生成obj
 * @param [in] p_amf_type type句柄
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
 *   prop_count
 *   p_prop
 */
int32 amf_mux_obj(AMF_OBJ* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成null
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 amf_mux_null(uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成undef
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 amf_mux_undef(uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成ref
 * @param [in] p_amf_type type句柄
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
 *   index
 */
int32 amf_mux_ref(AMF_REF* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成long string
 * @param [in] p_amf_type type句柄
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
 *   data_size
 *   p_data
 */
int32 amf_mux_long_string(AMF_LONG_STRING* p_amf_type, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief 生成xml doc
 * @param [in] p_amf_type type句柄
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
 *   data_size
 *   p_data
 */
int32 amf_mux_xml_doc(AMF_XML_DOC* p_amf_type, uint8* p_buf, uint32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__AMF_MUX_H__
