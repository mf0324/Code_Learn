/**
 * @file mp4_demux_public.h   MP4 Demux Public Define
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.19
 *
 *
 */


#ifndef __MP4_DEMUX_PUBLIC_H__
#define __MP4_DEMUX_PUBLIC_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"


/** Box */
typedef struct tag_mp4_box
{
	/** format */
	int32   format;

	/** payload */
	union
	{
		/** box */
		ISO_BOX box;
		/** box64 */
		ISO_BOX64 box64;
		/** full box */
		ISO_FULL_BOX full_box;
		/** full box64 */
		ISO_FULL_BOX64 full_box64;

	} payload;

} MP4_BOX, *PMP4_BOX;


#endif ///__MP4_DEMUX_PUBLIC_H__
