/**
 * @file flv_public.h   FLV Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.31
 *
 *
 */

#ifndef __FLV_PUBLIC_H__
#define __FLV_PUBLIC_H__

#include <public/gen_int.h>



/** Header */
typedef struct tag_flv_header
{
	/** signature & version */
	uint32 sig_ver;
	/** type flag */
	uint8  type_flag;
	/** padding */
	uint8  padding[3];
	/** data offset */
	uint32 header_size;

} FLV_HEADER, *PFLV_HEADER;

/** Tag Header */
typedef struct tag_flv_tag_header
{
	/** filter & tag type */
	uint8  tag_type;
	/** padding */
	uint8  padding[3];
	/** data size */
	uint32 data_size;
	/** timestamp */
	uint32 ts;
	/** stream id */
	uint32 stream_id;

} FLV_TAG_HEADER, *PFLV_TAG_HEADER;

/** Video Tag Header */
typedef struct tag_flv_video_tag_header
{
	/** frame type */
	uint8  frame_type;
	/** codec id */
	uint8  codec_id;
	/** avc pack type */
	uint8  avc_pack_type;
	/** padding */
	uint8  padding;
	/** cts offset */
	uint32 cts_offset;

} FLV_VIDEO_TAG_HEADER, *PFLV_VIDEO_TAG_HEADER;

/** Audio Tag Header */
typedef struct tag_flv_audio_tag_header
{
	/** format */
	uint8  format;
	/** sample rate */
	uint8  sample_rate;
	/** sample bit */
	uint8  sample_bit;
	/** chn */
	uint8  chn;
	/** aac pack type */
	uint8  aac_pack_type;
	/** padding */
	uint8  padding[3];

} FLV_AUDIO_TAG_HEADER, *PFLV_AUDIO_TAG_HEADER;

#endif ///__FLV_PUBLIC_H__
