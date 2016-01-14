/**
 * @file mpeg4_aac_const.h   MPEG4 AAC Constant
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.08.29
 *
 *
 */


#ifndef __MPEG4_AAC_CONST_H__
#define __MPEG4_AAC_CONST_H__

/** 
 * @name Audio Constant
 * @{
 */

/** audio spec config min size */
#define MPEG4_AUDIO_SPEC_CONFIG_MIN_SIZE                   (2)
/** audio obj type bit size */
#define MPEG4_AUDIO_OBJECT_TYPE_BIT_SIZE                   (5)
/** audio obj type ext bit size */
#define MPEG4_AUDIO_OBJECT_TYPE_EXT_BIT_SIZE               (6)
/** sample freq index bit size */
#define MPEG4_AUDIO_SAMPLE_FREQ_INDEX_BIT_SIZE             (4)
/** sample freq bit size */
#define MPEG4_AUDIO_SAMPLE_FREQ_BIT_SIZE                   (24)
/** chn config bit size */
#define MPEG4_AUDIO_CHN_CONFIG_BIT_SIZE                    (4)
/** ext sample freq index bit size */
#define MPEG4_AUDIO_EXT_SAMPLE_FREQ_INDEX_BIT_SIZE         (4)
/** ext sample freq bit size */
#define MPEG4_AUDIO_EXT_SAMPLE_FREQ_BIT_SIZE               (24)

/** 
 * @}
 */

/** 
 * @name Audio Object Type
 * @{
 */

/** audio obj type bit size */
#define MPEG4_AUDIO_OBJECT_TYPE_BIT_SIZE                   (5)
/** audio obj type ext bit size */
#define MPEG4_AUDIO_OBJECT_TYPE_EXT_BIT_SIZE               (6)

/** Null */
#define MPEG4_AUDIO_OBJECT_TYPE_NULL                       (0)
/** AAC MAIN */
#define MPEG4_AUDIO_OBJECT_TYPE_AAC_MAIN                   (1)
/** AAC LC */
#define MPEG4_AUDIO_OBJECT_TYPE_AAC_LC                     (2)
/** AAC SSR */
#define MPEG4_AUDIO_OBJECT_TYPE_AAC_SSR                    (3)
/** AAC LTP */
#define MPEG4_AUDIO_OBJECT_TYPE_AAC_LTP                    (4)
/** SBR */
#define MPEG4_AUDIO_OBJECT_TYPE_SBR                        (5)
/** ER BSAC */
#define MPEG4_AUDIO_OBJECT_TYPE_ER_BSAC                    (22)
/** PS */
#define MPEG4_AUDIO_OBJECT_TYPE_PS                         (29)


/** Samples Per Frame */
#define MPEG4_AAC_FRAME_SAMPLE                             (1024)

/** 
 * @}
 */

/** 
 * @name Sample Frequency Index
 * @{
 */

/** 96000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_96000                  (0x0)
/** 88200 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_88200                  (0x1)
/** 64000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_64000                  (0x2)
/** 48000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_48000                  (0x3)
/** 44100 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_44100                  (0x4)
/** 32000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_32000                  (0x5)
/** 24000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_24000                  (0x6)
/** 22050 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_22050                  (0x7)
/** 16000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_16000                  (0x8)
/** 12000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_12000                  (0x9)
/** 11025 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_11025                  (0xA)
/** 8000 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_8000                   (0xB)
/** 7350 */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_7350                   (0xC)
/** escape */
#define MPEG4_AAC_SAMPLE_FREQ_INDEX_ESCAPE                 (0xF)

/** 
 * @}
 */

/** 
 * @name Channel Config
 * @{
 */

/** 0 */
#define MPEG4_AAC_CHN_CONFIG_0                             (0)
/** 1 */
#define MPEG4_AAC_CHN_CONFIG_1                             (1)
/** 2 */
#define MPEG4_AAC_CHN_CONFIG_2                             (2)
/** 3 */
#define MPEG4_AAC_CHN_CONFIG_3                             (3)
/** 4 */
#define MPEG4_AAC_CHN_CONFIG_4                             (4)
/** 5 */
#define MPEG4_AAC_CHN_CONFIG_5                             (5)
/** 6 */
#define MPEG4_AAC_CHN_CONFIG_6                             (6)
/** 7 */
#define MPEG4_AAC_CHN_CONFIG_7                             (7)


/** 
 * @}
 */

#endif ///__MPEG4_AAC_CONST_H__
