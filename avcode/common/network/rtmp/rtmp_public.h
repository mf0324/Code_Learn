/**
 * @file rtmp_public.h   RTMP Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.20
 *
 *
 */

#ifndef __RTMP_PUBLIC_H__
#define __RTMP_PUBLIC_H__

#include <public/gen_int.h>
#include "rtmp_const.h"


/** c1s1 key */
typedef struct tag_rtmp_c1s1_key
{
	/** offset */
	int32   offset;

	/** key */
	uint8   key[RTMP_C1_KEY_SIZE];

	/** random0 */
	uint8*  p_random0;
	/** random0 size */
	int32   random0_size;

	/** random1 */
	uint8*  p_random1;
	/** random1 size */
	int32   random1_size;

} RTMP_C1S1_KEY, *PRTMP_C1S1_KEY;

/** c1s1 digest */
typedef struct tag_rtmp_c1s1_digest
{
	/** offset */
	int32   offset;

	/** digest */
	uint8   digest[RTMP_C1_DIGEST_SIZE];

	/** random0 */
	uint8*  p_random0;
	/** random0 size */
	int32   random0_size;

	/** random1 */
	uint8*  p_random1;
	/** random1 size */
	int32   random1_size;

} RTMP_C1S1_DIGEST, *PRTMP_C1S1_DIGEST;

/** C1 */
typedef struct tag_rtmp_c1
{
	/** time */
	uint32  time;
	/** version */
	uint32  version;

	/** key */
	RTMP_C1S1_KEY    key;
	/** digest */
	RTMP_C1S1_DIGEST digest;

} RTMP_C1, *PRTMP_C1;

/** S1 */
typedef struct tag_rtmp_s1
{
	/** time */
	uint32  time;
	/** version */
	uint32  version;

	/** key */
	RTMP_C1S1_KEY    key;
	/** digest */
	RTMP_C1S1_DIGEST digest;

} RTMP_S1, *PRTMP_S1;

/** C2 */
typedef struct tag_rtmp_c2
{
	/** random */
	uint8   random[RTMP_C1S1_JOIN_SIZE];
	/** digest */
	uint8   digest[RTMP_C1_DIGEST_SIZE];

} RTMP_C2, *PRTMP_C2;

/** S2 */
typedef struct tag_rtmp_s2
{
	/** random */
	uint8   random[RTMP_C1S1_JOIN_SIZE];
	/** digest */
	uint8   digest[RTMP_C1_DIGEST_SIZE];

} RTMP_S2, *PRTMP_S2;


/** Msg Header */
typedef struct tag_rtmp_msg_header
{
	union
	{
		/** timestamp */
		uint32 ts;
		/** timestamp delta */
		uint32 ts_delta;

	} udata;

	/** msg len */
	uint32 msg_len;
	/** msg type */
	uint32 msg_type;
	/** msg stream id */
	uint32 msg_stream_id;

} RTMP_MSG_HEADER, *PRTMP_MSG_HEADER;

/** Chunk Header */
typedef struct tag_rtmp_chunk_header
{
	/** format */
	uint32 format;
	/** chunk stream id */
	uint32 chunk_stream_id;

	/** msg header */
	RTMP_MSG_HEADER msg_header;
	/** has ext timestamp */
	int32  b_ext_ts;
	/** ext timestamp */
	uint32 ext_ts;

} RTMP_CHUNK_HEADER, *PRTMP_CHUNK_HEADER;

/** Chunk Payload */
typedef struct tag_rtmp_chunk_payload
{
	/** data */
	uint8* p_data;
	/** data size */
	uint32 data_size;

} RTMP_CHUNK_PAYLOAD, *PRTMP_CHUNK_PAYLOAD;

/** Msg */
typedef struct tag_rtmp_msg
{
	/** header format */
	uint32  header_format;
	/** chunk stream id */
	uint32  chunk_stream_id;
	/** msg type */
	uint32  msg_type;
	/** msg stream id */
	uint32  msg_stream_id;
	/** origin timestamp */
	uint32  origin_ts;
	/** timestamp */
	uint32  ts;

	/** header size */
	uint32  header_size;
	/** header */
	uint8*  p_header;

	/** payload size */
	uint32  payload_size;
	/** payload */
	uint8*  p_payload;

} RTMP_MSG, *PRTMP_MSG;

#endif ///__RTMP_PUBLIC_H__
