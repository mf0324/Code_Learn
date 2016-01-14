/**
 * @file h264_bs_parser.h   H264 Bit Stream Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.02.26
 *
 *
 */


#ifndef __H264_BS_PARSER_H__
#define __H264_BS_PARSER_H__


#include "h264_def.h"



#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 初始化H264 BS流状态
 * @param [in] p_pack BS状态句柄
 * @return
 * 0：成功
 * -1：参数错误
 */
int32 h264_bs_init(H264_BS_PACK *p_pack);

/**  
 * @brief 解析H264 BS流
 * @param [out] p_pack BS句柄
 * @param [in] p_buf 流地址
 * @param [in] buf_len 流长度
 * @param [out] p_parse_len 已解析的数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 * @remark
 * 1、无论解析成功与否，均会更新p_parse_len参数
 */
int32 h264_parse_bs(H264_BS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);

/**  
 * @brief 解析H264 BS流(分析更多可能情况)
 * @param [out] p_pack BS句柄
 * @param [in] p_buf 流地址
 * @param [in] buf_len 流长度
 * @param [out] p_parse_len 已解析的数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 * -12：数据不足以解析，但可能包含部分有效数据
 * @remark
 * 1、无论解析成功与否，均会更新p_parse_len参数
 */
int32 h264_parse_bs2(H264_BS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);

#ifdef __cplusplus
}
#endif

#endif ///__H264_BS_PARSER_H__
