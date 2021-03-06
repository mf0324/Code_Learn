/**
 * @file h264_def.h   H264 Nalu Define
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.19
 *
 *
 */

#ifndef __H264_DEF_H__
#define __H264_DEF_H__

#include <public/gen_int.h>

#include "h264_public.h"

/** SPS */
typedef struct tag_h264_sps
{
	/** profile */
	int32  profile;
	/** level */
	int32  level;
	/** sps id */
	int32  id;

	/** chroma */
	int32  chroma;
	/** width, unit: pixel */
	int32  width;
	/** height, unit: pixel */
	int32  height;

	/** *@^$(# */
	int32  log2_max_frame_num_minus4;
	/** frame_mbs_only_flag */
	int32  frame_mbs_only_flag;

} H264_SPS, *PH264_SPS;

/** PPS */
typedef struct tag_h264_pps
{
	/** pps id */
	int32  pps_id;
	/** sps id */
	int32  sps_id;

} H264_PPS, *PH264_PPS;

/** SEI */
typedef struct tag_h264_sei
{
	/** field_mode */
	int32  field_mode;

} H264_SEI, *PH264_SEI;

/** Slice Header */
typedef struct tag_h264_slice_header
{
	/** first_mb_in_slice */
	int32  first_mb_in_slice;
	/** type */
	int32  type;
	/** pps_id */
	int32  pps_id;
	/** frame_num */
	int32  frame_num;

	/** field_pic_flag */
	int32  field_pic_flag;
	/** bottom_field_flag */
	int32  bottom_field_flag;

} H264_SLICE_HEADER, *PH264_SLICE_HEADER;

/** ByteStream */
typedef struct tag_h264_bs_pack
{
	/** 起始地址 */
	uint8  *p_start;
	/** NALU Type */
	int32  nalu_type;

	/** 是否已知sps */
	int32  b_sps;

	/** sps */
	H264_SPS  sps;
	/** pps */
	H264_PPS  pps;
	/** sei */
	H264_SEI  sei;
	/** slice header */
	H264_SLICE_HEADER slice_header;

} H264_BS_PACK, *PH264_BS_PACK;

/** Nalu Pack */
typedef struct tag_h264_nalu_pack
{
	/** start pos */
	uint8* p_start;
	/** Nalu Type */
	int32  nalu_type;

	/** sps */
	int32  b_sps;

	/** sps */
	H264_SPS  sps;
	/** pps */
	H264_PPS  pps;
	/** sei */
	H264_SEI  sei;
	/** slice header */
	H264_SLICE_HEADER slice_header;

} H264_NALU_PACK, *PH264_NALU_PACK;


#endif ///__H264_DEF_H__
