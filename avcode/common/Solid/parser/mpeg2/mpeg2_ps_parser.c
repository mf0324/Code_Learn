
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include <public/gen_error.h>
#include <util/log_agent.h>

#include "mpeg2_ps_parser.h"

/**  
 * @brief demux pack header
 * @param [in] p_pack, pack
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
static int32 mpeg2_ps_demux_pack_header(MPEG2_PS_PACK* p_pack, uint8* p_buf, int32* p_buf_size)
{
	BIT_STREAM_READER  bs;
	int32 value;

	int32  left_len = *p_buf_size;

	int32  pack_stuff_len;

	int32  ret;

	*p_buf_size = 0;

	if( left_len < MPEG2_PS_HEADER_SIZE )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	bs_reader_init(&bs, p_buf, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** '01', 2 bit */
	bs_reader_skip_bits(&bs, 2);

	/** sys_clock_ref_base, 33 bit */
	p_pack->pack_header.sys_clock_ref_base = 0;
	/** [32..30] */
	bs_read_bits(&bs, &value, 3);
	p_pack->pack_header.sys_clock_ref_base = (p_pack->pack_header.sys_clock_ref_base | value) << 30;
	bs_reader_skip_bits(&bs, 1);

	/** [29..15] */
	bs_read_bits(&bs, &value, 15);
	p_pack->pack_header.sys_clock_ref_base |= (value << 15);
	bs_reader_skip_bits(&bs, 1);

	/** [14..0] */
	bs_read_bits(&bs, &value, 15);
	p_pack->pack_header.sys_clock_ref_base |= value;
	bs_reader_skip_bits(&bs, 1);

	/** sys_clock_ref_ext, 9 bit */
	ret = bs_read_bits(&bs, &p_pack->pack_header.sys_clock_ref_ext, 9);
	bs_reader_skip_bits(&bs, 1);

	/** program_mux_rate, 22 bit */
	ret = bs_read_bits(&bs, &p_pack->pack_header.program_mux_rate, 22);
	/** marker_bit+resv, 1+1+5 bit */
	bs_reader_skip_bits(&bs, 7);

	/** pack_stuff_len, 3 bit */
	bs_read_bits(&bs, &pack_stuff_len, 3);

	if( pack_stuff_len > 0 )
	{
		ret = bs_reader_skip_bits(&bs, pack_stuff_len * GEN_BYTE_BIT);
		if( ret )
		{
			return GEN_E_NEED_MORE_DATA;
		}
	}

	bs_reader_get_use_bytes(&bs, p_buf_size);


	return GEN_S_OK;
}

/**  
 * @brief demux sys header
 * @param [in] p_pack, pack
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
static int32 mpeg2_ps_demux_sys_header(MPEG2_PS_PACK* p_pack, uint8* p_buf, int32* p_buf_size)
{
	BIT_STREAM_READER  bs;

	int32  left_len = *p_buf_size;

	int32  header_len;
	int32  header_left_len;

	int32 ret;

	*p_buf_size = 0;

	if( left_len < MPEG2_SYS_HEADER_SIZE )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	bs_reader_init(&bs, p_buf, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** header_len, 16 bit */
	bs_read_bits(&bs, &header_len, 16);
	ret = bs_reader_need_bits(&bs, header_len * GEN_BYTE_BIT);
	if( ret )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	/** marker_bit, 1 bit */
	bs_reader_skip_bits(&bs, 1);

	/** rate_bound, 22 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.rate_bound, 22);

	/** marker_bit, 1 bit */
	bs_reader_skip_bits(&bs, 1);

	/** audio_bound, 6 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.audio_bound, 6);

	/** fixed_flag, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.b_fix_rate, 1);

	/** csps_flag, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.b_csps, 1);

	/** system_audio_lock_flag, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.b_sys_audio_lock, 1);

	/** system_video_lock_flag, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.b_sys_video_lock, 1);

	/** marker_bit, 1 bit */
	bs_reader_skip_bits(&bs, 1);

	/** video_bound, 5 bit */
	ret = bs_read_bits(&bs, &p_pack->sys_header.video_bound, 5);

	/** resv, 8 bit */
	bs_reader_skip_bits(&bs, 8);

	header_left_len = header_len - 6;
	p_pack->sys_header.stream_id_len = header_left_len;
	if( header_left_len > 0 )
	{
		p_pack->sys_header.p_stream_id = p_buf + MPEG2_SYS_HEADER_SIZE;
		p_pack->sys_header.stream_id_num = header_left_len / MPEG2_SYS_STREAM_ID_INFO_SIZE;

	}else
	{
		p_pack->sys_header.p_stream_id = NULL;
		p_pack->sys_header.stream_id_num = 0;
	}

	*p_buf_size = header_len + 6;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg2_ps_demux(MPEG2_PS_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_demux_size)
{
	int32  left_size;
	int32  total_left_size = buf_size;
	//int32  unparse_size;
	int32  parse_size;
	int32  total_parse_size;

	int32  b_find;
	int32  b_continue;

	int32  i;
	int32  ret;

	if( p_pack == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	b_find = 0;
	b_continue = 1;
	total_parse_size = 0;
	i = 0;
	while( b_continue && total_left_size > MPEG2_PS_START_CODE_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			total_parse_size++;
			total_left_size--;
			continue;
		}

		p_pack->p_start = p_buf + i;
		p_pack->start_code = p_buf[i + 3];

		switch( p_buf[i + 3] )
		{
		case MPEG2_PS_START_CODE:
			{
				parse_size = total_left_size;
				ret = mpeg2_ps_demux_pack_header(p_pack, p_buf + i, &parse_size);
				i += parse_size;
				total_parse_size += parse_size;
				total_left_size -= parse_size;

				if( ret == 0 )
				{
					b_find = 1;
				}

				b_continue = 0;
			}
			break;

		case MPEG2_SYSTEM_HEADER_START_CODE:
			{
				parse_size = total_left_size;
				ret = mpeg2_ps_demux_sys_header(p_pack, p_buf + i, &parse_size);
				i += parse_size;
				total_parse_size += parse_size;
				total_left_size -= parse_size;

				if( ret == 0 )
				{
					b_find = 1;
				}

				b_continue = 0;
			}
			break;

		case MPEG2_PS_MAP_START_CODE:
			{
				parse_size = total_left_size;
				ret = mpeg2_pes_demux_ps_map(&p_pack->ps_map, p_buf + i, &parse_size);
				i += parse_size;
				total_parse_size += parse_size;
				total_left_size -= parse_size;

				if( ret == 0 )
				{
					b_find = 1;
				}

				b_continue = 0;
			}
			break;

		default:
			{
				if( p_buf[i+3] >= MPEG2_PES_SID_START )
				{
					/** pes header */
					parse_size = 0;
					left_size = total_left_size;
					ret = mpeg2_pes_find_pes_header(&p_pack->pes_pack, p_buf + i, left_size, &parse_size);
					i += parse_size;
					total_parse_size += parse_size;
					total_left_size -= parse_size;

					if( ret == 0 )
					{
						/** check left size */
						if( total_left_size >= (p_pack->pes_pack.pes_pack_len + MPEG2_PES_HEADER_MIN_SIZE) )
						{
							parse_size = total_left_size;
							ret = mpeg2_pes_demux_pes_header(&p_pack->pes_header, p_buf + i, &parse_size);
							i += (p_pack->pes_pack.pes_pack_len + MPEG2_PES_HEADER_MIN_SIZE);
							total_parse_size += (p_pack->pes_pack.pes_pack_len + MPEG2_PES_HEADER_MIN_SIZE);
							total_left_size -= (p_pack->pes_pack.pes_pack_len + MPEG2_PES_HEADER_MIN_SIZE);

							b_find = 1;

						}else
						{
							/** need more data */
						}
					}

					b_continue = 0;

				}else
				{
					i += MPEG2_PS_START_CODE_SIZE;
					total_parse_size += MPEG2_PS_START_CODE_SIZE;
					total_left_size -= MPEG2_PS_START_CODE_SIZE;
				}
			}
		}

	}

	*p_demux_size = total_parse_size;

	if( b_find )
	{
		return GEN_S_OK;

	}else
	{
		return GEN_E_NEED_MORE_DATA;
	}
}

int32 mpeg2_ps_demux_stream_info(MPEG2_PS_STREAM* p_stream, int32* p_stream_num, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	uint8* p_uint8 = NULL;
	int32  left_size;
	int32  total_demux_size;

	int32  stream_id_num;
	int32  stream_id_count;

	int32  i;

	if( p_data == NULL || p_stream == NULL || p_stream_num == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	stream_id_num = *p_stream_num;

	p_uint8 = p_data;
	left_size = data_size;
	total_demux_size = 0;
	stream_id_count = 0;
	i = 0;

	while( left_size >= MPEG2_SYS_STREAM_ID_INFO_SIZE )
	{
		if( !(p_uint8[0] & 0x80) )
		{
			/** nextbits != '1' */
			break;
		}

		stream_id_count++;

		if( i >= stream_id_num )
		{
			break;
		}

		p_stream[i].stream_id = p_uint8[0];
		p_stream[i].std_buf_bound_scale = (p_uint8[1] >> 5) & 0x1;
		p_stream[i].std_buf_size_bound = p_uint8[1] & 0x1F;
		p_stream[i].std_buf_size_bound = (p_stream[i].std_buf_size_bound << 8) | p_uint8[2];

		i++;
		p_uint8 += MPEG2_SYS_STREAM_ID_INFO_SIZE;
		left_size -= MPEG2_SYS_STREAM_ID_INFO_SIZE;
		total_demux_size += MPEG2_SYS_STREAM_ID_INFO_SIZE;
	}

	if( left_size > 0 && (p_uint8[0] & 0x80) )
	{
		/** maybe more stream id */
		return GEN_E_NEED_MORE_DATA;
	}

	*p_demux_size = total_demux_size;

	if( i < stream_id_count )
	{
		/** need more buf */
		*p_stream_num = stream_id_count;
		return GEN_E_NEED_MORE_BUF;

	}else
	{
		*p_stream_num = i;
		return GEN_S_OK;
	}
}
