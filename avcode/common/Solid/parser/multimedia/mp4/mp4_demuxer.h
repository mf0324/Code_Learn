/**
 * @file mp4_demuxer.h   MP4 File Demuxer
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.03.13
 *
 *
 */


#ifndef __MP4_DEMUXER_H__
#define __MP4_DEMUXER_H__

#include <public/gen_int.h>
#include "mp4_public.h"
#include "mp4_file_info.h"

/** MP4 Demuxer Handle */
typedef void*  MP4_FILE_DEMUXER_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not NULL: success
 * NULL: fail
 */
MP4_FILE_DEMUXER_H mp4_file_demuxer_create();

/**  
 * @brief destroy
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_demuxer_destroy(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief init
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_demuxer_init(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief deinit
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_demuxer_deinit(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief reset
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_demuxer_reset(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief demux
 * @param [in] demuxer_h, demuxer handle
 * @param [in] p_file_path, file path
 * @param [in] file_path_size, file path size
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: wrong file format
 * -4: can not open file
 * -5: file size too large
 * @remark
 */
int32 mp4_file_demuxer_demux(MP4_FILE_DEMUXER_H demuxer_h, int8* p_file_path, int32 file_path_size);

/**  
 * @brief get file stat info
 * @param [in] demuxer_h, demuxer handle
 * @param [out] p_stat, stat info
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: file not demux
 * @remark
 */
int32 mp4_file_demuxer_get_file_stat_info(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_STAT* p_stat);

/**  
 * @brief get track stat info
 * @param [in] demuxer_h, demuxer handle
 * @param [out] p_stat, track stat info
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: file not demux
 * @remark
 * 1. you must refetch the stat info after each demux!
 */
int32 mp4_file_demuxer_get_track_stat_info(MP4_FILE_DEMUXER_H demuxer_h, MP4_TRACK_STAT* p_stat);

/**  
 * @brief get video track
 * @param [in] demuxer_h, demuxer handle
 * @param [out] p_track, track
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: file not demux
 * -4: track not exist
 * @remark
 * 1. you must refetch the stat info after each demux!
 */
int32 mp4_file_demuxer_get_video_track(MP4_FILE_DEMUXER_H demuxer_h, MP4_VIDEO_TRACK* p_track);

/**  
 * @brief get audio track
 * @param [in] demuxer_h, demuxer handle
 * @param [out] p_track, track
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: file not demux
 * -4: track not exist
 * @remark
 * 1. you must refetch the stat info after each demux!
 */
int32 mp4_file_demuxer_get_audio_track(MP4_FILE_DEMUXER_H demuxer_h, MP4_AUDIO_TRACK* p_track);


#ifdef __cplusplus
}
#endif

#endif ///__MP4_FILE_DEMUXER_H__
