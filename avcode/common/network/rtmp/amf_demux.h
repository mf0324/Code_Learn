/**
 * @file amf_demux.h   AMF Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.18
 *
 *
 */


#ifndef __AMF_DEMUX_H__
#define __AMF_DEMUX_H__

#include <public/gen_int.h>
#include "amf_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief number
 * @param [out] p_amf_type, type
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always updated
 */
int32 amf_demux_number(AMF_NUMBER* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief bool
 * @param [out] p_amf_type, type
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always updated
 */
int32 amf_demux_bool(AMF_BOOL* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief string
 * @param [out] p_amf_type, type
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always updated
 */
int32 amf_demux_string(AMF_STRING* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 计算object长度
 * @param [in/out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、返回时p_amf_type的prop_count表示实际的prop个数
 */
int32 amf_demux_calc_obj_size(AMF_OBJ* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析object
 * @param [in/out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * -4：输入prop集合不足以存放解析出来的数据
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、如果输入prop不足以存放解析数据，返回时p_amf_type的prop_count表示实际所需的prop个数
 */
int32 amf_demux_obj(AMF_OBJ* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析ref
 * @param [out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 amf_demux_ref(AMF_REF* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析ecma array
 * @param [in/out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * -4：输入prop集合不足以存放解析出来的数据
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、如果输入prop不足以存放解析数据，返回时p_amf_type的prop_count表示实际所需的prop个数
 */
int32 amf_demux_ecma_array(AMF_ECMA_ARRAY* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析strict array
 * @param [in/out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * -4：输入value集合不足以存放解析出来的数据
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、如果输入value不足以存放解析数据，返回时p_amf_type的value_count表示实际所需的value个数
 */
int32 amf_demux_strict_array(AMF_STRICT_ARRAY* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析long string
 * @param [out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 amf_demux_long_string(AMF_LONG_STRING* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析xml doc
 * @param [out] p_amf_type type句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 amf_demux_xml_doc(AMF_XML_DOC* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);


/**  
 * @brief 解析xml doc
 * @param [out] p_amf_type type句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_size 输入数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * -4：输入prop集合不足以存放解析出来的数据
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、如果输入prop不足以存放解析数据，返回时p_amf_type->obj.prop_count表示实际所需的prop个数
 * 3、p_buf[0] should be amf type
 */
int32 amf_demux_data_type(AMF_DATA* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__AMF_DEMUX_H__
