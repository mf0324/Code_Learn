
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
#include "rtmp_ctrl_msg.h"


////////////////////////////// Outter Interface ///////////////////////////////
int32 rtmp_ctrl_mux_set_chunk_size(uint32 chunk_size, uint8* p_buf, uint32* p_buf_size)
{
	RTMP_CHUNK_HEADER chunk_header;
	RTMP_CHUNK_PAYLOAD chunk_payload;

	uint32  left_size;
	uint32  need_size;
	uint32  uint32_data;

	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = RTMP_CHUNK_BASIC_HEADER_SIZE_1 + RTMP_CHUNK_MSG_HEADER_SIZE_0 + RTMP_SET_CHUNK_SIZE_MSG_PAYLOAD_SIZE;

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}


	chunk_header.format = RTMP_CHUNK_TYPE_0;
	chunk_header.chunk_stream_id = RTMP_CTRL_CHUNK_STREAM_ID;
	chunk_header.msg_header.udata.ts = 0;
	chunk_header.msg_header.msg_len = RTMP_SET_CHUNK_SIZE_MSG_PAYLOAD_SIZE;
	chunk_header.msg_header.msg_type = RTMP_MSG_TYPE_SET_CHUNK_SIZE;
	chunk_header.msg_header.msg_stream_id = 0x0;
	chunk_header.b_ext_ts = 0;

	uint32_data = hton_u32(chunk_size);
	chunk_payload.p_data = (uint8*)&uint32_data;
	chunk_payload.data_size = RTMP_SET_CHUNK_SIZE_MSG_PAYLOAD_SIZE;

	ret = rtmp_mux_msg(&chunk_header, &chunk_payload, chunk_size, p_buf, p_buf_size);

	return GEN_S_OK;
}

int32 rtmp_ctrl_mux_wnd_ack_size(uint32 wnd_ack_size, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 mux_size;

	RTMP_CHUNK_HEADER  chunk_header;
	RTMP_CHUNK_PAYLOAD chunk_payload;

	uint8*  p_uint8 = NULL;
	uint32  uint32_data;

	int32   ret;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = RTMP_CHUNK_BASIC_HEADER_SIZE_1 + RTMP_CHUNK_MSG_HEADER_SIZE_0 + RTMP_WIN_ACK_MSG_PAYLOAD_SIZE;

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	chunk_header.format = RTMP_CHUNK_TYPE_0;
	chunk_header.chunk_stream_id = RTMP_CTRL_CHUNK_STREAM_ID;
	chunk_header.msg_header.udata.ts = 0;
	chunk_header.msg_header.msg_len = RTMP_WIN_ACK_MSG_PAYLOAD_SIZE;
	chunk_header.msg_header.msg_type = RTMP_MSG_TYPE_WINDOW_ACK_SIZE;
	chunk_header.msg_header.msg_stream_id = 0x0;
	chunk_header.b_ext_ts = 0;

	uint32_data = hton_u32(wnd_ack_size);
	chunk_payload.p_data = (uint8*)&uint32_data;
	chunk_payload.data_size = RTMP_WIN_ACK_MSG_PAYLOAD_SIZE;

	ret = rtmp_mux_msg(&chunk_header, &chunk_payload, RTMP_CHUNK_SIZE, p_buf, p_buf_size);

	return GEN_S_OK;
}

int32 rtmp_ctrl_mux_set_peer_bandwidth(uint32 wnd_ack_size, int32 limit_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 mux_size;

	RTMP_CHUNK_HEADER  chunk_header;
	RTMP_CHUNK_PAYLOAD chunk_payload;

	uint8*  p_uint8 = NULL;

	uint8   data_buf[32];

	int32   ret;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = RTMP_CHUNK_BASIC_HEADER_SIZE_1 + RTMP_CHUNK_MSG_HEADER_SIZE_0 + RTMP_SET_PEER_BANDWIDTH_MSG_PAYLOAD_SIZE;

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	/** payload */
	p_uint8 = (uint8*)&wnd_ack_size;
	data_buf[0] = p_uint8[3];
	data_buf[1] = p_uint8[2];
	data_buf[2] = p_uint8[1];
	data_buf[3] = p_uint8[0];

	data_buf[4] = (limit_type & 0xFF);

	chunk_header.format = RTMP_CHUNK_TYPE_0;
	chunk_header.chunk_stream_id = RTMP_CTRL_CHUNK_STREAM_ID;
	chunk_header.msg_header.udata.ts = 0;
	chunk_header.msg_header.msg_len = RTMP_SET_PEER_BANDWIDTH_MSG_PAYLOAD_SIZE;
	chunk_header.msg_header.msg_type = RTMP_MSG_TYPE_SET_PEER_BANDWIDTH;
	chunk_header.msg_header.msg_stream_id = 0x0;
	chunk_header.b_ext_ts = 0;

	chunk_payload.p_data = data_buf;
	chunk_payload.data_size = RTMP_SET_PEER_BANDWIDTH_MSG_PAYLOAD_SIZE;

	ret = rtmp_mux_msg(&chunk_header, &chunk_payload, RTMP_CHUNK_SIZE, p_buf, p_buf_size);

	return GEN_S_OK;
}

int32 rtmp_ctrl_mux_event(uint32 event_type, uint32 event_data, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 mux_size;

	RTMP_CHUNK_HEADER  chunk_header;
	RTMP_CHUNK_PAYLOAD chunk_payload;

	uint8*  p_uint8 = NULL;

	uint8   data_buf[32];

	int32   ret;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	need_size = RTMP_CHUNK_BASIC_HEADER_SIZE_1 + RTMP_CHUNK_MSG_HEADER_SIZE_0 + RTMP_EVENT_TYPE_SIZE + RTMP_EVENT_PAYLOAD_SIZE;

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	/** event type */
	p_uint8 = (uint8*)&event_type;
	data_buf[0] = p_uint8[1];
	data_buf[1] = p_uint8[0];

	/** event data */
	p_uint8 = (uint8*)&event_data;
	data_buf[2] = p_uint8[3];
	data_buf[3] = p_uint8[2];
	data_buf[4] = p_uint8[1];
	data_buf[5] = p_uint8[0];

	chunk_header.format = RTMP_CHUNK_TYPE_0;
	chunk_header.chunk_stream_id = RTMP_CTRL_CHUNK_STREAM_ID;
	chunk_header.msg_header.udata.ts = 0;
	chunk_header.msg_header.msg_len = (RTMP_EVENT_TYPE_SIZE + RTMP_EVENT_PAYLOAD_SIZE);
	chunk_header.msg_header.msg_type = RTMP_MSG_TYPE_USER_CONTROL;
	chunk_header.msg_header.msg_stream_id = 0x0;
	chunk_header.b_ext_ts = 0;

	chunk_payload.p_data = data_buf;
	chunk_payload.data_size = (RTMP_EVENT_TYPE_SIZE + RTMP_EVENT_PAYLOAD_SIZE);

	ret = rtmp_mux_msg(&chunk_header, &chunk_payload, RTMP_CHUNK_SIZE, p_buf, p_buf_size);

	return ret;
}
