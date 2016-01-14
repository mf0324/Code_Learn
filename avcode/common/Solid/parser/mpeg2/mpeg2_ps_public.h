/**
 * @file mpeg2_ps_public.h  TS Stream Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.15
 *
 *
 */

#ifndef __MPEG2_PS_PUBLIC_H__
#define __MPEG2_PS_PUBLIC_H__

#include <public/gen_int.h>
//#include "mpeg2_public.h"
#include "mpeg2_pes_parser.h"

/** max pat sub section num */
//#define TS_STREAM_MAX_PAT_SUB_SECTION                      (32)


/** pack header */
typedef struct tag_mpeg2_ps_pack_header
{
	/** sys_clock_ref_base */
	int64  sys_clock_ref_base;
	/** sys_clock_ref_ext */
	int32  sys_clock_ref_ext;

	/** program_mux_rate */
	int32  program_mux_rate;

} MPEG2_PS_PACK_HEADER, *PMPEG2_PS_PACK_HEADER;

/** sys header */
typedef struct tag_mpeg2_ps_sys_header
{
	/** rate_bound */
	int32  rate_bound;
	/** audio_bound */
	int32  audio_bound;
	/** video_bound */
	int32  video_bound;

	/** fixed_flag */
	int32  b_fix_rate;
	/** csps_flag */
	int32  b_csps;

	/** sys_audio_lock_flag */
	int32  b_sys_audio_lock;
	/** sys_video_lock_flag */
	int32  b_sys_video_lock;

	/** stream_id 集合 */
	uint8  *p_stream_id;
	/** stream_id 长度 */
	int32  stream_id_len;
	/** stream_id 个数 */
	int32  stream_id_num;

} MPEG2_PS_SYS_HEADER, *PMPEG2_PS_SYS_HEADER;

/** ps stream */
typedef struct tag_mpeg2_ps_stream
{
	/** stream_id */
	uint8  stream_id;
	/** P-STD_buffer_bound_scale */
	uint8  std_buf_bound_scale;
	/** P-STD_buffer_size_bound */
	uint16 std_buf_size_bound;

} MPEG2_PS_STREAM, *PMPEG2_PS_STREAM;


/** ps pack */
typedef struct tag_mpeg2_ps_pack
{
	/** start pos */
	uint8*  p_start;
	/** start code */
	int32   start_code;

	/** pack header */
	MPEG2_PS_PACK_HEADER  pack_header;
	/** system header */
	MPEG2_PS_SYS_HEADER   sys_header;
	/** ps map */
	MPEG2_PS_MAP          ps_map;
	/** pes pack */
	MPEG2_PES_PACK        pes_pack;
	/** pes header */
	MPEG2_PES_HEADER      pes_header;

} MPEG2_PS_PACK, *PMPEG2_PS_PACK;



#endif /// __MPEG2_PS_PUBLIC_H__
