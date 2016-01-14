/**
 * @file mp4_demux_def.h   MP4 Demux Define
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.19
 *
 *
 */


#ifndef __MP4_DEMUX_DEF_H__
#define __MP4_DEMUX_DEF_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"

/** box */
#define MP4_DEMUX_BOX_FORMAT_ISO_BOX                       (1)
/** box64 */
#define MP4_DEMUX_BOX_FORMAT_ISO_BOX64                     (2)
/** full box */
#define MP4_DEMUX_BOX_FORMAT_ISO_FULL_BOX                  (3)
/** full box64 */
#define MP4_DEMUX_BOX_FORMAT_ISO_FULL_BOX64                (4)


/** Box Info */
typedef struct tag_mp4_box_info
{
	/** format */
	int32   format;

	/** param */
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

	} param;

} MP4_BOX_INFO, *PMP4_BOX_INFO;


#endif ///__MP4_DEMUX_DEF_H__
