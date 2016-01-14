
#include "flv_const.h"
#include "flv_demux.h"

#include <public/gen_error.h>


////////////////////////////// Outter Interface ///////////////////////////////
int32 flv_demux_tag_header(FLV_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32  left_size;
	uint32  need_size;
	uint8*  p_uint8 = NULL;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = FLV_TAG_HEADER_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_buf;

	p_header->tag_type = p_uint8[0];
	p_header->data_size = (((uint32)p_uint8[1]) << 16) | (((uint32)p_uint8[2]) << 8) | ((uint32)p_uint8[3]);
	p_header->ts = (((uint32)p_uint8[7]) << 24) | (((uint32)p_uint8[4]) << 16) | (((uint32)p_uint8[5]) << 8) | ((uint32)p_uint8[6]);
	p_header->stream_id = (((uint32)p_uint8[8]) << 16) | (((uint32)p_uint8[9]) << 8) | ((uint32)p_uint8[10]);

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 flv_demux_video_tag_header(FLV_VIDEO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32  left_size;
	uint32  need_size;
	uint8*  p_uint8 = NULL;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = FLV_VIDEO_TAG_HEADER_MIN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_buf;

	p_header->frame_type = ((p_uint8[0] >> 4) & 0xF);
	p_header->codec_id = (p_uint8[0] & 0xF);

	if( p_header->codec_id == FLV_VIDEO_CODEC_ID_AVC )
	{
		need_size = FLV_AVC_VIDEO_TAG_HEADER_SIZE;
		if( left_size < need_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_header->avc_pack_type = p_uint8[1];
		p_header->cts_offset = (((uint32)p_uint8[2]) << 16) | (((uint32)p_uint8[3]) << 8) | ((uint32)p_uint8[4]);
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}


int32 flv_demux_audio_tag_header(FLV_AUDIO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32  left_size;
	uint32  need_size;
	uint8*  p_uint8 = NULL;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = FLV_AUDIO_TAG_HEADER_MIN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_buf;

	p_header->format = ((p_uint8[0] >> 4) & 0xF);
	p_header->sample_rate = ((p_uint8[0] >> 2) & 0x3);
	p_header->sample_bit = ((p_uint8[0] >> 1) & 0x1);
	p_header->chn = (p_uint8[0] & 0x1);

	if( p_header->format == FLV_AUDIO_FORMAT_AAC )
	{
		need_size = FLV_AAC_AUDIO_TAG_HEADER_SIZE;
		if( left_size < need_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_header->aac_pack_type = p_uint8[1];
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}
