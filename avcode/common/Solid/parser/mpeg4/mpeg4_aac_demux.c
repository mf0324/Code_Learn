
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <public/bit_stream_reader.h>
//#include <util/log_debug.h>

#include "mpeg4_aac_const.h"
#include "mpeg4_public.h"
#include "mpeg4_sys_demux.h"
#include "mpeg4_aac_demux.h"


/////////////////////////////// Inner Interface ///////////////////////////////
int32 mpeg4_audio_get_sample_freq(int32 sample_freq_index, int32* p_sample_freq)
{
	switch( sample_freq_index )
	{
	case MPEG4_AAC_SAMPLE_FREQ_INDEX_96000:
		{
			*p_sample_freq = 96000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_88200:
		{
			*p_sample_freq = 88200;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_64000:
		{
			*p_sample_freq = 64000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_48000:
		{
			*p_sample_freq = 48000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_44100:
		{
			*p_sample_freq = 44100;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_32000:
		{
			*p_sample_freq = 32000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_24000:
		{
			*p_sample_freq = 24000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_22050:
		{
			*p_sample_freq = 22050;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_16000:
		{
			*p_sample_freq = 16000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_12000:
		{
			*p_sample_freq = 12000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_11025:
		{
			*p_sample_freq = 11025;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_8000:
		{
			*p_sample_freq = 8000;
		}
		break;

	case MPEG4_AAC_SAMPLE_FREQ_INDEX_7350:
		{
			*p_sample_freq = 7350;
		}
		break;

	default:
		{
			*p_sample_freq = 0;
		}
	}

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg4_audio_spec_config_demux(MPEG4_AUDIO_SPEC_CONFIG* p_config, uint8* p_data, int32* p_data_size)
{
	BIT_STREAM_READER bs;
	uint32 value;

	//int32  need_size;
	int32  left_size;
	int32  total_demux_size;
	//int32  ext_audio_obj_type;

	int32  depend_on_core_coder;

	int32  ret;

	if( p_config == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	p_config->b_ext = 0;

	ret = bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	/** audio obj type, 5 bit */
	ret = bs_read_bits(&bs, &p_config->audio_obj_type, MPEG4_AUDIO_OBJECT_TYPE_BIT_SIZE);
	if( ret )
	{
		*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	if( p_config->audio_obj_type == 31 )
	{
		/** resv */
		ret = bs_read_bits(&bs, &value, MPEG4_AUDIO_OBJECT_TYPE_EXT_BIT_SIZE);
		if( ret )
		{
			*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
			return GEN_E_NEED_MORE_DATA;
		}

		p_config->audio_obj_type = 32 + value;
	}

	/** sample freq index, 4 bit */
	ret = bs_read_bits(&bs, &p_config->sample_freq_index, MPEG4_AUDIO_SAMPLE_FREQ_INDEX_BIT_SIZE);
	if( ret )
	{
		*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	if( p_config->sample_freq_index == MPEG4_AAC_SAMPLE_FREQ_INDEX_ESCAPE )
	{
		ret = bs_read_bits(&bs, &p_config->sample_freq, MPEG4_AUDIO_SAMPLE_FREQ_BIT_SIZE);
		if( ret )
		{
			*p_data_size = (MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE + (MPEG4_AUDIO_SAMPLE_FREQ_BIT_SIZE / GEN_BYTE_BIT));
			return GEN_E_NEED_MORE_DATA;
		}

	}else
	{
		ret = mpeg4_audio_get_sample_freq(p_config->sample_freq_index, &p_config->sample_freq);
	}

	/** chn config, 4 bit */
	ret = bs_read_bits(&bs, &p_config->chn_config, MPEG4_AUDIO_CHN_CONFIG_BIT_SIZE);
	if( ret )
	{
		*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	p_config->sbr_present_flag = -1;
	p_config->ps_present_flag = -1;
	if( p_config->audio_obj_type == MPEG4_AUDIO_OBJECT_TYPE_SBR || p_config->audio_obj_type == MPEG4_AUDIO_OBJECT_TYPE_PS )
	{
		p_config->b_ext = 1;
		p_config->ext_audio_obj_type = MPEG4_AUDIO_OBJECT_TYPE_SBR;
		p_config->sbr_present_flag = 1;

		if( p_config->audio_obj_type == MPEG4_AUDIO_OBJECT_TYPE_PS )
		{
			p_config->ps_present_flag = 1;
		}

		/** ext sample freq index, 4 bit */
		ret = bs_read_bits(&bs, &p_config->sample_freq_index, MPEG4_AUDIO_EXT_SAMPLE_FREQ_INDEX_BIT_SIZE);
		if( ret )
		{
			*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_config->sample_freq_index == 0xF )
		{
			/** ext sample freq, 24 bit */
			ret = bs_read_bits(&bs, &p_config->sample_freq, MPEG4_AUDIO_EXT_SAMPLE_FREQ_BIT_SIZE);
			if( ret )
			{
				*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
				return GEN_E_NEED_MORE_DATA;
			}

		}else
		{
			ret = mpeg4_audio_get_sample_freq(p_config->sample_freq_index, &p_config->sample_freq);
		}

		/** audio obj type, 5 bit */
		ret = bs_read_bits(&bs, &p_config->audio_obj_type, MPEG4_AUDIO_OBJECT_TYPE_BIT_SIZE);
		if( ret )
		{
			*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_config->audio_obj_type == 31 )
		{
			/** resv */
			ret = bs_read_bits(&bs, &value, MPEG4_AUDIO_OBJECT_TYPE_EXT_BIT_SIZE);
			if( ret )
			{
				*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
				return GEN_E_NEED_MORE_DATA;
			}

			p_config->audio_obj_type = 32 + value;
		}

		if( p_config->audio_obj_type == MPEG4_AUDIO_OBJECT_TYPE_ER_BSAC )
		{
			/** ext chn config, 4 bit */
			ret = bs_read_bits(&bs, &p_config->chn_config, MPEG4_AUDIO_CHN_CONFIG_BIT_SIZE);
			if( ret )
			{
				*p_data_size = MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE;
				return GEN_E_NEED_MORE_DATA;
			}
		}

	}else
	{
		p_config->ext_audio_obj_type = 0;
	}

	switch( p_config->audio_obj_type )
	{
	case MPEG4_AUDIO_OBJECT_TYPE_AAC_MAIN:
	case MPEG4_AUDIO_OBJECT_TYPE_AAC_LC:
	case MPEG4_AUDIO_OBJECT_TYPE_AAC_SSR:
	case MPEG4_AUDIO_OBJECT_TYPE_AAC_LTP:
		{
			/** GASpecific Config */
			/** frame length flag, 1 bit */
			ret = bs_reader_skip_bits(&bs, 1);

			/** depend on core coder, 1 bit */
			ret = bs_read_bits(&bs, &depend_on_core_coder, 1);
			if( depend_on_core_coder )
			{
				/** core coder delay, 14 bit */
				ret = bs_reader_skip_bits(&bs, 14);
			}

			/** ext flag, 1 bit */
			ret = bs_reader_skip_bits(&bs, 1);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	ret = bs_reader_get_use_bytes(&bs, &total_demux_size);
	left_size -= total_demux_size;

	if( p_config->ext_audio_obj_type != MPEG4_AUDIO_OBJECT_TYPE_SBR && left_size >= (GEN_BYTE_BIT * 2) )
	{
		/** sync ext type, 11 bit */
		ret = bs_read_bits(&bs, &p_config->sync_ext_type, 11);
		if( ret )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_config->sync_ext_type == 0x2B7 )
		{
			p_config->b_ext = 1;

			/** ext audio obj type, 5 bit */
			ret = bs_read_bits(&bs, &p_config->audio_obj_type, MPEG4_AUDIO_OBJECT_TYPE_BIT_SIZE);
			if( ret )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			if( p_config->audio_obj_type == MPEG4_AUDIO_OBJECT_TYPE_SBR )
			{
				/** sbr present flag */
				ret = bs_read_bits(&bs, &p_config->sbr_present_flag, 1);
				if( ret )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				if( p_config->sbr_present_flag == 0x1 )
				{
					/** ext sample freq index, 4 bit */
					ret = bs_read_bits(&bs, &p_config->sample_freq_index, MPEG4_AUDIO_EXT_SAMPLE_FREQ_INDEX_BIT_SIZE);
					if( ret )
					{
						return GEN_E_NEED_MORE_DATA;
					}

					if( p_config->sample_freq_index == 0xF )
					{
						/** ext sample freq, 24 bit */
						ret = bs_read_bits(&bs, &p_config->sample_freq, MPEG4_AUDIO_EXT_SAMPLE_FREQ_BIT_SIZE);
						if( ret )
						{
							return GEN_E_NEED_MORE_DATA;
						}

					}else
					{
						ret = mpeg4_audio_get_sample_freq(p_config->sample_freq_index, &p_config->sample_freq);
					}

					/** fuck my lazy not sure left bits >= 12 */
					/** sync ext type, 11 bit */
					ret = bs_read_bits(&bs, &p_config->sync_ext_type, 11);
					if( ret )
					{
						return GEN_E_NEED_MORE_DATA;
					}

					if( p_config->sync_ext_type == 0x548 )
					{
						/** ps present flag, 1 bit */
						ret = bs_read_bits(&bs, &p_config->ps_present_flag, 1);
						if( ret )
						{
							return GEN_E_NEED_MORE_DATA;
						}
					}
				}
			}
		}
	}

	/** copy from faad2(mp4.c) */
	if( p_config->sbr_present_flag == -1)
	{
		if( p_config->sample_freq <= 24000 )
		{
			p_config->sample_freq *= 2;
		}
	}

	ret = bs_reader_get_use_bytes(&bs, &total_demux_size);
	*p_data_size = total_demux_size;

	return GEN_S_OK;
}

int32 mpeg4_adec_spec_info_demux(MPEG4_ADEC_SPEC_INFO* p_info, uint8* p_data, int32* p_data_size)
{
	//BIT_STREAM_READER bs;
	//uint32 value;

	int32  need_size;
	int32  left_size;
	int32  total_demux_size;
	int32  demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_info == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	total_demux_size = 0;
	p_uint8 = p_data;
	//bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	demux_size = left_size;
	ret = mpeg4_base_desc_demux(&p_info->base, p_uint8, &demux_size);
	if( ret )
	{
		*p_data_size = demux_size;
		return GEN_E_NEED_MORE_DATA;
	}

	left_size -= demux_size;
	total_demux_size += demux_size;
	p_uint8 += demux_size;
	//bs_reader_skip_bits(&bs, demux_size * GEN_BYTE_BIT);

	p_info->p_spec_config = p_data + total_demux_size;
	p_info->spec_config_size = p_info->base.size.value;

	need_size = total_demux_size + p_info->base.size.value;
	if( left_size < p_info->base.size.value )
	{
		*p_data_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	if( p_info->base.tag != MPEG4_DEC_SPEC_INFO_TAG )
	{
		*p_data_size = need_size;
		return GEN_E_WRONG_FORMAT;
	}

	/** audio spec config */
	demux_size = left_size;
	ret	= mpeg4_audio_spec_config_demux(&p_info->audio_spec_config, p_uint8, &demux_size);
	if( ret )
	{
		return GEN_E_WRONG_FORMAT;
	}

	*p_data_size = need_size;

	return GEN_S_OK;
}
