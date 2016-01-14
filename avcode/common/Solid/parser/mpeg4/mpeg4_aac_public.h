/**
 * @file mpeg4_aac_public.h   MPEG4 AAC Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.06.26
 *
 *
 */

#ifndef __MPEG4_AAC_PUBLIC_H__
#define __MPEG4_AAC_PUBLIC_H__

#include <public/gen_int.h>
#include "mpeg4_def.h"

/** audio specific config */
typedef struct tag_mpeg4_audio_spec_config
{
	/** audio object type */
	int32  audio_obj_type;
	/** sample frequency index */
	int32  sample_freq_index;
	/** sample frequency */
	int32  sample_freq;
	/** chn config */
	int32  chn_config;

	/** ext */
	int32  b_ext;
	/** ext audio object type */
	int32  ext_audio_obj_type;
	/** ext sample frequency index */
	int32  ext_sample_freq_index;
	/** ext sample frequency */
	int32  ext_sample_freq;
	/** ext chn config */
	int32  ext_chn_config;
	/** sync ext type */
	int32  sync_ext_type;
	/** sbr present flag */
	int32  sbr_present_flag;
	/** ps present flag */
	int32  ps_present_flag;

} MPEG4_AUDIO_SPEC_CONFIG, *PMPEG4_AUDIO_SPEC_CONFIG;

/** Audio Decoder Specific Info */
typedef struct tag_mpeg4_adec_spec_info
{
	/** base */
	MPEG4_BASE_DESCRIPTOR base;

	/** audio spec config */
	MPEG4_AUDIO_SPEC_CONFIG audio_spec_config;
	/** spec config */
	uint8* p_spec_config;
	/** spec config size */
	int32  spec_config_size;

} MPEG4_ADEC_SPEC_INFO, *PMPEG4_ADEC_SPEC_INFO;


#endif ///__MPEG4_AAC_PUBLIC_H__
