/**
 * @file sp_audio_format.h  SP Audio Format
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.10
 *
 *
 */

#ifndef __SP_AUDIO_FORMAT_H__
#define __SP_AUDIO_FORMAT_H__

/**
 * @name audio format
 *
 * @{
 */

/** unknown */
#define SP_AUDIO_FORMAT_UNKNOWN                            (0)
/** PCM */
#define SP_AUDIO_FORMAT_PCM                                (1)
/** ADPCM */
#define SP_AUDIO_FORMAT_ADPCM                              (2)
/** G711A */
#define SP_AUDIO_FORMAT_G711A                              (11)
/** G711U */
#define SP_AUDIO_FORMAT_G711U                              (12)
/** G726 */
#define SP_AUDIO_FORMAT_G726                               (13)
/** G726 Fast */
#define SP_AUDIO_FORMAT_G726_FAST                          (14)
/** G729 */
#define SP_AUDIO_FORMAT_G729                               (21)
/** MP3 */
#define SP_AUDIO_FORMAT_MP3                                (100)
/** MPEG1 */
#define SP_AUDIO_FORMAT_MPEG1                              (101)
/** MPEG2 */
#define SP_AUDIO_FORMAT_MPEG2                              (102)
/** MPEG4(AAC) */
#define SP_AUDIO_FORMAT_MPEG4					           (104)
/** AAC RAW(MP4) */
#define SP_AUDIO_FORMAT_AAC_RAW 					       (105)
/** AAC ADTS */
#define SP_AUDIO_FORMAT_AAC_ADTS 					       (106)
/** AAC ADIF */
#define SP_AUDIO_FORMAT_AAC_ADIF 					       (107)

/**
 * @}
 */

/**
 * @name audio sample bit
 *
 * @{
 */

/** 8bit */
#define SP_AUDIO_SAMPLE_SIZE_8BIT                          (8)
/** 16bit */
#define SP_AUDIO_SAMPLE_SIZE_16BIT                         (16)
/** 24bit */
#define SP_AUDIO_SAMPLE_SIZE_24BIT                         (24)
/** 32bit */
#define SP_AUDIO_SAMPLE_SIZE_32BIT                         (32)
/** 8bit sample byte size */
#define SP_AUDIO_SAMPLE_BYTE_8BIT                          (SP_AUDIO_SAMPLE_SIZE_8BIT / 8)
/** 16bit sample byte size */
#define SP_AUDIO_SAMPLE_BYTE_16BIT                         (SP_AUDIO_SAMPLE_SIZE_16BIT / 8)
/** 24bit sample byte */
#define SP_AUDIO_SAMPLE_BYTE_24BIT                         (SP_AUDIO_SAMPLE_SIZE_24BIT / 8)
/** 32bit sample byte */
#define SP_AUDIO_SAMPLE_BYTE_32BIT                         (SP_AUDIO_SAMPLE_SIZE_32BIT / 8)

/**
 * @}
 */

/**
 * @name audio sample rate
 *
 * @{
 */

/** 8000 */
#define SP_AUDIO_SAMPLE_RATE_8000                          (8000)
/** 11025 */
#define SP_AUDIO_SAMPLE_RATE_11025                         (11025)
/** 16000 */
#define SP_AUDIO_SAMPLE_RATE_16000                         (16000)
/** 22050 */
#define SP_AUDIO_SAMPLE_RATE_22050                         (22050)
/** 32000 */
#define SP_AUDIO_SAMPLE_RATE_32000                         (32000)
/** 44100 */
#define SP_AUDIO_SAMPLE_RATE_44100                         (44100)
/** 48000 */
#define SP_AUDIO_SAMPLE_RATE_48000                         (48000)
/** 64000 */
#define SP_AUDIO_SAMPLE_RATE_64000                         (64000)
/** 96000 */
#define SP_AUDIO_SAMPLE_RATE_96000                         (96000)

/**
 * @}
 */

/**
 * @name audio chn
 *
 * @{
 */

/** mono */
#define SP_AUDIO_CHN_MONO                                  (1)
/** dual */
#define SP_AUDIO_CHN_DUAL                                  (2)
/** stereo */
#define SP_AUDIO_CHN_STEREO                                (2)

/**
 * @}
 */

#endif ///__SP_AUDIO_FORMAT_H__
