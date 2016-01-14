/**
 * @file ps_frame_parser.h   PS Frame Parser
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.15
 *
 *
 */

#ifndef __PS_FRAME_PARSER_H__
#define __PS_FRAME_PARSER_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_frame.h>

/** ps frame parser handle */
typedef void*   PS_FRAME_PARSER_H;


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
PS_FRAME_PARSER_H ps_frame_parser_create();

/**  
 * @brief destroy
 * @param [in] h, parser handle
 * @return
 * 0: success
 * other: fail
 */
int32 ps_frame_parser_destroy(PS_FRAME_PARSER_H h);

/**  
 * @brief init
 * @param [in] h, parser handle
 * @return
 * 0: success
 * other: fail
 */
int32 ps_frame_parser_init(PS_FRAME_PARSER_H h);

/**  
 * @brief deinit
 * @param [in] h, parser handle
 * @return
 * 0: success
 * other: fail
 */
int32 ps_frame_parser_deinit(PS_FRAME_PARSER_H h);

/**  
 * @brief reset
 * @param [in] h, parser handle
 * @return
 * 0: success
 * other: fail
 */
int32 ps_frame_parser_reset(PS_FRAME_PARSER_H h);

/**  
 * @brief set max video frame size
 * @param [in] h, parser handle
 * @param [in] max_size, max video frame size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1、default max video frame size is 256*1024 after init
 */
int32 ps_frame_parser_set_max_video_frame_size(PS_FRAME_PARSER_H h, int32 max_size);

/**  
 * @brief set video frame offset
 * @param [in] h, parser handle
 * @param [in] offset, offset
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1、default video frame offset is 0 after init
 */
int32 ps_frame_parser_set_video_frame_offset(PS_FRAME_PARSER_H h, int32 offset);


/**  
 * @brief set max audio frame size
 * @param [in] h, parser handle
 * @param [in] max_size, max audio frame size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1、default max audio frame size is 16*1024 after init
 */
int32 ps_frame_parser_set_max_audio_frame_size(PS_FRAME_PARSER_H h, int32 max_size);

/**  
 * @brief set audio frame offset
 * @param [in] h, parser handle
 * @param [in] offset, offset
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1、default audio frame offset is 0 after init
 */
int32 ps_frame_parser_set_audio_frame_offset(PS_FRAME_PARSER_H h, int32 offset);

/**  
 * @brief demux frame
 * @param [in] h, parser handle
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_frame, output frame
 * @param [out] p_demux_size, actually demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_parse_size return actually parse size
 * 2. p_data can be null, means continue parse inner buf data
 * 3. if return success, app should call this interface again until it return GEN_E_NEED_MORE_DATA
 */
int32 ps_frame_parser_demux_frame(PS_FRAME_PARSER_H h, uint8* p_data, int32 data_size, SP_MEDIA_FRAME* p_frame, int32* p_demux_size);


#ifdef __cplusplus
}
#endif

#endif ///__PS_FRAME_PARSER_H__
