
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
//#include <util/log_debug.h>

#include "../../mpeg4/mpeg4_sys_mux.h"
#include "mp4_public.h"
#include "mp4_aac_mux.h"


/////////////////////////////////////外部接口//////////////////////////////////
int32 mp4_aac_calc_es_desc_box_total_size(AAC_ES_DESC_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;
	uint32 es_desc_size;

	int32  ret;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_FULL_BOX_SIZE;

	ret = mpeg4_calc_es_desc_size(&p_box->es_desc, &es_desc_size);
	need_size += es_desc_size;

	*p_size = need_size;

	return 0;
}

int32 mp4_aac_es_desc_box_mux(AAC_ES_DESC_BOX *p_box, uint8 *p_buf, uint32 *p_buf_size)
{
	AAC_ES_DESC_BOX box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;
	uint32 mux_size;
	//uint32 es_desc_size;

	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	left_size = *p_buf_size;

	need_size = p_box->full_box.box.size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;

	/** box */
	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_ES_DESC_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	memcpy(p_uint8, &box.full_box, MP4_FULL_BOX_SIZE);
	p_uint8 += MP4_FULL_BOX_SIZE;
	total_size += MP4_FULL_BOX_SIZE;
	left_size -= MP4_FULL_BOX_SIZE;

	mux_size = left_size;
	ret = mpeg4_es_desc_mux(&p_box->es_desc, p_uint8, &mux_size);
	p_uint8 += mux_size;
	total_size += mux_size;
	left_size -= mux_size;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_aac_calc_sample_entry_total_size(AAC_SAMPLE_ENTRY *p_box, uint32 *p_size)
{
	uint32 need_size;
	//uint32 audio_entry_size;
	uint32 es_desc_box_size;

	int32  ret;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_AUDIO_SAMPLE_ENTRY_SIZE;

	ret = mp4_aac_calc_es_desc_box_total_size(&p_box->es_box, &es_desc_box_size);
	need_size += es_desc_box_size;

	*p_size = need_size;

	return 0;
}

int32 mp4_aac_sample_entry_mux(AAC_SAMPLE_ENTRY* p_box, uint8 *p_buf, uint32 *p_buf_size)
{
	AAC_SAMPLE_ENTRY box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;
	uint32 mux_size;

	uint8* p_uint8 = NULL;
	uint32 es_desc_box_size;

	int32  ret;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = p_box->audio.entry.box.size;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;
	left_size = *p_buf_size;

	/** audio sample entry */
	box.audio.entry.box.size = hton_u32(p_box->audio.entry.box.size);
	box.audio.entry.box.type = hton_u32(MP4_AAC_SAMPLE_ENTRY_BOX_TYPE);
	box.audio.entry.resv[0] = 0x0;
	box.audio.entry.resv[1] = 0x0;
	box.audio.entry.resv[2] = 0x0;
	box.audio.entry.resv[3] = 0x0;
	box.audio.entry.resv[4] = 0x0;
	box.audio.entry.resv[5] = 0x0;
	box.audio.entry.data_ref_index = hton_u16(p_box->audio.entry.data_ref_index);

	box.audio.resv[0] = hton_u32(0x0);
	box.audio.resv[1] = hton_u32(0x0);
	box.audio.chn_count = hton_u16(MP4_AUDIO_SAMPLE_CHANNEL_COUNT);
	box.audio.sample_size = hton_u16(MP4_AUDIO_SAMPLE_SIZE);

	box.audio.pre_define = hton_u16(0x0);
	box.audio.padding = hton_u16(0x0);
	box.audio.sample_rate = hton_u32(p_box->audio.sample_rate);

	memcpy(p_buf + total_size, &box, MP4_AUDIO_SAMPLE_ENTRY_SIZE);
	total_size += MP4_AUDIO_SAMPLE_ENTRY_SIZE;
	left_size -= MP4_AUDIO_SAMPLE_ENTRY_SIZE;

	/** es desc box */
	ret = mp4_aac_calc_es_desc_box_total_size(&p_box->es_box, &es_desc_box_size);
	p_box->es_box.full_box.box.size = es_desc_box_size;

	mux_size = left_size;
	ret = mp4_aac_es_desc_box_mux(&p_box->es_box, p_buf + total_size, &mux_size);
	total_size += mux_size;
	left_size -= mux_size;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_aac_calc_sample_desc_box_total_size(AAC_SAMPLE_DESC_BOX* p_box, uint32* p_size)
{
	uint32 need_size;
	uint32 entry_size;
	//uint32 total_entry_size;

	uint32 i;
	int32  ret;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;

	//total_entry_size = 0;
	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			ret = mp4_aac_calc_sample_entry_total_size(p_box->p_entry + i, &entry_size);
			need_size += entry_size;
		}
	}

	*p_size = need_size;

	return 0;
}

int32 mp4_aac_sample_desc_box_mux(AAC_SAMPLE_DESC_BOX* p_box, uint8* p_buf, uint32* p_buf_size)
{
	AAC_SAMPLE_DESC_BOX box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;
	uint32 mux_size;

	uint8* p_uint8 = NULL;
	uint32 entry_size;

	uint32 i;
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

	/** full box */
	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_DESC_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_uint8, &box, MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE);
	p_uint8 += MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;
	total_size += MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;
	left_size -= MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;

	/** entry */
	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			ret = mp4_aac_calc_sample_entry_total_size(p_box->p_entry + i, &entry_size);
			p_box->p_entry[i].audio.entry.box.size = entry_size;

			mux_size = left_size;
			ret = mp4_aac_sample_entry_mux(p_box->p_entry + i, p_uint8, &mux_size);
			p_uint8 += mux_size;
			total_size += mux_size;
			left_size -= mux_size;
		}
	}

	*p_buf_size = need_size;

	return 0;
}
