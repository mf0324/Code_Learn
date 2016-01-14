/**
 * @file mp4_mux.h   MP4 Mux
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.04.28
 *
 *
 */


#ifndef __MP4_MUX_H__
#define __MP4_MUX_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算File Type Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括:
 *   brand_count
 */
int32 mp4_calc_file_type_box_total_size(ISO_FILE_TYPE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成File Type Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括:
 *   box.siz
 *   major_brand
 *   minor_version
 *   p_brand
 *   brand_count
 */
int32 mp4_file_type_box_mux(ISO_FILE_TYPE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_movie_box_mux(ISO_MOVIE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Media Data Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_media_data_box_mux(ISO_MEDIA_DATA_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   creation_time
 *   modification_time
 *   time_scale
 *   duration
 *   next_track_id
 */
int32 mp4_movie_header_box_mux(ISO_MOVIE_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Track Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_track_box_mux(ISO_TRACK_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Track Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.version_flag.value
 *   creation_time
 *   modification_time
 *   track_id
 *   duration
 *   volume
 *   width
 *   height
 */
int32 mp4_track_header_box_mux(ISO_TRACK_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Media Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_media_box_mux(ISO_MEDIA_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Media Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   creation_time
 *   modification_time
 *   time_scale
 *   duration
 *   language
 */
int32 mp4_media_header_box_mux(ISO_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Handler Reference Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   name_size
 *   其中name_size必须包括结尾的'\0'
 */
int32 mp4_calc_handler_box_total_size(ISO_HANDLER_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Handler Reference Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   handler_type
 *   p_name
 *   name_size
 *   其中name_size必须包括结尾的'\0'
 */
int32 mp4_handler_box_mux(ISO_HANDLER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Media Information Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_media_info_box_mux(ISO_MEDIA_INFO_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Video Media Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mp4_video_media_header_box_mux(ISO_VIDEO_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Sound Media Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
int32 mp4_sound_media_header_box_mux(ISO_SOUND_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Data Information Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_data_info_box_mux(ISO_DATA_INFO_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Data Reference Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 *   p_entry
 *   其中根据p_entry的entry_type字段不同，url和urn里相应的字段也要填写，包括
 *   p_name
 *   name_size
 *   p_location
 *   location_size
 *   接口返回时会修改url和urn里的full_box.box.size字段
 */
int32 mp4_calc_data_ref_box_total_size(ISO_DATA_REF_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Data Reference Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 *   p_entry
 *   其中根据p_entry的entry_type字段不同，url和urn里相应的字段也要填写，包括
 *   full_box.box.size
 *   full_box.version_flag.value
 *   p_name
 *   name_size
 *   p_location
 *   location_size
 */
int32 mp4_data_ref_box_mux(ISO_DATA_REF_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Sample Table Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_sample_table_box_mux(ISO_SAMPLE_TABLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Time to Sample Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的只是box长度，不包括p_entry长度
 */
int32 mp4_calc_time_to_sample_box_size(ISO_TIME_TO_SAMPLE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Time to Sample Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_time_to_sample_box_total_size(ISO_TIME_TO_SAMPLE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Time to Sample Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 */
int32 mp4_time_to_sample_box_mux(ISO_TIME_TO_SAMPLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Visual Sample Description Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的只是box长度，不包括p_entry长度
 */
int32 mp4_calc_visual_sample_desc_box_size(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Visual Sample Description Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_visual_sample_desc_box_total_size(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Visual Sample Description Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 *   p_entry
 *   其中p_entry相应的字段也要填写，包括
 *   entry.box.type
 *   entry.data_ref_index
 *   width
 *   height
 *   compressor_name
 */
int32 mp4_visual_sample_desc_box_mux(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Audio Sample Description Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的只是box长度，不包括p_entry长度
 */
int32 mp4_calc_audio_sample_desc_box_size(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Audio Sample Description Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_audio_sample_desc_box_total_size(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Audio Sample Description Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 *   p_entry
 *   其中p_entry相应的字段也要填写，包括
 *   entry.box.type
 *   entry.data_ref_index
 *   sample_rate
 */
int32 mp4_audio_sample_desc_box_mux(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Sample Size Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的只是box长度，不包括p_entry长度
 */
int32 mp4_calc_sample_size_box_size(ISO_SAMPLE_SIZE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Sample Size Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   sample_size
 *   sample_count
 */
int32 mp4_calc_sample_size_box_total_size(ISO_SAMPLE_SIZE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Sample Size Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   sample_size
 *   sample_count
 */
int32 mp4_sample_size_box_mux(ISO_SAMPLE_SIZE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Sample to Chunk Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的只是box长度，不包括p_entry长度
 */
int32 mp4_calc_sample_to_chunk_box_size(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Sample to Chunk Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_sample_to_chunk_box_total_size(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Sample to Chunk Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 *   p_entry
 */
int32 mp4_sample_to_chunk_box_mux(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Chunk Offset Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的是box长度，不包括p_entry长度
 */
int32 mp4_calc_chunk_offset_box_size(ISO_CHUNK_OFFSET_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Chunk Offset Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_chunk_offset_box_total_size(ISO_CHUNK_OFFSET_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Chunk Offset Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 */
int32 mp4_chunk_offset_box_mux(ISO_CHUNK_OFFSET_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Sync Sample Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的是box长度，不包括p_entry长度
 */
int32 mp4_calc_sync_sample_box_size(ISO_SYNC_SAMPLE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Sync Sample Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   entry_count
 */
int32 mp4_calc_sync_sample_box_total_size(ISO_SYNC_SAMPLE_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Sync Sample Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   entry_count
 */
int32 mp4_sync_sample_box_mux(ISO_SYNC_SAMPLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Free Space Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_free_space_box_mux(ISO_FREE_SPACE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Extend Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_movie_extend_box_mux(ISO_MOVIE_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Extend Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   fragment_duration
 */
int32 mp4_movie_extend_header_box_mux(ISO_MOVIE_EXTEND_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Track Extend Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   track_id
 *   def_sample_desc_index
 *   def_sample_duration
 *   def_sample_size
 *   def_sample_flag
 */
int32 mp4_track_extend_box_mux(ISO_TRACK_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Fragment Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_movie_fragment_box_mux(ISO_MOVIE_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Fragment Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   seq_no
 */
int32 mp4_movie_fragment_header_box_mux(ISO_MOVIE_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Track Fragment Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_track_fragment_box_mux(ISO_TRACK_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Track Fragment Header Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   full_box.version_flag
 */
int32 mp4_calc_track_fragment_header_box_size(ISO_TRACK_FRAGMENT_HEADER_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Track Fragment Header Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   full_box.version_flag
 *   track_id
 *   base_data_offset
 *   sample_desc_index
 *   def_sample_duration
 *   def_sample_size
 *   def_sample_flag
 */
int32 mp4_track_fragment_header_box_mux(ISO_TRACK_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Track Fragment Run Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   full_box.version_flag
 *   sample_count
 */
int32 mp4_calc_track_fragment_run_box_size(ISO_TRACK_FRAGMENT_RUN_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Track Fragment Run Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   full_box.version_flag
 *   sample_count
 *   data_offset
 *   first_sample_flag
 *   p_sample->sample_duration
 *   p_sample->sample_size
 *   p_sample->sample_flag
 *   p_sample->sample_composition_time_offset
 *  其中有效部分字段由full_box.version_flag决定
 */
int32 mp4_track_fragment_run_box_mux(ISO_TRACK_FRAGMENT_RUN_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Fragment Random Access Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   box.size
 */
int32 mp4_movie_fragment_random_access_box_mux(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Track Fragment Random Access Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   mixture
 *   entry_num
 *   p_entry
 */
int32 mp4_calc_track_fragment_random_access_box_size(ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Track Fragment Random Access Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   track_id
 *   mixture
 *   entry_num
 *   p_entry
 */
int32 mp4_track_fragment_random_access_box_mux(ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 生成Movie Fragment Random Access Offset Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   size
 */
int32 mp4_movie_fragment_random_access_offset_box_mux(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);

/**  
 * @brief 计算Meta Box长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、这里计算的是box长度，不包括其它部分长度
 */
int32 mp4_calc_meta_box_size(ISO_META_BOX *p_box, uint32 *p_size);

/**  
 * @brief 计算Meta Box总长度
 * @param [in] p_box Box句柄
 * @param [out] p_size 总长度
 * @return
 * 0：成功
 * -1：参数错误
 * -3：参数取值错误
 * @remark
 * 1、外部填写字段包括：
 *   handler_box.full_box.box.size
 *   handler_box.name_size
 *   其中name_size必须包括结尾的'\0'
 */
int32 mp4_calc_meta_box_total_size(ISO_META_BOX *p_box, uint32 *p_size);

/**  
 * @brief 生成Meta Box
 * @param [in] p_box Box句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 输入长度/实际使用长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * -3：参数取值错误
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 * 2、外部填写字段包括：
 *   full_box.box.size
 *   handler_box
 *   另外handler_box相应字段也要填写，包括
 *   full_box.box.size
 *   handler_type
 *   p_name
 *   name_size
 *   其中name_size必须包括结尾的'\0'
 */
int32 mp4_meta_box_mux(ISO_META_BOX *p_box, uint8 *p_buf, int32 *p_buf_size);


#ifdef __cplusplus
}
#endif

#endif ///__MP4_MUX_H__
