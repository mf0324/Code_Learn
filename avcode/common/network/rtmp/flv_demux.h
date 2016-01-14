/**
 * @file flv_demux.h  FLV Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.01
 *
 *
 */

#ifndef __FLV_DEMUX_H__
#define __FLV_DEMUX_H__

#include <public/gen_int.h>
#include "flv_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief tag header
 * @param [in] p_header, tag header
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data total size/actual parse size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_demux_tag_header(FLV_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);

/**  
 * @brief video tag header
 * @param [in] p_header, video tag header
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data total size/actual parse size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_demux_video_tag_header(FLV_VIDEO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);


/**  
 * @brief audio tag header
 * @param [in] p_header, audio tag header
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data total size/actual parse size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 flv_demux_audio_tag_header(FLV_AUDIO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__FLV_DEMUX_H__
