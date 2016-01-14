
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <util/log_debug.h>

#include "mp4_public.h"
#include "mp4_mux.h"


/////////////////////////////////////外部接口//////////////////////////////////
int32 mp4_calc_file_type_box_total_size(ISO_FILE_TYPE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_FILE_TYPE_BOX_MIN_SIZE + p_box->brand_count * MP4_FILE_TYPE_BOX_BRAND_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_file_type_box_mux(ISO_FILE_TYPE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_FILE_TYPE_BOX box;

	uint32 need_size;
	uint32 total_size;

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

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_FILE_TYPE_BOX_TYPE);

	box.major_brand = hton_u32(p_box->major_brand);
	box.minor_version = hton_u32(p_box->minor_version);

	memcpy(p_buf, &box, MP4_FILE_TYPE_BOX_MIN_SIZE);
	total_size = MP4_FILE_TYPE_BOX_MIN_SIZE;

	if( p_box->brand_count > 0 && p_box->p_brand )
	{
		memcpy(p_buf + total_size, p_box->p_brand, p_box->brand_count * MP4_FILE_TYPE_BOX_BRAND_SIZE);
		total_size += (p_box->brand_count * MP4_FILE_TYPE_BOX_BRAND_SIZE);
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_movie_box_mux(ISO_MOVIE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_BOX_SIZE;
		return -2;
	}

	p_box->box.size = hton_u32(p_box->box.size);
	p_box->box.type = hton_u32(MP4_MOVIE_BOX_TYPE);

	memcpy(p_buf, p_box, MP4_MOVIE_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_BOX_SIZE;

	return 0;
}

int32 mp4_media_data_box_mux(ISO_MEDIA_DATA_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MEDIA_DATA_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MEDIA_DATA_BOX_SIZE )
	{
		*p_buf_size = MP4_MEDIA_DATA_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MEDIA_DATA_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MEDIA_DATA_BOX_SIZE);

	*p_buf_size = MP4_MEDIA_DATA_BOX_SIZE;

	return 0;
}

int32 mp4_movie_header_box_mux(ISO_MOVIE_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_MOVIE_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_MOVIE_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.creation_time = hton_u32(p_box->creation_time);
	box.modification_time = hton_u32(p_box->modification_time);
	box.time_scale = hton_u32(p_box->time_scale);
	box.duration = hton_u32(p_box->duration);

	box.rate = hton_u32(MP4_MOVIE_HEADER_BOX_RATE);
	box.volume = hton_u16(MP4_MOVIE_HEADER_BOX_VOLUME);
	box.padding = hton_u16(0x0);
	box.resv[0] = hton_u32(0x0);
	box.resv[1] = hton_u32(0x0);

	/** see iso/iec 14496-12 */
	box.matrix[0] = hton_u32(0x00010000);
	box.matrix[1] = hton_u32(0x0);
	box.matrix[2] = hton_u32(0x0);
	box.matrix[3] = hton_u32(0x0);
	box.matrix[4] = hton_u32(0x00010000);
	box.matrix[5] = hton_u32(0x0);
	box.matrix[6] = hton_u32(0x0);
	box.matrix[7] = hton_u32(0x0);
	box.matrix[8] = hton_u32(0x40000000);

	box.pre_define[0] = hton_u32(0x0);
	box.pre_define[1] = hton_u32(0x0);
	box.pre_define[2] = hton_u32(0x0);
	box.pre_define[3] = hton_u32(0x0);
	box.pre_define[4] = hton_u32(0x0);
	box.pre_define[5] = hton_u32(0x0);

	box.next_track_id = hton_u32(p_box->next_track_id);

	memcpy(p_buf, &box, MP4_MOVIE_HEADER_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_track_box_mux(ISO_TRACK_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_TRACK_BOX_SIZE )
	{
		*p_buf_size = MP4_TRACK_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_TRACK_BOX_TYPE);

	memcpy(p_buf, &box, MP4_TRACK_BOX_SIZE);

	*p_buf_size = MP4_TRACK_BOX_SIZE;

	return 0;
}

int32 mp4_track_header_box_mux(ISO_TRACK_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_TRACK_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_TRACK_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_TRACK_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_TRACK_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( p_box->full_box.version_flag.value );

	box.creation_time = hton_u32(p_box->creation_time);
	box.modification_time = hton_u32(p_box->modification_time);
	box.track_id = hton_u32(p_box->track_id);
	box.resv = hton_u32(0x0);
	box.duration = hton_u32(p_box->duration);

	box.resv2[0] = hton_u32(0x0);
	box.resv2[1] = hton_u32(0x0);
	box.layer = hton_u16(0x0);
	box.alternate_group = hton_u16(0x0);
	box.volume = hton_u16(p_box->volume);
	box.padding = hton_u16(0x0);

	/** see iso/iec 14496-12 */
	box.matrix[0] = hton_u32(0x00010000);
	box.matrix[1] = hton_u32(0x0);
	box.matrix[2] = hton_u32(0x0);
	box.matrix[3] = hton_u32(0x0);
	box.matrix[4] = hton_u32(0x00010000);
	box.matrix[5] = hton_u32(0x0);
	box.matrix[6] = hton_u32(0x0);
	box.matrix[7] = hton_u32(0x0);
	box.matrix[8] = hton_u32(0x40000000);

	box.width = hton_u32(p_box->width);
	box.height = hton_u32(p_box->height);

	memcpy(p_buf, &box, MP4_TRACK_HEADER_BOX_SIZE);

	*p_buf_size = MP4_TRACK_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_media_box_mux(ISO_MEDIA_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MEDIA_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MEDIA_BOX_SIZE )
	{
		*p_buf_size = MP4_MEDIA_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MEDIA_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MEDIA_BOX_SIZE);

	*p_buf_size = MP4_MEDIA_BOX_SIZE;

	return 0;
}

int32 mp4_media_header_box_mux(ISO_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MEDIA_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MEDIA_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_MEDIA_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_MEDIA_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_MEDIA_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.creation_time = hton_u32(p_box->creation_time);
	box.modification_time = hton_u32(p_box->modification_time);
	box.time_scale = hton_u32(p_box->time_scale);
	box.duration = hton_u32(p_box->duration);

	box.language = hton_u16(p_box->language);
	box.pre_define = hton_u16(0x0);

	memcpy(p_buf, &box, MP4_MEDIA_HEADER_BOX_SIZE);

	*p_buf_size = MP4_MEDIA_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_calc_handler_box_total_size(ISO_HANDLER_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_HANDLER_BOX_MIN_SIZE + p_box->name_size;

	*p_size = need_size;

	return 0;
}

int32 mp4_handler_box_mux(ISO_HANDLER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_HANDLER_BOX box;

	uint32 need_size;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_HANDLER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.pre_define = hton_u32(0x0);
	box.handler_type = hton_u32(p_box->handler_type);
	box.resv[0] = hton_u32(0x0);
	box.resv[1] = hton_u32(0x0);
	box.resv[2] = hton_u32(0x0);

	memcpy(p_buf, &box, MP4_HANDLER_BOX_MIN_SIZE);

	if( p_box->name_size > 0 && p_box->p_name )
	{
		memcpy(p_buf + MP4_HANDLER_BOX_MIN_SIZE, p_box->p_name, p_box->name_size);
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_media_info_box_mux(ISO_MEDIA_INFO_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MEDIA_INFO_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MEDIA_INFO_BOX_SIZE )
	{
		*p_buf_size = MP4_MEDIA_INFO_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MEDIA_INFO_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MEDIA_INFO_BOX_SIZE);

	*p_buf_size = MP4_MEDIA_INFO_BOX_SIZE;

	return 0;
}

int32 mp4_video_media_header_box_mux(ISO_VIDEO_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_VIDEO_MEDIA_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_VIDEO_MEDIA_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_VIDEO_MEDIA_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_VIDEO_MEDIA_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_VIDEO_MEDIA_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x1 );

	//box.graph_mode = hton_u16(p_box->graph_mode);
	box.graph_mode = hton_u16(0x0);
	box.op_color[0] = hton_u16(0x0);
	box.op_color[1] = hton_u16(0x0);
	box.op_color[2] = hton_u16(0x0);

	memcpy(p_buf, &box, MP4_VIDEO_MEDIA_HEADER_BOX_SIZE);

	*p_buf_size = MP4_VIDEO_MEDIA_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_sound_media_header_box_mux(ISO_SOUND_MEDIA_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_SOUND_MEDIA_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_SOUND_MEDIA_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_SOUND_MEDIA_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_SOUND_MEDIA_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_SOUND_MEDIA_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.balance = hton_u16(0x0);
	box.resv = hton_u16(0x0);

	memcpy(p_buf, &box, MP4_SOUND_MEDIA_HEADER_BOX_SIZE);

	*p_buf_size = MP4_SOUND_MEDIA_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_data_info_box_mux(ISO_DATA_INFO_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_DATA_INFO_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_DATA_INFO_BOX_SIZE )
	{
		*p_buf_size = MP4_DATA_INFO_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_DATA_INFO_BOX_TYPE);

	memcpy(p_buf, &box, MP4_DATA_INFO_BOX_SIZE);

	*p_buf_size = MP4_DATA_INFO_BOX_SIZE;

	return 0;
}

int32 mp4_calc_data_ref_box_total_size(ISO_DATA_REF_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	uint32 i;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_DATA_REF_BOX_MIN_SIZE;

	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			switch( p_box->p_entry[i].entry_type )
			{
			case MP4_DATA_ENTRY_TYPE_URL:
				{
					p_box->p_entry[i].entry_box.url.full_box.box.size = MP4_DATA_ENTRY_URL_BOX_MIN_SIZE + p_box->p_entry[i].entry_box.url.location_size;

					need_size += p_box->p_entry[i].entry_box.url.full_box.box.size;
				}
				break;

			case MP4_DATA_ENTRY_TYPE_URN:
				{
					p_box->p_entry[i].entry_box.urn.full_box.box.size =
						MP4_DATA_ENTRY_URN_BOX_MIN_SIZE + p_box->p_entry[i].entry_box.urn.name_size + p_box->p_entry[i].entry_box.urn.location_size;

					need_size += p_box->p_entry[i].entry_box.urn.full_box.box.size;
				}
				break;

			default:
				{
				}
			}

		}
	}

	*p_size = need_size;

	return 0;
}

int32 mp4_data_ref_box_mux(ISO_DATA_REF_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_DATA_REF_BOX box;
	ISO_DATA_ENTRY entry;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_DATA_REF_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_DATA_REF_BOX_MIN_SIZE);
	total_size = MP4_DATA_REF_BOX_MIN_SIZE;

	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			switch( p_box->p_entry[i].entry_type )
			{
			case MP4_DATA_ENTRY_TYPE_URL:
				{
					entry.entry_box.url.full_box.box.size = hton_u32(p_box->p_entry[i].entry_box.url.full_box.box.size);
					entry.entry_box.url.full_box.box.type = hton_u32(MP4_DATA_ENTRY_URL_BOX_TYPE);
					entry.entry_box.url.full_box.version_flag.value = hton_u32(p_box->p_entry[i].entry_box.url.full_box.version_flag.value);

					memcpy(p_buf + total_size, &entry.entry_box.url, MP4_DATA_ENTRY_URL_BOX_MIN_SIZE);
					total_size += MP4_DATA_ENTRY_URL_BOX_MIN_SIZE;

					if( p_box->p_entry[i].entry_box.url.location_size > 0 && p_box->p_entry[i].entry_box.url.p_location )
					{
						memcpy(p_buf + total_size, p_box->p_entry[i].entry_box.url.p_location, p_box->p_entry[i].entry_box.url.location_size);
						total_size += p_box->p_entry[i].entry_box.url.location_size;
					}
				}
				break;

			case MP4_DATA_ENTRY_TYPE_URN:
				{
					entry.entry_box.urn.full_box.box.size = hton_u32(p_box->p_entry[i].entry_box.urn.full_box.box.size);
					entry.entry_box.urn.full_box.box.type = hton_u32(MP4_DATA_ENTRY_URN_BOX_TYPE);
					entry.entry_box.urn.full_box.version_flag.value = hton_u32(p_box->p_entry[i].entry_box.urn.full_box.version_flag.value);

					memcpy(p_buf + total_size, &entry.entry_box.urn, MP4_DATA_ENTRY_URN_BOX_MIN_SIZE);
					total_size += MP4_DATA_ENTRY_URN_BOX_MIN_SIZE;

					if( p_box->p_entry[i].entry_box.urn.name_size > 0 && p_box->p_entry[i].entry_box.urn.p_name )
					{
						memcpy(p_buf + total_size, p_box->p_entry[i].entry_box.urn.p_name, p_box->p_entry[i].entry_box.urn.name_size);
						total_size += p_box->p_entry[i].entry_box.urn.name_size;
					}

					if( p_box->p_entry[i].entry_box.urn.location_size > 0 && p_box->p_entry[i].entry_box.urn.p_location )
					{
						memcpy(p_buf + total_size, p_box->p_entry[i].entry_box.urn.p_location, p_box->p_entry[i].entry_box.urn.location_size);
						total_size += p_box->p_entry[i].entry_box.urn.location_size;
					}
				}
				break;

			default:
				{
				}
			}

		}//end for
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_sample_table_box_mux(ISO_SAMPLE_TABLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_SAMPLE_TABLE_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_SAMPLE_TABLE_BOX_SIZE )
	{
		*p_buf_size = MP4_SAMPLE_TABLE_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_SAMPLE_TABLE_BOX_TYPE);

	memcpy(p_buf, &box, MP4_SAMPLE_TABLE_BOX_SIZE);

	*p_buf_size = MP4_SAMPLE_TABLE_BOX_SIZE;

	return 0;
}

int32 mp4_calc_time_to_sample_box_size(ISO_TIME_TO_SAMPLE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_time_to_sample_box_total_size(ISO_TIME_TO_SAMPLE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE + MP4_TIME_TO_SAMPLE_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_time_to_sample_box_mux(ISO_TIME_TO_SAMPLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TIME_TO_SAMPLE_BOX box;
	//ISO_TIME_TO_SAMPLE_ENTRY entry;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_TIME_TO_SAMPLE_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE);
	total_size = MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_visual_sample_desc_box_size(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_visual_sample_desc_box_total_size(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE + MP4_VISUAL_SAMPLE_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_visual_sample_desc_box_mux(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_VISUAL_SAMPLE_DESC_BOX box;
	ISO_VISUAL_SAMPLE_ENTRY entry;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_DESC_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE);
	total_size = MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE;

	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			entry.entry.box.size = hton_u32(MP4_VISUAL_SAMPLE_ENTRY_SIZE);
			//entry.entry.box.type = hton_u32(p_box->p_entry[i].entry.box.type);
			entry.entry.box.type = p_box->p_entry[i].entry.box.type;
			entry.entry.resv[0] = 0x0;
			entry.entry.resv[1] = 0x0;
			entry.entry.resv[2] = 0x0;
			entry.entry.resv[3] = 0x0;
			entry.entry.resv[4] = 0x0;
			entry.entry.resv[5] = 0x0;
			entry.entry.data_ref_index = hton_u16(p_box->p_entry[i].entry.data_ref_index);

			entry.pre_define = hton_u16(0x0);
			entry.padding = hton_u16(0x0);
			entry.pre_define2[0] = hton_u32(0x0);
			entry.pre_define2[1] = hton_u32(0x0);
			entry.pre_define2[2] = hton_u32(0x0);

			entry.width = hton_u16(p_box->p_entry[i].width);
			entry.height = hton_u16(p_box->p_entry[i].height);

			entry.horiz_resolution = hton_u32(MP4_VISUAL_SAMPLE_HORIZ_RESOLUTION);
			entry.vert_resolution = hton_u32(MP4_VISUAL_SAMPLE_VERT_RESOLUTION);
			entry.resv = hton_u32(0x0);
			entry.frame_count = hton_u16(MP4_VISUAL_SAMPLE_FRAME_COUNT);

			memcpy(entry.compressor_name, p_box->p_entry[i].compressor_name, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);

			entry.depth = hton_u16(MP4_VISUAL_SAMPLE_DEPTH);
			entry.pre_define3 = hton_u16(-1);

			memcpy(p_buf + total_size, &entry, MP4_VISUAL_SAMPLE_ENTRY_SIZE);
			total_size += MP4_VISUAL_SAMPLE_ENTRY_SIZE;
		}
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_audio_sample_desc_box_size(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_audio_sample_desc_box_total_size(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE + MP4_AUDIO_SAMPLE_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_audio_sample_desc_box_mux(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_AUDIO_SAMPLE_DESC_BOX box;
	ISO_AUDIO_SAMPLE_ENTRY entry;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_DESC_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE);
	total_size = MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE;

	if( p_box->entry_count > 0 && p_box->p_entry )
	{
		for( i = 0; i < p_box->entry_count; i++ )
		{
			entry.entry.box.size = hton_u32(MP4_AUDIO_SAMPLE_ENTRY_SIZE);
			//entry.entry.box.type = hton_u32(p_box->p_entry[i].entry.box.type);
			entry.entry.box.type = p_box->p_entry[i].entry.box.type;
			entry.entry.resv[0] = 0x0;
			entry.entry.resv[1] = 0x0;
			entry.entry.resv[2] = 0x0;
			entry.entry.resv[3] = 0x0;
			entry.entry.resv[4] = 0x0;
			entry.entry.resv[5] = 0x0;
			entry.entry.data_ref_index = hton_u16(p_box->p_entry[i].entry.data_ref_index);

			entry.resv[0] = hton_u32(0x0);
			entry.resv[1] = hton_u32(0x0);
			entry.chn_count = hton_u16(p_box->p_entry[i].chn_count);
			entry.sample_size = hton_u16(MP4_AUDIO_SAMPLE_SIZE);

			entry.pre_define = hton_u16(0x0);
			entry.padding = hton_u16(0x0);
			entry.sample_rate = hton_u32(p_box->p_entry[i].sample_rate);

			memcpy(p_buf + total_size, &entry, MP4_AUDIO_SAMPLE_ENTRY_SIZE);
			total_size += MP4_AUDIO_SAMPLE_ENTRY_SIZE;
		}
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_sample_size_box_size(ISO_SAMPLE_SIZE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_sample_size_box_total_size(ISO_SAMPLE_SIZE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE;

	if( p_box->sample_size == 0 )
	{
		need_size += (MP4_SAMPLE_SIZE_ENTRY_SIZE * p_box->sample_count);
	}

	*p_size = need_size;

	return 0;
}

int32 mp4_sample_size_box_mux(ISO_SAMPLE_SIZE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_SAMPLE_SIZE_BOX box;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_SIZE_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.sample_size = hton_u32(p_box->sample_size);
	box.sample_count = hton_u32(p_box->sample_count);

	memcpy(p_buf, &box, MP4_SAMPLE_SIZE_BOX_MIN_SIZE);
	total_size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_sample_to_chunk_box_size(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_sample_to_chunk_box_total_size(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE + MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_sample_to_chunk_box_mux(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_SAMPLE_TO_CHUNK_BOX box;
	//ISO_SAMPLE_TO_CHUNK_ENTRY entry;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SAMPLE_TO_CHUNK_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE);
	total_size = MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_chunk_offset_box_size(ISO_CHUNK_OFFSET_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_chunk_offset_box_total_size(ISO_CHUNK_OFFSET_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE + MP4_CHUNK_OFFSET_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_chunk_offset_box_mux(ISO_CHUNK_OFFSET_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_CHUNK_OFFSET_BOX box;


	uint32 need_size;
	uint32 total_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_CHUNK_OFFSET_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_CHUNK_OFFSET_BOX_MIN_SIZE);
	total_size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_sync_sample_box_size(ISO_SYNC_SAMPLE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_sync_sample_box_total_size(ISO_SYNC_SAMPLE_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE + MP4_SYNC_SAMPLE_ENTRY_SIZE * p_box->entry_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_sync_sample_box_mux(ISO_SYNC_SAMPLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_SYNC_SAMPLE_BOX box;

	uint32 need_size;
	uint32 total_size;

	uint32 i;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	need_size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE;
	if( *p_buf_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_SYNC_SAMPLE_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.entry_count = hton_u32(p_box->entry_count);

	memcpy(p_buf, &box, MP4_SYNC_SAMPLE_BOX_MIN_SIZE);
	total_size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE;

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_free_space_box_mux(ISO_FREE_SPACE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_FREE_SPACE_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_FREE_SPACE_BOX_SIZE )
	{
		*p_buf_size = MP4_FREE_SPACE_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_FREE_SPACE_BOX_TYPE);

	memcpy(p_buf, &box, MP4_FREE_SPACE_BOX_SIZE);

	*p_buf_size = MP4_FREE_SPACE_BOX_SIZE;

	return 0;
}

int32 mp4_movie_extend_box_mux(ISO_MOVIE_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_EXTEND_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_EXTEND_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_EXTEND_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MOVIE_EXTEND_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MOVIE_EXTEND_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_EXTEND_BOX_SIZE;

	return 0;
}

int32 mp4_movie_extend_header_box_mux(ISO_MOVIE_EXTEND_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_EXTEND_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_EXTEND_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_EXTEND_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_MOVIE_EXTEND_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_MOVIE_EXTEND_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.fragment_duration = hton_u32(p_box->fragment_duration);

	memcpy(p_buf, &box, MP4_MOVIE_EXTEND_HEADER_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_EXTEND_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_track_extend_box_mux(ISO_TRACK_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_EXTEND_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_TRACK_EXTEND_BOX_SIZE )
	{
		*p_buf_size = MP4_TRACK_EXTEND_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_TRACK_EXTEND_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_TRACK_EXTEND_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.track_id = hton_u32(p_box->track_id);
	box.def_sample_desc_index = hton_u32(p_box->def_sample_desc_index);
	box.def_sample_duration = hton_u32(p_box->def_sample_duration);
	box.def_sample_size = hton_u32(p_box->def_sample_size);
	box.def_sample_flag = hton_u32(p_box->def_sample_flag);

	memcpy(p_buf, &box, MP4_TRACK_EXTEND_BOX_SIZE);

	*p_buf_size = MP4_TRACK_EXTEND_BOX_SIZE;

	return 0;
}

int32 mp4_movie_fragment_box_mux(ISO_MOVIE_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_FRAGMENT_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_FRAGMENT_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_FRAGMENT_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MOVIE_FRAGMENT_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MOVIE_FRAGMENT_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_FRAGMENT_BOX_SIZE;

	return 0;
}

int32 mp4_movie_fragment_header_box_mux(ISO_MOVIE_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_FRAGMENT_HEADER_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_MOVIE_FRAGMENT_HEADER_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.seq_no = hton_u32(p_box->seq_no);

	memcpy(p_buf, &box, MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE;

	return 0;
}

int32 mp4_track_fragment_box_mux(ISO_TRACK_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_FRAGMENT_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_TRACK_FRAGMENT_BOX_SIZE )
	{
		*p_buf_size = MP4_TRACK_FRAGMENT_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_TRACK_FRAGMENT_BOX_TYPE);

	memcpy(p_buf, &box, MP4_TRACK_FRAGMENT_BOX_SIZE);

	*p_buf_size = MP4_TRACK_FRAGMENT_BOX_SIZE;

	return 0;
}

int32 mp4_calc_track_fragment_header_box_size(ISO_TRACK_FRAGMENT_HEADER_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;
	uint32 flag;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TRACK_FRAGMENT_HEADER_BOX_MIN_SIZE;

	flag = p_box->full_box.version_flag.value & 0xFFFFFF;

	if( flag & MP4_BASE_DATA_OFFSET_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->base_data_offset);
	}

	if( flag & MP4_SAMPLE_DESC_INDEX_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->sample_desc_index);
	}

	if( flag & MP4_DEFAULT_SAMPLE_DURATION_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->def_sample_duration);
	}

	if( flag & MP4_DEFAULT_SAMPLE_SIZE_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->def_sample_size);
	}

	if( flag & MP4_DEFAULT_SAMPLE_FLAG_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->def_sample_flag);
	}

	*p_size = need_size;

	return 0;
}

int32 mp4_track_fragment_header_box_mux(ISO_TRACK_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_FRAGMENT_HEADER_BOX box;

	uint32 need_size;
	uint32 total_size;
	uint32 flag;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_TRACK_FRAGMENT_HEADER_BOX_TYPE);
	flag = p_box->full_box.version_flag.value & 0xFFFFFF;
	box.full_box.version_flag.value = hton_u32(p_box->full_box.version_flag.value);

	box.track_id = hton_u32(p_box->track_id);

	memcpy(p_buf, &box, MP4_TRACK_FRAGMENT_HEADER_BOX_MIN_SIZE);
	total_size = MP4_TRACK_FRAGMENT_HEADER_BOX_MIN_SIZE;

	if( flag & MP4_BASE_DATA_OFFSET_PRESENT_FLAG )
	{
		box.base_data_offset = hton_u64(p_box->base_data_offset);

		memcpy(p_buf + total_size, &box.base_data_offset, sizeof(box.base_data_offset));
		total_size += sizeof(box.base_data_offset);
	}

	if( flag & MP4_SAMPLE_DESC_INDEX_PRESENT_FLAG )
	{
		box.sample_desc_index = hton_u32(p_box->sample_desc_index);

		memcpy(p_buf + total_size, &box.sample_desc_index, sizeof(box.sample_desc_index));
		total_size += sizeof(box.sample_desc_index);
	}

	if( flag & MP4_DEFAULT_SAMPLE_DURATION_PRESENT_FLAG )
	{
		box.def_sample_duration = hton_u32(p_box->def_sample_duration);

		memcpy(p_buf + total_size, &box.def_sample_duration, sizeof(box.def_sample_duration));
		total_size += sizeof(box.def_sample_duration);
	}

	if( flag & MP4_DEFAULT_SAMPLE_SIZE_PRESENT_FLAG )
	{
		box.def_sample_size = hton_u32(p_box->def_sample_size);

		memcpy(p_buf + total_size, &box.def_sample_size, sizeof(box.def_sample_size));
		total_size += sizeof(box.def_sample_size);
	}

	if( flag & MP4_DEFAULT_SAMPLE_FLAG_PRESENT_FLAG )
	{
		box.def_sample_flag = hton_u32(p_box->def_sample_flag);

		memcpy(p_buf + total_size, &box.def_sample_flag, sizeof(box.def_sample_flag));
		total_size += sizeof(box.def_sample_flag);
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_calc_track_fragment_run_box_size(ISO_TRACK_FRAGMENT_RUN_BOX *p_box, uint32 *p_size)
{
	ISO_TRACK_FRAGMENT_SAMPLE frag_sample;
	uint32 frag_sample_size;

	uint32 need_size;
	uint32 flag;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TRACK_FRAGMENT_RUN_BOX_MIN_SIZE;

	flag = p_box->full_box.version_flag.value & 0xFFFFFF;

	if( flag & MP4_DATA_OFFSET_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->data_offset);
	}

	if( flag & MP4_FIRST_SAMPLE_FLAG_PRESENT_FLAG )
	{
		need_size += sizeof(p_box->first_sample_flag);
	}

	frag_sample_size = 0;

	if( flag & MP4_SAMPLE_DURATION_PRESENT_FLAG )
	{
		frag_sample_size += sizeof(frag_sample.sample_duration);
	}

	if( flag & MP4_SAMPLE_SIZE_PRESENT_FLAG )
	{
		frag_sample_size += sizeof(frag_sample.sample_size);
	}

	if( flag & MP4_SAMPLE_FLAG_PRESENT_FLAG )
	{
		frag_sample_size += sizeof(frag_sample.sample_flag);
	}

	if( flag & MP4_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT_FLAG )
	{
		frag_sample_size += sizeof(frag_sample.sample_composition_time_offset);
	}

	need_size += frag_sample_size * p_box->sample_count;

	*p_size = need_size;

	return 0;
}

int32 mp4_track_fragment_run_box_mux(ISO_TRACK_FRAGMENT_RUN_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_TRACK_FRAGMENT_RUN_BOX box;
	ISO_TRACK_FRAGMENT_SAMPLE  frag_sample;
	ISO_TRACK_FRAGMENT_SAMPLE* p_sample = NULL;

	uint32 need_size;
	uint32 total_size;
	uint32 flag;

	uint32 i;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_TRACK_FRAGMENT_RUN_BOX_TYPE);
	flag = p_box->full_box.version_flag.value & 0xFFFFFF;
	box.full_box.version_flag.value = hton_u32(p_box->full_box.version_flag.value);

	box.sample_count = hton_u32(p_box->sample_count);

	memcpy(p_buf, &box, MP4_TRACK_FRAGMENT_RUN_BOX_MIN_SIZE);
	total_size = MP4_TRACK_FRAGMENT_RUN_BOX_MIN_SIZE;

	if( flag & MP4_DATA_OFFSET_PRESENT_FLAG )
	{
		box.data_offset = hton_u32(p_box->data_offset);

		memcpy(p_buf + total_size, &box.data_offset, sizeof(box.data_offset));
		total_size += sizeof(box.data_offset);
	}

	if( flag & MP4_FIRST_SAMPLE_FLAG_PRESENT_FLAG )
	{
		box.first_sample_flag = hton_u32(p_box->first_sample_flag);

		memcpy(p_buf + total_size, &box.first_sample_flag, sizeof(box.first_sample_flag));
		total_size += sizeof(box.first_sample_flag);
	}

	if( p_box->sample_count > 0 && p_box->p_sample )
	{
		for( i = 0; i < p_box->sample_count; i++ )
		{
			p_sample = p_box->p_sample + i;

			if( flag & MP4_SAMPLE_DURATION_PRESENT_FLAG )
			{
				frag_sample.sample_duration = hton_u32(p_sample->sample_duration);

				memcpy(p_buf + total_size, &frag_sample.sample_duration, sizeof(frag_sample.sample_duration));
				total_size += sizeof(frag_sample.sample_duration);
			}

			if( flag & MP4_SAMPLE_SIZE_PRESENT_FLAG )
			{
				frag_sample.sample_size = hton_u32(p_sample->sample_size);

				memcpy(p_buf + total_size, &frag_sample.sample_size, sizeof(frag_sample.sample_size));
				total_size += sizeof(frag_sample.sample_size);
			}

			if( flag & MP4_SAMPLE_FLAG_PRESENT_FLAG )
			{
				frag_sample.sample_flag = hton_u32(p_sample->sample_flag);

				memcpy(p_buf + total_size, &frag_sample.sample_flag, sizeof(frag_sample.sample_flag));
				total_size += sizeof(frag_sample.sample_flag);
			}

			if( flag & MP4_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT_FLAG )
			{
				frag_sample.sample_composition_time_offset = hton_u32(p_sample->sample_composition_time_offset);

				memcpy(p_buf + total_size, &frag_sample.sample_composition_time_offset, sizeof(frag_sample.sample_composition_time_offset));
				total_size += sizeof(frag_sample.sample_composition_time_offset);
			}
		}
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_movie_fragment_random_access_box_mux(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_SIZE;
		return -2;
	}

	box.box.size = hton_u32(p_box->box.size);
	box.box.type = hton_u32(MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_TYPE);

	memcpy(p_buf, &box, MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_SIZE;

	return 0;
}

int32 mp4_calc_track_fragment_random_access_box_size(ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;
	uint32 length_size_of_traf_num;
	uint32 length_size_of_trun_num;
	uint32 length_size_of_sample_num;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_MIN_SIZE;

	length_size_of_traf_num = (p_box->mixture >> MP4_TRAF_NUM_BIT_OFFSET) & 0x3;
	length_size_of_trun_num = (p_box->mixture >> MP4_TRUN_NUM_BIT_OFFSET) & 0x3;
	length_size_of_sample_num = (p_box->mixture >> MP4_SAMPLE_NUM_BIT_OFFSET) & 0x3;

	need_size += ((sizeof(uint32) * 2 + (length_size_of_traf_num + 1) + (length_size_of_trun_num + 1) + (length_size_of_sample_num + 1)) * p_box->entry_num);

	*p_size = need_size;

	return 0;
}

int32 mp4_track_fragment_random_access_box_mux(ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	BIT_STREAM_WRITER bs;

	ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX box;

	uint32 need_size;
	uint32 total_size;
	uint32 left_size;

	uint32 length_size_of_traf_num;
	uint32 length_size_of_trun_num;
	uint32 length_size_of_sample_num;

	uint32 i;

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
	left_size = *p_buf_size;

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.track_id = hton_u32(p_box->track_id);

	length_size_of_traf_num = (p_box->mixture >> MP4_TRAF_NUM_BIT_OFFSET) & 0x3;
	length_size_of_trun_num = (p_box->mixture >> MP4_TRUN_NUM_BIT_OFFSET) & 0x3;
	length_size_of_sample_num = (p_box->mixture >> MP4_SAMPLE_NUM_BIT_OFFSET) & 0x3;
	box.mixture = hton_u32(p_box->mixture);

	box.entry_num = hton_u32(p_box->entry_num);

	memcpy(p_buf, &box, MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_MIN_SIZE);
	total_size = MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_MIN_SIZE;
	left_size -= MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_MIN_SIZE;

	if( p_box->entry_num > 0 && p_box->p_entry )
	{
		bs_writer_init(&bs, p_buf + total_size, left_size, GEN_BYTE_BIT);

		for( i = 0; i < p_box->entry_num; i++ )
		{
			//p_box->p_entry[i].time = hton_u32(p_box->p_entry[i].time);
			bs_write_bits(&bs, p_box->p_entry[i].time, sizeof(uint32) * GEN_BYTE_BIT);

			//p_box->p_entry[i].moof_offset = hton_u32(p_box->p_entry[i].moof_offset);
			bs_write_bits(&bs, p_box->p_entry[i].moof_offset, sizeof(uint32) * GEN_BYTE_BIT);

			//p_box->p_entry[i].traf_number = hton_u32(p_box->p_entry[i].traf_number);
			bs_write_bits(&bs, p_box->p_entry[i].traf_number, (length_size_of_traf_num + 1) * GEN_BYTE_BIT);

			//p_box->p_entry[i].trun_number = hton_u32(p_box->p_entry[i].trun_number);
			bs_write_bits(&bs, p_box->p_entry[i].trun_number, (length_size_of_trun_num + 1) * GEN_BYTE_BIT);

			//p_box->p_entry[i].sample_number = hton_u32(p_box->p_entry[i].sample_number);
			bs_write_bits(&bs, p_box->p_entry[i].sample_number, (length_size_of_sample_num + 1) * GEN_BYTE_BIT);
		}
	}

	*p_buf_size = need_size;

	return 0;
}

int32 mp4_movie_fragment_random_access_offset_box_mux(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX box;

	if( p_box == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE )
	{
		*p_buf_size = MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE;
		return -2;
	}

	box.full_box.box.size = hton_u32(MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE);
	box.full_box.box.type = hton_u32(MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	box.size = hton_u32(p_box->size);

	memcpy(p_buf, &box, MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE);

	*p_buf_size = MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE;

	return 0;
}

int32 mp4_calc_meta_box_size(ISO_META_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_META_BOX_MIN_SIZE;

	*p_size = need_size;

	return 0;
}

int32 mp4_calc_meta_box_total_size(ISO_META_BOX *p_box, uint32 *p_size)
{
	uint32 need_size;

	if( p_box == NULL || p_size == NULL )
	{
		return -1;
	}

	need_size = MP4_META_BOX_MIN_SIZE + p_box->handler_box.full_box.box.size;

	*p_size = need_size;

	return 0;
}

int32 mp4_meta_box_mux(ISO_META_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	ISO_META_BOX box;

	uint32 need_size;
	uint32 total_size;

	int32  mux_size;

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

	box.full_box.box.size = hton_u32(p_box->full_box.box.size);
	box.full_box.box.type = hton_u32(MP4_META_BOX_TYPE);
	box.full_box.version_flag.value = hton_u32( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | 0x0 );

	memcpy(p_buf, &box, MP4_META_BOX_MIN_SIZE);
	total_size = MP4_META_BOX_MIN_SIZE;

	mux_size = *p_buf_size - total_size;
	ret = mp4_handler_box_mux(&p_box->handler_box, p_buf + total_size, &mux_size);
	if( ret )
	{
		*p_buf_size = need_size;
		return -2;
	}

	*p_buf_size = need_size;

	return 0;
}
