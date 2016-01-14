/**
 * @file ps_stream_public.h  PS Stream Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.16
 *
 *
 */

#ifndef __PS_STREAM_PUBLIC_H__
#define __PS_STREAM_PUBLIC_H__

#include <public/gen_int.h>
//#include "mpeg2_public.h"

/** max es stream num */
#define PS_STREAM_MAX_ES_STREAM_NUM                        (16)


/** es stream */
typedef struct tag_ps_es_stream
{
	/** stream type */
	int32  type;
	/** stream id */
	int32  id;

} PS_ES_STREAM, *PPS_ES_STREAM;


/** stream */
typedef struct tag_ps_stream
{
	/** get ps mag */
	int32  b_get_ps_map;
	/** es stream count */
	int32  es_stream_count;
	/** es stream */
	PS_ES_STREAM es_stream[PS_STREAM_MAX_ES_STREAM_NUM];

} PS_STREAM, *PPS_STREAM;


#endif /// __PS_STREAM_PUBLIC_H__
