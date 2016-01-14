
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <public/bit_stream_reader.h>
#include <util/log_debug.h>

#include "mpeg4_public.h"
#include "mpeg4_sys_demux.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg4_size_encoding_demux(MPEG4_SIZE_ENCODING* p_encoding, uint8* p_data, int32* p_data_size)
{
	//int32  need_size;
	int32  left_size;
	int32  total_demux_size;
	int32  instance_size;
	int32  b_next_byte;
	uint8* p_uint8 = NULL;

	//int32  ret;

	if( p_encoding == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	total_demux_size = 0;

	if( left_size < MPEG4_SIZE_ENCODING_MIN_LEN )
	{
		*p_data_size = MPEG4_SIZE_ENCODING_MIN_LEN;
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;

	b_next_byte = (p_uint8[0] >> MPEG4_SIZE_ENCODING_NEXT_BYTE_BIT_OFFSET) & 0x1;
	instance_size = p_uint8[0] & 0x7F;

	left_size -= MPEG4_SIZE_ENCODING_MIN_LEN;
	total_demux_size += MPEG4_SIZE_ENCODING_MIN_LEN;
	p_uint8 += MPEG4_SIZE_ENCODING_MIN_LEN;

	while( b_next_byte )
	{
		if( left_size < MPEG4_SIZE_ENCODING_MIN_LEN )
		{
			*p_data_size = (total_demux_size + MPEG4_SIZE_ENCODING_MIN_LEN);
			return GEN_E_NEED_MORE_DATA;
		}

		if( total_demux_size >= (MPEG4_SIZE_ENCODING_32_BIT / GEN_BYTE_BIT) )
		{
			break;
		}

		b_next_byte = (p_uint8[0] >> MPEG4_SIZE_ENCODING_NEXT_BYTE_BIT_OFFSET) & 0x1;
		instance_size = ((instance_size << MPEG4_SIZE_ENCODING_NEXT_BYTE_BIT_OFFSET) | (p_uint8[0] & 0x7F));

		left_size -= MPEG4_SIZE_ENCODING_MIN_LEN;
		total_demux_size += MPEG4_SIZE_ENCODING_MIN_LEN;
		p_uint8 += MPEG4_SIZE_ENCODING_MIN_LEN;
	}

	p_encoding->value = instance_size;
	p_encoding->bit_size = total_demux_size * GEN_BYTE_BIT;

	*p_data_size = total_demux_size;

	return GEN_S_OK;
}

int32 mpeg4_base_desc_demux(MPEG4_BASE_DESCRIPTOR* p_desc, uint8* p_data, int32* p_data_size)
{
	int32  left_size;
	int32  total_demux_size;
	int32  demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_desc == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;

	if( left_size < MPEG4_TAG_SIZE )
	{
		*p_data_size = MPEG4_TAG_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;
	total_demux_size = 0;

	/** tag */
	p_desc->tag = p_uint8[0];
	left_size -= MPEG4_TAG_SIZE;
	total_demux_size += MPEG4_TAG_SIZE;
	p_uint8 += MPEG4_TAG_SIZE;

	/** size encoding */
	demux_size = left_size;
	ret = mpeg4_size_encoding_demux(&p_desc->size, p_uint8, &demux_size);
	if( ret )
	{
		*p_data_size = total_demux_size + demux_size;
		return GEN_E_NEED_MORE_DATA;
	}
	left_size -= demux_size;
	total_demux_size += demux_size;
	p_uint8 += demux_size;

	*p_data_size = total_demux_size;

	return GEN_S_OK;
}

int32 mpeg4_dec_config_desc_demux(MPEG4_DEC_CONFIG_DESCRIPTOR* p_desc, uint8* p_data, int32* p_data_size)
{
	BIT_STREAM_READER bs;
	uint32 value;

	int32  need_size;
	int32  left_size;
	int32  total_demux_size;
	int32  demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_desc == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	total_demux_size = 0;
	p_uint8 = p_data;
	bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	demux_size = left_size;
	ret = mpeg4_base_desc_demux(&p_desc->base, p_uint8, &demux_size);
	if( ret )
	{
		*p_data_size = demux_size;
		return GEN_E_NEED_MORE_DATA;
	}

	left_size -= demux_size;
	total_demux_size += demux_size;
	p_uint8 += demux_size;
	bs_reader_skip_bits(&bs, demux_size * GEN_BYTE_BIT);

	need_size = total_demux_size + p_desc->base.size.value;
	if( left_size < p_desc->base.size.value )
	{
		*p_data_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	if( p_desc->base.tag != MPEG4_DEC_CONFIG_DESC_TAG )
	{
		*p_data_size = need_size;
		return GEN_E_WRONG_FORMAT;
	}

	/** obj type id, 8 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_OBJ_TYPE_INDICATION_BIT_SIZE);
	p_desc->obj_type = value;

	/** stream type, 6 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_STREAM_TYPE_BIT_SIZE);
	p_desc->stream_type = value;

	/** upstream + resv, 1 + 1 bit */
	ret	= bs_reader_skip_bits(&bs, 1+1);

	/** buf size db, 24 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_BUF_SIZE_DB_BIT_SIZE);
	p_desc->buf_size = value;

	/** max bit rate, 32 bit */
	ret	= bs_read_bits(&bs, &p_desc->max_bitrate, MPEG4_MAX_BITRATE_BIT_SIZE);

	/** avg bit rate, 32 bit */
	ret	= bs_read_bits(&bs, &p_desc->avg_bitrate, MPEG4_AVG_BITRATE_BIT_SIZE);

	/** decoder specific info */
	ret = bs_reader_get_use_bytes(&bs, &demux_size);
	left_size = need_size - demux_size;
	if( left_size > 0 )
	{
		p_desc->p_vdec_spec_info = p_data + demux_size;
		p_desc->vdec_spec_info_size = left_size;

	}else
	{
		p_desc->p_vdec_spec_info = NULL;
		p_desc->vdec_spec_info_size = 0;
	}

	*p_data_size = need_size;

	return GEN_S_OK;
}

int32 mpeg4_es_desc_demux(MPEG4_ES_DESCRIPTOR *p_desc, uint8* p_data, uint32* p_data_size)
{
	BIT_STREAM_READER bs;
	uint32 value;

	int32  need_size;
	int32  left_size;
	int32  total_demux_size;
	int32  demux_size;
	uint8* p_uint8 = NULL;

	int32  url_len;

	int32  ret;

	if( p_desc == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	total_demux_size = 0;
	p_uint8 = p_data;
	bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	demux_size = left_size;
	ret = mpeg4_base_desc_demux(&p_desc->base, p_uint8, &demux_size);
	if( ret )
	{
		*p_data_size = demux_size;
		return GEN_E_NEED_MORE_DATA;
	}

	left_size -= demux_size;
	total_demux_size += demux_size;
	p_uint8 += demux_size;
	bs_reader_skip_bits(&bs, demux_size * GEN_BYTE_BIT);

	need_size = total_demux_size + p_desc->base.size.value;
	if( left_size < p_desc->base.size.value )
	{
		*p_data_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	if( p_desc->base.tag != MPEG4_ES_DESC_TAG )
	{
		*p_data_size = need_size;
		return GEN_E_WRONG_FORMAT;
	}

	/** es id, 16 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_ES_ID_BIT_SIZE);
	p_desc->es_id = value;

	/** stream depend flag, 1 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_STREAM_DEPEND_FLAG_BIT_SIZE);
	p_desc->stream_depend_flag = value;

	/** url flag, 1 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_URL_FLAG_BIT_SIZE);
	p_desc->url_flag = value;

	/** ocr stream flag, 1 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_OCR_STREAM_FLAG_BIT_SIZE);
	p_desc->ocr_stream_flag = value;

	/** stream priority, 5 bit */
	ret	= bs_read_bits(&bs, &value, MPEG4_STREAM_PRIORITY_BIT_SIZE);
	p_desc->stream_priority = value;

	if( p_desc->stream_depend_flag )
	{
		/** depend on es id, 16 bit */
		ret = bs_reader_skip_bits(&bs, MPEG4_DEPEND_ON_ES_ID_BIT_SIZE);
	}

	if( p_desc->url_flag )
	{
		/** url length, 8 bit */
		ret = bs_read_bits(&bs, &url_len, MPEG4_URL_LENGTH_BIT_SIZE);
		/** url string */
		ret = bs_reader_skip_bits(&bs, url_len * GEN_BYTE_BIT);
	}

	if( p_desc->ocr_stream_flag )
	{
		/** ocr es id, 16 bit */
		ret = bs_reader_skip_bits(&bs, MPEG4_OCR_ES_ID_BIT_SIZE);
	}

	/** decoder config desc */
	ret = bs_reader_get_use_bytes(&bs, &demux_size);

	left_size = (*p_data_size - demux_size);
	total_demux_size = demux_size;
	p_uint8 = p_data + demux_size;

	demux_size = left_size;
	ret = mpeg4_dec_config_desc_demux(&p_desc->dec_config, p_uint8, &demux_size);
	if( ret )
	{
		return GEN_E_WRONG_FORMAT;
	}

	*p_data_size = need_size;

	return GEN_S_OK;
}
