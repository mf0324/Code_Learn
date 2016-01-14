/**
 * @file h264_public.h   H264 Public Macro
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.02.26
 *
 *
 */


#ifndef __H264_PUBLIC_H__
#define __H264_PUBLIC_H__

/** 
 * @name H264 Nalu Header
 * @{
 */

/** forbidden zero bit value */
#define H264_FORBIDDEN_ZERO_BIT                    (0)
/** undefined */
#define H264_NALU_UNDEFINED                        (0)
/** non IDR */
#define H264_NALU_NON_IDR_SLICE                    (1)
/** slice data partition A */
#define H264_NALU_DATA_A_SLICE                     (2)
/** slice data partition B */
#define H264_NALU_DATA_B_SLICE                     (3)
/** slice data partition C */
#define H264_NALU_DATA_C_SLICE                     (4)
/** IDR */
#define H264_NALU_IDR_SLICE                        (5)
/** SEI */
#define H264_NALU_SEI                              (6)
/** SPS */
#define H264_NALU_SPS                              (7)
/** PPS */
#define H264_NALU_PPS                              (8)
/** Access Unit Delimeter */
#define H264_NALU_AUD                              (9)
/** end of seq */
#define H264_NALU_SEQ_END                          (10)
/** end of stream */
#define H264_NALU_STREAM_END                       (11)
/** filler data */
#define H264_NALU_FILLER_DATA                      (12)
/** SPS Extension */
#define H264_NALU_SPS_EXT                          (13)
/** auxiliary */
#define H264_NALU_AUX_SLICE                        (19)
/** Reserve(24) */
#define H264_NALU_RESV_BEGIN                       (24)
/** Reserve(31) */
#define H264_NALU_RESV_END                         (31)

/** bytestream header size */
#define H264_BS_HEADER_SIZE                        (4)
/** nalu length size */
#define H264_NALU_LENGTH_SIZE                      (4)
/** nalu header size */
#define H264_NALU_HEADER_SIZE                      (1)
/** sps header size */
#define H264_SPS_HEADER_SIZE                       (3)

/** max sps nalu size */
#define H264_MAX_SPS_NALU_SIZE                     (128)
/** max pps nalu size */
#define H264_MAX_PPS_NALU_SIZE                     (128)

/** 
 * @}
 */

/** 
 * @name H264 Profile
 * @{
 */

/** Base Profile */
#define H264_BASE_PROFILE                          (66)
/** Main Profile */
#define H264_MAIN_PROFILE                          (77)
/** Ext Profile */
#define H264_EXT_PROFILE                           (88)
/** High Profile */
#define H264_HIGH_PROFILE                          (100)
/** High 10 Profile */
#define H264_HIGH_10_PROFILE                       (110)
/** High 4:2:2 Profile */
#define H264_HIGH_422_PROFILE                      (122)
/** High 4:4:4 Profile */
#define H264_HIGH_444_PROFILE                      (144)

/** 
 * @}
 */

/** 
 * @name H264 Level
 * @{
 */

/** Level 1 */
#define H264_LEVEL_1                               (10)
/** Level 1b */
#define H264_LEVEL_1B                              (11)
/** Level 1.1 */
#define H264_LEVEL_11                              (11)
/** Level 1.2 */
#define H264_LEVEL_12                              (12)
/** Level 1.3 */
#define H264_LEVEL_13                              (13)
/** Level 2 */
#define H264_LEVEL_2                               (20)
/** Level 2.1 */
#define H264_LEVEL_21                              (21)
/** Level 2.2 */
#define H264_LEVEL_22                              (22)
/** Level 3 */
#define H264_LEVEL_3                               (30)
/** Level 3.1 */
#define H264_LEVEL_31                              (31)
/** Level 3.2 */
#define H264_LEVEL_32                              (32)
/** Level 4 */
#define H264_LEVEL_4                               (40)
/** Level 4.1 */
#define H264_LEVEL_41                              (41)
/** Level 4.2 */
#define H264_LEVEL_42                              (42)
/** Level 5 */
#define H264_LEVEL_5                               (50)
/** Level 5.1 */
#define H264_LEVEL_51                              (51)


/** 
 * @}
 */

/** 
 * @name H264 SEI
 * @{
 */

/** Buffering Period */
#define H264_SEI_BUFFERING_PERIOD                  (0)
/** Pic Timing */
#define H264_SEI_PIC_TIMING                        (1)
/** Pan Scan Rect */
#define H264_SEI_PAN_SCAN_RECT                     (2)
/** Filler Payload */
#define H264_SEI_FILLER_PAYLOAD                    (3)
/** User Data Register */
#define H264_SEI_USER_DATA_REGISTER                (4)
/** User Data Unregister */
#define H264_SEI_USER_DATA_UNREGISTER              (5)
/** Recovery Point */
#define H264_SEI_RECOVERY_POINT                    (6)
/** Dec Ref Pic */
#define H264_SEI_DEC_REF_PIC                       (7)
/** Spare Pic */
#define H264_SEI_SPARE_PIC                         (8)
/** Scene Info */
#define H264_SEI_SCENE_INFO                        (9)
/** Sub Seq Info */
#define H264_SEI_SUB_SEQ_INFO                      (10)
/** Sub Seq Layer */
#define H264_SEI_SUB_SEQ_LAYER                     (11)
/** Sub Seq Characteristics */
#define H264_SEI_SUB_SEQ_CHARACTERISTICS           (12)
/** Full Frame Freeze */
#define H264_SEI_FULL_FRAME_FREEZE                 (13)
/** Full Frame Freeze Release */
#define H264_SEI_FULL_FRAME_FREEZE_RELEASE         (14)
/** Full Frame Snapshot */
#define H264_SEI_FULL_FRAME_SNAPSHOT               (15)
/** Progressive Refinement Segment Start */
#define H264_SEI_PRS_START                         (16)
/** Progressive Refinement Segment End */
#define H264_SEI_PRS_END                           (17)
/** Motion Constrained Slice Group Set */
#define H264_SEI_MCSGS                             (18)
/** Film Grain Characteristics */
#define H264_SEI_FILM_GRAIN_CHARACTERISTICS        (19)
/** Deblocking Filter Display Preference */
#define H264_SEI_DFDP                              (20)
/** Stereo Video Info */
#define H264_SEI_STEREO_VIDEO_INFO                 (21)
/** Hisi Video Info */
#define H264_SEI_HISI_VIDEO_INFO                   (224)

/** 
 * @}
 */

/** 
 * @name H264 Field Mode
 * @{
 */

/** Î´Öª */
#define H264_FRAME_UNKNOWN                         (0)
/** Frame */
#define H264_FRAME_MODE                            (1)
/** Field */
#define H264_FIELD_MODE                            (2)

/** Even Field */
#define H264_FIELD_EVEN                            (2)
/** Odd Field */
#define H264_FIELD_ODD                             (3)

/** 
 * @}
 */

/** 
 * @name H264 SPS
 * @{
 */

/** SPS ID Begin */
#define H264_SPS_ID_BEGIN                          (0)
/** SPS ID End */
#define H264_SPS_ID_END                            (31)
/** Chroma Format Mono */
#define H264_CHROMA_FORMAT_MONO                    (0)
/** Chroma Format 4:2:0 */
#define H264_CHROMA_FORMAT_420                     (1)
/** Chroma Format 4:2:2 */
#define H264_CHROMA_FORMAT_422                     (2)
/** Chroma Format 4:4:4 */
#define H264_CHROMA_FORMAT_444                     (3)
/** LOG2 MAX FRAME NUM MINUS4 End */
#define H264_LOG2_MAX_FRAME_NUM_MINUS4_END         (12)
/** PIC ORDER CNT TYPE End */
#define H264_PIC_ORDER_CNT_TYPE_END                (2)

/** 
 * @}
 */

/** 
 * @name H264 PPS
 * @{
 */

/** PPS ID Begin */
#define H264_PPS_ID_BEGIN                          (0)
/** PPS ID End */
#define H264_PPS_ID_END                            (0xFF)

/** SGM Type Interleave */
#define H264_SGM_TYPE_INTERLEAVE                   (0)

/** 
 * @}
 */

/** 
 * @name H264 Slice Header
 * @{
 */

/** P Slice */
#define H264_P_SLICE                               (0)
/** B Slice */
#define H264_B_SLICE                               (1)
/** I Slice */
#define H264_I_SLICE                               (2)
/** SP Slice */
#define H264_SP_SLICE                              (3)
/** SI Slice */
#define H264_SI_SLICE                              (4)
/** Dup P Slice */
#define H264_DUP_P_SLICE                           (5)
/** Dup B Slice */
#define H264_DUP_B_SLICE                           (6)
/** Dup I Slice */
#define H264_DUP_I_SLICE                           (7)
/** Dup SP Slice */
#define H264_DUP_SP_SLICE                          (8)
/** Dup SI Slice */
#define H264_DUP_SI_SLICE                          (9)

/** 
 * @}
 */

#endif ///__H264_PUBLIC_H__
