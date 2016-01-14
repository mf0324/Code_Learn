
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/mpeg2_crc32.h>
#include <public/bit_stream_writer.h>
#include "mpeg2_public.h"
#include "mpeg2_dvb_public.h"
#include "mpeg2_dvb_muxer.h"

#include <util/log_debug.h>


/**  
 * @brief 生成SIT字段
 * @param [in] p_param 参数句柄
 * @param [in] p_buf 数据缓冲地址
 * @param [in/out] p_buf_size 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
static int32 mpeg2_dvb_build_sit(MPEG2_DVB_SIT_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	BIT_STREAM_WRITER bs_writer;

	int32  left_size;
	int32  need_size;
	int32  write_size;
	int32  total_section_size;

	int32  section_len;
	uint32 crc32_value;

	int32  i;
	int32  ret;

	total_section_size = 0;
	for( i = 0; i < p_param->section_num; i++ )
	{
		total_section_size += MPEG2_DVB_SUB_SIS_MIN_SIZE;
		total_section_size += p_param->p_section[i].service_loop_len;
	}

	left_size = *p_buf_size;
	need_size = MPEG2_PSI_POINTER_SIZE + MPEG2_DVB_SIT_MIN_SIZE + p_param->trans_info_loop_len + total_section_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	bs_writer_init(&bs_writer, p_buf, left_size, GEN_BYTE_BIT);

	/** pointer_field, 8 bit */
	p_buf[0] = 0;
	bs_writer_skip_bits(&bs_writer, 8);

	/** table_id, 8 bit */
	p_buf[1] = MPEG2_DVB_SIT_TABLE_ID;
	bs_writer_skip_bits(&bs_writer, 8);

	/** section_syntax_indicator, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** dvb_reserved_future_use, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** iso_reserved, 2 bit */
	bs_write_bits(&bs_writer, 0x3, 2);

	/** section length, 12 bit */
	section_len = need_size - (MPEG2_PSI_POINTER_SIZE + 1 + 2);
	if( section_len > 1024 )
	{
		return -3;
	}
	bs_write_bits(&bs_writer, section_len, 12);

	/** dvb_reserved_future_use, 16 bit */
	bs_write_bits(&bs_writer, 0xFFFF, 16);

	/** iso_reserved, 2 bit */
	bs_write_bits(&bs_writer, 0x3, 2);

	/** version_number, 5 bit */
	bs_write_bits(&bs_writer, p_param->version_number, 5);

	/** current_next_indicator, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_current_next, 1);

	/** section_number, 8 bit */
	bs_write_bits(&bs_writer, p_param->section_number, 8);

	/** last_section_number, 8 bit */
	bs_write_bits(&bs_writer, p_param->last_section_number, 8);

	/** dvb_reserved_future_use, 4 bit */
	bs_write_bits(&bs_writer, 0xF, 4);

	/** transmission_info_loop_length, 12 bit */
	if( p_param->trans_info_loop_len > 1024 )
	{
		return -3;
	}
	bs_write_bits(&bs_writer, p_param->trans_info_loop_len, 12);

	if( p_param->trans_info_loop_len > 0 )
	{
		/** descriptor */
		if( p_param->p_descriptor == NULL )
		{
			return -3;
		}

		bs_writer_get_use_bytes(&bs_writer, &write_size);
		memcpy(p_buf + write_size, p_param->p_descriptor, p_param->trans_info_loop_len);
		bs_writer_skip_bits(&bs_writer, p_param->trans_info_loop_len * GEN_BYTE_BIT);
	}

	/** section */
	for( i = 0; i < p_param->section_num; i++ )
	{
		/** service_id, 16 bit */
		bs_write_bits(&bs_writer, p_param->p_section[i].service_id, 16);
		/** dvb_reserved_future_use, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** running_status, 3 bit */
		bs_write_bits(&bs_writer, p_param->p_section[i].running_status, 3);
		/** es_info_length, 12 bit */
		bs_write_bits(&bs_writer, p_param->p_section[i].service_loop_len, 12);

		if( p_param->p_section[i].service_loop_len > 0 )
		{
			if( p_param->p_section[i].p_descriptor == NULL )
			{
				return -3;
			}

			bs_writer_get_use_bytes(&bs_writer, &write_size);
			memcpy(p_buf + write_size, p_param->p_section[i].p_descriptor, p_param->p_section[i].service_loop_len);
			bs_writer_skip_bits(&bs_writer, p_param->p_section[i].service_loop_len * GEN_BYTE_BIT);
		}
	}

	/** crc, 32 bit */
	crc32_value = mpeg2_crc32(&p_buf[1], need_size - (MPEG2_PSI_POINTER_SIZE + 4));
	bs_write_bits(&bs_writer, crc32_value, 32);

	*p_buf_size = need_size;

	return 0;
}


/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg2_dvb_calc_sit_size(MPEG2_DVB_SIT_PARAM *p_param, int32 *p_buf_size)
{
	int32  need_size;
	int32  total_section_size;

	int32  i;

	if( p_param == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( p_param->trans_info_loop_len > 1024 )
	{
		return -3;
	}

	total_section_size = 0;
	for( i = 0; i < p_param->section_num; i++ )
	{
		total_section_size += MPEG2_DVB_SUB_SIS_MIN_SIZE;
		total_section_size += p_param->p_section[i].service_loop_len;
	}

	need_size = MPEG2_PSI_POINTER_SIZE + MPEG2_DVB_SIT_MIN_SIZE + p_param->trans_info_loop_len + total_section_size;

	*p_buf_size = need_size;

	return 0;
}

int32 mpeg2_dvb_mux_sit(MPEG2_DVB_SIT_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	int32 ret;

	if( p_param == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg2_dvb_build_sit(p_param, p_buf, p_buf_size);

	return ret;
}
