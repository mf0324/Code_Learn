/**
 * @file mpeg2_public.h   MPEG2 Public
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.05.12
 *
 *
 */


#ifndef __MPEG2_PUBLIC_H__
#define __MPEG2_PUBLIC_H__

/** 
 * @name MPEG2 TS
 * @{
 */

/** TS同步字节 */
#define MPEG2_TS_SYNC_BYTE                         (0x47)
/** TS包固定长度 */
#define MPEG2_TS_PACK_SIZE                         (188)
/** TS头长度 */
#define MPEG2_TS_HEADER_SIZE                       (4)
/** TS Max PID */
#define MPEG2_TS_MAX_PID                           (0x1FFF)
/** TS最大counter */
#define MPEG2_TS_MAX_COUNTER                       (16)
/** TS最大调整字段长度 */
#define MPEG2_TS_MAX_ADAPT_LEN                     (183)
/** TS最大载荷长度 */
#define MPEG2_TS_MAX_PAYLOAD_LEN                   (184)
/** TS Adapt头长度 */
#define MPEG2_TS_ADAPT_HEADER_LEN                  (1)
/** TS Adapt Flag长度 */
#define MPEG2_TS_ADAPT_FLAG_LEN                    (1)
/** TS 私有数据头长度 */
#define MPEG2_TS_PRIVATE_HEADER_LEN                (1)
/** TS填充字节 */
#define MPEG2_TS_PADDING_BYTE                      (0xFF)

/** 不加密 */
#define MPEG2_TS_SCRAMBLE_NONE                     (0x0)
/** TS Adapt PCR Flag */
#define MPEG2_TS_ADAPT_PCR_FLAG                    (0x10)

/** 
 * @}
 */

/** 
 * @name MPEG2 PS
 * @{
 */

/** PS起始码 */
#define MPEG2_PS_START_CODE                        (0xBA)
/** PS结束码 */
#define MPEG2_PS_END_CODE                          (0xB9)
/** 系统首部起始码 */
#define MPEG2_SYSTEM_HEADER_START_CODE             (0xBB)
/** PS Map起始码 */
#define MPEG2_PS_MAP_START_CODE                    (0xBC)
/** PS Directory起始码 */
#define MPEG2_PS_DIRECTORY_START_CODE              (0xFF)

/** PS起始码长度 */
#define MPEG2_PS_START_CODE_SIZE                   (4)
/** PS首部长度，4+6+3+1 */
#define MPEG2_PS_HEADER_SIZE                       (14)
/** 系统首部长度，4+2+6 */
#define MPEG2_SYS_HEADER_SIZE                      (12)
/** Stream ID Info长度 */
#define MPEG2_SYS_STREAM_ID_INFO_SIZE              (3)

/** PS map最小长度 */
#define MPEG2_PS_MAP_MIN_SIZE                      (6)
/** PS map最大长度 */
#define MPEG2_PS_MAP_MAX_LEN                       (1024)
/** es map min size */
#define MPEG2_ES_MAP_MIN_SIZE                      (4)
/** PS directory最小长度 */
#define MPEG2_PS_DIRECTORY_MIN_SIZE                (6)
/** PS directory AU长度 */
#define MPEG2_PS_DIRECTORY_AU_SIZE                 (16)

/** PS首部填充最大长度 */
#define MPEG2_PS_HEADER_MAX_STUFF_SIZE             (7)

/** 
 * @}
 */

/** 
 * @name MPEG2 PES
 * @{
 */

/** PES起始码 */
#define MPEG2_PES_START_CODE                       (0x000001)
/** PES起始码长度 */
#define MPEG2_PES_START_CODE_SIZE                  (4)
/** PES 头部最小长度 */
#define MPEG2_PES_HEADER_MIN_SIZE                  (6)
/** PES头部长度，4+2+3 */
#define MPEG2_PES_HEADER_SIZE                      (9)
/** PES stream id 起始范围 */
#define MPEG2_PES_SID_START                        (0xBC)
/** PES stream id 结束范围 */
#define MPEG2_PES_SID_END                          (0xFF)
/** PES private stream 1 */
#define MPEG2_PES_SID_PRIVATE_STREAM_1             (0xBD)
/** PES padding stream */
#define MPEG2_PES_SID_PADDING_STREAM               (0xBE)
/** PES private stream 2 */
#define MPEG2_PES_SID_PRIVATE_STREAM_2             (0xBF)
/** PES Audio stream id 起始范围 */
#define MPEG2_PES_AUDIO_SID_START                  (0xC0)
/** PES Audio stream id 结束范围 */
#define MPEG2_PES_AUDIO_SID_END                    (0xDF)
/** PES Video stream id 起始范围 */
#define MPEG2_PES_VIDEO_SID_START                  (0xE0)
/** PES Video stream id 结束范围 */
#define MPEG2_PES_VIDEO_SID_END                    (0xEF)
/** PES ECM stream */
#define MPEG2_PES_SID_ECM_STREAM                   (0xF0)
/** PES EMM stream */
#define MPEG2_PES_SID_EMM_STREAM                   (0xF1)
/** PES DSM CC stream */
#define MPEG2_PES_SID_DSM_CC_STREAM                (0xF2)
/** PES ISO/IEC_13522 stream */
#define MPEG2_PES_SID_ISO_13522_STREAM             (0xF3)

/** PES包最大长度 */
#define MPEG2_PES_MAX_PACK_SIZE                    (0xFFFF)

/** 
 * @}
 */

/** 
 * @name MPEG2系统时间
 * @{
 */

/** MPEG2系统时钟每秒Tick数 */
#define MPEG2_SYS_TICK_PER_SECOND                  (90000)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频ES
 * @{
 */

/** 视频ES起始码前缀 */
#define MPEG2_VES_START_CODE_PREFIX                (0x000001)
/** 视频ES起始码前缀长度 */
#define MPEG2_VES_START_CODE_PREFIX_SIZE           (3)
/** 视频ES起始码长度 */
#define MPEG2_VES_START_CODE_SIZE                  (4)
/** 视频图象起始码 */
#define MPEG2_PICTURE_START_CODE                   (0x00)
/** 视频slice起始码起始范围 */
#define MPEG2_SLICE_START_CODE_BEGIN               (0x01)
/** 视频slice起始码结束范围 */
#define MPEG2_SLICE_START_CODE_END                 (0xAF)
/** 视频用户数据起始码 */
#define MPEG2_USER_DATA_START_CODE                 (0xB2)
/** 视频序列头起始码 */
#define MPEG2_SEQ_START_CODE                       (0xB3)
/** 视频序列错误起始码 */
#define MPEG2_SEQ_ERR_CODE                         (0xB4)
/** 视频扩展起始码 */
#define MPEG2_EXT_START_CODE                       (0xB5)
/** 视频序列结束码 */
#define MPEG2_SEQ_END_CODE                         (0xB7)
/** 视频GOP起始码 */
#define MPEG2_GROUP_START_CODE                     (0xB8)
/** 系统起始码起始范围 */
#define MPEG2_SYS_START_CODE_BEGIN                 (0xB9)
/** 系统起始码结束范围 */
#define MPEG2_SYS_START_CODE_END                   (0xFF)

/** 序列扩展ID */
#define MPEG2_SEQ_EXT_ID                           (0x1)
/** 序列显示扩展ID */
#define MPEG2_SEQ_DISPLAY_EXT_ID                   (0x2)
/** 量化矩阵扩展ID */
#define MPEG2_QUANT_MATRIX_EXT_ID                  (0x3)
/** 序列分级扩展ID */
#define MPEG2_SEQ_SCALABLE_EXT_ID                  (0x5)
/** 图像显示扩展ID */
#define MPEG2_PIC_DISPLAY_EXT_ID                   (0x7)
/** 图像编码扩展ID */
#define MPEG2_PIC_CODING_EXT_ID                    (0x8)

/** 视频扩展头长度 */
#define MPEG2_VIDEO_EXT_HEADER_SIZE                (5)
/** 视频序列头首部长度，95 bit */
#define MPEG2_SEQ_START_HEADER_SIZE                (12)
/** 视频序列扩展头长度，80 bit */
#define MPEG2_SEQ_EXT_HEADER_SIZE                  (10)
/** 视频序列显示扩展头长度，69+24 bit */
#define MPEG2_SEQ_DISPLAY_EXT_SIZE                 (12)
/** 视频GOP头首部长度，59 bit */
#define MPEG2_GOP_HEADER_SIZE                      (8)
/** 视频图像头长度 */
#define MPEG2_VIDEO_PIC_HEADER_SIZE                (8)
/** 视频图像编码扩展头长度, 66 bit */
#define MPEG2_VIDEO_PIC_CODING_EXT_SIZE            (9)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频比例
 * @{
 */

/** 1:1 */
#define MPEG2_VIDEO_ASPECT_RATIO_1_1               (0x1)
/** 4:3 */
#define MPEG2_VIDEO_ASPECT_RATIO_4_3               (0x2)
/** 16:9 */
#define MPEG2_VIDEO_ASPECT_RATIO_16_9              (0x3)
/** 2.21:1 */
#define MPEG2_VIDEO_ASPECT_RATIO_221_100           (0x4)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频帧率
 * @{
 */

/** 23.976 */
#define MPEG2_VIDEO_FRAME_RATE_23                  (0x1)
/** 24 */
#define MPEG2_VIDEO_FRAME_RATE_24                  (0x2)
/** 25 */
#define MPEG2_VIDEO_FRAME_RATE_25                  (0x3)
/** 29.97 */
#define MPEG2_VIDEO_FRAME_RATE_29                  (0x4)
/** 30 */
#define MPEG2_VIDEO_FRAME_RATE_30                  (0x5)
/** 50 */
#define MPEG2_VIDEO_FRAME_RATE_50                  (0x6)
/** 59.94 */
#define MPEG2_VIDEO_FRAME_RATE_59                  (0x7)
/** 60 */
#define MPEG2_VIDEO_FRAME_RATE_60                  (0x8)

/** 
 * @}
 */

/** 
 * @name MPEG2 Profile
 * @{
 */

/** Simple Profile */
#define MPEG2_SIMPLE_PROFILE                       (0x5)
/** Main Profile */
#define MPEG2_MAIN_PROFILE                         (0x4)
/** SNR Profile */
#define MPEG2_SNR_PROFILE                          (0x3)
/** Spatial Profile */
#define MPEG2_SPATIAL_PROFILE                      (0x2)
/** High Profile */
#define MPEG2_HIGH_PROFILE                         (0x1)

/** 
 * @}
 */

/** 
 * @name MPEG2 Level
 * @{
 */

/** low level */
#define MPEG2_LOW_LEVEL                            (0xA)
/** main level */
#define MPEG2_MAIN_LEVEL                           (0x8)
/** high1440 level */
#define MPEG2_HIGH1440_LEVEL                       (0x6)
/** high level */
#define MPEG2_HIGH_LEVEL                           (0x4)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频帧格式
 * @{
 */

/** 4:2:0 */
#define MPEG2_CHROMA_FORMAT_420                    (0x1)
/** 4:2:2 */
#define MPEG2_CHROMA_FORMAT_422                    (0x2)
/** 4:4:4 */
#define MPEG2_CHROMA_FORMAT_444                    (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频制式
 * @{
 */

/** PAL */
#define MPEG2_PAL                                  (0x1)
/** NTSC */
#define MPEG2_NTSC                                 (0x2)
/** SECAM */
#define MPEG2_SECAM                                (0x3)
/** MAC */
#define MPEG2_MAC                                  (0x4)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频帧类型
 * @{
 */

/** I帧 */
#define MPEG2_I_FRAME                              (0x1)
/** P帧 */
#define MPEG2_P_FRAME                              (0x2)
/** B帧 */
#define MPEG2_B_FRAME                              (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频DC精度
 * @{
 */

/** 8 bit */
#define MPEG2_VIDEO_DC_PRECISION_8                 (0x0)
/** 9 bit */
#define MPEG2_VIDEO_DC_PRECISION_9                 (0x1)
/** 10 bit */
#define MPEG2_VIDEO_DC_PRECISION_10                (0x2)
/** 11 bit */
#define MPEG2_VIDEO_DC_PRECISION_11                (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG2 视频帧信息
 * @{
 */

/** 顶场 */
#define MPEG2_VIDEO_PIC_TOP_FIELD                  (0x1)
/** 底场 */
#define MPEG2_VIDEO_PIC_BOTTOM_FIELD               (0x2)
/** 帧 */
#define MPEG2_VIDEO_PIC_FRAME                      (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG2 音频ES
 * @{
 */

/** 音频ES起始码前缀 */
#define MPEG2_AES_START_PREFIX                     (0x000001)
/** 音频帧头首部长度，32 bit */
#define MPEG2_AES_HEADER_SIZE                      (4)

/** Layer I */
#define MPEG2_AUDIO_LAYER_I                        (0x3)
/** Layer II */
#define MPEG2_AUDIO_LAYER_II                       (0x2)
/** Layer III */
#define MPEG2_AUDIO_LAYER_III                      (0x1)

/** 44.1 kHz */
#define MPEG2_AUDIO_44KHZ                          (0x0)
/** 48 kHz */
#define MPEG2_AUDIO_48KHZ                          (0x1)
/** 32 kHz */
#define MPEG2_AUDIO_32KHZ                          (0x2)

/** stereo */
#define MPEG2_AUDIO_STEREO                         (0x0)
/** audio mode joint_stereo */
#define MPEG2_AUDIO_JOINT_STEREO                   (0x1)
/** audio mode dual_channel */
#define MPEG2_AUDIO_DUAL_CHANNEL                   (0x2)
/** audio mode single_channel */
#define MPEG2_AUDIO_SINGLE_CHANNEL                 (0x3)

/** 
 * @}
 */

/** 
 * @name MPEG2 PSI
 * @{
 */

/** PAT(节目关联表) PID */
#define MPEG2_PAT_PID                              (0x0)
/** CAT(条件访问表) PID */
#define MPEG2_CAT_PID                              (0x1)
/** 空分组PID */
#define MPEG2_NULL_PID                             (0x1FFF)

/** program association section */
#define MPEG2_PAT_TABLE_ID                         (0x0)
/** condition access section */
#define MPEG2_CAT_TABLE_ID                         (0x1)
/** program map section */
#define MPEG2_PMT_TABLE_ID                         (0x2)

/** Pointer Field */
#define MPEG2_PSI_POINTER_SIZE                     (1)
/** PAT header size */
#define MPEG2_PAT_HEADER_SIZE                      (8)
/** PAT sub section size */
#define MPEG2_PAT_SUB_SECTION_SIZE                 (4)
/** PAT Min size */
#define MPEG2_PAT_MIN_SIZE                         (12)
/** CAT Min size */
#define MPEG2_CAT_MIN_SIZE                         (12)
/** PMT header size */
#define MPEG2_PMT_HEADER_SIZE                      (12)
/** PMT Min size */
#define MPEG2_PMT_MIN_SIZE                         (16)
/** PMT Section Min size */
#define MPEG2_PMT_SECTION_MIN_SIZE                 (5)
/** Network Program Number */
#define MPEG2_NETWORK_PROGRAM_NUMBER               (0x0)

/** CRC32 size */
#define MPEG2_PSI_CRC32_SIZE                       (4)

/** 
 * @}
 */

/** 
 * @name MPEG stream type
 * @{
 */

/** MPEG1 Video */
#define MPEG_STREAM_MPEG1_VIDEO                    (0x1)
/** MPEG2 Video */
#define MPEG_STREAM_MPEG2_VIDEO                    (0x2)
/** MPEG1 Audio */
#define MPEG_STREAM_MPEG1_AUDIO                    (0x3)
/** MPEG2 Audio */
#define MPEG_STREAM_MPEG2_AUDIO                    (0x4)
/** MPEG2 Private */
#define MPEG_STREAM_MPEG2_PRIVATE                  (0x5)
/** MPEG2 PES Private */
#define MPEG_STREAM_MPEG2_PES_PRIVATE              (0x6)
/** MHEG */
#define MPEG_STREAM_MHEG                           (0x7)
/** MPEG2 DSM CC */
#define MPEG_STREAM_MPEG2_DSM_CC                   (0x8)
/** MPEG 1/2 */
#define MPEG_STREAM_MPEG1_MPEG2                    (0x9)
/** MPEG AAC */
#define MPEG_STREAM_MPEG2_AAC                      (0xF)
/** MPEG4 Video */
#define MPEG_STREAM_MPEG4_VIDEO                    (0x10)
/** H264 */
#define MPEG_STREAM_H264                           (0x1B)

/** 
 * @}
 */

/** 
 * @name MPEG2 Descriptor
 * @{
 */

/** Video Stream */
#define MPEG2_VIDEO_STREAM_DESCRIPTOR              (0x2)
/** Audio Stream */
#define MPEG2_AUDIO_STREAM_DESCRIPTOR              (0x3)
/** Hierarchy */
#define MPEG2_HIERARCHY_DESCRIPTOR                 (0x4)
/** Registration */
#define MPEG2_REGISTRATION_DESCRIPTOR              (0x5)
/** Data Stream Alignment */
#define MPEG2_DATA_STREAM_ALIGNMENT_DESCRIPTOR     (0x6)
/** Target Background Grid */
#define MPEG2_TBG_DESCRIPTOR                       (0x7)
/** Video Window */
#define MPEG2_VIDEO_WINDOW_DESCRIPTOR              (0x8)
/** CA */
#define MPEG2_CA_DESCRIPTOR                        (0x9)
/** ISO 639 LANG */
#define MPEG2_ISO_639_LANG_DESCRIPTOR              (0xA)
/** System Clock */
#define MPEG2_SYSTEM_CLOCK_DESCRIPTOR              (0xB)
/** Multiplex Buffer Utilization */
#define MPEG2_MBU_DESCRIPTOR                       (0xC)
/** Copyright */
#define MPEG2_COPYRIGHT_DESCRIPTOR                 (0xD)
/** Max Bitrate */
#define MPEG2_MAX_BITRATE_DESCRIPTOR               (0xE)
/** Private Indicator */
#define MPEG2_PRIVATE_INDICATOR_DESCRIPTOR         (0xF)

/** 
 * @}
 */

#endif ///__MPEG2_PUBLIC_H__
