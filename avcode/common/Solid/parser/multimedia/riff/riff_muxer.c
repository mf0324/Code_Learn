
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include "riff_public.h"
#include "riff_muxer.h"

//#include "../../util/log_debug.h"


/////////////////////////////////////外部接口//////////////////////////////////
int32 riff_list_header_mux(RIFF_LIST_HEADER *p_header, uint8 *p_buf, int32 *p_buf_size)
{
	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < RIFF_LIST_HEADER_SIZE )
	{
		*p_buf_size = RIFF_LIST_HEADER_SIZE;
		return -2;
	}

	memcpy(p_buf, p_header, sizeof(RIFF_LIST_HEADER));

	*p_buf_size = RIFF_LIST_HEADER_SIZE;

	return 0;
}

int32 riff_chunk_header_mux(RIFF_CHUNK_HEADER *p_header, uint8 *p_buf, int32 *p_buf_size)
{
	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < RIFF_CHUNK_HEADER_SIZE )
	{
		*p_buf_size = RIFF_CHUNK_HEADER_SIZE;
		return -2;
	}

	memcpy(p_buf, p_header, sizeof(RIFF_CHUNK_HEADER));

	*p_buf_size = RIFF_CHUNK_HEADER_SIZE;

	return 0;
}
