/**
 * @file http_mux.h  HTTP Mux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.24
 *
 *
 */

#ifndef __HTTP_MUX_H__
#define __HTTP_MUX_H__

#include <public/gen_int.h>
#include "http_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * @brief mux start line
 * @param [in] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size hold need size
 */
int32 http_mux_start_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size);


/** 
 * @brief mux header
 * @param [in] p_header, header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size hold need size
 */
int32 http_mux_header(HTTP_HEADER* p_header, uint8* p_buf, int32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__HTTP_MUX_H__
