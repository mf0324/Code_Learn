/**
 * @file mp4_demux.h   MP4 Demux
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.19
 *
 *
 */


#ifndef __MP4_DEMUX_H__
#define __MP4_DEMUX_H__

#include <public/gen_int.h>
#include "mp4_demux_public.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**  
 * @brief demux box
 * @param [out] p_box, box
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * -1: invalid param
 * -2: input data not enough
 * -3: invalid value
 * @remark
 * 1、actual demux size is always update whether demux success or not
 */
int32 mp4_demux_box(MP4_BOX* p_box, uint8* p_data, uint32 data_size, uint32* p_demux_size);

/**  
 * @brief demux full box
 * @param [out] p_box, box
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * -1: invalid param
 * -2: input data not enough
 * -3: invalid value
 * @remark
 */
int32 mp4_demux_full_box(MP4_BOX* p_box, uint8* p_data, uint32 data_size, uint32* p_demux_size);

/**  
 * @brief 解析Movie Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_movie_header_box(ISO_MOVIE_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Track Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_track_header_box(ISO_TRACK_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Media Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_media_header_box(ISO_MEDIA_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Handler Reference Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_handler_box(ISO_HANDLER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Visual Sample Desc Box
 * @param [in/out] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * -4：p_entry长度不足
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、p_box的下列字段由外部填写
 *    entry_count
 *    p_entry
 *    在返回时entry_count被改为实际的个数
 */
int32 mp4_demux_visual_sample_desc_box(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Audio Sample Desc Box
 * @param [in/out] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * -4：p_entry长度不足
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 * 2、p_box的下列字段由外部填写
 *    entry_count
 *    p_entry
 *    在返回时entry_count被改为实际的个数
 */
int32 mp4_demux_audio_sample_desc_box(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief demux stts box
 * @param [in] p_box, box
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * -1: invalid param
 * -2: input data not enough to demux
 * -3: wrong param
 * @remark
 */
int32 mp4_demux_time_to_sample_box(ISO_TIME_TO_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief demux ctts box
 * @param [in] p_box, box
 * @param [in] p_buf, input data
 * @param [in] buf_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * -1: invalid param
 * -2: input data not enough to demux
 * -3: wrong param
 * @remark
 */
int32 mp4_demux_comp_time_to_sample_box(ISO_COMP_TIME_TO_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Sample Size Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_sample_size_box(ISO_SAMPLE_SIZE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Sample to Chunk Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_sample_to_chunk_box(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Chunk Offset Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_chunk_offset_box(ISO_CHUNK_OFFSET_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);

/**  
 * @brief 解析Sync Sample Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in] buf_size 数据长度
 * @param [out] p_demux_size 实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足以解析
 * -3：参数取值错误
 * @remark
 * 1、无论解析成功与否，均会更新实际解析长度
 */
int32 mp4_demux_sync_sample_box(ISO_SYNC_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__MP4_DEMUX_H__
