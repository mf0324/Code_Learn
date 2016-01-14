/**
 * @file mpeg2_aac_const.h   MPEG2 AAC Constant
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.09.27
 *
 *
 */

#ifndef __MPEG2_AAC_CONST_H__
#define __MPEG2_AAC_CONST_H__

/** 
 * @name AAC Constant
 * @{
 */

/** Main Profile */
#define MPEG2_AAC_MAIN_PROFILE                             (0)
/** LC Profile */
#define MPEG2_AAC_LC_PROFILE                               (1)
/** SSR Profile */
#define MPEG2_AAC_SSR_PROFILE                              (2)
/** Resv Profile */
#define MPEG2_AAC_RESV_PROFILE                             (3)

/** MPEG2 ID */
#define MPEG2_AAC_MPEG2_ID                                 (1)
/** MPEG4 ID */
#define MPEG2_AAC_MPEG4_ID                                 (0)

/** 96000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_96000                  (0x0)
/** 88200 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_88200                  (0x1)
/** 64000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_64000                  (0x2)
/** 48000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_48000                  (0x3)
/** 44100 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_44100                  (0x4)
/** 32000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_32000                  (0x5)
/** 24000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_24000                  (0x6)
/** 22050 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_22050                  (0x7)
/** 16000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_16000                  (0x8)
/** 12000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_12000                  (0x9)
/** 11025 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_11025                  (0xA)
/** 8000 */
#define MPEG2_AAC_SAMPLE_FREQ_INDEX_8000                   (0xB)

/** SCE */
#define MPEG2_AAC_ID_SCE                                   (0)
/** CPE */
#define MPEG2_AAC_ID_CPE                                   (1)
/** CCE */
#define MPEG2_AAC_ID_CCE                                   (2)
/** LFE */
#define MPEG2_AAC_ID_LFE                                   (3)
/** DSE */
#define MPEG2_AAC_ID_DSE                                   (4)
/** PCE */
#define MPEG2_AAC_ID_PCE                                   (5)
/** FIL */
#define MPEG2_AAC_ID_FIL                                   (6)
/** END */
#define MPEG2_AAC_ID_END                                   (7)

/** Samples Per Frame */
#define MPEG2_AAC_FRAME_SAMPLE                             (1024)

/** 
 * @}
 */

/** 
 * @name AAC ADTS
 * @{
 */

/** ADTS包头(固定头+可变头)长度 */
#define MPEG2_AAC_ADTS_HEADER_SIZE                         (7)

/** ADTS ID */
#define MPEG2_AAC_ADTS_ID                                  (1)
/** ADTS layer */
#define MPEG2_AAC_ADTS_LAYER                               (0)

/** 
 * @}
 */

#endif ///__MPEG2_AAC_CONST_H__
