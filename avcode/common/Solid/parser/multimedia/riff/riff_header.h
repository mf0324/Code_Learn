/**
 * @file riff_header.h   RIFF Header
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.28
 *
 *
 */


#ifndef __RIFF_HEADER_H__
#define __RIFF_HEADER_H__

#include <public/gen_int.h>

/** RIFF List */
typedef struct tag_riff_list_header
{
	/** id(ÍøÂç×Ö½ÚÐò) */
	uint32 id;
	/** size */
	uint32 size;
	/** fourcc(ÍøÂç×Ö½ÚÐò) */
	uint32 fourcc;

} RIFF_LIST_HEADER, *PRIFF_LIST_HEADER;

/** RIFF Chunk */
typedef struct tag_riff_chunk_header
{
	/** fourcc(ÍøÂç×Ö½ÚÐò) */
	uint32 fourcc;
	/** size */
	uint32 size;

} RIFF_CHUNK_HEADER, *PRIFF_CHUNK_HEADER;


#endif ///__RIFF_HEADER_H__
