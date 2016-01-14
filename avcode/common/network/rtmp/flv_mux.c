
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent.h>

#include "flv_const.h"
#include "flv_public.h"
#include "flv_mux.h"


////////////////////////////// Outter Interface ///////////////////////////////
int32 flv_mux_header(FLV_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	uint32 uint32_data;

	//int32  ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	left_size = *p_buf_size;

	//ret = rtmp_calc_chunk_header_size(p_header, &need_size);
	need_size = FLV_HEADER_SIZE_1 + FLV_PREV_TAG_SIZE_LEN;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	total_size = 0;
	p_uint8 = p_buf;

	/** signature & version */
	p_uint8[0] = 'F';
	p_uint8[1] = 'L';
	p_uint8[2] = 'V';
	p_uint8[3] = FLV_HEADER_VERSION;

	/** type flag */
	p_uint8[4] = p_header->type_flag;

	/** data offset */
	p_src = (uint8*)&uint32_data;
	uint32_data = FLV_HEADER_SIZE_1;
	p_uint8[5] = p_src[3];
	p_uint8[6] = p_src[2];
	p_uint8[7] = p_src[1];
	p_uint8[8] = p_src[0];

	/** prev tag size 0 */
	uint32_data = FLV_PREV_TAG_SIZE_0;
	//p_src = (uint8*)&uint32_data;
	p_uint8[9] = p_src[3];
	p_uint8[10] = p_src[2];
	p_uint8[11] = p_src[1];
	p_uint8[12] = p_src[0];

	*p_buf_size = need_size;

	return 0;
}

int32 flv_mux_tag_header(FLV_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;
	uint32 uint32_data;

	//int32  ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = FLV_TAG_HEADER_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	total_size = 0;
	p_uint8 = p_buf;

	/** tag type */
	p_uint8[0] = p_header->tag_type;

	/** data size */
	p_src = (uint8*)&uint32_data;
	uint32_data = p_header->data_size;
	p_uint8[1] = p_src[2];
	p_uint8[2] = p_src[1];
	p_uint8[3] = p_src[0];

	/** ts */
	uint32_data = p_header->ts;
	p_uint8[4] = p_src[2];
	p_uint8[5] = p_src[1];
	p_uint8[6] = p_src[0];
	p_uint8[7] = p_src[3];

	/** stream id */
	//p_uint8[8] = 0x00;
	//p_uint8[9] = 0x00;
	//p_uint8[10] = 0x00;

	p_uint8[8] = ((p_header->stream_id >> 16) & 0xFF);
	p_uint8[9] = ((p_header->stream_id >> 8) & 0xFF);
	p_uint8[10] = ((p_header->stream_id) & 0xFF);

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 flv_mux_video_tag_header(FLV_VIDEO_TAG_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;
	uint32 uint32_data;

	//int32  ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = FLV_VIDEO_TAG_HEADER_MIN_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	total_size = 0;
	p_uint8 = p_buf;

	/** frame_type + codec_id, 4 + 4 bit */
	p_uint8[0] = (p_header->frame_type << 4) | (p_header->codec_id);
	total_size += FLV_VIDEO_TAG_HEADER_MIN_SIZE;

	if( p_header->codec_id == FLV_VIDEO_CODEC_ID_AVC )
	{
		need_size = FLV_AVC_VIDEO_TAG_HEADER_SIZE;
		if( left_size < need_size )
		{
			*p_buf_size = need_size;
			return GEN_E_NEED_MORE_BUF;
		}

		/** avc_pack_type */
		p_uint8[1] = p_header->avc_pack_type;

		/** cts_offset */
		uint32_data = p_header->cts_offset;
		p_src = (uint8*)&uint32_data;
		p_uint8[2] = p_src[2];
		p_uint8[3] = p_src[1];
		p_uint8[4] = p_src[0];
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}
