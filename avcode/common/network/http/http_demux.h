/**
 * @file http_demux.h  HTTP Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.28
 *
 *
 */

#ifndef __HTTP_DEMUX_H__
#define __HTTP_DEMUX_H__

#include <public/gen_int.h>
#include "http_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * @brief demux start line
 * @param [out] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_buf_size would always update
 */
int32 http_demux_start_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size);

/** 
 * @brief demux header
 * @param [in] p_header, header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_buf_size would always update
 * 2. header maybe empty line, it is valid.
 */
int32 http_demux_header(HTTP_HEADER* p_header, uint8* p_buf, int32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__HTTP_DEMUX_H__
