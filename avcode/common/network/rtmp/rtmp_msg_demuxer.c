
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>
#include <util/log_agent_public.h>
#include <util/log_agent.h>

#include "rtmp_const.h"
#include "rtmp_demux.h"
#include "rtmp_msg_demuxer.h"

/** max payload buf size */
#define RTMP_MSG_DEMUXER_MAX_PAYLOAD_BUF_SIZE              (512 * 1024)

/** print log */
#define RTMP_MSG_DEMUXER_PRINT_LOG                         (0)


/** RTMP Chunk Stream */
typedef struct tag_rtmp_chunk_stream
{
	/** valid */
	int32   b_valid;

	/** chunk stream id */
	uint32  chunk_stream_id;

	/** fore valid */
	int32   b_fore_valid;
	/** fore format */
	uint32  fore_format;
	/** origin timestamp */
	uint32  origin_ts;
	/** fore timestamp */
	uint32  fore_ts;
	/** fore timestamp delta */
	uint32  fore_ts_delta;
	/** fore msg len */
	uint32  fore_msg_len;
	/** fore msg type */
	uint32  fore_msg_type;
	/** fore msg stream id */
	uint32  fore_msg_stream_id;

	/** more payload */
	int32   b_more_payload;
	/** total payload size */
	uint32  total_payload_size;
	/** need payload size */
	uint32  need_payload_size;

	/** payload buf */
	uint8*  p_payload_buf;
	/** payload buf size */
	uint32  payload_buf_size;
	/** payload buf use size */
	uint32  payload_buf_use_size;
	/** payload buf left size */
	uint32  payload_buf_left_size;

} RTMP_CHUNK_STREAM, *PRTMP_CHUNK_STREAM;


/** RTMP Msg Demuxer */
typedef struct tag_rtmp_msg_demuxer
{
	/** init */
	int32   b_init;

	/** chunk size */
	uint32  chunk_size;

	/** update chunk */
	int32   b_update_chunk;
	/** update chunk index */
	int32   update_chunk_index;

	/** chunk stream count */
	uint32  chunk_stream_count;
	/** chunk stream */
	RTMP_CHUNK_STREAM* chunk_stream[RTMP_MAX_CHUNK_STREAM];

} RTMP_MSG_DEMUXER, *PRTMP_MSG_DEMUXER;


/**  
 * @brief init all chunk stream
 * @param [in] p_demuxer, demuxer
 * @return
 * 0: success
 * other: fail
 */
static int32 rtmp_msg_demuxer_init_all_chunk_stream(RTMP_MSG_DEMUXER* p_demuxer)
{
	//RTMP_CHUNK_STREAM** p_chunk_stream = NULL;
	int32  i;

	for( i = 0; i < RTMP_MAX_CHUNK_STREAM; i++ )
	{
		//p_chunk_stream = p_demuxer->chunk_stream[i];
		p_demuxer->chunk_stream[i] = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit all chunk stream
 * @param [in] p_demuxer, demuxer
 * @return
 * 0: success
 * other: fail
 */
static int32 rtmp_msg_demuxer_deinit_all_chunk_stream(RTMP_MSG_DEMUXER* p_demuxer)
{
	//RTMP_CHUNK_STREAM* p_chunk_stream = NULL;
	int32  i;

	for( i = 0; i < RTMP_MAX_CHUNK_STREAM; i++ )
	{
		if( p_demuxer->chunk_stream[i] )
		{
			if( p_demuxer->chunk_stream[i]->p_payload_buf )
			{
				free(p_demuxer->chunk_stream[i]->p_payload_buf);
				p_demuxer->chunk_stream[i]->p_payload_buf = NULL;
			}

			free(p_demuxer->chunk_stream[i]);
			p_demuxer->chunk_stream[i] = NULL;
		}
	}

	return GEN_S_OK;
}

/**  
 * @brief create chunk stream
 * @param [in] p_demuxer, demuxer
 * @param [out] pp_chunk_stream, chunk stream
 * @return
 * 0: success
 * other: fail
 */
static int32 rtmp_msg_demuxer_create_chunk_stream(RTMP_MSG_DEMUXER* p_demuxer, RTMP_CHUNK_STREAM** pp_chunk_stream)
{
	RTMP_CHUNK_STREAM* p_chunk_stream;

	*pp_chunk_stream = NULL;

	p_chunk_stream = (RTMP_CHUNK_STREAM*)malloc( sizeof(RTMP_CHUNK_STREAM) );
	if( p_chunk_stream == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	memset(p_chunk_stream, 0, sizeof(RTMP_CHUNK_STREAM));

	p_chunk_stream->p_payload_buf = (uint8*)malloc( RTMP_MSG_DEMUXER_MAX_PAYLOAD_BUF_SIZE );
	if( p_chunk_stream->p_payload_buf == NULL )
	{
		free(p_chunk_stream);
		p_chunk_stream = NULL;

		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_chunk_stream->payload_buf_size = RTMP_MSG_DEMUXER_MAX_PAYLOAD_BUF_SIZE;
	p_chunk_stream->payload_buf_use_size = 0;
	p_chunk_stream->payload_buf_left_size = p_chunk_stream->payload_buf_size;

	*pp_chunk_stream = p_chunk_stream;

	return GEN_S_OK;
}

/**  
 * @brief init chunk stream
 * @param [in] p_demuxer, demuxer
 * @param [out] pp_chunk_stream, chunk stream
 * @return
 * 0: success
 * other: fail
 */
static int32 rtmp_msg_demuxer_init_chunk_stream(RTMP_MSG_DEMUXER* p_demuxer, RTMP_CHUNK_STREAM* p_chunk_stream)
{
	p_chunk_stream->b_valid = 0;
	p_chunk_stream->b_fore_valid = 0;
	p_chunk_stream->b_more_payload = 0;
	p_chunk_stream->payload_buf_use_size = 0;
	p_chunk_stream->payload_buf_left_size = p_chunk_stream->payload_buf_size;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
RTMP_MSG_DEMUXER_H rtmp_msg_demuxer_create()
{
	RTMP_MSG_DEMUXER *p_demuxer = NULL;
	p_demuxer = (RTMP_MSG_DEMUXER*)malloc( sizeof(RTMP_MSG_DEMUXER) );

	if( p_demuxer == NULL )
	{
		return NULL;
	}

	memset(p_demuxer, 0, sizeof(RTMP_MSG_DEMUXER));

	return p_demuxer;
}


int32 rtmp_msg_demuxer_destroy(RTMP_MSG_DEMUXER_H h)
{
	RTMP_MSG_DEMUXER *p_demuxer = (RTMP_MSG_DEMUXER*)h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		rtmp_msg_demuxer_deinit(p_demuxer);
	}

	free(p_demuxer);

	return GEN_S_OK;
}


int32 rtmp_msg_demuxer_init(RTMP_MSG_DEMUXER_H h)
{
	RTMP_MSG_DEMUXER *p_demuxer = (RTMP_MSG_DEMUXER*)h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		return GEN_S_OK;
	}

	p_demuxer->chunk_size = RTMP_CHUNK_SIZE;

	p_demuxer->b_update_chunk = 0;

	p_demuxer->chunk_stream_count = 0;
	rtmp_msg_demuxer_init_all_chunk_stream(p_demuxer);

	p_demuxer->b_init = 1;

	return GEN_S_OK;
}


int32 rtmp_msg_demuxer_deinit(RTMP_MSG_DEMUXER_H h)
{
	RTMP_MSG_DEMUXER *p_demuxer = (RTMP_MSG_DEMUXER*)h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	rtmp_msg_demuxer_deinit_all_chunk_stream(p_demuxer);

	p_demuxer->b_init = 0;

	return GEN_S_OK;
}


int32 rtmp_msg_demuxer_reset(RTMP_MSG_DEMUXER_H h)
{
	RTMP_MSG_DEMUXER *p_demuxer = (RTMP_MSG_DEMUXER*)h;

	int32  i;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	for( i = 0; i < RTMP_MAX_CHUNK_STREAM; i++ )
	{
		if( p_demuxer->chunk_stream[i] )
		{
			rtmp_msg_demuxer_init_chunk_stream(p_demuxer, p_demuxer->chunk_stream[i]);
		}
	}

	p_demuxer->b_update_chunk = 0;

	return GEN_S_OK;
}

int32 rtmp_msg_demuxer_set_chunk_size(RTMP_MSG_DEMUXER_H h, uint32 chunk_size)
{
	RTMP_MSG_DEMUXER *p_demuxer = (RTMP_MSG_DEMUXER*)h;

	int32  ret;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	//ret = rtmp_msg_demuxer_reset(rtmp_msg_demuxer);

	p_demuxer->chunk_size = chunk_size;

	return GEN_S_OK;
}

int32 rtmp_parse_msg(RTMP_MSG_DEMUXER_H h, RTMP_MSG* p_msg, uint8* p_data, uint32 data_size, uint32* p_parse_size)
{
	RTMP_MSG_DEMUXER*  p_demuxer = (RTMP_MSG_DEMUXER*)h;
	RTMP_CHUNK_STREAM* p_chunk_stream = NULL;

	RTMP_CHUNK_HEADER chunk_header;
	uint32  chunk_header_size;

	int32   b_get_msg;

	uint32  left_size;
	uint32  need_size;
	uint32  unparse_size;
	uint32  demux_size;
	uint32  total_demux_size;
	uint32  copy_size;

	uint8*  p_uint8 = NULL;


	int32   ret;

	if( p_demuxer == NULL || p_msg == NULL || p_data == NULL || p_parse_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_parse_size = 0;

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_demuxer->b_update_chunk )
	{
		p_demuxer->b_update_chunk = 0;

		p_chunk_stream = p_demuxer->chunk_stream[p_demuxer->update_chunk_index];
		if( p_chunk_stream )
		{
			p_chunk_stream->payload_buf_use_size = 0;
			p_chunk_stream->payload_buf_left_size = p_chunk_stream->payload_buf_size;
		}
	}

	b_get_msg = 0;

	p_uint8 = p_data;
	left_size = data_size;
	unparse_size = data_size;
	total_demux_size = 0;

	while( !b_get_msg && unparse_size > 0 )
	{
		/** chunk header */
		ret = rtmp_demux_chunk_header(&chunk_header, p_uint8, unparse_size, &demux_size);

		p_uint8 += demux_size;
		left_size -= demux_size;
		unparse_size -= demux_size;
		total_demux_size += demux_size;

		chunk_header_size = demux_size;

		if( ret )
		{
			break;
		}

		p_chunk_stream = p_demuxer->chunk_stream[chunk_header.chunk_stream_id];
		if( p_chunk_stream == NULL )
		{
			ret = rtmp_msg_demuxer_create_chunk_stream(p_demuxer, &p_chunk_stream);
			if( ret )
			{
				*p_parse_size = total_demux_size;
				return ret;
			}

			rtmp_msg_demuxer_init_chunk_stream(p_demuxer, p_chunk_stream);
			p_chunk_stream->chunk_stream_id = chunk_header.chunk_stream_id;
			p_demuxer->chunk_stream[chunk_header.chunk_stream_id] = p_chunk_stream;
		}

		switch( chunk_header.format )
		{
		case RTMP_CHUNK_TYPE_0:
			{
				/** type 0 */
				if( chunk_header.msg_header.msg_len <= p_demuxer->chunk_size )
				{
					need_size = chunk_header.msg_header.msg_len;

				}else
				{
					need_size = p_demuxer->chunk_size;
				}

				if( left_size < need_size )
				{
					total_demux_size -= chunk_header_size;
					*p_parse_size = total_demux_size;
					return GEN_E_NEED_MORE_DATA;
				}

#if RTMP_MSG_DEMUXER_PRINT_LOG
				//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 0 chunk stream id = %d, ts = %u, msg len = %d, msg type = %d, stream id = %d\n"),
				//	chunk_header.chunk_stream_id, chunk_header.msg_header.udata.ts, chunk_header.msg_header.msg_len, chunk_header.msg_header.msg_type, chunk_header.msg_header.msg_stream_id);
#endif

				if( p_chunk_stream->payload_buf_use_size > 0 )
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 0 msg overlap!\n"));
					p_chunk_stream->payload_buf_use_size = 0;
					p_chunk_stream->payload_buf_left_size = p_chunk_stream->payload_buf_size;
				}

				if( need_size <= p_chunk_stream->payload_buf_left_size )
				{
					copy_size = need_size;

				}else
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 0 msg too large!\n"));
					copy_size = p_chunk_stream->payload_buf_left_size;
				}

				if( copy_size > 0 )
				{
					memcpy(p_chunk_stream->p_payload_buf + p_chunk_stream->payload_buf_use_size, p_uint8, copy_size);
					p_chunk_stream->payload_buf_use_size += copy_size;
					p_chunk_stream->payload_buf_left_size -= copy_size;
				}

				p_uint8 += copy_size;
				left_size -= copy_size;
				unparse_size -= copy_size;
				total_demux_size += copy_size;

				if( chunk_header.msg_header.msg_len <= p_demuxer->chunk_size )
				{
					/** no more payload */
					p_chunk_stream->b_more_payload = 0;
					b_get_msg = 1;

				}else
				{
					/** more payload */
					p_chunk_stream->b_more_payload = 1;
					p_chunk_stream->total_payload_size = chunk_header.msg_header.msg_len;
					p_chunk_stream->need_payload_size = chunk_header.msg_header.msg_len - p_demuxer->chunk_size;
				}

				p_chunk_stream->b_fore_valid = 1;
				p_chunk_stream->fore_format = RTMP_CHUNK_TYPE_0;

				if( !chunk_header.b_ext_ts )
				{
					p_chunk_stream->origin_ts = chunk_header.msg_header.udata.ts;
					p_chunk_stream->fore_ts = chunk_header.msg_header.udata.ts;
					p_chunk_stream->fore_ts_delta = 0;

				}else
				{
					p_chunk_stream->origin_ts = chunk_header.ext_ts;
					p_chunk_stream->fore_ts = chunk_header.ext_ts;
					p_chunk_stream->fore_ts_delta = 0;
				}

				p_chunk_stream->fore_msg_len = chunk_header.msg_header.msg_len;
				p_chunk_stream->fore_msg_type = chunk_header.msg_header.msg_type;
				p_chunk_stream->fore_msg_stream_id = chunk_header.msg_header.msg_stream_id;
			}
			break;

		case RTMP_CHUNK_TYPE_1:
			{
				/** type 1 */
				if( chunk_header.msg_header.msg_len <= p_demuxer->chunk_size )
				{
					need_size = chunk_header.msg_header.msg_len;

				}else
				{
					need_size = p_demuxer->chunk_size;
				}

				if( left_size < need_size )
				{
					total_demux_size -= chunk_header_size;
					*p_parse_size = total_demux_size;
					return GEN_E_NEED_MORE_DATA;
				}

#if RTMP_MSG_DEMUXER_PRINT_LOG
				//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 1 chunk stream id = %d, ts = %u, msg len = %d, msg type = %d\n"),
				//	chunk_header.chunk_stream_id, chunk_header.msg_header.udata.ts, chunk_header.msg_header.msg_len, chunk_header.msg_header.msg_type);
#endif

				if( p_chunk_stream->payload_buf_use_size > 0 )
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 1 msg overlap!\n"));
					p_chunk_stream->payload_buf_use_size = 0;
					p_chunk_stream->payload_buf_left_size = p_chunk_stream->payload_buf_size;
				}

				if( need_size <= p_chunk_stream->payload_buf_left_size )
				{
					copy_size = need_size;

				}else
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 1 msg too large!\n"));
					copy_size = p_chunk_stream->payload_buf_left_size;
				}

				if( copy_size > 0 )
				{
					memcpy(p_chunk_stream->p_payload_buf + p_chunk_stream->payload_buf_use_size, p_uint8, copy_size);
					p_chunk_stream->payload_buf_use_size += copy_size;
					p_chunk_stream->payload_buf_left_size -= copy_size;
				}

				p_uint8 += copy_size;
				left_size -= copy_size;
				unparse_size -= copy_size;
				total_demux_size += copy_size;

				if( chunk_header.msg_header.msg_len <= p_demuxer->chunk_size )
				{
					/** no more payload */
					p_chunk_stream->b_more_payload = 0;
					b_get_msg = 1;

				}else
				{
					/** more payload */
					p_chunk_stream->b_more_payload = 1;
					p_chunk_stream->total_payload_size = chunk_header.msg_header.msg_len;
					p_chunk_stream->need_payload_size = chunk_header.msg_header.msg_len - p_demuxer->chunk_size;
				}

				p_chunk_stream->fore_format = RTMP_CHUNK_TYPE_1;
				if( p_chunk_stream->b_fore_valid )
				{
					if( !chunk_header.b_ext_ts )
					{
						p_chunk_stream->fore_ts += chunk_header.msg_header.udata.ts_delta;
						p_chunk_stream->fore_ts_delta = chunk_header.msg_header.udata.ts_delta;

					}else
					{
						p_chunk_stream->fore_ts += chunk_header.ext_ts;
						p_chunk_stream->fore_ts_delta = chunk_header.ext_ts;
					}

				}else
				{
					//log_warn(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 1 fore chunk invalid!!!\n"));
				}
				p_chunk_stream->fore_msg_len = chunk_header.msg_header.msg_len;
				p_chunk_stream->fore_msg_type = chunk_header.msg_header.msg_type;
				//p_chunk_stream->b_fore_valid = 1;
			}
			break;

		case RTMP_CHUNK_TYPE_2:
			{
				/** type 2 */
				if( p_chunk_stream->b_more_payload )
				{
					if( p_chunk_stream->need_payload_size <= p_demuxer->chunk_size )
					{
						need_size = p_chunk_stream->need_payload_size;

					}else
					{
						need_size = p_demuxer->chunk_size;
					}

				}else if( p_chunk_stream->b_fore_valid )
				{
					if( p_chunk_stream->fore_msg_len <= p_demuxer->chunk_size )
					{
						need_size = p_chunk_stream->fore_msg_len;

					}else
					{
						need_size = p_demuxer->chunk_size;
					}

				}else
				{
					need_size = p_demuxer->chunk_size;
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 2 msg has no fore header!\n"));
				}

				if( left_size < need_size )
				{
					total_demux_size -= chunk_header_size;
					*p_parse_size = total_demux_size;
					return GEN_E_NEED_MORE_DATA;
				}

#if RTMP_MSG_DEMUXER_PRINT_LOG
				//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 2 chunk stream id = %d, ts = %u, payload size = %d\n"), chunk_header.chunk_stream_id, chunk_header.msg_header.udata.ts_delta, need_size);
#endif

				//if( p_demuxer->payload_buf_use_size < 1 )
				//{
				//	log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 2 msg may be corrupt!\n"));
				//}

				if( need_size <= p_chunk_stream->payload_buf_left_size )
				{
					copy_size = need_size;

				}else
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 2 msg too large!\n"));
					copy_size = p_chunk_stream->payload_buf_left_size;
				}

				if( copy_size > 0 )
				{
					memcpy(p_chunk_stream->p_payload_buf + p_chunk_stream->payload_buf_use_size, p_uint8, copy_size);
					p_chunk_stream->payload_buf_use_size += copy_size;
					p_chunk_stream->payload_buf_left_size -= copy_size;
				}

				p_uint8 += copy_size;
				left_size -= copy_size;
				unparse_size -= copy_size;
				total_demux_size += copy_size;

				if( p_chunk_stream->b_more_payload )
				{
					p_chunk_stream->need_payload_size -= need_size;

					if( p_chunk_stream->need_payload_size < 1 )
					{
						p_chunk_stream->b_more_payload = 0;
						b_get_msg = 1;
					}

				}else if( p_chunk_stream->b_fore_valid )
				{
					if( p_chunk_stream->fore_msg_len <= p_demuxer->chunk_size )
					{
						p_chunk_stream->b_more_payload = 0;
						b_get_msg = 1;

					}else
					{
						p_chunk_stream->b_more_payload = 1;
						p_chunk_stream->total_payload_size = p_chunk_stream->fore_msg_len;
						p_chunk_stream->need_payload_size = p_chunk_stream->fore_msg_len - p_demuxer->chunk_size;
					}

				}else
				{
					p_chunk_stream->b_more_payload = 0;
					b_get_msg = 1;
				}

				//p_chunk_stream->b_fore_valid = 1;
				p_chunk_stream->fore_format = RTMP_CHUNK_TYPE_2;
				if( p_chunk_stream->b_fore_valid )
				{
					if( !chunk_header.b_ext_ts )
					{
						p_chunk_stream->fore_ts += chunk_header.msg_header.udata.ts_delta;
						p_chunk_stream->fore_ts_delta = chunk_header.msg_header.udata.ts_delta;

					}else
					{
						p_chunk_stream->fore_ts += chunk_header.ext_ts;
						p_chunk_stream->fore_ts_delta = chunk_header.ext_ts;
					}

				}else
				{
					//log_warn(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 2 fore chunk invalid!!!\n"));
				}
			}
			break;

		case RTMP_CHUNK_TYPE_3:
			{
				/** type 3 */
				if( p_chunk_stream->b_more_payload )
				{
					if( p_chunk_stream->need_payload_size <= p_demuxer->chunk_size )
					{
						need_size = p_chunk_stream->need_payload_size;

					}else
					{
						need_size = p_demuxer->chunk_size;
					}

				}else if( p_chunk_stream->b_fore_valid )
				{
					if( p_chunk_stream->fore_msg_len <= p_demuxer->chunk_size )
					{
						need_size = p_chunk_stream->fore_msg_len;

					}else
					{
						need_size = p_demuxer->chunk_size;
					}

				}else
				{
					need_size = p_demuxer->chunk_size;
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 3 msg has no fore header!\n"));
				}

				if( left_size < need_size )
				{
					total_demux_size -= chunk_header_size;
					*p_parse_size = total_demux_size;
					return GEN_E_NEED_MORE_DATA;
				}

#if RTMP_MSG_DEMUXER_PRINT_LOG
				//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] format 3 chunk stream id = %d, payload size = %d\n"), chunk_header.chunk_stream_id, need_size);
#endif

				if( need_size <= p_chunk_stream->payload_buf_left_size )
				{
					copy_size = need_size;

				}else
				{
					//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] 3 msg too large!\n"));
					copy_size = p_chunk_stream->payload_buf_left_size;
				}

				if( copy_size > 0 )
				{
					memcpy(p_chunk_stream->p_payload_buf + p_chunk_stream->payload_buf_use_size, p_uint8, copy_size);
					p_chunk_stream->payload_buf_use_size += copy_size;
					p_chunk_stream->payload_buf_left_size -= copy_size;
				}

				p_uint8 += copy_size;
				left_size -= copy_size;
				unparse_size -= copy_size;
				total_demux_size += copy_size;

				if( p_chunk_stream->b_more_payload )
				{
					p_chunk_stream->need_payload_size -= need_size;

					if( p_chunk_stream->need_payload_size < 1 )
					{
						p_chunk_stream->b_more_payload = 0;
						b_get_msg = 1;
					}

				}else if( p_chunk_stream->b_fore_valid )
				{
					if( p_chunk_stream->fore_msg_len <= p_demuxer->chunk_size )
					{
						p_chunk_stream->b_more_payload = 0;
						b_get_msg = 1;

					}else
					{
						p_chunk_stream->b_more_payload = 1;
						p_chunk_stream->total_payload_size = p_chunk_stream->fore_msg_len;
						p_chunk_stream->need_payload_size = p_chunk_stream->fore_msg_len - p_demuxer->chunk_size;

						/** update ts */
						p_chunk_stream->fore_ts += p_chunk_stream->fore_ts_delta;
					}

				}else
				{
					p_chunk_stream->b_more_payload = 0;
					b_get_msg = 1;
				}
			}
			break;

		default:
			{
				//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] unknown format = %d\n"), chunk_header.format);
			}
			break;

		}//end switch

	}//end while

	*p_parse_size = total_demux_size;

	if( !b_get_msg )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_msg->header_format = chunk_header.format;
	p_msg->chunk_stream_id = chunk_header.chunk_stream_id;
	p_msg->msg_type = p_chunk_stream->fore_msg_type;
	p_msg->msg_stream_id = p_chunk_stream->fore_msg_stream_id;
	p_msg->origin_ts = p_chunk_stream->origin_ts;
	p_msg->ts = p_chunk_stream->fore_ts;

	p_msg->p_header = p_data;
	p_msg->header_size = chunk_header_size;
	p_msg->p_payload = p_chunk_stream->p_payload_buf;
	p_msg->payload_size = p_chunk_stream->payload_buf_use_size;

#if RTMP_MSG_DEMUXER_PRINT_LOG
	//log_debug(_T("[rtmp_msg_demuxer::rtmp_parse_msg] chunk stream id = %d, msg type = %d, origin ts = %u, now ts = %u\n"), p_msg->chunk_stream_id, p_msg->msg_type, p_msg->origin_ts, p_msg->ts);
#endif

	p_demuxer->b_update_chunk = 1;
	p_demuxer->update_chunk_index = chunk_header.chunk_stream_id;

	return GEN_S_OK;
}
