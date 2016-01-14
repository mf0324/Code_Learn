
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <util/log_debug.h>

#include "mpeg4_public.h"
#include "mpeg4_sys_mux.h"


/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg4_calc_base_desc_size(MPEG4_BASE_DESCRIPTOR *p_desc, uint32 *p_size)
{
	uint32 need_size;

	if( p_desc == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MPEG4_TAG_SIZE;

	switch( p_desc->size.bit_size )
	{
	case MPEG4_SIZE_ENCODING_8_BIT:
		{
			need_size += 1;
		}
		break;

	case MPEG4_SIZE_ENCODING_16_BIT:
		{
			need_size += 2;
		}
		break;

	case MPEG4_SIZE_ENCODING_24_BIT:
		{
			need_size += 3;
		}
		break;

	case MPEG4_SIZE_ENCODING_32_BIT:
		{
			need_size += 4;
		}
		break;

	default:
		{
			return -3;
		}
	}

	*p_size = need_size;

	return 0;
}

int32 mpeg4_base_desc_mux(MPEG4_BASE_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size)
{
	uint32 need_size;

	int32  ret;

	if( p_desc == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg4_calc_base_desc_size(p_desc, &need_size);
	if( ret )
	{
		return ret;
	}

	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	/** tag */
	p_buf[0] = p_desc->tag & 0xFF;

	/** size encoding */
	switch( p_desc->size.bit_size )
	{
	case MPEG4_SIZE_ENCODING_8_BIT:
		{
			p_buf[1] = (p_desc->size.value & 0x7F);
		}
		break;

	case MPEG4_SIZE_ENCODING_16_BIT:
		{
			p_buf[1] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 7) & 0x7F));
			p_buf[2] = (p_desc->size.value & 0x7F);
		}
		break;

	case MPEG4_SIZE_ENCODING_24_BIT:
		{
			p_buf[1] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 14) & 0x7F));
			p_buf[2] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 7) & 0x7F));
			p_buf[3] = (p_desc->size.value & 0x7F);
		}
		break;

	case MPEG4_SIZE_ENCODING_32_BIT:
		{
			p_buf[1] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 21) & 0x7F));
			p_buf[2] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 14) & 0x7F));
			p_buf[3] = (MPEG4_SIZE_ENCODING_NEXT_BYTE | ((p_desc->size.value >> 7) & 0x7F));
			p_buf[4] = (p_desc->size.value & 0x7F);
		}
		break;

	default:
		return -3;
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mpeg4_calc_sl_config_desc_size(MPEG4_SL_CONFIG_DESCRIPTOR *p_desc, uint32 *p_size)
{
	uint32 need_size;
	int32  ret;

	if( p_desc == NULL || p_size == NULL )
	{
		return -1;
	}

	ret = mpeg4_calc_base_desc_size(&p_desc->base, &need_size);
	if( ret )
	{
		return ret;
	}

	need_size += MPEG4_PREDEFINE_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mpeg4_sl_config_desc_mux(MPEG4_SL_CONFIG_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size)
{
	uint32 need_size;
	uint32 left_size;
	uint32 mux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	ret = mpeg4_calc_sl_config_desc_size(p_desc, &need_size);
	if( ret )
	{
		return ret;
	}

	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	left_size = *p_buf_size;
	p_uint8 = p_buf;

	/** base */
	p_desc->base.tag = MPEG4_SL_CONFIG_DESC_TAG;
	p_desc->base.size.value = MPEG4_PREDEFINE_SIZE;

	mux_size = left_size;
	ret = mpeg4_base_desc_mux(&p_desc->base, p_uint8, &mux_size);
	p_uint8 += mux_size;
	left_size -= mux_size;

	/** predefine */
	p_uint8[0] = (p_desc->predefine & 0xFF);

	*p_buf_size = need_size;

	return 0;
}

int32 mpeg4_calc_vdec_spec_info_size(MPEG4_VDEC_SPEC_INFO *p_info, uint32 *p_size)
{
	uint32 need_size;

	uint32 total_bit = 0;

	if( p_info == NULL || p_size == NULL )
	{
		return -1;
	}

	/** vos */
	total_bit += (MPEG4_VOS_HEADER_SIZE * GEN_BYTE_BIT);

	/** visual object */
	total_bit += MPEG4_IS_VISUAL_OBJ_BIT_SIZE;
	if( p_info->visual_obj.is_visual_obj )
	{
		total_bit += (MPEG4_VISUAL_OBJ_VERID_BIT_SIZE + MPEG4_VISUAL_OBJ_PRIORITY_BIT_SIZE);
	}

	total_bit += MPEG4_VISUAL_OBJ_TYPE_BIT_SIZE;

	total_bit += MPEG4_VIDEO_SIGNAL_TYPE_BIT_SIZE;
	if( p_info->visual_obj.signal_type )
	{
		total_bit += (MPEG4_VIDEO_FORMAT_BIT_SIZE + MPEG4_VIDEO_RANGE_BIT_SIZE + MPEG4_COLOR_DESC_BIT_SIZE);
		if( p_info->visual_obj.color_desc )
		{
			total_bit += MPEG4_COLOR_PARAM_BIT_SIZE;
		}
	}

	/** video obj */
	total_bit += (MPEG4_VIDEO_OBJECT_HEADER_SIZE * GEN_BYTE_BIT);

	//fuck my lazy
	/** vol */
	total_bit += (MPEG4_VOL_HEADER_SIZE * GEN_BYTE_BIT);

	need_size = (total_bit + GEN_BYTE_BIT) / GEN_BYTE_BIT;

	*p_size = need_size;

	return 0;
}

int32 mpeg4_calc_dec_config_desc_size(MPEG4_DEC_CONFIG_DESCRIPTOR *p_desc, uint32 *p_size)
{
	uint32 need_size;

	int32  ret;

	if( p_desc == NULL || p_size == NULL )
	{
		return -1;
	}

	ret = mpeg4_calc_base_desc_size(&p_desc->base, &need_size);
	if( ret )
	{
		return ret;
	}

	need_size += MPEG4_DEC_CONFIG_DESC_MIN_SIZE;
	need_size += p_desc->vdec_spec_info_size;

	*p_size = need_size;

	return 0;
}

int32 mpeg4_dec_config_desc_mux(MPEG4_DEC_CONFIG_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size)
{
	BIT_STREAM_WRITER bs;
	uint32 need_size;
	uint32 left_size;
	uint32 mux_size;

	int32  ret;

	if( p_desc == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg4_calc_dec_config_desc_size(p_desc, &need_size);
	if( ret )
	{
		return ret;
	}

	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	left_size = *p_buf_size;
	bs_writer_init(&bs, p_buf, left_size, GEN_BYTE_BIT);

	/** base */
	p_desc->base.tag = MPEG4_DEC_CONFIG_DESC_TAG;
	p_desc->base.size.value = MPEG4_DEC_CONFIG_DESC_MIN_SIZE + p_desc->vdec_spec_info_size;

	mux_size = left_size;
	ret = mpeg4_base_desc_mux(&p_desc->base, p_buf, &mux_size);

	bs_writer_skip_bits(&bs, mux_size * GEN_BYTE_BIT);

	/** obj type indication */
	bs_write_bits(&bs, p_desc->obj_type, MPEG4_OBJ_TYPE_INDICATION_BIT_SIZE);

	/** stream type */
	bs_write_bits(&bs, p_desc->stream_type, MPEG4_STREAM_TYPE_BIT_SIZE);

	/** upstream */
	bs_write_bits(&bs, 0x0, MPEG4_UPSTREAM_BIT_SIZE);

	/** resv */
	bs_write_bits(&bs, 0x1, 1);

	/** buf size db */
	bs_write_bits(&bs, p_desc->buf_size, MPEG4_BUF_SIZE_DB_BIT_SIZE);

	/** max bitrate */
	bs_write_bits(&bs, p_desc->max_bitrate, MPEG4_MAX_BITRATE_BIT_SIZE);

	/** avg bitrate */
	bs_write_bits(&bs, p_desc->avg_bitrate, MPEG4_AVG_BITRATE_BIT_SIZE);

	if( p_desc->vdec_spec_info_size > 0 && p_desc->p_vdec_spec_info )
	{
		ret = bs_writer_get_use_bytes(&bs, &mux_size);
		memcpy(p_buf + mux_size, p_desc->p_vdec_spec_info, p_desc->vdec_spec_info_size);
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mpeg4_calc_es_desc_size(MPEG4_ES_DESCRIPTOR *p_desc, uint32 *p_size)
{
	uint32 need_size;
	uint32 base_desc_size;
	uint32 total_size = 0;

	int32  ret;

	if( p_desc == NULL || p_size == NULL )
	{
		return -1;
	}

	/** base */
	ret = mpeg4_calc_base_desc_size(&p_desc->base, &base_desc_size);
	if( ret )
	{
		return ret;
	}
	total_size += base_desc_size;

	/** es */
	total_size += MPEG4_ES_DESC_MIN_SIZE;

	/** dec config */
	ret = mpeg4_calc_dec_config_desc_size(&p_desc->dec_config, &need_size);
	if( ret )
	{
		return ret;
	}
	total_size += need_size;

	/** sl config */
	ret = mpeg4_calc_sl_config_desc_size(&p_desc->sl_config, &need_size);
	if( ret )
	{
		return ret;
	}
	total_size += need_size;

	/** update value */
	p_desc->base.size.value = total_size - base_desc_size;

	*p_size = total_size;

	return 0;
}

int32 mpeg4_es_desc_mux(MPEG4_ES_DESCRIPTOR *p_desc, uint8 *p_buf, uint32 *p_buf_size)
{
	BIT_STREAM_WRITER bs;

	uint32 need_size;
	uint32 left_size;
	uint32 mux_size;
	uint32 total_mux_size;

	int32  ret;

	if( p_desc == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg4_calc_es_desc_size(p_desc, &need_size);
	if( ret )
	{
		return ret;
	}

	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	left_size = *p_buf_size;
	total_mux_size = 0;

	bs_writer_init(&bs, p_buf, left_size, GEN_BYTE_BIT);

	/** base */
	p_desc->base.tag = MPEG4_ES_DESC_TAG;
	mux_size = left_size;
	ret = mpeg4_base_desc_mux(&p_desc->base, p_buf + total_mux_size, &mux_size);
	bs_writer_skip_bits(&bs, mux_size * GEN_BYTE_BIT);

	/** es */
	bs_write_bits(&bs, p_desc->es_id, MPEG4_ES_ID_BIT_SIZE);
	bs_write_bits(&bs, p_desc->stream_depend_flag, MPEG4_STREAM_DEPEND_FLAG_BIT_SIZE);
	bs_write_bits(&bs, p_desc->url_flag, MPEG4_URL_FLAG_BIT_SIZE);
	bs_write_bits(&bs, p_desc->ocr_stream_flag, MPEG4_OCR_STREAM_FLAG_BIT_SIZE);
	bs_write_bits(&bs, p_desc->stream_priority, MPEG4_STREAM_PRIORITY_BIT_SIZE);
	bs_writer_get_use_bytes(&bs, &total_mux_size);

	/** dec config desc */
	mux_size = left_size - total_mux_size;
	ret = mpeg4_dec_config_desc_mux(&p_desc->dec_config, p_buf + total_mux_size, &mux_size);
	total_mux_size += mux_size;
	bs_writer_skip_bits(&bs, mux_size * GEN_BYTE_BIT);

	/** sl config desc */
	mux_size = left_size - total_mux_size;
	ret = mpeg4_sl_config_desc_mux(&p_desc->sl_config, p_buf + total_mux_size, &mux_size);
	total_mux_size += mux_size;
	bs_writer_skip_bits(&bs, mux_size * GEN_BYTE_BIT);

	*p_buf_size = need_size;

	return 0;
}
