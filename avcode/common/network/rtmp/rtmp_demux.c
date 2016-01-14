
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>


#include "rtmp_const.h"
#include "rtmp_public.h"
#include "rtmp_demux.h"


////////////////////////////// Outter Interface ///////////////////////////////
#if 0
int32 rtmp_demux_s1(RTMP_S1* p_s1, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	if( p_s1 == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_S1_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	//p_s1->data_size = RTMP_S1_SIZE;
	//p_s1->p_data = p_buf;

	total_size += RTMP_S1_SIZE;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 rtmp_demux_s2(RTMP_S2* p_s2, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	if( p_s2 == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_S2_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	//p_s2->data_size = RTMP_S2_SIZE;
	//p_s2->p_data = p_buf;

	total_size += RTMP_S2_SIZE;

	*p_demux_size = total_size;

	return GEN_S_OK;
}
#endif

int32 rtmp_demux_chunk_header(RTMP_CHUNK_HEADER* p_header, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_header == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_CHUNK_BASIC_HEADER_MIN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	/** format */
	p_header->format = ((p_uint8[0] >> 6) & 0x3);

	/** chunk stream id */
	p_header->chunk_stream_id = (p_uint8[0] & 0x3F);

	switch( p_header->chunk_stream_id )
	{
	case 0:
		{
			/** 2 byte */
			if( left_size < RTMP_CHUNK_BASIC_HEADER_SIZE_2 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_header->chunk_stream_id = p_uint8[1] + 64;

			p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_2;
			left_size -= RTMP_CHUNK_BASIC_HEADER_SIZE_2;
			total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_2;
		}
		break;

	case 1:
		{
			/** 3 byte */
			if( left_size < RTMP_CHUNK_BASIC_HEADER_SIZE_3 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_header->chunk_stream_id = ( (((uint32)p_uint8[1]) << 8) | (p_uint8[2]) ) + 64;

			p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_3;
			left_size -= RTMP_CHUNK_BASIC_HEADER_SIZE_3;
			total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_3;
		}
		break;

	default:
		{
			p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_1;
			left_size -= RTMP_CHUNK_BASIC_HEADER_SIZE_1;
			total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_1;
		}
		break;
	}

	/** msg header */
	p_header->b_ext_ts = 0;
	switch( p_header->format )
	{
	case RTMP_CHUNK_TYPE_0:
		{
			/** chunk type 0 */
			if( left_size < RTMP_CHUNK_MSG_HEADER_SIZE_0 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_header->msg_header.udata.ts = (((uint32)p_uint8[0]) << 16) | (((uint32)p_uint8[1]) << 8) | (p_uint8[2]);
			p_header->msg_header.msg_len = (((uint32)p_uint8[3]) << 16) | (((uint32)p_uint8[4]) << 8) | (p_uint8[5]);
			p_header->msg_header.msg_type = p_uint8[6];
			p_header->msg_header.msg_stream_id = (((uint32)p_uint8[10]) << 24) | (((uint32)p_uint8[9]) << 16) | (((uint32)p_uint8[8]) << 8) | (p_uint8[7]);

			if( p_header->msg_header.udata.ts == RTMP_CHUNK_INVALID_TIMESTAMP )
			{
				p_header->b_ext_ts = 1;
			}

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_0;
			left_size -= RTMP_CHUNK_MSG_HEADER_SIZE_0;
			total_size += RTMP_CHUNK_MSG_HEADER_SIZE_0;
		}
		break;

	case RTMP_CHUNK_TYPE_1:
		{
			/** chunk type 1 */
			if( left_size < RTMP_CHUNK_MSG_HEADER_SIZE_1 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_header->msg_header.udata.ts_delta = (((uint32)p_uint8[0]) << 16) | (((uint32)p_uint8[1]) << 8) | (p_uint8[2]);
			p_header->msg_header.msg_len = (((uint32)p_uint8[3]) << 16) | (((uint32)p_uint8[4]) << 8) | (p_uint8[5]);
			p_header->msg_header.msg_type = p_uint8[6];

			if( p_header->msg_header.udata.ts_delta == RTMP_CHUNK_INVALID_TIMESTAMP )
			{
				p_header->b_ext_ts = 1;
			}

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_1;
			left_size -= RTMP_CHUNK_MSG_HEADER_SIZE_1;
			total_size += RTMP_CHUNK_MSG_HEADER_SIZE_1;
		}
		break;

	case RTMP_CHUNK_TYPE_2:
		{
			/** chunk type 2 */
			if( left_size < RTMP_CHUNK_MSG_HEADER_SIZE_2 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_header->msg_header.udata.ts_delta = (((uint32)p_uint8[0]) << 16) | (((uint32)p_uint8[1]) << 8) | (p_uint8[2]);

			if( p_header->msg_header.udata.ts_delta == RTMP_CHUNK_INVALID_TIMESTAMP )
			{
				p_header->b_ext_ts = 1;
			}

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_2;
			left_size -= RTMP_CHUNK_MSG_HEADER_SIZE_2;
			total_size += RTMP_CHUNK_MSG_HEADER_SIZE_2;
		}
		break;

	default:
		{
		}
		break;
	}

	/** ext ts */
	if( p_header->b_ext_ts )
	{
		if( left_size < RTMP_CHUNK_EXT_TS_SIZE )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_header->ext_ts = (((uint32)p_uint8[0]) << 24) | (((uint32)p_uint8[1]) << 16) | (((uint32)p_uint8[2]) << 8) | (p_uint8[3]);

		p_uint8 += RTMP_CHUNK_EXT_TS_SIZE;
		left_size -= RTMP_CHUNK_EXT_TS_SIZE;
		total_size += RTMP_CHUNK_EXT_TS_SIZE;
	}

	*p_demux_size = total_size;

	return GEN_S_OK;
}
