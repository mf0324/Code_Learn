
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "rtmp_const.h"
#include "rtmp_public.h"
#include "rtmp_mux.h"


////////////////////////////// Outter Interface ///////////////////////////////
int32 rtmp_calc_chunk_header_size(RTMP_CHUNK_HEADER* p_header, uint32* p_size)
{
	uint32 need_size;

	if( p_header == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	need_size = RTMP_CHUNK_BASIC_HEADER_MIN_SIZE;

	/** chunk stream id */
	if( p_header->chunk_stream_id >= RTMP_CHUNK_STREAM_ID2_BEGIN )
	{
		if( p_header->chunk_stream_id <= RTMP_CHUNK_STREAM_ID2_END )
		{
			/** 64 - 319 */
			need_size += 1;

		}else
		{
			/** 320 - 65599 */
			need_size += 2;
		}
	}

	/** format */
	switch( p_header->format )
	{
	case RTMP_CHUNK_TYPE_0:
		{
			/** chunk type 0 */
			need_size += RTMP_CHUNK_MSG_HEADER_SIZE_0;
		}
		break;

	case RTMP_CHUNK_TYPE_1:
		{
			/** chunk type 1 */
			need_size += RTMP_CHUNK_MSG_HEADER_SIZE_1;
		}
		break;

	case RTMP_CHUNK_TYPE_2:
		{
			/** chunk type 2 */
			need_size += RTMP_CHUNK_MSG_HEADER_SIZE_2;
		}
		break;

	default:
		{
			/** chunk type 3 */
		}
		break;
	}

	/** ext ts */
	if( p_header->b_ext_ts )
	{
		need_size += RTMP_CHUNK_EXT_TS_SIZE;
	}

	*p_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_mux_chunk_header(RTMP_CHUNK_HEADER* p_header, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;
	uint32 chunk_stream_id;

	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	ret = rtmp_calc_chunk_header_size(p_header, &need_size);
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	total_size = 0;
	p_uint8 = p_buf;

	/** format */
	p_uint8[0] = ((p_header->format & 0x3) << 6);

	/** chunk stream id */
	if( p_header->chunk_stream_id <= RTMP_CHUNK_STREAM_ID1_END )
	{
		/** 1 byte */
		p_uint8[0] |= (p_header->chunk_stream_id & 0x3F);

		p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_1;
		total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_1;

	}else if( p_header->chunk_stream_id <= RTMP_CHUNK_STREAM_ID2_END )
	{
		/** 2 byte */
		p_uint8[0] = (p_uint8[0] & 0xC0);
		p_uint8[1] = ((p_header->chunk_stream_id - 64) & 0xFF);

		p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_2;
		total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_2;

	}else
	{
		/** 3 byte */
		chunk_stream_id = p_header->chunk_stream_id - 64;
		p_uint8[0] = (p_uint8[0] & 0xC1);
		p_uint8[1] = ((chunk_stream_id >> 8) & 0xFF);
		p_uint8[2] = (chunk_stream_id & 0xFF);

		p_uint8 += RTMP_CHUNK_BASIC_HEADER_SIZE_3;
		total_size += RTMP_CHUNK_BASIC_HEADER_SIZE_3;
	}

	/** msg header */
	switch( p_header->format & 0x3 )
	{
	case RTMP_CHUNK_TYPE_0:
		{
			/** chunk type 0 */
			/** timestamp, 3 byte */
			p_uint8[0] = ((p_header->msg_header.udata.ts >> 16) & 0xFF);
			p_uint8[1] = ((p_header->msg_header.udata.ts >> 8) & 0xFF);
			p_uint8[2] = (p_header->msg_header.udata.ts & 0xFF);

			/** msg len, 3 byte */
			p_uint8[3] = ((p_header->msg_header.msg_len >> 16) & 0xFF);
			p_uint8[4] = ((p_header->msg_header.msg_len >> 8) & 0xFF);
			p_uint8[5] = (p_header->msg_header.msg_len & 0xFF);

			/** msg type, 1 byte */
			p_uint8[6] = (p_header->msg_header.msg_type & 0xFF);

			/** msg stream id, 4 byte */
			p_uint8[10] = ((p_header->msg_header.msg_stream_id >> 24) & 0xFF);
			p_uint8[9] = ((p_header->msg_header.msg_stream_id >> 16) & 0xFF);
			p_uint8[8] = ((p_header->msg_header.msg_stream_id >> 8) & 0xFF);
			p_uint8[7] = (p_header->msg_header.msg_stream_id & 0xFF);

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_0;
			total_size += RTMP_CHUNK_MSG_HEADER_SIZE_0;
		}
		break;

	case RTMP_CHUNK_TYPE_1:
		{
			/** chunk type 1 */
			/** timestamp delta, 3 byte */
			p_uint8[0] = ((p_header->msg_header.udata.ts_delta >> 16) & 0xFF);
			p_uint8[1] = ((p_header->msg_header.udata.ts_delta >> 8) & 0xFF);
			p_uint8[2] = (p_header->msg_header.udata.ts_delta & 0xFF);

			/** msg len, 3 byte */
			p_uint8[3] = ((p_header->msg_header.msg_len >> 16) & 0xFF);
			p_uint8[4] = ((p_header->msg_header.msg_len >> 8) & 0xFF);
			p_uint8[5] = (p_header->msg_header.msg_len & 0xFF);

			/** msg type, 1 byte */
			p_uint8[6] = (p_header->msg_header.msg_type & 0xFF);

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_1;
			total_size += RTMP_CHUNK_MSG_HEADER_SIZE_1;
		}
		break;

	case RTMP_CHUNK_TYPE_2:
		{
			/** chunk type 2 */
			/** timestamp delta, 3 byte */
			p_uint8[0] = ((p_header->msg_header.udata.ts_delta >> 16) & 0xFF);
			p_uint8[1] = ((p_header->msg_header.udata.ts_delta >> 8) & 0xFF);
			p_uint8[2] = (p_header->msg_header.udata.ts_delta & 0xFF);

			p_uint8 += RTMP_CHUNK_MSG_HEADER_SIZE_2;
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
		p_uint8[0] = ((p_header->ext_ts >> 24) & 0xFF);
		p_uint8[1] = ((p_header->ext_ts >> 16) & 0xFF);
		p_uint8[2] = ((p_header->ext_ts >> 8) & 0xFF);
		p_uint8[3] = (p_header->ext_ts & 0xFF);

		p_uint8 += RTMP_CHUNK_EXT_TS_SIZE;
		total_size += RTMP_CHUNK_EXT_TS_SIZE;
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_calc_msg_size(RTMP_CHUNK_HEADER* p_header, RTMP_CHUNK_PAYLOAD* p_payload, uint32 chunk_size, uint32* p_size)
{
	uint32 need_size;
	uint32 chunk_stream_id_size;
	uint32 first_header_size;
	uint32 left_header_size;
	uint32 chunk_count;
	//uint32 last_payload_size;
	uint32 remain_size;

	if( p_header == NULL || p_payload == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( chunk_size == 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	chunk_stream_id_size = RTMP_CHUNK_BASIC_HEADER_MIN_SIZE;

	/** chunk stream id */
	if( p_header->chunk_stream_id >= RTMP_CHUNK_STREAM_ID2_BEGIN )
	{
		if( p_header->chunk_stream_id <= RTMP_CHUNK_STREAM_ID2_END )
		{
			/** 64 - 319 */
			chunk_stream_id_size += 1;

		}else
		{
			/** 320 - 65599 */
			chunk_stream_id_size += 2;
		}
	}

	first_header_size = chunk_stream_id_size;
	left_header_size = chunk_stream_id_size;

	/** format */
	switch( p_header->format )
	{
	case RTMP_CHUNK_TYPE_0:
		{
			/** chunk type 0 */
			first_header_size += RTMP_CHUNK_MSG_HEADER_SIZE_0;
		}
		break;

	case RTMP_CHUNK_TYPE_1:
		{
			/** chunk type 1 */
			first_header_size += RTMP_CHUNK_MSG_HEADER_SIZE_1;
		}
		break;

	case RTMP_CHUNK_TYPE_2:
		{
			/** chunk type 2 */
			first_header_size += RTMP_CHUNK_MSG_HEADER_SIZE_2;
		}
		break;

	default:
		{
			/** chunk type 3 */
		}
		break;
	}

	/** ext ts */
	if( p_header->b_ext_ts )
	{
		first_header_size += RTMP_CHUNK_EXT_TS_SIZE;
	}

	/** chunk size */
	if( p_payload->data_size <= chunk_size )
	{
		/** single chunk */
		need_size = first_header_size + p_payload->data_size;

	}else
	{
		/** multiple chunk */
		chunk_count = p_payload->data_size / chunk_size;
		remain_size = p_payload->data_size % chunk_size;
		if( remain_size )
		{
			//chunk_count++;
			//last_payload_size = remain_size;
			need_size = (first_header_size + chunk_size) + (left_header_size + chunk_size) * (chunk_count - 1) + (left_header_size + remain_size);

		}else
		{
			//last_payload_size = 0;
			need_size = (first_header_size + chunk_size) + (left_header_size + chunk_size) * (chunk_count - 1);
		}

	}

	*p_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_mux_msg(RTMP_CHUNK_HEADER* p_header, RTMP_CHUNK_PAYLOAD* p_payload, uint32 chunk_size, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 mux_size;
	uint32 total_size;
	//uint32 total_payload_size;

	RTMP_CHUNK_HEADER first_chunk_header;
	RTMP_CHUNK_HEADER left_chunk_header;
	uint32 chunk_count;
	uint32 remain_size;

	uint8* p_uint8 = NULL;
	uint8* p_payload_data = NULL;

	uint32 i;
	int32  ret;

	if( p_header == NULL || p_payload == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_payload->p_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	ret = rtmp_calc_msg_size(p_header, p_payload, chunk_size, &need_size);
	if( ret )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	total_size = 0;
	p_uint8 = p_buf;
	p_payload_data = p_payload->p_data;
	//total_payload_size = 0;

	memcpy(&first_chunk_header, p_header, sizeof(first_chunk_header));
	memset(&left_chunk_header, 0, sizeof(left_chunk_header));
	left_chunk_header.format = RTMP_CHUNK_TYPE_3;
	left_chunk_header.chunk_stream_id = p_header->chunk_stream_id;

	if( p_payload->data_size <= chunk_size )
	{
		/** single chunk */
		mux_size = left_size;
		ret = rtmp_mux_chunk_header(&first_chunk_header, p_uint8, &mux_size);
		p_uint8 += mux_size;
		left_size -= mux_size;
		total_size += mux_size;

		memcpy(p_uint8, p_payload->p_data, p_payload->data_size);

	}else
	{
		/** multiple chunk */
		chunk_count = p_payload->data_size / chunk_size;
		remain_size = p_payload->data_size % chunk_size;
		if( remain_size )
		{
			/** first chunk */
			mux_size = left_size;
			ret = rtmp_mux_chunk_header(&first_chunk_header, p_uint8, &mux_size);
			p_uint8 += mux_size;
			left_size -= mux_size;
			total_size += mux_size;

			memcpy(p_uint8, p_payload_data, chunk_size);
			p_uint8 += chunk_size;
			left_size -= chunk_size;
			total_size += chunk_size;
			p_payload_data += chunk_size;

			/** middle chunk */
			for( i = 1; i < chunk_count; i++ )
			{
				mux_size = left_size;
				ret = rtmp_mux_chunk_header(&left_chunk_header, p_uint8, &mux_size);
				p_uint8 += mux_size;
				left_size -= mux_size;
				total_size += mux_size;

				memcpy(p_uint8, p_payload_data, chunk_size);
				p_uint8 += chunk_size;
				left_size -= chunk_size;
				total_size += chunk_size;
				p_payload_data += chunk_size;
			}

			/** last chunk */
			mux_size = left_size;
			ret = rtmp_mux_chunk_header(&left_chunk_header, p_uint8, &mux_size);
			p_uint8 += mux_size;
			left_size -= mux_size;
			total_size += mux_size;

			memcpy(p_uint8, p_payload_data, remain_size);
			p_uint8 += remain_size;
			left_size -= remain_size;
			total_size += remain_size;
			p_payload_data += remain_size;

		}else
		{
			/** first chunk */
			mux_size = left_size;
			ret = rtmp_mux_chunk_header(&first_chunk_header, p_uint8, &mux_size);
			p_uint8 += mux_size;
			left_size -= mux_size;
			total_size += mux_size;

			memcpy(p_uint8, p_payload_data, chunk_size);
			p_uint8 += chunk_size;
			left_size -= chunk_size;
			total_size += chunk_size;
			p_payload_data += chunk_size;

			/** middle chunk */
			for( i = 1; i < chunk_count; i++ )
			{
				mux_size = left_size;
				ret = rtmp_mux_chunk_header(&left_chunk_header, p_uint8, &mux_size);
				p_uint8 += mux_size;
				left_size -= mux_size;
				total_size += mux_size;

				memcpy(p_uint8, p_payload_data, chunk_size);
				p_uint8 += chunk_size;
				left_size -= chunk_size;
				total_size += chunk_size;
				p_payload_data += chunk_size;
			}
		}

	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}
