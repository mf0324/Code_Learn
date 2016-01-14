/**
 * @file mpeg2_ps_param.h   MPEG2 PS Param
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.16
 *
 *
 */


#ifndef __MPEG2_PS_PARAM_H__
#define __MPEG2_PS_PARAM_H__

#include <public/gen_int.h>

/** PS头部参数 */
typedef struct tag_mpeg2_ps_header_param
{
	/** sys_clock_ref_base(low 32 bit) */
	uint32 sys_clock_ref_base_low;
	/** sys_clock_ref_base(high 32 bit) */
	uint32 sys_clock_ref_base_high;
	/** sys_clock_ref_ext(9 bit) */
	uint32 sys_clock_ref_ext;

	/** program_mux_rate(22 bit) */
	uint32 program_mux_rate;

	/** pack_stuff_len(3 bit) */
	uint32 pack_stuff_len;

} MPEG2_PS_HEADER_PARAM, *PMPEG2_PS_HEADER_PARAM;

/** PS流参数 */
typedef struct tag_mpeg2_ps_stream_param
{
	/** stream_id */
	uint8  stream_id;
	/** p-std_buf_bound_scale(1 bit) */
	uint8  pstd_buf_bound_scale;
	/** p-std_buf_size_bound(13 bit) */
	uint16 pstd_buf_size_bound;

} MPEG2_PS_STREAM_PARAM, *PMPEG2_PS_STREAM_PARAM;

/** 系统头部参数 */
typedef struct tag_mpeg2_sys_header_param
{
	/** rate_bound(22 bit) */
	uint32 rate_bound;
	/** audio_bound(6 bit) */
	uint32 audio_bound;
	/** video_bound(5 bit) */
	uint32 video_bound;

	/** fixed_flag(1 bit) */
	int32  b_fixed;
	/** csps_flag(1 bit) */
	int32  b_csps;
	/** sys_audio_lock_flag(1 bit) */
	int32  b_sys_audio_lock;
	/** sys_video_lock_flag(1 bit) */
	int32  b_sys_video_lock;

	/** stream param */
	MPEG2_PS_STREAM_PARAM *p_stream;
	/** stream num */
	int32  stream_num;

} MPEG2_SYS_HEADER_PARAM, *PMPEG2_SYS_HEADER_PARAM;


#endif ///__MPEG2_PS_PARAM_H__
