
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_writer.h>
#include "mpeg2_public.h"
#include "mpeg2_pes_muxer.h"

#include <util/log_debug.h>


/**  
 * @brief 生成PES包头
 *
 * @param [in] p_param 打包参数句柄
 * @param [in] p_buf 数据缓冲地址
 * @param [in/out] p_buf_len 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_len返回
 */
static int32 mpeg2_build_pes_header(MPEG2_PES_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_len)
{
	BIT_STREAM_WRITER bs_writer;

	int32 left_size;
	int32 need_size;

	int32 ret;

	left_size = *p_buf_len;
	ret = mpeg2_calc_pes_header_size(p_param, &need_size);
	if( left_size < need_size )
	{
		*p_buf_len = need_size;
		return -2;
	}

	bs_writer_init(&bs_writer, p_buf, left_size, GEN_BYTE_BIT);

	/** start_code+stream_id, 24+8 bit */
	p_buf[0] = 0x00;
	p_buf[1] = 0x00;
	p_buf[2] = 0x01;
	p_buf[3] = p_param->stream_id;

	bs_writer_skip_bits(&bs_writer, 32);

	/** pes_packet_length, 16 bit */
	bs_write_bits(&bs_writer, p_param->pack_len, 16);

	/** '10', 2 bit */
	bs_write_bits(&bs_writer, 0x2, 2);

	/** scramble control, 2 bit */
	bs_write_bits(&bs_writer, p_param->scramble_control, 2);

	/** pes priority, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_priority, 1);

	/** data alignment indicator, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_data_align, 1);

	/** copyright, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_copyright, 1);

	/** original or copy, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_original, 1);

	/** pts_dts_flag, 2 bit */
	bs_write_bits(&bs_writer, p_param->pts_dts_flag, 2);

	/** escr flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_escr, 1);

	/** es rate flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_es_rate, 1);

	/** dsm trick mode flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_dsm_trick_mode, 1);

	/** additional copy info flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_copy_info, 1);

	/** pes crc flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_crc, 1);

	/** extension flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_ext, 1);

	/** pes header data length, 8 bit */
	bs_write_bits(&bs_writer, p_param->header_data_len, 8);

	if( p_param->pts_dts_flag == 0x2 )
	{
		/** '0010', 4 bit */
		bs_write_bits(&bs_writer, 0x2, 4);
		/** pts[32..30], 3 bit */
		bs_write_bits(&bs_writer, p_param->pts_high, 1);
		bs_write_bits(&bs_writer, p_param->pts_low >> 30, 2);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** pts[29..15], 15 bit */
		bs_write_bits(&bs_writer, p_param->pts_low >> 15, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** pts[14..0], 15 bit */
		bs_write_bits(&bs_writer, p_param->pts_low, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);

	}else if( p_param->pts_dts_flag == 0x3 )
	{
		/** '0011', 4 bit */
		bs_write_bits(&bs_writer, 0x3, 4);
		/** pts[32..30], 3 bit */
		bs_write_bits(&bs_writer, p_param->pts_high, 1);
		bs_write_bits(&bs_writer, p_param->pts_low >> 30, 2);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** pts[29..15], 15 bit */
		bs_write_bits(&bs_writer, p_param->pts_low >> 15, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** pts[14..0], 15 bit */
		bs_write_bits(&bs_writer, p_param->pts_low, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);

		/** '0001', 4 bit */
		bs_write_bits(&bs_writer, 0x1, 4);
		/** dts[32..30], 3 bit */
		bs_write_bits(&bs_writer, p_param->dts_high, 1);
		bs_write_bits(&bs_writer, p_param->dts_low >> 30, 2);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** dts[29..15], 15 bit */
		bs_write_bits(&bs_writer, p_param->dts_low >> 15, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
		/** dts[14..0], 15 bit */
		bs_write_bits(&bs_writer, p_param->dts_low, 15);
		/** marker bit, 1 bit */
		bs_write_bits(&bs_writer, 0x1, 1);
	}

	*p_buf_len = need_size;

	return 0;
}

/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg2_calc_pes_header_data_size(MPEG2_PES_HEADER_PARAM *p_param, int32 *p_header_data_size)
{
	int32 need_bits = 0;

	if( p_param == NULL || p_header_data_size == NULL )
	{
		return -1;
	}

	if( p_param->pts_dts_flag == 0x2 )
	{
		/** pts, 40 bit */
		need_bits += 40;

	}else if( p_param->pts_dts_flag == 0x3 )
	{
		/** pts+dts, 40+40 bit */
		need_bits += (40+40);
	}

	if( p_param->b_escr )
	{
		/** escr, 48 bit */
		need_bits += 48;
	}

	if( p_param->b_es_rate )
	{
		/** es_rate, 24 bit */
		need_bits += 24;
	}

	*p_header_data_size = need_bits / GEN_BYTE_BIT;
	if( need_bits % GEN_BYTE_BIT != 0 )
	{
		(*p_header_data_size)++;
	}

	return 0;
}

int32 mpeg2_calc_pes_header_size(MPEG2_PES_HEADER_PARAM *p_param, int32 *p_header_size)
{
	int32 need_bits = 0;
	int32 total_size;

	int32 ret;

	if( p_param == NULL || p_header_size == NULL )
	{
		return -1;
	}

	/** start_code+stream_id+pack_len, 24+8+16 bit */
	need_bits += (24+8+16);

	/** misc_flag+header_data_len, 16+8 bit */
	need_bits += (16+8);

	total_size = need_bits / GEN_BYTE_BIT;
	total_size += p_param->header_data_len;

	*p_header_size = total_size;

	return 0;
}

int32 mpeg2_pes_mux(MPEG2_PES_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_len)
{
	int32 ret;

	if( p_param == NULL || p_buf == NULL || p_buf_len == NULL )
	{
		return -1;
	}

	ret = mpeg2_build_pes_header(p_param, p_buf, p_buf_len);

	return ret;
}

int32 mpeg2_pes_padding_mux(uint8 *p_buf, int32 buf_len)
{
	int32 left_size = buf_len;
	int32 ret;

	if( p_buf == NULL )
	{
		return -1;
	}

	if( buf_len < MPEG2_PES_HEADER_MIN_SIZE || buf_len > (MPEG2_PES_HEADER_MIN_SIZE + MPEG2_PES_MAX_PACK_SIZE) )
	{
		return -1;
	}

	p_buf[0] = 0x00;
	p_buf[1] = 0x00;
	p_buf[2] = 0x01;
	p_buf[3] = MPEG2_PES_SID_PADDING_STREAM;

	left_size -= MPEG2_PES_HEADER_MIN_SIZE;

	p_buf[4] = (left_size >> 8) & 0xFF;
	p_buf[5] = left_size & 0xFF;

	if( left_size > 0 )
	{
		memset(p_buf + MPEG2_PES_HEADER_MIN_SIZE, 0xFF, left_size);
	}

	return 0;
}
