/**
 * @file mp4_aac_mux.h   MP4 AAC Mux
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.26
 *
 *
 */


#ifndef __MP4_AAC_MUX_H__
#define __MP4_AAC_MUX_H__

#include <public/gen_int.h>
#include "mp4_aac_file.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算Es Desc Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   es_desc(see also mpeg4_calc_es_desc_size)
 */
int32 mp4_aac_calc_es_desc_box_total_size(AAC_ES_DESC_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Es Desc Box
 * @param [in] p_box Box句柄
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
 *   full_box.box.size
 *   es_desc(see also mpeg4_es_desc_mux)
 */
int32 mp4_aac_es_desc_box_mux(AAC_ES_DESC_BOX *p_box, uint8 *p_buf, uint32 *p_buf_size);

/**  
 * @brief 计算AAC Sample Entry总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   es_box.es_desc(see also mpeg4_calc_es_desc_mux)
 */
int32 mp4_aac_calc_sample_entry_total_size(AAC_SAMPLE_ENTRY *p_box, uint32 *p_size);

/**  
 * @brief 生成AAC Sample Entry
 * @param [in] p_box Box句柄
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
 *   audio.entry.box.size
 *   es_box.full_box.box.size
 *   es_box.es_desc(see also mpeg4_es_desc_mux)
 */
int32 mp4_aac_sample_entry_mux(AAC_SAMPLE_ENTRY* p_box, uint8 *p_buf, uint32 *p_buf_size);

/**  
 * @brief 计算AAC Sample Description Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_aac_calc_sample_desc_box_total_size(AAC_SAMPLE_DESC_BOX* p_box, uint32* p_size);

/**  
 * @brief 生成AAC Sample Description Box总长度
 * @param [in] p_box Box句柄
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
 *   full_box.box.size
 *   entry_count
 *   p_entry(see also @mp4_aac_sample_entry_mux)
 */
int32 mp4_aac_sample_desc_box_mux(AAC_SAMPLE_DESC_BOX* p_box, uint8* p_buf, uint32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MP4_AAC_MUX_H__
