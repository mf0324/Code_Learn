/**
 * @file mpeg4_def.h   MPEG4 Public Define
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.10
 *
 *
 */


#ifndef __MPEG4_DEF_H__
#define __MPEG4_DEF_H__

#include <public/gen_int.h>
#include "mpeg4_video_def.h"


/** size encoding */
typedef struct tag_mpeg4_size_encoding
{
	/** value */
	uint32 value;
	/** bit size */
	uint32 bit_size;

} MPEG4_SIZE_ENCODING, *PMPEG4_SIZE_ENCODING;

/** Base Descriptor */
typedef struct tag_mpeg4_base_descriptor
{
	/** tag */
	uint32 tag;

	/** size */
	MPEG4_SIZE_ENCODING size;

} MPEG4_BASE_DESCRIPTOR, *PMPEG4_BASE_DESCRIPTOR;

/** Video Decoder Specific Info */
typedef struct tag_mpeg4_vdec_spec_info
{
	/** vos */
	MPEG4_VOS vos;
	/** visaul object */
	MPEG4_VISUAL_OBJECT visual_obj;
	/** video object */
	MPEG4_VIDEO_OBJECT video_obj;
	/** vol */
	MPEG4_VOL vol;

} MPEG4_VDEC_SPEC_INFO, *PMPEG4_VDEC_SPEC_INFO;

/** Decoder Config Descriptor */
typedef struct tag_mpeg4_dec_config_descriptor
{
	/** base */
	MPEG4_BASE_DESCRIPTOR base;

	/** object type indication */
	uint16 obj_type;
	/** stream type */
	uint16 stream_type;
	/** buf size */
	uint32 buf_size;
	/** max bitrate */
	uint32 max_bitrate;
	/** avg bitrate */
	uint32 avg_bitrate;

	/** vdec spec info */
	//MPEG4_VDEC_SPEC_INFO* p_vdec_spec_info;
	uint8* p_vdec_spec_info;
	/** vdec spec info size */
	int32  vdec_spec_info_size;

} MPEG4_DEC_CONFIG_DESCRIPTOR, *PMPEG4_DEC_CONFIG_DESCRIPTOR;

/** SL Config Descriptor */
typedef struct tag_mpeg4_sl_config_descriptor
{
	/** base */
	MPEG4_BASE_DESCRIPTOR base;

	/** predefined */
	uint32 predefine;

} MPEG4_SL_CONFIG_DESCRIPTOR, *PMPEG4_SL_CONFIG_DESCRIPTOR;

/** ES Descriptor */
typedef struct tag_mpeg4_es_descriptor
{
	/** base */
	MPEG4_BASE_DESCRIPTOR base;

	/** es id */
	uint32 es_id;

	/** stream dependence flag */
	uint8  stream_depend_flag;
	/** url flag */
	uint8  url_flag;
	/** OCR stream flag */
	uint8  ocr_stream_flag;
	/** stream priority */
	uint8  stream_priority;

	/** decoder config descriptor */
	MPEG4_DEC_CONFIG_DESCRIPTOR dec_config;
	/** SL config descriptor */
	MPEG4_SL_CONFIG_DESCRIPTOR sl_config;

} MPEG4_ES_DESCRIPTOR, *PMPEG4_ES_DESCRIPTOR;


#endif ///__MPEG4_DEF_H__
