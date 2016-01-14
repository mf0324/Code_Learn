/**
 * @file mpeg2_dvb_parser.h   MPEG2 DVB Parser
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.11.15
 *
 *
 */


#ifndef __MPEG2_DVB_PARSER_H__
#define __MPEG2_DVB_PARSER_H__

#include <public/gen_int.h>
#include "mpeg2_dvb_public.h"
#include "mpeg2_ts_pack.h"

/** service descriptor */
typedef struct tag_dvb_service_descriptor
{
	/** type */
	int32  type;

	/** provider name */
	int8*  p_provider_name;
	/** provider name size */
	int32  provider_name_size;

	/** service name */
	int8*  p_service_name;
	/** service name size */
	int32  service_name_size;

} DVB_SERVICE_DESCRIPTOR, *PDVB_SERVICE_DESCRIPTOR;

/** SDS Section */
typedef struct tag_sds_section
{
	/** service id */
	int32  service_id;
	/** eit schedule flag */
	int32  eit_schedule_flag;
	/** eit present following flag */
	int32  eit_present_flag;
	/** running status */
	int32  running_status;

	/** descriptor data */
	uint8* p_descriptor_data;
	/** descriptor data size */
	int32  descriptor_data_size;

} SDS_SECTION, *PSDS_SECTION;

/** dvb sdt */
typedef struct tag_mpeg2_dvb_sdt
{
	/** table id */
	int32 table_id;

	union
	{
		/** data */
		uint8* p_data;

		/** sds section */
		SDS_SECTION* p_sds_section;

	} buf;

	union
	{
		/** data size, unit: byte */
		int32  data_size;

		/** sds section num, unit: number */
		int32  sds_section_num;

	} size;

} MPEG2_DVB_SDT, *PMPEG2_DVB_SDT;

/** SIT Section */
typedef struct tag_sit_section
{
	/** service id */
	int32  service_id;
	/** running status */
	int32  running_status;

	/** descriptor data */
	uint8* p_descriptor_data;
	/** descriptor data size */
	int32  descriptor_data_size;

} SIT_SECTION, *PSIT_SECTION;

/** dvb sit */
typedef struct tag_mpeg2_dvb_sit
{
	/** table id */
	int32 table_id;

	/** descriptor */
	uint8* p_descriptor;
	/** descriptor size */
	int32  descriptor_size;

	/** sit section */
	SIT_SECTION* p_section;
	/** sit section num, unit: number */
	int32  section_num;

} MPEG2_DVB_SIT, *PMPEG2_DVB_SIT;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 解析SDT
 * @param [in] p_pack TS句柄
 * @param [in/out] p_sdt SDT句柄
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入缓冲区长度不足
 * -3：数据错误
 * @remark
 * 1、调用时由外部填写p_sdt的buf.p_data和size.data_size字段，如果输入缓冲区长度不足，返回时size.data_size字段表示实际所需长度
 * 2、返回时p_sdt的table_id字段决定了data和size的哪些成员为有效
 */
int32 mpeg2_dvb_demux_sdt(MPEG2_TS_PACK *p_pack, MPEG2_DVB_SDT *p_sdt);

/**
 * @brief 解析SIT
 * @param [in] p_pack TS句柄
 * @param [in/out] p_sit SIT句柄
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入缓冲区长度不足
 * -3：数据错误
 * @remark
 * 1、调用时由外部填写p_sit的section_num字段，如果输入section数量不足，返回时section_num字段表示实际所需数量
 */
int32 mpeg2_dvb_demux_sit(MPEG2_TS_PACK *p_pack, MPEG2_DVB_SIT *p_sit);

/**
 * @brief 解析service descriptor
 * @param [out] p_descriptor descriptor句柄
 * @param [in] p_buf 输入数据地址
 * @param [in] len 数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入缓冲区长度不足
 * -3：数据错误
 * @remark
 * 1、输入数据包含descriptor tag和length
 */
int32 mpeg2_dvb_demux_service_descriptor(DVB_SERVICE_DESCRIPTOR *p_descriptor, uint8 *p_buf, int32 len);


#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_DVB_PARSER_H__
