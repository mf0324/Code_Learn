/**
 * @file mp4_file_demuxer.h   MP4 File Demuxer
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.03.13
 *
 *
 */


#ifndef __MP4_FILE_DEMUXER_H__
#define __MP4_FILE_DEMUXER_H__

#include <public/gen_int.h>
#include <public/gen_file.h>
#include "mp4_public.h"
#include "mp4_file_tree.h"
#include "mp4_file_stat.h"

/** MP4 File Demuxer Handle */
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
 * other: fail
 */
int32 mp4_file_demuxer_destroy(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief init
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_demuxer_init(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief deinit
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_demuxer_deinit(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief reset
 * @param [in] demuxer_h, demuxer handle
 * @return
 * 0: success
 * other: fail
 */
int32 mp4_file_demuxer_reset(MP4_FILE_DEMUXER_H demuxer_h);

/**  
 * @brief demux
 * @param [in] demuxer_h, demuxer handle
 * @param [in] p_tree, file tree
 * @param [in] file_h, file handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_demuxer_demux(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_TREE* p_tree, GEN_FILE_H file_h);

/**  
 * @brief get file stat
 * @param [in] demuxer_h, demuxer handle
 * @param [out] p_stat, stat info
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 mp4_file_demuxer_get_file_stat(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_STAT* p_stat);



#ifdef __cplusplus
}
#endif

#endif ///__MP4_FILE_DEMUXER_H__
