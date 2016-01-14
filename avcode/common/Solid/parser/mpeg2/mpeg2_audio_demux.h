/**
 * @file mpeg2_audio_demux.h   MPEG2 Audio Demux
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.07.15
 *
 *
 */

#ifndef __MPEG2_AUDIO_DEMUX_H__
#define __MPEG2_AUDIO_DEMUX_H__

#include <public/gen_int.h>

//#include "mpeg2_public.h"
#include "mpeg2_audio_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief init
 * @param [in] p_header, header
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_audio_header_init(MPEG2_AUDIO_HEADER* p_header);

/**  
 * @brief demux
 * @param [out] p_header, header
 * @param [in]  p_data, input data
 * @param [in]  data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 */
int32 mpeg2_audio_header_demux(MPEG2_AUDIO_HEADER* p_header, uint8* p_data, int32 data_size, int32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_AUDIO_DEMUX_H__
