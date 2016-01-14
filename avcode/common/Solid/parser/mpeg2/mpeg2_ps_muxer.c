
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_writer.h>
#include "mpeg2_public.h"
#include "mpeg2_ps_muxer.h"

#include <util/log_debug.h>

/**  
 * @brief 生成PS首部
 *
 * @param [in] p_param 打包参数句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
static int32 mpeg2_build_ps_header(MPEG2_PS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	BIT_STREAM_WRITER bs_writer;

	int32  left_size;
	int32  need_size;

	int32  i;
	int32  ret;

	ret = mpeg2_calc_ps_header_size(p_param, &need_size);
	if( ret )
	{
		return ret;
	}

	left_size = *p_buf_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	bs_writer_init(&bs_writer, p_buf, left_size, GEN_BYTE_BIT);

	/** start_code, 32 bit */
	p_buf[0] = 0x00;
	p_buf[1] = 0x00;
	p_buf[2] = 0x01;
	p_buf[3] = MPEG2_PS_START_CODE;

	bs_writer_skip_bits(&bs_writer, 32);

	/** '01', 2 bit */
	bs_write_bits(&bs_writer, 0x1, 2);

	/** sys_clock_ref_base[32..30], 3 bit */
	bs_write_bits(&bs_writer, p_param->sys_clock_ref_base_high, 1);
	bs_write_bits(&bs_writer, p_param->sys_clock_ref_base_low >> 30, 2);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);
	/** sys_clock_ref_base[29..15], 15 bit */
	bs_write_bits(&bs_writer, p_param->sys_clock_ref_base_low >> 15, 15);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);
	/** sys_clock_ref_base[14..0], 15 bit */
	bs_write_bits(&bs_writer, p_param->sys_clock_ref_base_low, 15);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** sys_clock_ref_ext, 9 bit */
	bs_write_bits(&bs_writer, p_param->sys_clock_ref_ext, 9);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** program_mux_rate, 22 bit */
	bs_write_bits(&bs_writer, p_param->program_mux_rate, 22);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);
	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** resv, 5 bit */
	bs_write_bits(&bs_writer, 0x1F, 5);

	/** pack_stuffing_length, 3 bit */
	bs_write_bits(&bs_writer, p_param->pack_stuff_len, 3);
	if( p_param->pack_stuff_len > 0 )
	{
		/** stuff_byte(0xFF) */
		for( i=0; i<p_param->pack_stuff_len; i++ )
		{
			bs_write_bits(&bs_writer, 0xFF, 8);
		}
	}

	*p_buf_size = need_size;

	return 0;
}

/**  
 * @brief 生成系统首部
 *
 * @param [in] p_param 系统首部参数
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_size 数据长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_size返回
 */
static int32 mpeg2_build_sys_header(MPEG2_SYS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	BIT_STREAM_WRITER bs_writer;

	int32  left_size;
	int32  need_size;
	int32  header_len;

	int32  i;
	int32  ret;

	ret = mpeg2_calc_sys_header_size(p_param, &need_size);

	left_size = *p_buf_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return -2;
	}

	bs_writer_init(&bs_writer, p_buf, left_size, GEN_BYTE_BIT);

	/** start_code, 32 bit */
	p_buf[0] = 0x00;
	p_buf[1] = 0x00;
	p_buf[2] = 0x01;
	p_buf[3] = MPEG2_SYSTEM_HEADER_START_CODE;

	bs_writer_skip_bits(&bs_writer, 32);

	/** header_length, 16 bit */
	header_len = need_size - MPEG2_PES_HEADER_MIN_SIZE;
	bs_write_bits(&bs_writer, header_len, 16);

	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** rate_bound, 22 bit */
	bs_write_bits(&bs_writer, p_param->rate_bound, 22);

	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** audio_bound, 6 bit */
	bs_write_bits(&bs_writer, p_param->audio_bound, 6);

	/** fixed_flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_fixed, 1);

	/** csps_flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_csps, 1);

	/** sys_audio_lock_flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_sys_audio_lock, 1);

	/** sys_video_lock_flag, 1 bit */
	bs_write_bits(&bs_writer, p_param->b_sys_video_lock, 1);

	/** marker bit, 1 bit */
	bs_write_bits(&bs_writer, 0x1, 1);

	/** video_bound, 5 bit */
	bs_write_bits(&bs_writer, p_param->video_bound, 5);

	/** resv, 8 bit */
	bs_write_bits(&bs_writer, 0xFF, 8);

	if( p_param->p_stream )
	{
		for( i=0; i<p_param->stream_num; i++ )
		{
			/** stream_id, 8 bit */
			bs_write_bits(&bs_writer, p_param->p_stream[i].stream_id, 8);
			/** '11', 2 bit */
			bs_write_bits(&bs_writer, 0x3, 2);
			/** pstd_buf_bound_scale, 1 bit */
			bs_write_bits(&bs_writer, p_param->p_stream[i].pstd_buf_bound_scale, 1);
			/** pstd_buf_size_bound, 13 bit */
			bs_write_bits(&bs_writer, p_param->p_stream[i].pstd_buf_size_bound, 13);
		}
	}

	*p_buf_size = need_size;

	return 0;
}

/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg2_calc_ps_header_size(MPEG2_PS_HEADER_PARAM *p_param, int32 *p_header_size)
{
	int32  need_size;

	if( p_param == NULL || p_header_size == NULL )
	{
		return -1;
	}

	need_size = MPEG2_PS_HEADER_SIZE;

	if( p_param->pack_stuff_len > MPEG2_PS_HEADER_MAX_STUFF_SIZE )
	{
		return -3;
	}
	need_size += p_param->pack_stuff_len;

	*p_header_size = need_size;

	return 0;
}

int32 mpeg2_ps_header_mux(MPEG2_PS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	int32 ret;

	if( p_param == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg2_build_ps_header(p_param, p_buf, p_buf_size);

	return ret;
}

int32 mpeg2_calc_sys_header_size(MPEG2_SYS_HEADER_PARAM *p_param, int32 *p_header_size)
{
	int32  need_size;

	if( p_param == NULL || p_header_size == NULL )
	{
		return -1;
	}

	need_size = MPEG2_SYS_HEADER_SIZE;

	if( p_param->p_stream )
	{
		need_size += (p_param->stream_num * MPEG2_SYS_STREAM_ID_INFO_SIZE);
	}

	*p_header_size = need_size;

	return 0;
}

int32 mpeg2_sys_header_mux(MPEG2_SYS_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_size)
{
	int32 ret;

	if( p_param == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg2_build_sys_header(p_param, p_buf, p_buf_size);

	return ret;
}

int32 mpeg2_ps_end_mux(uint8 *p_buf, int32 *p_buf_size)
{
	if( p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	if( *p_buf_size < MPEG2_PS_START_CODE_SIZE )
	{
		*p_buf_size = MPEG2_PS_START_CODE_SIZE;
		return -2;
	}

	p_buf[0] = 0x00;
	p_buf[1] = 0x00;
	p_buf[2] = 0x00;
	p_buf[3] = MPEG2_PS_END_CODE;

	*p_buf_size = MPEG2_PS_START_CODE_SIZE;

	return 0;
}
