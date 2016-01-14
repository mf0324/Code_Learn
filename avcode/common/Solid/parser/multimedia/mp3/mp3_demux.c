
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>
//#include <public/gen_endian.h>
//#include <util/log_debug.h>

#include "mp3_file_const.h"
#include "mp3_demux.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 mp3_demux_tag(MP3_ID3V2_TAG* p_tag, uint8* p_data, uint32 data_size, uint32* p_demux_size)
{
	uint32 left_size = data_size;
	//uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_tag == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	if( left_size < MP3_TAG_HEADER_SIZE )
	{
		*p_demux_size = MP3_TAG_HEADER_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;

	if( !(p_uint8[0] == 0x49 && p_uint8[1] == 0x44 && p_uint8[2] == 0x33) )
	{
		/** invalid id */
		*p_demux_size = 1;
		return GEN_E_WRONG_FORMAT;
	}

	p_tag->id = (((uint32)p_uint8[0]) << 24) | (((uint32)p_uint8[1]) << 16) | (((uint32)p_uint8[2]) << 8);
	p_tag->version = (((uint32)p_uint8[3]) << 8) | (p_uint8[4]);
	p_tag->flag = p_uint8[5];
	p_tag->size = ((p_uint8[6] & 0x7F) << 21) | ((p_uint8[7] & 0x7F) << 14) | ((p_uint8[8] & 0x7F) << 7) | (p_uint8[9] & 0x7F);

	*p_demux_size = MP3_TAG_HEADER_SIZE;

	return GEN_S_OK;
}

int32 mp3_demux_frame(MP3_ID3V2_FRAME* p_frame, uint8* p_data, uint32 data_size, uint32* p_demux_size)
{
	uint32 left_size = data_size;

	uint8* p_uint8 = NULL;

	if( p_frame == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	if( left_size < MP3_TAG_FRAME_HEADER_SIZE )
	{
		*p_demux_size = MP3_TAG_FRAME_HEADER_SIZE;
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;

	p_frame->id[0] = p_uint8[0];
	p_frame->id[1] = p_uint8[1];
	p_frame->id[2] = p_uint8[2];
	p_frame->id[3] = p_uint8[3];

	p_frame->size = (((uint32)p_uint8[4]) << 24) | (((uint32)p_uint8[5]) << 16) | (((uint32)p_uint8[6]) << 8) | (p_uint8[7]);
	p_frame->flag = (((uint32)p_uint8[8]) << 8) | p_uint8[9];

	*p_demux_size = MP3_TAG_FRAME_HEADER_SIZE;

	return GEN_S_OK;
}
