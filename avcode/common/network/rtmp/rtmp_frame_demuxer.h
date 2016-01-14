/**
 * @file rtmp_frame_demuxer.h   RTMP Frame Demuxer
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.01
 *
 *
 */

#ifndef __RTMP_FRAME_DEMUXER_H__
#define __RTMP_FRAME_DEMUXER_H__


#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

#include "rtmp_public.h"

/** rtmp frame demuxer handle */
typedef void*  RTMP_FRAME_DEMUXER_H;


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
RTMP_FRAME_DEMUXER_H rtmp_frame_demuxer_create();

/**  
 * @brief destroy
 * @param [in] h, demuxer
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_frame_demuxer_destroy(RTMP_FRAME_DEMUXER_H h);

/**  
 * @brief init
 * @param [in] h, demuxer
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_frame_demuxer_init(RTMP_FRAME_DEMUXER_H h);

/**  
 * @brief deinit
 * @param [in] h, demuxer
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_frame_demuxer_deinit(RTMP_FRAME_DEMUXER_H h);

/**  
 * @brief reset
 * @param [in] h, demuxer
 * @return
 * 0: success
 * other: fail
 */
int32 rtmp_frame_demuxer_reset(RTMP_FRAME_DEMUXER_H h);

/**  
 * @brief demux frame
 * @param [in] h, demuxer
 * @param [in] p_msg, msg
 * @param [out] p_frame, output frame
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 rtmp_frame_demux(RTMP_FRAME_DEMUXER_H h, RTMP_MSG* p_msg, SP_MEDIA_FRAME* p_frame);



#ifdef __cplusplus
}
#endif

#endif ///__RTMP_FRAME_DEMUXER_H__
