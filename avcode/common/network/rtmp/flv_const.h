/**
 * @file flv_const.h   FLV Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.31
 *
 *
 */

#ifndef __FLV_CONST_H__
#define __FLV_CONST_H__

/** 
 * @name FLV Header
 * @{
 */

/** Version */
#define FLV_HEADER_VERSION                                 (1)
/** Signature & Version */
#define FLV_HEADER_SIGNATURE_VERSION                       (0x464C5601)
/** Audio Flag Bit Pos  */
#define FLV_HEADER_AUDIO_FLAG_BIT_POS                      (2)
/** Audio Tag Present  */
#define FLV_HEADER_AUDIO_TAG_PRESENT                       (1)
/** Video Flag Bit Pos  */
#define FLV_HEADER_VIDEO_FLAG_BIT_POS                      (0)
/** Video Tag Present  */
#define FLV_HEADER_VIDEO_TAG_PRESENT                       (1)
/** Header Size(version 1)  */
#define FLV_HEADER_SIZE_1                                  (9)
/** Prev Tag Size Len */
#define FLV_PREV_TAG_SIZE_LEN                              (4)
/** Prev Tag Size 0 */
#define FLV_PREV_TAG_SIZE_0                                (0)


/** 
 * @}
 */


/** 
 * @name Tag Header
 * @{
 */

/** Tag Header Size */
#define FLV_TAG_HEADER_SIZE                                (11)
/** Filter Bit Pos */
#define FLV_TAG_FILTER_BIT_POS                             (5)
/** Filter 0 */
#define FLV_TAG_FILTER_0                                   (0)
/** Filter 1 */
#define FLV_TAG_FILTER_1                                   (1)
/** Audio Tag */
#define FLV_TAG_TYPE_AUDIO                                 (8)
/** Video Tag */
#define FLV_TAG_TYPE_VIDEO                                 (9)
/** Script Data Tag */
#define FLV_TAG_TYPE_SCRIPT_DATA                           (18)
/** Data Size Len */
#define FLV_TAG_DATA_SIZE_LEN                              (3)
/** Timestamp Size */
#define FLV_TAG_TS_SIZE                                    (4)
/** Stream ID Size */
#define FLV_TAG_STREAM_ID_SIZE                             (3)
/** Stream ID */
#define FLV_TAG_STREAM_ID                                  (0)


/** 
 * @}
 */

/** 
 * @name Video Tag
 * @{
 */

/** Video Tag Header Min Size */
#define FLV_VIDEO_TAG_HEADER_MIN_SIZE                      (1)
/** AVC Video Tag Header Size */
#define FLV_AVC_VIDEO_TAG_HEADER_SIZE                      (5)

/** key frame */
#define FLV_VIDEO_KEY_FRAME                                (1)
/** inter frame */
#define FLV_VIDEO_INTER_FRAME                              (2)
/** disposable inter frame */
#define FLV_VIDEO_DISPOSABLE_INTER_FRAME                   (3)
/** generated key frame */
#define FLV_VIDEO_GENERATED_KEY_FRAME                      (4)
/** cmd frame */
#define FLV_VIDEO_CMD_FRAME                                (5)

/** H.263 */
#define FLV_VIDEO_CODEC_ID_H263                            (2)
/** Screen Video */
#define FLV_VIDEO_CODEC_ID_SCREEN_VIDEO                    (3)
/** On2 VP6 */
#define FLV_VIDEO_CODEC_ID_ON2_VP6                         (4)
/** On2 VP6 Alpha */
#define FLV_VIDEO_CODEC_ID_ON2_VP6_ALPHA                   (5)
/** Screen Video Version 2 */
#define FLV_VIDEO_CODEC_ID_SCREEN_VIDEO_2                  (6)
/** AVC */
#define FLV_VIDEO_CODEC_ID_AVC                             (7)

/** Sequence Header */
#define FLV_VIDEO_AVC_SEQ_HEADER                           (0)
/** Nalu */
#define FLV_VIDEO_AVC_NALU                                 (1)
/** End of Sequence */
#define FLV_VIDEO_AVC_EOS                                  (2)


/** 
 * @}
 */

/** 
 * @name Audio Tag
 * @{
 */

/** Audio Tag Header Min Size */
#define FLV_AUDIO_TAG_HEADER_MIN_SIZE                      (1)
/** AAC Audio Tag Header Size */
#define FLV_AAC_AUDIO_TAG_HEADER_SIZE                      (2)

/** LPCM */
#define FLV_AUDIO_FORMAT_LPCM                              (0)
/** ADPCM */
#define FLV_AUDIO_FORMAT_ADPCM                             (1)
/** MP3 */
#define FLV_AUDIO_FORMAT_MP3                               (2)
/** LPCM(little endian) */
#define FLV_AUDIO_FORMAT_LPCM_LITTLE_ENDIAN                (3)
/** AAC */
#define FLV_AUDIO_FORMAT_AAC                               (10)
/** Speex */
#define FLV_AUDIO_FORMAT_SPEEX                             (11)
/** MP3 8K */
#define FLV_AUDIO_FORMAT_MP3_8K                            (14)

/** 5.5K */
#define FLV_AUDIO_SAMPLE_RATE_5K                           (0)
/** 11K */
#define FLV_AUDIO_SAMPLE_RATE_11K                          (1)
/** 22K */
#define FLV_AUDIO_SAMPLE_RATE_22K                          (2)
/** 44K */
#define FLV_AUDIO_SAMPLE_RATE_44K                          (3)
/** 8 bit */
#define FLV_AUDIO_SAMPLE_8_BIT                             (0)
/** 16 bit */
#define FLV_AUDIO_SAMPLE_16_BIT                            (1)
/** Mono */
#define FLV_AUDIO_MONO                                     (0)
/** Stereo */
#define FLV_AUDIO_STEREO                                   (1)
/** Sequence Header */
#define FLV_AUDIO_AAC_SEQ_HEADER                           (0)
/** Raw */
#define FLV_AUDIO_AAC_RAW                                  (1)


/** 
 * @}
 */

#endif ///__FLV_CONST_H__
