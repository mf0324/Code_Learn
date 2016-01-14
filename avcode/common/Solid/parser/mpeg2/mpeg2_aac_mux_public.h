/**
 * @file mpeg2_aac_mux_public.h   MPEG2 AAC Mux Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.11.21
 *
 *
 */

#ifndef __MPEG2_AAC_MUX_PUBLIC_H__
#define __MPEG2_AAC_MUX_PUBLIC_H__

#include <public/gen_int.h>

/** adts header */
typedef struct tag_mpeg2_aac_adts_header
{
	/** protection_absent */
	int32 protection_absent;
	/** profile */
	int32 profile;
	/** sample_freq_index */
	int32 sample_freq_index;
	/** private_bit */
	int32 b_private;
	/** chn_config */
	int32 chn_config;
	/** original */
	int32 b_original;
	/** home */
	int32 home;

	/** copyright_id */
	int32 b_copyright_id;
	/** copyright_id_start */
	int32 b_copyright_id_start;
	/** frame_len */
	int32 frame_len;
	/** buf_fullness */
	int32 buf_fullness;
	/** raw_data_blocks */
	int32 raw_data_block;

} MPEG2_AAC_ADTS_HEADER, *PMPEG2_AAC_ADTS_HEADER;


#endif ///__MPEG2_AAC_MUX_PUBLIC_H__
