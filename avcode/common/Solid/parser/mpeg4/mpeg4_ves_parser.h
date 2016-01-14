/**
 * @file mpeg4_ves_parser.h   MPEG4 VES Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.05.15
 *
 *
 */


#ifndef __MPEG4_VES_PARSER_H__
#define __MPEG4_VES_PARSER_H__

#include <public/gen_int.h>
#include "mpeg4_public.h"
#include "mpeg4_video_def.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 初始化MPEG4视频ES状态
 * @param [in] p_state 视频ES状态句柄
 * @return
 * 0：成功
 * -1：参数错误
 */
int32 mpeg4_ves_init(MPEG4_VES_STATE *p_state);

/**  
 * @brief 解析MPEG4视频ES
 * @param [out] p_state 视频ES状态句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_len 输入数据长度
 * @param [out] p_parse_len 已解析数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 * @remark
 * 1、无论解析是否成功，均会更新p_parse_len参数
 */
int32 mpeg4_ves_parse(MPEG4_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);

/**  
 * @brief 解析MPEG4视频ES(分析更多可能情况)
 * @param [out] p_state 视频ES状态句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_len 输入数据长度
 * @param [out] p_parse_len 已解析数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 * -12：数据不足以解析，但可能包含部分有效数据
 * @remark
 * 1、无论解析是否成功，均会更新p_parse_len参数
 */
int32 mpeg4_ves_parse2(MPEG4_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG4_VES_PARSER_H__
