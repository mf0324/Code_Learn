/**
 * @file aac_frame_parser.h   AAC Frame Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.10.14
 *
 *
 */

#ifndef __AAC_FRAME_PARSER_H__
#define __AAC_FRAME_PARSER_H__

#include <public/gen_int.h>
#include <Liquid/public/media_def.h>

typedef void* AAC_FRAME_PARSER;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建帧解析器
 * @return
 * 非空值：成功
 * NULL：失败
 */
AAC_FRAME_PARSER aac_frame_parser_create();

/**  
 * @brief 销毁帧解析器
 * @param [in] frame_parser 帧解析器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 aac_frame_parser_destroy(AAC_FRAME_PARSER frame_parser);

/**  
 * @brief 初始化帧解析器
 * @param [in] frame_parser 帧解析器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 aac_frame_parser_init(AAC_FRAME_PARSER frame_parser);

/**  
 * @brief 反初始化帧解析器
 * @param [in] frame_parser 帧解析器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 aac_frame_parser_deinit(AAC_FRAME_PARSER frame_parser);

/**  
 * @brief 重置帧解析器
 * @param [in] frame_parser 帧解析器句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 aac_frame_parser_reset(AAC_FRAME_PARSER frame_parser);

/**  
 * @brief aac组帧
 * @param [in] frame_parser 帧解析器句柄
 * @param [out] p_frame 帧句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] buf_len 输入数据长度
 * @param [out] p_parse_len 已解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入数据不足以解析
 */
int32 aac_frame_parse(AAC_FRAME_PARSER frame_parser, MEDIA_FRAME *p_frame, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);

#ifdef __cplusplus
}
#endif

#endif ///__AAC_FRAME_PARSER_H__
