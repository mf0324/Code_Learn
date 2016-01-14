/**
 * @file mpeg2_ves_parser.h   MPEG2 VES Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.05.15
 *
 *
 */


#ifndef __MPEG2_VES_PARSER_H__
#define __MPEG2_VES_PARSER_H__

#include <public/gen_int.h>

#include "mpeg2_public.h"

/** 视频ES SEQ */
typedef struct tag_mpeg2_ves_seq
{
	/** profile */
	int32  profile;
	/** level */
	int32  level;

	/** 图像制式 */
	int32  video_format;
	/** 图像格式 */
	int32  chroma_format;

	/** 图像宽度，单位：像素 */
	int32  width;
	/** 图像高度，单位：像素 */
	int32  height;
	/** 图像显示宽度，单位：像素 */
	int32  display_width;
	/** 图像显示高度，单位：像素 */
	int32  display_height;
	/** 图像比例 */
	int32  aspect_ratio;

	/** 帧率 */
	int32  frame_rate;
	/** 比特率，单位：bps */
	int32  bit_rate;
	/** vbv buffer size */
	int32  vbv_buf_size;

	/** progressive_sequence */
	int32  progs_seq;
	/** low delay */
	int32  b_low_delay;
	
} MPEG2_VES_SEQ, *PMPEG2_VES_SEQ;

/** 视频ES PIC */
typedef struct tag_mpeg2_ves_pic
{
	/** 帧类型 */
	int32  type;
	/** temporal reference */
	int32  temp_ref;

	/** 是否逐行显示 */
	int32  b_progressive;
	/** intra dc precision */
	int32  dc_precision;
	/** picture structure */
	int32  pic_struct;
	
} MPEG2_VES_PIC, *PMPEG2_VES_PIC;

/** 视频ES状态 */
typedef struct tag_mpeg2_ves_state
{
	/** 起始码地址 */
	uint8  *p_start;
	/** 当前起始码 */
	int32  start_code;
	/** 扩展ID */
	int32  ext_id;

	/** seq */
	MPEG2_VES_SEQ seq;
	/** pic */
	MPEG2_VES_PIC pic;

} MPEG2_VES_STATE, *PMPEG2_VES_STATE;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 初始化MPEG2视频ES流状态
 *
 * @param [in] p_state 视频ES状态句柄
 * @return
 * 0：成功
 * -1：参数错误
 */
int32 mpeg2_ves_init(MPEG2_VES_STATE *p_state);

/**  
 * @brief 解析MPEG2视频ES流
 *
 * @param [out] p_state 视频ES状态句柄
 * @param [in] p_buf 流数据地址
 * @param [in] buf_len 流数据长度
 * @param [out] p_parse_len 已解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入数据不足以解析
 */
int32 mpeg2_ves_parse(MPEG2_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_VES_PARSER_H__
