
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
//#include <util/log_debug.h>

#include "mp4_public.h"
#include "mp4_demux.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 mp4_demux_box(MP4_BOX* p_box, uint8* p_data, uint32 data_size, uint32* p_demux_size)
{
	uint32 left_size = data_size;
	uint32 total_size;

	if( p_box == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	if( left_size < MP4_BOX_SIZE )
	{
		return -2;
	}

	memcpy(&p_box->payload.box, p_data, MP4_BOX_SIZE);
	p_box->payload.box.size = ntoh_u32(p_box->payload.box.size);
	p_box->payload.box.type = ntoh_u32(p_box->payload.box.type);

	if( p_box->payload.box.size == 1 )
	{
		if( left_size < MP4_BOX64_SIZE )
		{
			return -2;
		}

		memcpy(&p_box->payload.box64, p_data, MP4_BOX64_SIZE);
		p_box->payload.box64.size = ntoh_u32(p_box->payload.box64.size);
		p_box->payload.box64.type = ntoh_u32(p_box->payload.box64.type);
		p_box->payload.box64.large_size = ntoh_u64(p_box->payload.box64.large_size);

		p_box->format = MP4_BOX_FORMAT_ISO_BOX64;
		total_size = MP4_BOX64_SIZE;

	}else
	{
		p_box->format = MP4_BOX_FORMAT_ISO_BOX;
		total_size = MP4_BOX_SIZE;
	}

	*p_demux_size = total_size;

	return 0;
}

int32 mp4_demux_full_box(MP4_BOX* p_box, uint8* p_data, uint32 data_size, uint32* p_demux_size)
{
	uint32 left_size = data_size;
	uint32 total_size;

	if( p_box == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	if( left_size < MP4_FULL_BOX_SIZE )
	{
		return -2;
	}

	memcpy(&p_box->payload.full_box, p_data, MP4_FULL_BOX_SIZE);
	p_box->payload.full_box.box.size = ntoh_u32(p_box->payload.full_box.box.size);
	p_box->payload.full_box.box.type = ntoh_u32(p_box->payload.full_box.box.type);
	p_box->payload.full_box.version_flag.value = ntoh_u32(p_box->payload.full_box.version_flag.value);

	if( p_box->payload.full_box.box.size == 1 )
	{
		if( left_size < MP4_FULL_BOX64_SIZE )
		{
			return -2;
		}

		memcpy(&p_box->payload.full_box64, p_data, MP4_FULL_BOX64_SIZE);
		p_box->payload.full_box64.box.size = ntoh_u32(p_box->payload.full_box64.box.size);
		p_box->payload.full_box64.box.type = ntoh_u32(p_box->payload.full_box64.box.type);
		p_box->payload.full_box64.box.large_size = ntoh_u64(p_box->payload.full_box64.box.large_size);
		p_box->payload.full_box64.version_flag.value = ntoh_u32(p_box->payload.full_box.version_flag.value);

		p_box->format = MP4_BOX_FORMAT_ISO_FULL_BOX64;
		total_size = MP4_FULL_BOX64_SIZE;

	}else
	{
		p_box->format = MP4_BOX_FORMAT_ISO_FULL_BOX;
		total_size = MP4_FULL_BOX_SIZE;
	}

	*p_demux_size = total_size;

	return 0;
}

int32 mp4_demux_movie_header_box(ISO_MOVIE_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	need_size = MP4_MOVIE_HEADER_BOX_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, need_size);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->creation_time = ntoh_u32(p_box->creation_time);
	p_box->modification_time = ntoh_u32(p_box->modification_time);
	p_box->time_scale = hton_u32(p_box->time_scale);
	p_box->duration = hton_u32(p_box->duration);

	p_box->rate = ntoh_u32(p_box->rate);
	p_box->volume = ntoh_u16(p_box->volume);

	p_box->next_track_id = ntoh_u32(p_box->next_track_id);

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_track_header_box(ISO_TRACK_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	need_size = MP4_TRACK_HEADER_BOX_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, need_size);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->creation_time = ntoh_u32(p_box->creation_time);
	p_box->modification_time = ntoh_u32(p_box->modification_time);
	p_box->track_id = hton_u32(p_box->track_id);
	p_box->duration = hton_u32(p_box->duration);

	p_box->layer = ntoh_u16(p_box->layer);
	p_box->alternate_group = ntoh_u16(p_box->alternate_group);
	p_box->volume = ntoh_u16(p_box->volume);

	p_box->width = ntoh_u32(p_box->width);
	p_box->height = ntoh_u32(p_box->height);

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_media_header_box(ISO_MEDIA_HEADER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	need_size = MP4_MEDIA_HEADER_BOX_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, need_size);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->creation_time = ntoh_u32(p_box->creation_time);
	p_box->modification_time = ntoh_u32(p_box->modification_time);
	p_box->time_scale = hton_u32(p_box->time_scale);
	p_box->duration = hton_u32(p_box->duration);

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_handler_box(ISO_HANDLER_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;

	need_size = MP4_FULL_BOX_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(&p_box->full_box, p_buf, MP4_FULL_BOX_SIZE);

	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_HANDLER_BOX_MIN_SIZE);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->handler_type = ntoh_u32(p_box->handler_type);
	p_box->p_name = p_buf + MP4_HANDLER_BOX_MIN_SIZE;
	p_box->name_size = need_size - MP4_HANDLER_BOX_MIN_SIZE;

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_visual_sample_desc_box(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_entry_count;
	uint32 total_demux_size;
	int32  box_left_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_entry_count = p_box->entry_count;
	total_demux_size = 0;

	need_size = MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE);

	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;
	box_left_size -= MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);

	if( p_box->entry_count > 0 )
	{
		if( p_box->p_entry == NULL )
		{
			return -3;
		}

		if( total_entry_count < p_box->entry_count )
		{
			return -4;
		}

		for( i = 0; i < p_box->entry_count; i++ )
		{
			if( box_left_size < MP4_VISUAL_SAMPLE_ENTRY_SIZE )
			{
				return -1;
			}

			memcpy(p_box->p_entry + i, p_buf + total_demux_size, MP4_VISUAL_SAMPLE_ENTRY_SIZE);
			p_box->p_entry[i].entry.box.size = ntoh_u32(p_box->p_entry[i].entry.box.size);
			p_box->p_entry[i].entry.box.type = ntoh_u32(p_box->p_entry[i].entry.box.type);
			p_box->p_entry[i].entry.data_ref_index = ntoh_u16(p_box->p_entry[i].entry.data_ref_index);
			p_box->p_entry[i].width = ntoh_u16(p_box->p_entry[i].width);
			p_box->p_entry[i].height = ntoh_u16(p_box->p_entry[i].height);
			p_box->p_entry[i].horiz_resolution = ntoh_u32(p_box->p_entry[i].horiz_resolution);
			p_box->p_entry[i].vert_resolution = ntoh_u32(p_box->p_entry[i].vert_resolution);
			p_box->p_entry[i].frame_count = ntoh_u16(p_box->p_entry[i].frame_count);
			p_box->p_entry[i].depth = ntoh_u16(p_box->p_entry[i].depth);

			if( p_box->p_entry[i].entry.box.size < MP4_VISUAL_SAMPLE_ENTRY_SIZE )
			{
				return -1;
			}

			total_demux_size += p_box->p_entry[i].entry.box.size;
			box_left_size -= p_box->p_entry[i].entry.box.size;
		}
	}

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_audio_sample_desc_box(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_entry_count;
	uint32 total_demux_size;
	int32  box_left_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_entry_count = p_box->entry_count;
	total_demux_size = 0;

	need_size = MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE);

	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;
	box_left_size -= MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);

	if( p_box->entry_count > 0 )
	{
		if( p_box->p_entry == NULL )
		{
			return -3;
		}

		if( total_entry_count < p_box->entry_count )
		{
			return -4;
		}

		for( i = 0; i < p_box->entry_count; i++ )
		{
			if( box_left_size < MP4_AUDIO_SAMPLE_ENTRY_SIZE )
			{
				return -1;
			}

			memcpy(p_box->p_entry + i, p_buf + total_demux_size, MP4_AUDIO_SAMPLE_ENTRY_SIZE);
			p_box->p_entry[i].entry.box.size = ntoh_u32(p_box->p_entry[i].entry.box.size);
			p_box->p_entry[i].entry.box.type = ntoh_u32(p_box->p_entry[i].entry.box.type);
			p_box->p_entry[i].entry.data_ref_index = ntoh_u16(p_box->p_entry[i].entry.data_ref_index);
			p_box->p_entry[i].chn_count = ntoh_u16(p_box->p_entry[i].chn_count);
			p_box->p_entry[i].sample_size = ntoh_u16(p_box->p_entry[i].sample_size);
			p_box->p_entry[i].sample_rate = ntoh_u32(p_box->p_entry[i].sample_rate);

			if( p_box->p_entry[i].entry.box.size < MP4_AUDIO_SAMPLE_ENTRY_SIZE )
			{
				return -1;
			}

			total_demux_size += p_box->p_entry[i].entry.box.size;
			box_left_size -= p_box->p_entry[i].entry.box.size;
		}
	}

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_time_to_sample_box(ISO_TIME_TO_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE);

#if 0
	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;
	box_left_size -= MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;
#endif

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);
	p_box->p_entry = NULL;

#if 0
	if( p_box->entry_count > 0 )
	{
		p_box->p_entry = (ISO_TIME_TO_SAMPLE_ENTRY*)(p_buf + total_demux_size);
		for( i = 0; i < p_box->entry_count; i++ )
		{
			if( box_left_size < MP4_TIME_TO_SAMPLE_ENTRY_SIZE )
			{
				return -1;
			}

			p_box->p_entry[i].sample_count = ntoh_u32(p_box->p_entry[i].sample_count);
			p_box->p_entry[i].sample_delta = ntoh_u32(p_box->p_entry[i].sample_delta);

			total_demux_size += MP4_TIME_TO_SAMPLE_ENTRY_SIZE;
			box_left_size -= MP4_TIME_TO_SAMPLE_ENTRY_SIZE;
		}
	}
#endif

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_comp_time_to_sample_box(ISO_COMP_TIME_TO_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_COMP_TIME_TO_SAMPLE_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_COMP_TIME_TO_SAMPLE_BOX_MIN_SIZE);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);
	p_box->p_entry = NULL;

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_sample_size_box(ISO_SAMPLE_SIZE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_SAMPLE_SIZE_BOX_MIN_SIZE);

#if 0
	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_SAMPLE_SIZE_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_SAMPLE_SIZE_BOX_MIN_SIZE;
	box_left_size -= MP4_SAMPLE_SIZE_BOX_MIN_SIZE;
#endif

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->sample_size = ntoh_u32(p_box->sample_size);
	p_box->sample_count = ntoh_u32(p_box->sample_count);
	p_box->p_entry = NULL;

#if 0
	if( p_box->sample_size == 0 )
	{
		p_box->p_entry = (ISO_SAMPLE_SIZE_ENTRY*)(p_buf + total_demux_size);
		for( i = 0; i < p_box->sample_count; i++ )
		{
			if( box_left_size < MP4_SAMPLE_SIZE_ENTRY_SIZE )
			{
				return -1;
			}

			p_box->p_entry[i].entry_size = ntoh_u32(p_box->p_entry[i].entry_size);

			total_demux_size += MP4_SAMPLE_SIZE_ENTRY_SIZE;
			box_left_size -= MP4_SAMPLE_SIZE_ENTRY_SIZE;
		}
	}
#endif

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_sample_to_chunk_box(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE);

#if 0
	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;
	box_left_size -= MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;
#endif

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);
	p_box->p_entry = NULL;

#if 0
	if( p_box->entry_count > 0 )
	{
		p_box->p_entry = (ISO_SAMPLE_TO_CHUNK_ENTRY*)(p_buf + total_demux_size);
		for( i = 0; i < p_box->entry_count; i++ )
		{
			if( box_left_size < MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE )
			{
				return -1;
			}

			p_box->p_entry[i].first_chunk = ntoh_u32(p_box->p_entry[i].first_chunk);
			p_box->p_entry[i].sample_per_chunk = ntoh_u32(p_box->p_entry[i].sample_per_chunk);
			p_box->p_entry[i].sample_desc_index = ntoh_u32(p_box->p_entry[i].sample_desc_index);

			total_demux_size += MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
			box_left_size -= MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
		}
	}
#endif

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_chunk_offset_box(ISO_CHUNK_OFFSET_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_CHUNK_OFFSET_BOX_MIN_SIZE);

#if 0
	need_size = ntoh_u32(p_box->full_box.box.size);
	if( left_size < need_size )
	{
		return -2;
	}

	box_left_size = need_size;
	if( box_left_size < MP4_CHUNK_OFFSET_BOX_MIN_SIZE )
	{
		return -1;
	}

	total_demux_size += MP4_CHUNK_OFFSET_BOX_MIN_SIZE;
	box_left_size -= MP4_CHUNK_OFFSET_BOX_MIN_SIZE;
#endif

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);
	p_box->p_entry = NULL;

#if 0
	if( p_box->entry_count > 0 )
	{
		p_box->p_entry = (ISO_CHUNK_OFFSET_ENTRY*)(p_buf + total_demux_size);
		for( i = 0; i < p_box->entry_count; i++ )
		{
			if( box_left_size < MP4_CHUNK_OFFSET_ENTRY_SIZE )
			{
				return -1;
			}

			p_box->p_entry[i].chunk_offset = ntoh_u32(p_box->p_entry[i].chunk_offset);

			total_demux_size += MP4_CHUNK_OFFSET_ENTRY_SIZE;
			box_left_size -= MP4_CHUNK_OFFSET_ENTRY_SIZE;
		}
	}
#endif

	*p_demux_size = need_size;

	return 0;
}

int32 mp4_demux_sync_sample_box(ISO_SYNC_SAMPLE_BOX *p_box, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_demux_size;
	//int32  box_left_size;

	//uint32 i;

	if( p_box == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return -1;
	}

	*p_demux_size = 0;
	total_demux_size = 0;

	need_size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	memcpy(p_box, p_buf, MP4_SYNC_SAMPLE_BOX_MIN_SIZE);

	p_box->full_box.box.size = ntoh_u32(p_box->full_box.box.size);
	p_box->full_box.box.type = ntoh_u32(p_box->full_box.box.type);
	p_box->full_box.version_flag.value = ntoh_u32(p_box->full_box.version_flag.value);

	p_box->entry_count = ntoh_u32(p_box->entry_count);
	p_box->p_entry = NULL;

	*p_demux_size = need_size;

	return 0;
}
