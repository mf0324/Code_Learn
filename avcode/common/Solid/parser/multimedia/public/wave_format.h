/**
 * @file wave_format.h  WAVEFORMAT
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.27
 *
 *
 */

#ifndef __WAVE_FORMAT_H__
#define __WAVE_FORMAT_H__

/** WAVEFORMATEX长度 */
#define WAVEFORMAT_EX_SIZE                         (18)
/** WAVEFORMATEXTENSIBLE长度 */
#define WAVEFORMAT_EXTENSIBLE_SIZE                 (40)
/** MPEG1 WAVEFORMAT长度 */
#define WAVEFORMAT_MPEG1_SIZE                      (40)
/** MP3 WAVEFORMAT长度 */
#define WAVEFORMAT_MP3_SIZE                        (30)

/** copy from mmreg.h */
#define WAVE_FORMAT_TAG_UNKNOWN                    (0x0000)
#define WAVE_FORMAT_TAG_PCM                        (0x0001)
#define WAVE_FORMAT_TAG_ADPCM                      (0x0002)
#define WAVE_FORMAT_TAG_IEEE_FLOAT                 (0x0003)
#define WAVE_FORMAT_TAG_G711A                      (0x0006)
#define WAVE_FORMAT_TAG_G711MU                     (0x0007)
#define WAVE_FORMAT_TAG_DTS                        (0x0008)
#define WAVE_FORMAT_TAG_MPEG                       (0x0050)
#define WAVE_FORMAT_TAG_MP3                        (0x0055)
/** from AVIMux_GUI */
#define WAVE_FORMAT_TAG_AAC                        (0x00FF)

/** 结构体按1字节对齐 */
#pragma pack(push)
#pragma pack(1)

/** WAVEFORMATEX */
typedef struct tag_WAVEFORMAT_EX
{
	/** format tag */
	uint16 format_tag;
	/** channels */
	uint16 chn;
	/** samples per second */
	uint32 sample_rate;
	/** avg bytes per second */
	uint32 byte_rate;
	/** block align */
	uint16 block_align;
	/** sample bits */
	uint16 sample_bit;
	/** size */
	uint16 size;

} WAVEFORMAT_EX, *PWAVEFORMAT_EX;

/** WAVEFORMATEXTENSIBLE */
typedef struct tag_WAVFORMAT_EXTENSIBLE
{
	/** wavformatex */
	WAVEFORMAT_EX format;

	/** sample */
	union
	{
		uint16  valid_sample_bits;
		uint16  sample_per_block;
		uint16  resv;
	} sample;
	/** channel mask */
	uint32 chn_mask;
	/** sub format */
	uint8  sub_format[16];

} WAVEFORMAT_EXTENSIBLE, *PWAVEFORMAT_EXTENSIBLE;

/** MPEG1 */
typedef struct tag_mpeg1_waveformat
{
	/** waveformatex */
	WAVEFORMAT_EX   format;

	/** mpeg1 audio header */
	uint16          layer;
	uint32          bitrate;
	uint16          mode;
	uint16          mode_ext;
	uint16          emphasis;
	uint16          flag;
	uint32          pts_low;
	uint32          pts_high;

} MPEG1_WAVEFORMAT, *PMPEG1_WAVEFORMAT;

/** MP3 */
typedef struct tag_mp3_waveformat
{
	/** waveformatex */
	WAVEFORMAT_EX   format;

	/** mp3 header */
	uint16          id;
	uint32          flag;
	uint16          block_size;
	uint16          frame_per_block;
	uint16          codec_delay;

} MP3_WAVEFORMAT, *PMP3_WAVEFORMAT;

#pragma pack(pop)

#endif ///__WAVE_FORMAT_H__
