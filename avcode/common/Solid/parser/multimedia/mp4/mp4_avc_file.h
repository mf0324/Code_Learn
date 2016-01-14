/**
 * @file mp4_avc_file.h   MP4 AVC File Format
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.19
 *
 *
 */

#ifndef __MP4_AVC_FILE_H__
#define __MP4_AVC_FILE_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"

/** SPS Record */
typedef struct tag_avc_sps_record
{
	/** nalu len */
	uint16  nalu_len;
	/** nalu */
	uint8*  p_nalu;

} AVC_SPS_RECORD, *PAVC_SPS_RECORD;

/** PPS Record */
typedef struct tag_avc_pps_record
{
	/** nalu len */
	uint16  nalu_len;
	/** nalu */
	uint8*  p_nalu;

} AVC_PPS_RECORD, *PAVC_PPS_RECORD;

/** AVC Decoder Config Record */
typedef struct tag_avc_dec_config_record
{
	/** config version */
	uint8   config_version;
	/** profile */
	uint8   profile;
	/** profile compatibility */
	uint8   profile_compat;
	/** level */
	uint8   level;

	/** length size - 1 */
	uint32  len_size_minus_1;

	/** sps count */
	uint32  sps_count;
	/** sps record */
	AVC_SPS_RECORD* p_sps;

	/** pps count */
	uint32  pps_count;
	/** pps record */
	AVC_PPS_RECORD* p_pps;

} AVC_DEC_CONFIG_RECORD, *PAVC_DEC_CONFIG_RECORD;

/** AVC Config Box */
typedef struct tag_avc_config_box
{
	/** box */
	ISO_BOX box;

	/** dec config */
	AVC_DEC_CONFIG_RECORD  record;

	/** dec config buf */
	uint8*  p_dec_config;
	/** dec config size */
	uint32  dec_config_size;

} AVC_CONFIG_BOX, *PAVC_CONFIG_BOX;

/** AVC Sample Entry */
typedef struct tag_avc_sample_entry
{
	/** visual sample entry */
	ISO_VISUAL_SAMPLE_ENTRY visual;

	/** config */
	AVC_CONFIG_BOX config;

} AVC_SAMPLE_ENTRY, *PAVC_SAMPLE_ENTRY;

/** AVC Sample Description Box */
typedef struct tag_avc_sample_desc_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	AVC_SAMPLE_ENTRY entry;

} AVC_SAMPLE_DESC_BOX, *PAVC_SAMPLE_DESC_BOX;


#endif ///__MP4_AVC_FILE_H__
