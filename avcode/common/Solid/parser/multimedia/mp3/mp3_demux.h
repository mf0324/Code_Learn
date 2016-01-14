/**
 * @file mp3_demux.h   MP3 Demux
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.09.10
 *
 *
 */

#ifndef __MP3_DEMUX_H__
#define __MP3_DEMUX_H__

#include <public/gen_int.h>
#include "mp3_demux_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief demux tag
 * @param [out] p_tag, tag
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1¡¢if return value is GEN_E_NEED_MORE_DATA, *p_demux_size means actual need size
 */
int32 mp3_demux_tag(MP3_ID3V2_TAG* p_tag, uint8* p_data, uint32 data_size, uint32* p_demux_size);

/**  
 * @brief demux frame
 * @param [out] p_frame, frame
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1¡¢if return value is GEN_E_NEED_MORE_DATA, *p_demux_size means actual need size
 */
int32 mp3_demux_frame(MP3_ID3V2_FRAME* p_frame, uint8* p_data, uint32 data_size, uint32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__MP3_DEMUX_H__
