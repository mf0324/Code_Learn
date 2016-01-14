/**
 * @file mpeg2_aac_parser.h   MPEG2 AAC Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.09.27
 *
 *
 */


#ifndef __MPEG2_AAC_PARSER_H__
#define __MPEG2_AAC_PARSER_H__

#include <public/gen_int.h>
#include "mpeg2_aac_const.h"

/** ADTS固定头 */
typedef struct tag_adts_fix_header
{
	/** protection_absent */
	int32  b_protection_absent;
	/** profile */
	int32  profile;
	/** sample_freq */
	int32  sample_freq;
	/** channel_configuration */
	int32  chn_config;

} ADTS_FIX_HEADER, *PADTS_FIX_HEADER;

/** ADTS可变头 */
typedef struct tag_adts_var_header
{
	/** copyright */
	int32  b_copyright;
	/** copyright_start */
	int32  b_copyright_start;
	/** frame_length */
	int32  frame_len;
	/** buffer_fullness */
	int32  buf_fullness;
	/** number_of_raw_data_blocks_in_frame */
	int32  rdb_num;

} ADTS_VAR_HEADER, *PADTS_VAR_HEADER;

/** ADTS包 */
typedef struct tag_adts_pack
{
	/** 起始地址 */
	uint8   *p_start;

	/** fix header */
	ADTS_FIX_HEADER  fix_header;
	/** var header */
	ADTS_VAR_HEADER  var_header;

} ADTS_PACK, *PADTS_PACK;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 初始化AAC ADTS包
 *
 * @param [in] p_pack ADTS包
 * @return
 * 0：成功
 * -1：参数错误
 */
int32 aac_adts_init(ADTS_PACK *p_pack);

/**  
 * @brief 解析AAC ADTS包
 * @param [out] p_pack ADTS包句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_len 数据长度
 * @param [out] p_parse_len 已解析的长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 * @remark
 * 1、无论解析成功与否，p_parse_len均会返回实际解析长度
 */
int32 aac_adts_parse(ADTS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);

/**  
 * @brief 获取AAC对应的采样率索引
 * @param [in] sample_freq 采样率
 * @param [out] p_index 对应的索引
 * @return
 * 0：成功
 * -1：参数错误
 * -3：未找到对应索引
 */
int32 aac_get_sample_freq_index(int32 sample_freq, int32 *p_index);

#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_AAC_PARSER_H__
