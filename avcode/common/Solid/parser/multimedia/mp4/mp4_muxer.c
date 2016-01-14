
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <public/gen_endian.h>
#include "mp4_mux.h"
#include "mp4_avc_mux.h"
#include "mp4_aac_mux.h"
#include "mp4_muxer.h"



/////////////////////////////// Outter Interface //////////////////////////////
int32 mp4_make_file_type_box(ISO_FILE_TYPE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_file_type_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->box.size = need_size;
	ret = mp4_file_type_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_handler_box(ISO_HANDLER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_handler_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_handler_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_data_ref_box(ISO_DATA_REF_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_data_ref_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_data_ref_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_visual_sample_desc_box(ISO_VISUAL_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_visual_sample_desc_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_visual_sample_desc_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_avc_sample_desc_box(AVC_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_avc_calc_sample_desc_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_avc_sample_desc_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_audio_sample_desc_box(ISO_AUDIO_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_audio_sample_desc_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_audio_sample_desc_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_aac_sample_desc_box(AAC_SAMPLE_DESC_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_aac_calc_sample_desc_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_aac_sample_desc_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_time_to_sample_box(ISO_TIME_TO_SAMPLE_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{

	uint32 need_size;
	int32  ret;

	ret = mp4_calc_time_to_sample_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_time_to_sample_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_sample_to_chunk_box(ISO_SAMPLE_TO_CHUNK_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_sample_to_chunk_box_total_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_sample_to_chunk_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_extend_box(ISO_MOVIE_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{

	int32  ret;

	ret = mp4_movie_extend_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_extend_header_box(ISO_MOVIE_EXTEND_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_movie_extend_header_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_track_extend_box(ISO_TRACK_EXTEND_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_track_extend_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_fragment_box(ISO_MOVIE_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_movie_fragment_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_fragment_header_box(ISO_MOVIE_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_movie_fragment_header_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_track_fragment_box(ISO_TRACK_FRAGMENT_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_track_fragment_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_track_fragment_header_box(ISO_TRACK_FRAGMENT_HEADER_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_track_fragment_header_box_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_track_fragment_header_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_track_fragment_run_box(ISO_TRACK_FRAGMENT_RUN_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_track_fragment_run_box_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_track_fragment_run_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_fragment_random_access_box(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_movie_fragment_random_access_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_track_fragment_random_access_box(ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_track_fragment_random_access_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_movie_fragment_random_access_offset_box(ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	int32  ret;

	ret = mp4_movie_fragment_random_access_offset_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}

int32 mp4_make_meta_box(ISO_META_BOX *p_box, uint8 *p_buf, int32 *p_buf_size)
{
	uint32 need_size;
	int32  ret;

	ret = mp4_calc_meta_box_size(p_box, &need_size);
	if( ret )
	{
		return ret;
	}

	p_box->full_box.box.size = need_size;
	ret = mp4_meta_box_mux(p_box, p_buf, p_buf_size);

	return ret;
}
