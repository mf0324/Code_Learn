/**
 * @file mpeg2_audio_public.h   MPEG2 Audio Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.07.15
 *
 *
 */

#ifndef __MPEG2_AUDIO_PUBLIC_H__
#define __MPEG2_AUDIO_PUBLIC_H__

#include <public/gen_int.h>


/** mpeg1 */
#define MPEG2_AUDIO_VERSION_MPEG1                          (1)
/** mpeg2 */
#define MPEG2_AUDIO_VERSION_MPEG2                          (2)
/** mpeg2.5 */
#define MPEG2_AUDIO_VERSION_MPEG2_5                        (3)


/** Audio Header */
typedef struct tag_mpeg2_audio_header
{
	/** start */
	uint8* p_start;

	/** mpeg version */
	int32  version;
	/** layer */
	int32  layer;

	/** sample bit */
	int32  sample_bit;
	/** sample rate */
	int32  sample_rate;
	/** chn */
	int32  chn;

	/** bit rate, unit: kbps */
	int32  bit_rate;
	/** max bit rate, unit: kbps */
	int32  max_bit_rate;
	/** frame size */
	int32  frame_size;
	/** max frame size */
	int32  max_frame_size;

	/** crc */
	int32  b_crc;
	/** padding */
	int32  b_padding;
	/** copyright */
	int32  b_copyright;
	/** origin */
	int32  b_origin;

	/** mode */
	int32  mode;
	/** mode ext */
	int32  mode_ext;
	/** emphasis */
	int32  emphasis;

} MPEG2_AUDIO_HEADER, *PMPEG2_AUDIO_HEADER;

#endif ///__MPEG2_AUDIO_PUBLIC_H__
