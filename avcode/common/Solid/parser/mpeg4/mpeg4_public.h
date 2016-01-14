/**
 * @file mpeg4_public.h   MPEG4 Constant
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.06.18
 *
 *
 */


#ifndef __MPEG4_PUBLIC_H__
#define __MPEG4_PUBLIC_H__

/** 
 * @name MPEG4 System
 * @{
 */

/** object desc tag */
#define MPEG4_OBJECT_DESC_TAG                              (0x01)
/** initial object desc tag */
#define MPEG4_INITIAL_OBJECT_DESC_TAG                      (0x02)
/** es desc tag */
#define MPEG4_ES_DESC_TAG                                  (0x03)
/** decoder config desc tag */
#define MPEG4_DEC_CONFIG_DESC_TAG                          (0x04)
/** decoder specific info tag */
#define MPEG4_DEC_SPEC_INFO_TAG                            (0x05)
/** SL config desc tag */
#define MPEG4_SL_CONFIG_DESC_TAG                           (0x06)
/** Qos desc tag */
#define MPEG4_QOS_DESC_TAG                                 (0x0C)
/** mp4 iod tag */
#define MPEG4_MP4_IOD_TAG                                  (0x10)
/** mp4 od tag */
#define MPEG4_MP4_OD_TAG                                   (0x11)


/** 8 bit */
#define MPEG4_SIZE_ENCODING_8_BIT                          (8)
/** 16 bit */
#define MPEG4_SIZE_ENCODING_16_BIT                         (16)
/** 24 bit */
#define MPEG4_SIZE_ENCODING_24_BIT                         (24)
/** 32 bit */
#define MPEG4_SIZE_ENCODING_32_BIT                         (32)
/** next byte bit offset */
#define MPEG4_SIZE_ENCODING_NEXT_BYTE_BIT_OFFSET           (7)
/** next byte enable */
#define MPEG4_SIZE_ENCODING_NEXT_BYTE_ENABLE               (0x1)
/** next byte */
#define MPEG4_SIZE_ENCODING_NEXT_BYTE                      (0x80)
/** size encoding min len, unit: byte */
#define MPEG4_SIZE_ENCODING_MIN_LEN                        (1)

/** tag size */
#define MPEG4_TAG_SIZE                                     (1)
/** predefine size */
#define MPEG4_PREDEFINE_SIZE                               (1)
/** decoder config descriptor min size */
#define MPEG4_DEC_CONFIG_DESC_MIN_SIZE                     (13)
/** object type indication bit size */
#define MPEG4_OBJ_TYPE_INDICATION_BIT_SIZE                 (8)
/** stream type bit size */
#define MPEG4_STREAM_TYPE_BIT_SIZE                         (6)
/** upstream bit size */
#define MPEG4_UPSTREAM_BIT_SIZE                            (1)
/** buf size db bit size */
#define MPEG4_BUF_SIZE_DB_BIT_SIZE                         (24)
/** max bitrate bit size */
#define MPEG4_MAX_BITRATE_BIT_SIZE                         (32)
/** avg bitrate bit size */
#define MPEG4_AVG_BITRATE_BIT_SIZE                         (32)
/** es descriptor min size */
#define MPEG4_ES_DESC_MIN_SIZE                             (3)
/** es id bit size */
#define MPEG4_ES_ID_BIT_SIZE                               (16)
/** stream dependence flag bit size */
#define MPEG4_STREAM_DEPEND_FLAG_BIT_SIZE                  (1)
/** url flag bit size */
#define MPEG4_URL_FLAG_BIT_SIZE                            (1)
/** OCR stream flag bit size */
#define MPEG4_OCR_STREAM_FLAG_BIT_SIZE                     (1)
/** stream priority bit size */
#define MPEG4_STREAM_PRIORITY_BIT_SIZE                     (5)
/** depend on es id bit size */
#define MPEG4_DEPEND_ON_ES_ID_BIT_SIZE                     (16)
/** url length bit size */
#define MPEG4_URL_LENGTH_BIT_SIZE                          (8)
/** ocr es id bit size */
#define MPEG4_OCR_ES_ID_BIT_SIZE                           (16)


/** system(a) */
#define MPEG4_OBJECT_TYPE_SYS_1                            (0x01)
/** system(b) */
#define MPEG4_OBJECT_TYPE_SYS_2                            (0x02)
/** visual mpeg4 */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG4                     (0x20)
/** visual h264 */
#define MPEG4_OBJECT_TYPE_VISUAL_H264                      (0x21)
/** audio mpeg4 */
#define MPEG4_OBJECT_TYPE_AUDIO_MPEG4                      (0x40)
/** visual mpeg2 simple profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_SIMPLE_PROFILE      (0x60)
/** visual mpeg2 main profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_MAIN_PROFILE        (0x61)
/** visual mpeg2 snr profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_SNR_PROFILE         (0x62)
/** visual mpeg2 spatial profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_SPATIAL_PROFILE     (0x63)
/** visual mpeg2 high profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_HIGH_PROFILE        (0x64)
/** visual mpeg2 422 profile */
#define MPEG4_OBJECT_TYPE_VISUAL_MPEG2_422_PROFILE         (0x65)
/** audio mpeg2 main profile */
#define MPEG4_OBJECT_TYPE_AUDIO_MPEG2_MAIN_PROFILE         (0x66)
/** audio mpeg2 LC profile */
#define MPEG4_OBJECT_TYPE_AUDIO_MPEG2_LC_PROFILE           (0x67)
/** audio mpeg2 SSR profile */
#define MPEG4_OBJECT_TYPE_AUDIO_MPEG2_SSR_PROFILE          (0x68)


/** object descriptor stream */
#define MPEG4_STREAM_TYPE_OBJ_DESC_STREAM                  (0x01)
/** clock reference stream */
#define MPEG4_STREAM_TYPE_CLK_REF_STREAM                   (0x02)
/** scene description stream */
#define MPEG4_STREAM_TYPE_SCENE_DESC_STREAM                (0x03)
/** visual stream */
#define MPEG4_STREAM_TYPE_VISUAL_STREAM                    (0x04)
/** audio stream */
#define MPEG4_STREAM_TYPE_AUDIO_STREAM                     (0x05)

/** custom */
#define MPEG4_PREDEFINE_CUSTOM                             (0x00)
/** null SL packet header */
#define MPEG4_PREDEFINE_NULL_SL_PACK_HEAD                  (0x01)
/** mp4 */
#define MPEG4_PREDEFINE_MP4                                (0x02)

/** 
 * @}
 */


/** 
 * @name MPEG4 视频ES
 * @{
 */

/** 视频ES起始码前缀 */
#define MPEG4_VES_START_CODE_PREFIX                        (0x000001)
/** 视频ES起始码前缀长度 */
#define MPEG4_VES_START_CODE_PREFIX_SIZE                   (3)
/** 视频ES起始码长度 */
#define MPEG4_VES_START_CODE_SIZE                          (4)

/** 视频对象起始码开始 */
#define MPEG4_VIDEO_OBJECT_START_CODE_BEGIN                (0x00)
/** 视频对象起始码结束 */
#define MPEG4_VIDEO_OBJECT_START_CODE_END                  (0x1F)
/** 视频VOL起始码开始范围 */
#define MPEG4_VOL_START_CODE_BEGIN                         (0x20)
/** 视频VOL起始码结束范围 */
#define MPEG4_VOL_START_CODE_END                           (0x2F)
/** 可视对象序列起始码 */
#define MPEG4_VOS_START_CODE                               (0xB0)
/** 可视对象序列结束码 */
#define MPEG4_VOS_END_CODE                                 (0xB1)
/** 视频用户数据起始码 */
#define MPEG4_USER_DATA_START_CODE                         (0xB2)
/** 视频Group of VOP起始码 */
#define MPEG4_GOP_START_CODE                               (0xB3)
/** 视频会话错误起始码 */
#define MPEG4_VIDEO_SESSION_ERR_CODE                       (0xB4)
/** 可视对象起始码 */
#define MPEG4_VISUAL_OBJECT_START_CODE                     (0xB5)
/** 视频VOP起始码 */
#define MPEG4_VOP_START_CODE                               (0xB6)
/** 视频fba object起始码 */
#define MPEG4_FBA_OBJECT_START_CODE                        (0xBA)
/** 视频fba object plane起始码 */
#define MPEG4_FBA_OBJECT_PLANE_START_CODE                  (0xBB)
/** 视频mesh object起始码 */
#define MPEG4_MESH_OBJECT_START_CODE                       (0xBC)
/** 视频mesh object plane起始码 */
#define MPEG4_MESH_OBJECT_PLANE_START_CODE                 (0xBD)
/** 视频still texture object起始码 */
#define MPEG4_STILL_TEXTURE_OBJECT_START_CODE              (0xBE)
/** 视频texture spatial layer起始码 */
#define MPEG4_TEXTURE_SPATIAL_LAYER_START_CODE             (0xBF)
/** 视频texture snr layer起始码 */
#define MPEG4_TEXTURE_SNR_LAYER_START_CODE                 (0xC0)
/** 视频texture tile起始码 */
#define MPEG4_TEXTURE_TILE_START_CODE                      (0xC1)
/** 视频texture shape layer起始码 */
#define MPEG4_TEXTURE_SHAPE_LAYER_START_CODE               (0xC2)
/** 视频填充数据起始码 */
#define MPEG4_VIDEO_STUFFING_START_CODE                    (0xC3)


/** VOS长度 */
#define MPEG4_VOS_HEADER_SIZE                              (5)
/** Visual Object长度 */
#define MPEG4_VISUAL_OBJECT_HEADER_SIZE                    (5)
/** Video Object长度 */
#define MPEG4_VIDEO_OBJECT_HEADER_SIZE                     (4)
/** VOL长度 */
#define MPEG4_VOL_HEADER_SIZE                              (7)
/** GOP长度 */
#define MPEG4_GOP_HEADER_SIZE                              (7)
/** VOP长度 */
#define MPEG4_VOP_HEADER_SIZE                              (5)

/** is visual object */
#define MPEG4_IS_VISUAL_OBJ_BIT_SIZE                       (1)
/** visual object verid */
#define MPEG4_VISUAL_OBJ_VERID_BIT_SIZE                    (4)
/** visual object priority */
#define MPEG4_VISUAL_OBJ_PRIORITY_BIT_SIZE                 (3)
/** visual object type */
#define MPEG4_VISUAL_OBJ_TYPE_BIT_SIZE                     (4)
/** video signal type */
#define MPEG4_VIDEO_SIGNAL_TYPE_BIT_SIZE                   (1)
/** video format */
#define MPEG4_VIDEO_FORMAT_BIT_SIZE                        (3)
/** video range */
#define MPEG4_VIDEO_RANGE_BIT_SIZE                         (1)
/** color desc */
#define MPEG4_COLOR_DESC_BIT_SIZE                          (1)
/** color param */
#define MPEG4_COLOR_PARAM_BIT_SIZE                         (8 + 8 + 8)

/** 
 * @}
 */

/** 
 * @name MPEG4 Profile and Level
 * @{
 */

/** Simple Profile/Level 1 */
#define MPEG4_SP_L1                                (0x1)
/** Simple Profile/Level 2 */
#define MPEG4_SP_L2                                (0x2)
/** Simple Profile/Level 3 */
#define MPEG4_SP_L3                                (0x3)
/** Simple Scalable Profile/Level 1 */
#define MPEG4_SSP_L1                               (0x11)
/** Simple Scalable Profile/Level 2 */
#define MPEG4_SSP_L2                               (0x12)
/** Core Profile/Level 1 */
#define MPEG4_CP_L1                                (0x21)
/** Core Profile/Level 2 */
#define MPEG4_CP_L2                                (0x22)
/** Main Profile/Level 2 */
#define MPEG4_MP_L2                                (0x32)
/** Main Profile/Level 3 */
#define MPEG4_MP_L3                                (0x33)
/** Main Profile/Level 4 */
#define MPEG4_MP_L4                                (0x34)
/** N-bit Profile/Level 2 */
#define MPEG4_NP_L2                                (0x42)
/** Scalable Texture Profile/Level 1 */
#define MPEG4_STP_L1                               (0x51)
/** Simple Face Animation Profile/Level 1 */
#define MPEG4_SFAP_L1                              (0x61)
/** Simple Face Animation Profile/Level 2 */
#define MPEG4_SFAP_L2                              (0x62)
/** Simple FBA Profile/Level 1 */
#define MPEG4_SFBAP_L1                             (0x63)
/** Simple FBA Profile/Level 2 */
#define MPEG4_SFBAP_L2                             (0x64)
/** Basic Animated Texture Profile/Level 1 */
#define MPEG4_BATP_L1                              (0x71)
/** Basic Animated Texture Profile/Level 2 */
#define MPEG4_BATP_L2                              (0x72)
/** Hybrid Profile/Level 1 */
#define MPEG4_HP_L1                                (0x81)
/** Hybrid Profile/Level 2 */
#define MPEG4_HP_L2                                (0x82)
/** Advanced Real Time Simple Profile/Level 1 */
#define MPEG4_ASP_L1                               (0x91)
/** Advanced Real Time Simple Profile/Level 2 */
#define MPEG4_ASP_L2                               (0x92)
/** Advanced Real Time Simple Profile/Level 3 */
#define MPEG4_ASP_L3                               (0x93)
/** Advanced Real Time Simple Profile/Level 4 */
#define MPEG4_ASP_L4                               (0x94)
/** Core Scalable Profile/Level 1 */
#define MPEG4_CSP_L1                               (0xA1)
/** Core Scalable Profile/Level 2 */
#define MPEG4_CSP_L2                               (0xA2)
/** Core Scalable Profile/Level 3 */
#define MPEG4_CSP_L3                               (0xA3)
/** Advanced Coding Efficiency Profile/Level 1 */
#define MPEG4_ACEP_L1                               (0xB1)
/** Advanced Coding Efficiency Profile/Level 2 */
#define MPEG4_ACEP_L2                               (0xB2)
/** Advanced Coding Efficiency Profile/Level 3 */
#define MPEG4_ACEP_L3                               (0xB3)
/** Advanced Coding Efficiency Profile/Level 4 */
#define MPEG4_ACEP_L4                               (0xB4)
/** Advanced Core Profile/Level 1 */
#define MPEG4_ACP_L1                                (0xC1)
/** Advanced Core Profile/Level 2 */
#define MPEG4_ACP_L2                                (0xC2)
/** Advanced Scalable Texture/Level 1 */
#define MPEG4_AST_L1                                (0xD1)
/** Advanced Scalable Texture/Level 2 */
#define MPEG4_AST_L2                                (0xD2)
/** Advanced Scalable Texture/Level 3 */
#define MPEG4_AST_L3                                (0xD3)

/** 
 * @}
 */

/** 
 * @name MPEG4 VISUAL OBJECT
 * @{
 */

/** video ID */
#define MPEG4_VO_VIDEO_ID                          (0x1)
/** still texture ID */
#define MPEG4_VO_STILL_TEXTURE_ID                  (0x2)
/** mesh ID */
#define MPEG4_VO_MESH_ID                           (0x3)
/** FBA ID */
#define MPEG4_VO_FBA_ID                            (0x4)
/** 3D mesh ID */
#define MPEG4_VO_3D_MESH_ID                        (0x5)


/** 
 * @}
 */

/** 
 * @name MPEG4 视频制式
 * @{
 */

/** Component */
#define MPEG4_COMPONENT                            (0x0)
/** PAL */
#define MPEG4_PAL                                  (0x1)
/** NTSC */
#define MPEG4_NTSC                                 (0x2)
/** SECAM */
#define MPEG4_SECAM                                (0x3)
/** MAC */
#define MPEG4_MAC                                  (0x4)

/** 
 * @}
 */

/** 
 * @name MPEG4 Video Object Type
 * @{
 */

/** Simple Object */
#define MPEG4_SIMPLE_OBJECT                        (0x1)
/** Simple Scalable Object */
#define MPEG4_SIMPLE_SCALABLE_OBJECT               (0x2)
/** Core Object */
#define MPEG4_CORE_OBJECT                          (0x3)
/** Main Object */
#define MPEG4_MAIN_OBJECT                          (0x4)
/** N-bit Object */
#define MPEG4_N_BIT_OBJECT                         (0x5)
/** Basic Anim. 2D Texture */
#define MPEG4_BASIC_ANIM_2D_TEXTURE                (0x6)
/** Anim. 2D Mesh */
#define MPEG4_ANIM_2D_MESH                         (0x7)
/** Simple Face */
#define MPEG4_SIMPLE_FACE                          (0x8)
/** Still Scalable Texture */
#define MPEG4_STILL_SCALABLE_TEXTURE               (0x9)
/** Advanced Real Time Simple */
#define MPEG4_ADVANCED_REAL_TIME_SIMPLE            (0xA)
/** Core Scalable */
#define MPEG4_CORE_SCALABLE                        (0xB)
/** Advanced Coding Efficiency */
#define MPEG4_ADVANCED_CODING_EFFICIENCY           (0xC)
/** Advanced Scalable Texture */
#define MPEG4_ADVANCED_SCALABLE_TEXTURE            (0xD)
/** Simple FBA */
#define MPEG4_SIMPLE_FBA                           (0xE)

/** 
 * @}
 */

/** 
 * @name MPEG4 Aspect Ratio
 * @{
 */

/** 1:1 */
#define MPEG4_PAR_1_1                              (0x1)
/** 12:11 */
#define MPEG4_PAR_12_11                            (0x2)
/** 10:11 */
#define MPEG4_PAR_10_11                            (0x3)
/** 16:11 */
#define MPEG4_PAR_16_11                            (0x4)
/** 40:33 */
#define MPEG4_PAR_40_33                            (0x5)
/** extend */
#define MPEG4_PAR_EXTEND                           (0xF)

/** 
 * @}
 */

/** 
 * @name MPEG4 Chroma Format
 * @{
 */

/** 4:2:0 */
#define MPEG4_CHROMA_FORMAT_420                    (0x1)

/** 
 * @}
 */

/** 
 * @name MPEG4 VOL Shape
 * @{
 */

/** Rectangular */
#define MPEG4_VOL_SHAPE_RECT                       (0x0)
/** Binary */
#define MPEG4_VOL_SHAPE_BINARY                     (0x1)
/** Binary Only */
#define MPEG4_VOL_SHAPE_BINARY_ONLY                (0x2)
/** Grayscale */
#define MPEG4_VOL_SHAPE_GRAYSCALE                  (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG4 视频帧类型
 * @{
 */

/** I帧 */
#define MPEG4_I_FRAME                              (0x0)
/** P帧 */
#define MPEG4_P_FRAME                              (0x1)
/** B帧 */
#define MPEG4_B_FRAME                              (0x2)
/** Sprite帧 */
#define MPEG4_SPRITE_FRAME                         (0x3)

/** 
 * @}
 */


#endif ///__MPEG4_PUBLIC_H__
