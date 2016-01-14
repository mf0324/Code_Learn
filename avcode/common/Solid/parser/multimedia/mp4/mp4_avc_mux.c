
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <public/gen_error.h>
#include <util/log_agent.h>

#include "mp4_public.h"
#include "mp4_avc_mux.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 mp4_avc_calc_dec_config_record_total_size(AVC_DEC_CONFIG_RECORD* p_record, uint32* p_size)
{
	uint32  need_size;

	uint32  i;

	if( p_record == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	need_size = MP4_AVC_DEC_CONFIG_RECORD_MIN_SIZE;

	if( p_record->sps_count > 0 && p_record->p_sps )
	{
		for( i = 0; i < p_record->sps_count; i++ )
		{
			need_size += MP4_AVC_SPS_RECORD_MIN_SIZE;
			need_size += p_record->p_sps[i].nalu_len;
		}
	}

	if( p_record->pps_count > 0 && p_record->p_pps )
	{
		for( i = 0; i < p_record->pps_count; i++ )
		{
			need_size += MP4_AVC_PPS_RECORD_MIN_SIZE;
			need_size += p_record->p_pps[i].nalu_len;
		}
	}

	*p_size = need_size;

	return GEN_S_OK;
}

int32 mp4_avc_dec_config_record_mux(AVC_DEC_CONFIG_RECORD* p_record, uint8* p_buf, uint32* p_buf_size)
{
	BIT_STREAM_WRITER  bs;
	uint32  value;

	uint32  need_size;
	uint32  total_size;

	uint8* p_uint8 = NULL;
	//uint8  uint8_data;

	uint32  i;
	int32   ret;

	if( p_record == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	ret = mp4_avc_calc_dec_config_record_total_size(p_record, &need_size);
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_size = 0;

	bs_writer_init(&bs, p_buf, need_size, 8);

	/** version + profile + compat + level, 32 bit */
	p_record->config_version = 0x1;
	//box.record.profile = p_box->record.profile;
	//box.record.profile_compat = p_box->record.profile_compat;
	//box.record.level = p_box->record.level;

	memcpy(p_uint8, p_record, MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE);
	p_uint8 += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	total_size += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	bs_writer_skip_bits(&bs, MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE * 8);

	/** resv + length size minus 1, 8 bit */
	value = (p_record->len_size_minus_1 & 0x3) | 0xFC;
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	/** resv + sps count, 8 bit */
	value = (p_record->sps_count & 0x1F) | 0xE0;
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	if( p_record->sps_count > 0 && p_record->p_sps )
	{
		for( i = 0; i < p_record->sps_count; i++ )
		{
			/** nalu len, 16bit */
			value = p_record->p_sps[i].nalu_len;
			bs_write_bits(&bs, value, 16);
			p_uint8 += MP4_AVC_SPS_RECORD_MIN_SIZE;
			total_size += MP4_AVC_SPS_RECORD_MIN_SIZE;

			/** nalu */
			if( p_record->p_sps[i].nalu_len > 0 && p_record->p_sps[i].p_nalu )
			{
				memcpy(p_uint8, p_record->p_sps[i].p_nalu, p_record->p_sps[i].nalu_len);
				p_uint8 += p_record->p_sps[i].nalu_len;
				total_size += p_record->p_sps[i].nalu_len;
				bs_writer_skip_bits(&bs, p_record->p_sps[i].nalu_len * 8);
			}
		}
	}

	/** pps count, 8 bit */
	value = (p_record->pps_count & 0xFF);
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	if( p_record->pps_count > 0 && p_record->p_pps )
	{
		for( i = 0; i < p_record->pps_count; i++ )
		{
			/** nalu len, 16bit */
			value = p_record->p_pps[i].nalu_len;
			bs_write_bits(&bs, value, 16);
			p_uint8 += MP4_AVC_PPS_RECORD_MIN_SIZE;
			total_size += MP4_AVC_PPS_RECORD_MIN_SIZE;

			/** nalu */
			if( p_record->p_pps[i].nalu_len > 0 && p_record->p_pps[i].p_nalu )
			{
				memcpy(p_uint8, p_record->p_pps[i].p_nalu, p_record->p_pps[i].nalu_len);
				p_uint8 += p_record->p_pps[i].nalu_len;
				total_size += p_record->p_pps[i].nalu_len;
				bs_writer_skip_bits(&bs, p_record->p_pps[i].nalu_len * 8);
			}
		}
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 mp4_avc_calc_config_box_total_size(AVC_CONFIG_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	uint32 i;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_AVC_CONFIG_BOX_MIN_SIZE;

	if( p_box->record.sps_count > 0 && p_box->record.p_sps )
	{
		for( i = 0; i < p_box->record.sps_count; i++ )
		{
			need_size += MP4_AVC_SPS_RECORD_MIN_SIZE;
			need_size += p_box->record.p_sps[i].nalu_len;
		}
	}

	if( p_box->record.pps_count > 0 && p_box->record.p_pps )
	{
		for( i = 0; i < p_box->record.pps_count; i++ )
		{
			need_size += MP4_AVC_PPS_RECORD_MIN_SIZE;
			need_size += p_box->record.p_pps[i].nalu_len;
		}
	}

	*p_size = need_size;

	return 0;
}

int32 mp4_avc_config_box_mux(AVC_CONFIG_BOX *p_box, uint8 *p_buf, uint32 *p_buf_size)
{
	AVC_CONFIG_BOX box;

	BIT_STREAM_WRITER bs;
	uint32 value;

	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	//uint8  uint8_data;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = p_box->box.size;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;

	bs_writer_init(&bs, p_buf, need_size, 8);

	/** box */
	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_AVC_CONFIG_BOX_TYPE);

	memcpy(p_uint8, &box.box, MP4_BOX_SIZE);
	p_uint8 += MP4_BOX_SIZE;
	total_size += MP4_BOX_SIZE;
	bs_writer_skip_bits(&bs, MP4_BOX_SIZE * 8);

	/** version + profile + compat + level, 32 bit */
	box.record.config_version = 0x1;
	box.record.profile = p_box->record.profile;
	box.record.profile_compat = p_box->record.profile_compat;
	box.record.level = p_box->record.level;

	memcpy(p_uint8, &box.record, MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE);
	p_uint8 += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	total_size += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	bs_writer_skip_bits(&bs, MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE * 8);

	/** resv + length size minus 1, 8 bit */
	value = (p_box->record.len_size_minus_1 & 0x3) | 0xFC;
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	/** resv + sps count, 8 bit */
	value = (p_box->record.sps_count & 0x1F) | 0xE0;
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	if( p_box->record.sps_count > 0 && p_box->record.p_sps )
	{
		for( i = 0; i < p_box->record.sps_count; i++ )
		{
			/** nalu len, 16bit */
			value = p_box->record.p_sps[i].nalu_len;
			bs_write_bits(&bs, value, 16);
			p_uint8 += MP4_AVC_SPS_RECORD_MIN_SIZE;
			total_size += MP4_AVC_SPS_RECORD_MIN_SIZE;

			/** nalu */
			if( p_box->record.p_sps[i].nalu_len > 0 && p_box->record.p_sps[i].p_nalu )
			{
				memcpy(p_uint8, p_box->record.p_sps[i].p_nalu, p_box->record.p_sps[i].nalu_len);
				p_uint8 += p_box->record.p_sps[i].nalu_len;
				total_size += p_box->record.p_sps[i].nalu_len;
				bs_writer_skip_bits(&bs, p_box->record.p_sps[i].nalu_len * 8);
			}
		}
	}

	/** pps count, 8 bit */
	value = (p_box->record.pps_count & 0xFF);
	bs_write_bits(&bs, value, 8);
	p_uint8 += 1;
	total_size += 1;

	if( p_box->record.pps_count > 0 && p_box->record.p_pps )
	{
		for( i = 0; i < p_box->record.pps_count; i++ )
		{
			/** nalu len, 16bit */
			value = p_box->record.p_pps[i].nalu_len;
			bs_write_bits(&bs, value, 16);
			p_uint8 += MP4_AVC_PPS_RECORD_MIN_SIZE;
			total_size += MP4_AVC_PPS_RECORD_MIN_SIZE;

			/** nalu */
			if( p_box->record.p_pps[i].nalu_len > 0 && p_box->record.p_pps[i].p_nalu )
			{
				memcpy(p_uint8, p_box->record.p_pps[i].p_nalu, p_box->record.p_pps[i].nalu_len);
				p_uint8 += p_box->record.p_pps[i].nalu_len;
				total_size += p_box->record.p_pps[i].nalu_len;
				bs_writer_skip_bits(&bs, p_box->record.p_pps[i].nalu_len * 8);
			}
		}
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_avc_calc_sample_entry_total_size(AVC_SAMPLE_ENTRY *p_box, uint32 *p_size)
{
	uint32 need_size;
	uint32 config_box_size;

	int32  ret;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_VISUAL_SAMPLE_ENTRY_SIZE;

	ret = mp4_avc_calc_config_box_total_size(&p_box->config, &config_box_size);
	need_size += config_box_size;

	*p_size = need_size;

	return 0;
}

int32 mp4_avc_sample_entry_mux(AVC_SAMPLE_ENTRY* p_box, uint8 *p_buf, uint32 *p_buf_size)
{
	AVC_SAMPLE_ENTRY box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;
	uint32 mux_size;

	uint8* p_uint8 = NULL;
	uint32 config_box_size;

	//uint32 i;
	int32  ret;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = p_box->visual.entry.box.size;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;
	left_size = *p_buf_size;

	/** visual sample entry */
	box.visual.entry.box.size = hton_u32(p_box->visual.entry.box.size);
	box.visual.entry.box.type = hton_u32(MP4_AVC_SAMPLE_ENTRY_BOX_TYPE);
	box.visual.entry.resv[0] = 0x0;
	box.visual.entry.resv[1] = 0x0;
	box.visual.entry.resv[2] = 0x0;
	box.visual.entry.resv[3] = 0x0;
	box.visual.entry.resv[4] = 0x0;
	box.visual.entry.resv[5] = 0x0;
	box.visual.entry.data_ref_index = hton_u16(p_box->visual.entry.data_ref_index);

	box.visual.pre_define = hton_u16(0x0);
	box.visual.padding = hton_u16(0x0);
	box.visual.pre_define2[0] = hton_u32(0x0);
	box.visual.pre_define2[1] = hton_u32(0x0);
	box.visual.pre_define2[2] = hton_u32(0x0);

	box.visual.width = hton_u16(p_box->visual.width);
	box.visual.height = hton_u16(p_box->visual.height);

	box.visual.horiz_resolution = hton_u32(MP4_VISUAL_SAMPLE_HORIZ_RESOLUTION);
	box.visual.vert_resolution = hton_u32(MP4_VISUAL_SAMPLE_VERT_RESOLUTION);
	box.visual.resv = hton_u32(0x0);
	box.visual.frame_count = hton_u16(MP4_VISUAL_SAMPLE_FRAME_COUNT);

	memcpy(box.visual.compressor_name, p_box->visual.compressor_name, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);

	box.visual.depth = hton_u16(MP4_VISUAL_SAMPLE_DEPTH);
	box.visual.pre_define3 = hton_u16(-1);

	memcpy(p_uint8, &box.visual, MP4_VISUAL_SAMPLE_ENTRY_SIZE);
	p_uint8 += MP4_VISUAL_SAMPLE_ENTRY_SIZE;
	total_size += MP4_VISUAL_SAMPLE_ENTRY_SIZE;
	left_size -= MP4_VISUAL_SAMPLE_ENTRY_SIZE;

	/** config box */
	ret = mp4_avc_calc_config_box_total_size(&p_box->config, &config_box_size);
	p_box->config.box.size = config_box_size;

	mux_size = left_size;
	ret = mp4_avc_config_box_mux(&p_box->config, p_uint8, &mux_size);
	p_uint8 += mux_size;
	total_size += mux_size;
	left_size -= mux_size;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_avc_calc_sample_desc_box_total_size(AVC_SAMPLE_DESC_BOX* p_box, uint32* p_size)
{
	uint32 need_size;
	uint32 entry_size;

	int32  ret;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;

	ret = mp4_avc_calc_sample_entry_total_size(&p_box->entry, &entry_size);
	need_size += entry_size;

	*p_size = need_size;

	return 0;
}

int32 mp4_avc_sample_desc_box_mux(AVC_SAMPLE_DESC_BOX* p_box, uint8* p_buf, uint32* p_buf_size)
{
	AVC_SAMPLE_DESC_BOX box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;
	uint32 mux_size;

	uint8* p_uint8 = NULL;
	uint32 entry_size;

	int32  ret;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = p_box->full_box.box.size;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;
	left_size = *p_buf_size;

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_DESC_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(0x1);

	memcpy(p_uint8, &box, MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE);
	p_uint8 += MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;
	total_size += MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;
	left_size -= MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;

	/** entry */
	ret = mp4_avc_calc_sample_entry_total_size(&p_box->entry, &entry_size);
	p_box->entry.visual.entry.box.size = entry_size;

	mux_size = left_size;
	ret = mp4_avc_sample_entry_mux(&p_box->entry, p_uint8, &mux_size);
	p_uint8 += mux_size;
	total_size += mux_size;
	left_size -= mux_size;

	*p_buf_size = need_size;

	return 0;
}
