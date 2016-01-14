/**
 * @file flv_mux.h  FLV Mux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.31
 *
 *
 */

#ifndef __FLV_MUX_H__
#define __FLV_MUX_H__

#include <public/gen_int.h>
#include "flv_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief flv header
 * @param [in] p_header, flv header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill data:
 *   type_flag
 */
int32 flv_mux_header(FLV_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief tag header
 * @param [in] p_header, tag header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill data:
 *   tag_type
 *   data_size
 *   timestamp
 *   stream_id
 */
int32 flv_mux_tag_header(FLV_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief mux video tag header
 * @param [in] p_header, tag header
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf total size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size means actual require size
 * 2. caller must fill following member of p_header:
 *   frame_type
 *   codec_id
 *   avc_pack_type
 *   cts_offset
 */
int32 flv_mux_video_tag_header(FLV_VIDEO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);

#ifdef __cplusplus
}
#endif

#endif ///__FLV_MUX_H__
