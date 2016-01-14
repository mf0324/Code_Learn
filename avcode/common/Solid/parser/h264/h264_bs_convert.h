/**
 * @file h264_bs_convert.h  H264 ByteStream Convert
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.01.15
 *
 *
 */

#ifndef __H264_BS_CONVERT_H__
#define __H264_BS_CONVERT_H__

#include <public/gen_int.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief bs to mp4 format
 * @param [in] p_data bs data
 * @param [in] data_size bs data size
 * @return
 * 0: success
 * -1: fail
 */
int32 h264_bs_to_mp4_format(uint8* p_data, int32 data_size);

/**  
 * @brief mp4 format to bs
 * @param [in] p_data mp4 format data
 * @param [in] data_size mp4 format data size
 * @return
 * 0: success
 * -1: fail
 * -2: input data not enough
 */
int32 h264_mp4_format_to_bs(uint8* p_data, int32 data_size);


#ifdef __cplusplus
}
#endif

#endif ///__H264_BS_CONVERT_H__
