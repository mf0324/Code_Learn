/**
 * @file mp3_demux_public.h   MP3 Demux Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.09.10
 *
 *
 */

#ifndef __MP3_DEMUX_PUBLIC_H__
#define __MP3_DEMUX_PUBLIC_H__

#include <public/gen_int.h>


/** id3v2 tag */
typedef struct tag_mp3_id3v2_tag
{
	/** id */
	uint32  id;
	/** version */
	uint32  version;
	/** flag */
	uint32  flag;
	/** size */
	uint32  size;

} MP3_ID3V2_TAG, *PMP3_ID3V2_TAG;

/** frame */
typedef struct tag_mp3_id3v2_frame
{
	/** id */
	uint8   id[4];
	/** size */
	uint32  size;
	/** flag */
	uint32  flag;

} MP3_ID3V2_FRAME, *PMP3_ID3V2_FRAME;

#endif ///__MP3_DEMUX_PUBLIC_H__
