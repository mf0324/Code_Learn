/**
 * @file mpeg2_aes_parser.h   MPEG2 AES Parser
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.05.21
 *
 *
 */

#ifndef __MPEG2_AES_PARSER_H__
#define __MPEG2_AES_PARSER_H__

#include <public/gen_int.h>

#include "mpeg2_public.h"

/** 音频ES头 */
typedef struct tag_mpeg2_aes_header
{
	/** 音频头起始地址 */
	int8   *p_start;

	/** layer */
	int32  layer;

	/** 采样位宽 */
	int32  bits;
	/** 采样率 */
	int32  freq;
	/** 声道数 */
	int32  chn;

	/** 比特率，单位：kbit/s */
	int32  bit_rate;

	/** 是否填充 */
	int32  b_padding;

} MPEG2_AES_HEADER, *PMPEG2_AES_HEADER;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 初始化MPEG2音频ES头
 *
 * @param [in] p_header 音频ES头
 * @return
 * 0：成功
 * -1：参数错误
 */
int32 mpeg2_aes_init(MPEG2_AES_HEADER *p_header);

/**  
 * @brief 解析MPEG2音频ES头
 *
 * @param [out] p_header 音频ES头句柄
 * @param [in] p_buf 流数据地址
 * @param [in] buf_len 流数据长度
 * @param [out] p_parse_len 已解析的长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 */
int32 mpeg2_aes_parse(MPEG2_AES_HEADER *p_header, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_AES_PARSER_H__
