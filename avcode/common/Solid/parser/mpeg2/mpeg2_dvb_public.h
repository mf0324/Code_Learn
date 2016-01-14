/**
 * @file mpeg2_dvb_public.h   MPEG2 DVB
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.11.12
 *
 *
 */


#ifndef __MPEG2_DVB_PUBLIC_H__
#define __MPEG2_DVB_PUBLIC_H__

/** 
 * @name DVB PSI
 * @{
 */

/** NIT PID */
#define MPEG2_DVB_NIT_PID                                  (0x0010)
/** SDT PID */
#define MPEG2_DVB_SDT_PID                                  (0x0011)
/** EIT PID */
#define MPEG2_DVB_EIT_PID                                  (0x0012)
/** RST PID */
#define MPEG2_DVB_RST_PID                                  (0x0013)
/** TDT PID */
#define MPEG2_DVB_TDT_PID                                  (0x0014)
/** Network Synchronization PID */
#define MPEG2_DVB_NETWORK_SYNC_PID                         (0x0015)
/** DIT PID */
#define MPEG2_DVB_DIT_PID                                  (0x001E)
/** SIT PID */
#define MPEG2_DVB_SIT_PID                                  (0x001F)


/** network information section(actual network) */
#define MPEG2_DVB_NIS_ACTUAL_TABLE_ID                      (0x40)
/** network information section(other network) */
#define MPEG2_DVB_NIS_OTHER_TABLE_ID                       (0x41)
/** service description section(actual ts) */
#define MPEG2_DVB_SDS_ACTUAL_TABLE_ID                      (0x42)
/** service description section(other ts) */
#define MPEG2_DVB_SDS_OTHER_TABLE_ID                       (0x46)
/** bouquet association section */
#define MPEG2_DVB_BAS_TABLE_ID                             (0x4A)
/** time date section */
#define MPEG2_DVB_TDS_TABLE_ID                             (0x70)
/** selection information section */
#define MPEG2_DVB_SIT_TABLE_ID                             (0x7F)

/** sds min size(include crc32) */
#define MPEG2_DVB_SDS_MIN_SIZE                             (15)
/** sub sds min size */
#define MPEG2_DVB_SUB_SDS_MIN_SIZE                         (5)

/** sit min size(include crc32) */
#define MPEG2_DVB_SIT_MIN_SIZE                             (14)
/** sub sis min size */
#define MPEG2_DVB_SUB_SIS_MIN_SIZE                         (4)

/** service descriptor */
#define MPEG2_DVB_SERVICE_DESCRIPTOR_TAG                   (0x48)
/** service descriptor min size */
#define MPEG2_DVB_SERVICE_DESCRIPTOR_MIN_SIZE              (5)


/** 
 * @}
 */




#endif ///__MPEG2_DVB_PUBLIC_H__
