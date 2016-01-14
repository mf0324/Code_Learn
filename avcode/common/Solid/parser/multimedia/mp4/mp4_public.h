/**
 * @file mp4_public.h   MP4 Constant
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.04.28
 *
 *
 */


#ifndef __MP4_PUBLIC_H__
#define __MP4_PUBLIC_H__

/** 
 * @name MP4
 * @{
 */

/** Box长度 */
#define MP4_BOX_SIZE                                       (8)
/** Box64长度 */
#define MP4_BOX64_SIZE                                     (16)
/** Full Box长度 */
#define MP4_FULL_BOX_SIZE                                  (MP4_BOX_SIZE + 4)
/** Full Box64长度 */
#define MP4_FULL_BOX64_SIZE                                (MP4_BOX64_SIZE + 4)
/** Brand长度 */
#define MP4_FILE_TYPE_BOX_BRAND_SIZE                       (4)
/** File Type Box最小长度 */
#define MP4_FILE_TYPE_BOX_MIN_SIZE                         (MP4_BOX_SIZE + 8)
/** Movie Box长度 */
#define MP4_MOVIE_BOX_SIZE                                 (MP4_BOX_SIZE)
/** Media Data Box长度 */
#define MP4_MEDIA_DATA_BOX_SIZE                            (MP4_BOX_SIZE)
/** Movie Header Box长度 */
#define MP4_MOVIE_HEADER_BOX_SIZE                          (MP4_FULL_BOX_SIZE + 96)
/** Track Box长度 */
#define MP4_TRACK_BOX_SIZE                                 (MP4_BOX_SIZE)
/** Track Header Box长度 */
#define MP4_TRACK_HEADER_BOX_SIZE                          (MP4_FULL_BOX_SIZE + 80)
/** Media Box长度 */
#define MP4_MEDIA_BOX_SIZE                                 (MP4_BOX_SIZE)
/** Media Header Box长度 */
#define MP4_MEDIA_HEADER_BOX_SIZE                          (MP4_FULL_BOX_SIZE + 20)
/** Handler Box最小长度 */
#define MP4_HANDLER_BOX_MIN_SIZE                           (MP4_FULL_BOX_SIZE + 20)
/** Media Info Box长度 */
#define MP4_MEDIA_INFO_BOX_SIZE                            (MP4_BOX_SIZE)
/** Video Media Header Box长度 */
#define MP4_VIDEO_MEDIA_HEADER_BOX_SIZE                    (MP4_FULL_BOX_SIZE + 8)
/** Sound Media Header Box长度 */
#define MP4_SOUND_MEDIA_HEADER_BOX_SIZE                    (MP4_FULL_BOX_SIZE + 4)
/** Data Info Box长度 */
#define MP4_DATA_INFO_BOX_SIZE                             (MP4_BOX_SIZE)
/** Data Entry URL Box最小长度 */
#define MP4_DATA_ENTRY_URL_BOX_MIN_SIZE                    (MP4_FULL_BOX_SIZE)
/** Data Entry URN Box最小长度 */
#define MP4_DATA_ENTRY_URN_BOX_MIN_SIZE                    (MP4_FULL_BOX_SIZE)
/** Data Ref Box最小长度 */
#define MP4_DATA_REF_BOX_MIN_SIZE                          (MP4_FULL_BOX_SIZE + 4)
/** Sample Table Box长度 */
#define MP4_SAMPLE_TABLE_BOX_SIZE                          (MP4_BOX_SIZE)
/** Time to Sample Entry长度 */
#define MP4_TIME_TO_SAMPLE_ENTRY_SIZE                      (8)
/** Time to Sample Box长度 */
#define MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE                    (MP4_FULL_BOX_SIZE + 4)
/** Composition Time to Sample Entry长度 */
#define MP4_COMP_TIME_TO_SAMPLE_ENTRY_SIZE                 (8)
/** Composition Time to Sample Box长度 */
#define MP4_COMP_TIME_TO_SAMPLE_BOX_MIN_SIZE               (MP4_FULL_BOX_SIZE + 4)
/** Sample Entry长度 */
#define MP4_SAMPLE_ENTRY_SIZE                              (MP4_BOX_SIZE + 8)
/** Visual Sample Entry长度 */
#define MP4_VISUAL_SAMPLE_ENTRY_SIZE                       (MP4_SAMPLE_ENTRY_SIZE + 70)
/** Visual Sample Desc Box最小长度 */
#define MP4_VISUAL_SAMPLE_DESC_BOX_MIN_SIZE                (MP4_FULL_BOX_SIZE + 4)
/** Audio Sample Desc Entry长度 */
#define MP4_AUDIO_SAMPLE_ENTRY_SIZE                        (MP4_SAMPLE_ENTRY_SIZE + 20)
/** Audio Sample Desc Box最小长度 */
#define MP4_AUDIO_SAMPLE_DESC_BOX_MIN_SIZE                 (MP4_FULL_BOX_SIZE + 4)
/** Sample Size Entry长度 */
#define MP4_SAMPLE_SIZE_ENTRY_SIZE                         (4)
/** Sample Size Box最小长度 */
#define MP4_SAMPLE_SIZE_BOX_MIN_SIZE                       (MP4_FULL_BOX_SIZE + 8)
/** Sample to Chunk Entry长度 */
#define MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE                     (12)
/** Sample to Chunk Box最小长度 */
#define MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE                   (MP4_FULL_BOX_SIZE + 4)
/** Chunk Offset Entry长度 */
#define MP4_CHUNK_OFFSET_ENTRY_SIZE                        (4)
/** Chunk Offset Box最小长度 */
#define MP4_CHUNK_OFFSET_BOX_MIN_SIZE                      (MP4_FULL_BOX_SIZE + 4)
/** Sync Sample Entry长度 */
#define MP4_SYNC_SAMPLE_ENTRY_SIZE                         (4)
/** Sync Sample Box最小长度 */
#define MP4_SYNC_SAMPLE_BOX_MIN_SIZE                       (MP4_FULL_BOX_SIZE + 4)
/** Free Space Box长度 */
#define MP4_FREE_SPACE_BOX_SIZE                            (MP4_BOX_SIZE)
/** Movie Extend Box长度 */
#define MP4_MOVIE_EXTEND_BOX_SIZE                          (MP4_BOX_SIZE)
/** Movie Extend Header Box长度 */
#define MP4_MOVIE_EXTEND_HEADER_BOX_SIZE                   (MP4_FULL_BOX_SIZE + 4)
/** Track Extend Box长度 */
#define MP4_TRACK_EXTEND_BOX_SIZE                          (MP4_FULL_BOX_SIZE + 20)
/** Movie Fragment Box长度 */
#define MP4_MOVIE_FRAGMENT_BOX_SIZE                        (MP4_BOX_SIZE)
/** Movie Fragment Header Box长度 */
#define MP4_MOVIE_FRAGMENT_HEADER_BOX_SIZE                 (MP4_FULL_BOX_SIZE + 4)
/** Track Fragment Box长度 */
#define MP4_TRACK_FRAGMENT_BOX_SIZE                        (MP4_BOX_SIZE)
/** Track Fragment Header Box最小长度 */
#define MP4_TRACK_FRAGMENT_HEADER_BOX_MIN_SIZE             (MP4_FULL_BOX_SIZE + 4)
/** Track Fragment Run Box最小长度 */
#define MP4_TRACK_FRAGMENT_RUN_BOX_MIN_SIZE                (MP4_FULL_BOX_SIZE + 4)
/** Movie Fragment Random Access Box长度 */
#define MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_SIZE          (MP4_BOX_SIZE)
/** Track Fragment Random Access Entry长度 */
#define MP4_TRACK_FRAGMENT_RANDOM_ACCESS_ENTRY_SIZE        (20)
/** Track Fragment Random Access Box最小长度 */
#define MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_MIN_SIZE      (MP4_FULL_BOX_SIZE + 12)
/** Movie Fragment Random Access Offset Box长度 */
#define MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_SIZE   (MP4_FULL_BOX_SIZE + 4)
/** Meta Box最小长度 */
#define MP4_META_BOX_MIN_SIZE                              (MP4_FULL_BOX_SIZE)

/** AVC SPS Record最小长度 */
#define MP4_AVC_SPS_RECORD_MIN_SIZE                        (2)
/** AVC PPS Record最小长度 */
#define MP4_AVC_PPS_RECORD_MIN_SIZE                        (2)
/** AVC Dec Config Record头部长度 */
#define MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE              (4)
/** AVC Dec Config Record最小长度 */
#define MP4_AVC_DEC_CONFIG_RECORD_MIN_SIZE                 (7)
/** AVC Config Box最小长度 */
#define MP4_AVC_CONFIG_BOX_MIN_SIZE                        (MP4_BOX_SIZE + MP4_AVC_DEC_CONFIG_RECORD_MIN_SIZE)
/** AVC Sample Entry最小长度 */
#define MP4_AVC_SAMPLE_ENTRY_MIN_SIZE                      (MP4_VISUAL_SAMPLE_ENTRY_SIZE + MP4_AVC_CONFIG_BOX_MIN_SIZE)

/** UTC time 1904 - 1970，unit：second */
#define MP4_UTC_TIME_1904_TO_1970                          (0x7C25B080)
/** time scale in ms，unit：hz */
#define MP4_TIMESCALE_MS                                   (1000)
/** time scale in MPEG2 ts，unit：hz */
#define MP4_TIMESCALE_MPEG2_TS                             (90000)


/** version bit offset */
#define MP4_FULL_BOX_VERSION_BIT_OFFSET                    (24)
/** version 0 */
#define MP4_FULL_BOX_VERSION_0                             (0x0)
/** version 1 */
#define MP4_FULL_BOX_VERSION_1                             (0x1)
/** Movie Header Box Rate */
#define MP4_MOVIE_HEADER_BOX_RATE                          (0x00010000)
/** Movie Header Box Volume */
#define MP4_MOVIE_HEADER_BOX_VOLUME                        (0x0100)
/** video track */
#define MP4_TRACK_TYPE_VIDEO                               (1)
/** audio track */
#define MP4_TRACK_TYPE_AUDIO                               (2)
/** hint track */
#define MP4_TRACK_TYPE_HINT                                (3)
/** track enable */
#define MP4_TRACK_ENABLE_FLAG                              (0x000001)
/** track in movie */
#define MP4_TRACK_IN_MOVIE_FLAG                            (0x000002)
/** track in preview */
#define MP4_TRACK_IN_PREVIEW_FLAG                          (0x000004)
/** Data Entry URL */
#define MP4_DATA_ENTRY_TYPE_URL                            (1)
/** Data Entry URN */
#define MP4_DATA_ENTRY_TYPE_URN                            (2)
/** self contain */
#define MP4_DATA_ENTRY_FLAG_SELF_CONTAIN                   (0x000001)
/** Horiz Resolution */
#define MP4_VISUAL_SAMPLE_HORIZ_RESOLUTION                 (0x00480000)
/** Vert Resolution */
#define MP4_VISUAL_SAMPLE_VERT_RESOLUTION                  (0x00480000)
/** Frame Count */
#define MP4_VISUAL_SAMPLE_FRAME_COUNT                      (0x1)
/** Depth */
#define MP4_VISUAL_SAMPLE_DEPTH                            (0x0018)
/** Channel Count */
#define MP4_AUDIO_SAMPLE_CHANNEL_COUNT                     (2)
/** Sample Size */
#define MP4_AUDIO_SAMPLE_SIZE                              (16)
/** base data offset present */
#define MP4_BASE_DATA_OFFSET_PRESENT_FLAG                  (0x000001)
/** sample desc index present */
#define MP4_SAMPLE_DESC_INDEX_PRESENT_FLAG                 (0x000002)
/** default sample duration present */
#define MP4_DEFAULT_SAMPLE_DURATION_PRESENT_FLAG           (0x000008)
/** default sample size present */
#define MP4_DEFAULT_SAMPLE_SIZE_PRESENT_FLAG               (0x000010)
/** default sample flag present */
#define MP4_DEFAULT_SAMPLE_FLAG_PRESENT_FLAG               (0x000020)
/** duration is empty */
#define MP4_DURATION_IS_EMPTY_FLAG                         (0x010000)
/** length size of traf num bit offset */
#define MP4_TRAF_NUM_BIT_OFFSET                            (4)
/** length size of trun num bit offset */
#define MP4_TRUN_NUM_BIT_OFFSET                            (2)
/** length size of sample num bit offset */
#define MP4_SAMPLE_NUM_BIT_OFFSET                          (0)
/** data offset present */
#define MP4_DATA_OFFSET_PRESENT_FLAG                       (0x000001)
/** first sample flag present */
#define MP4_FIRST_SAMPLE_FLAG_PRESENT_FLAG                 (0x000004)
/** sample duration present */
#define MP4_SAMPLE_DURATION_PRESENT_FLAG                   (0x000100)
/** sample size present */
#define MP4_SAMPLE_SIZE_PRESENT_FLAG                       (0x000200)
/** sample flag present */
#define MP4_SAMPLE_FLAG_PRESENT_FLAG                       (0x000400)
/** sample composition time offset present */
#define MP4_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT_FLAG    (0x000800)

/** 'ftyp' */
#define MP4_FILE_TYPE_BOX_TYPE                             (0x66747970)
/** 'isom' */
#define MP4_BRAND_ISOM                                     (0x69736F6D)
/** 'mp41' */
#define MP4_BRAND_MP41                                     (0x6D703431)
/** 'mp42' */
#define MP4_BRAND_MP42                                     (0x6D703432)
/** 'frag' */
#define MP4_BRAND_FRAG                                     (0x66726167)
/** 'moov' */
#define MP4_MOVIE_BOX_TYPE                                 (0x6D6F6F76)
/** 'mdat' */
#define MP4_MEDIA_DATA_BOX_TYPE                            (0x6D646174)
/** 'free' */
#define MP4_FREE_BOX_TYPE                                  (0x66726565)
/** 'mvhd' */
#define MP4_MOVIE_HEADER_BOX_TYPE                          (0x6D766864)
/** 'iods' */
#define MP4_OBJECT_DESC_BOX_TYPE                           (0x696F6473)
/** 'trak' */
#define MP4_TRACK_BOX_TYPE                                 (0x7472616B)
/** 'tkhd' */
#define MP4_TRACK_HEADER_BOX_TYPE                          (0x746B6864)
/** Track Header Box flag */
#define MP4_TRACK_HEADER_BOX_FLAG                          (0x7)
/** 'mdia' */
#define MP4_MEDIA_BOX_TYPE                                 (0x6D646961)
/** 'mdhd' */
#define MP4_MEDIA_HEADER_BOX_TYPE                          (0x6D646864)
/** 'hdlr' */
#define MP4_HANDLER_BOX_TYPE                               (0x68646C72)
/** 'vide' */
#define MP4_HANDLER_TYPE_VIDE                              (0x76696465)
/** 'soun' */
#define MP4_HANDLER_TYPE_SOUN                              (0x736F756E)
/** 'hint' */
#define MP4_HANDLER_TYPE_HINT                              (0x68696E74)
/** 'minf' */
#define MP4_MEDIA_INFO_BOX_TYPE                            (0x6D696E66)
/** 'vmhd' */
#define MP4_VIDEO_MEDIA_HEADER_BOX_TYPE                    (0x766D6864)
/** 'smhd' */
#define MP4_SOUND_MEDIA_HEADER_BOX_TYPE                    (0x736D6864)
/** 'dinf' */
#define MP4_DATA_INFO_BOX_TYPE                             (0x64696E66)
/** 'url ' */
#define MP4_DATA_ENTRY_URL_BOX_TYPE                        (0x75726C20)
/** 'urn ' */
#define MP4_DATA_ENTRY_URN_BOX_TYPE                        (0x75726E20)
/** 'dref' */
#define MP4_DATA_REF_BOX_TYPE                              (0x64726566)
/** 'stbl' */
#define MP4_SAMPLE_TABLE_BOX_TYPE                          (0x7374626C)
/** 'stts' */
#define MP4_TIME_TO_SAMPLE_BOX_TYPE                        (0x73747473)
/** 'ctts' */
#define MP4_COMPOSITION_TIME_TO_SAMPLE_BOX_TYPE            (0x63747473)
/** 'stsd' */
#define MP4_SAMPLE_DESC_BOX_TYPE                           (0x73747364)
/** 'stsz' */
#define MP4_SAMPLE_SIZE_BOX_TYPE                           (0x7374737A)
/** 'stsc' */
#define MP4_SAMPLE_TO_CHUNK_BOX_TYPE                       (0x73747363)
/** 'stco' */
#define MP4_CHUNK_OFFSET_BOX_TYPE                          (0x7374636F)
/** 'stss' */
#define MP4_SYNC_SAMPLE_BOX_TYPE                           (0x73747373)
/** 'free' */
#define MP4_FREE_SPACE_BOX_TYPE                            (0x66726565)
/** 'mvex' */
#define MP4_MOVIE_EXTEND_BOX_TYPE                          (0x6D766578)
/** 'mehd' */
#define MP4_MOVIE_EXTEND_HEADER_BOX_TYPE                   (0x6D656864)
/** 'trex' */
#define MP4_TRACK_EXTEND_BOX_TYPE                          (0x74726578)
/** 'moof' */
#define MP4_MOVIE_FRAGMENT_BOX_TYPE                        (0x6D6F6F66)
/** 'mfhd' */
#define MP4_MOVIE_FRAGMENT_HEADER_BOX_TYPE                 (0x6D666864)
/** 'traf' */
#define MP4_TRACK_FRAGMENT_BOX_TYPE                        (0x74726166)
/** 'tfhd' */
#define MP4_TRACK_FRAGMENT_HEADER_BOX_TYPE                 (0x74666864)
/** 'trun' */
#define MP4_TRACK_FRAGMENT_RUN_BOX_TYPE                    (0x7472756E)
/** 'mfra' */
#define MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX_TYPE          (0x6D667261)
/** 'tfra' */
#define MP4_TRACK_FRAGMENT_RANDOM_ACCESS_BOX_TYPE          (0x74667261)
/** 'mfro' */
#define MP4_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX_TYPE   (0x6D66726F)
/** 'meta' */
#define MP4_META_BOX_TYPE                                  (0x6D657461)

/** 'esds' */
#define MP4_ES_DESC_BOX_TYPE                               (0x65736473)
/** 'mp4a' */
#define MP4_AAC_SAMPLE_ENTRY_BOX_TYPE                      (0x6D703461)
/** 'avcC' */
#define MP4_AVC_CONFIG_BOX_TYPE                            (0x61766343)
/** 'avc1' */
#define MP4_AVC_SAMPLE_ENTRY_BOX_TYPE                      (0x61766331)

/** 
 * @}
 */

/** 
 * @name MP4 Box Format
 * @{
 */

/** box */
#define MP4_BOX_FORMAT_ISO_BOX                             (1)
/** box64 */
#define MP4_BOX_FORMAT_ISO_BOX64                           (2)
/** full box */
#define MP4_BOX_FORMAT_ISO_FULL_BOX                        (3)
/** full box64 */
#define MP4_BOX_FORMAT_ISO_FULL_BOX64                      (4)

/** 
 * @}
 */

#endif ///__MP4_PUBLIC_H__
