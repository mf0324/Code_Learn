/**
 * @file mp4_aac_file.h   MP4 AAC File Format
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.26
 *
 *
 */


#ifndef __MP4_AAC_FILE_H__
#define __MP4_AAC_FILE_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"
#include "../../mpeg4/mpeg4_def.h"


/** Es Desc Box */
typedef struct tag_aac_es_desc_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** es descriptor */
	MPEG4_ES_DESCRIPTOR  es_desc;

} AAC_ES_DESC_BOX, *PAAC_ES_DESC_BOX;

/** AAC Sample Entry */
typedef struct tag_aac_sample_entry
{
	/** audio sample entry */
	ISO_AUDIO_SAMPLE_ENTRY audio;

	/** es desc box */
	AAC_ES_DESC_BOX es_box;

} AAC_SAMPLE_ENTRY, *PAAC_SAMPLE_ENTRY;

/** AAC Sample Description Box */
typedef struct tag_aac_sample_desc_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	AAC_SAMPLE_ENTRY* p_entry;

} AAC_SAMPLE_DESC_BOX, *PAAC_SAMPLE_DESC_BOX;


#endif ///__MP4_AAC_FILE_H__
