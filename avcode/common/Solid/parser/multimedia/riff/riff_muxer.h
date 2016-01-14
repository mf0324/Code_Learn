/**
 * @file riff_muxer.h   RIFF Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.26
 *
 *
 */


#ifndef __RIFF_MUXER_H__
#define __RIFF_MUXER_H__

#include <public/gen_int.h>
#include "riff_header.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 生成LIST包头
 *
 * @param [in] p_header 参数句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_len返回
 */
int32 riff_list_header_mux(RIFF_LIST_HEADER *p_header, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成CHUNK包头
 *
 * @param [in] p_header 参数句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_len返回
 */
int32 riff_chunk_header_mux(RIFF_CHUNK_HEADER *p_header, uint8 *p_buf, int32 *p_buf_size);

#ifdef __cplusplus
}
#endif

#endif ///__RIFF_MUXER_H__
