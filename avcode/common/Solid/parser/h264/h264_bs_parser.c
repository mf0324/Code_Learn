
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "h264_bs_parser.h"

#include <util/log_debug.h>


#define H264_ON_ERR_RET(ERR)                       {if( ERR ) return ERR;}

/**
 * @brief 解析H264 SEI
 * @param [in] p_pack BS句柄
 * @param [in] p_sps SEI数据地址
 * @param [in/out] p_sei_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足
 * -3：数据格式错误
 */
static int32 h264_demux_sei(H264_BS_PACK *p_pack, uint8 *p_sei, int32 *p_sei_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_sei_len;

	int32 payload_type;
	int32 payload_size;

	int32 ret;

	*p_sei_len = 0;

	bs_reader_init(&bs, p_sei, left_len, GEN_BYTE_BIT);

	bs_reader_skip_bits(&bs, GEN_BYTE_BIT * H264_NALU_HEADER_SIZE);

	payload_type = 0;
	while( 1 )
	{
		/** ff_byte, 8 bit */
		ret = bs_read_bits(&bs, &value, 8);
		H264_ON_ERR_RET(ret);

		if( value != 0xFF )
		{
			break;
		}

		payload_type += 255;
	}
	/** last_payload_type_byte, 8 bit */
	//ret = bs_read_bits(&bs, &value, 8);
	//H264_ON_ERR_RET(ret);

	payload_type += value;

	payload_size = 0;
	while( 1 )
	{
		/** ff_byte, 8 bit */
		ret = bs_read_bits(&bs, &value, 8);
		H264_ON_ERR_RET(ret);

		if( value != 0xFF )
		{
			break;
		}

		payload_size += 255;
	}
	/** last_payload_size_byte, 8 bit */
	//ret = bs_read_bits(&bs, &value, 8);
	//H264_ON_ERR_RET(ret);

	payload_size += value;

	switch( payload_type )
	{
	case H264_SEI_HISI_VIDEO_INFO:
		{
			/** hisi */
			//ret = bit_stream_need_bits(&bs, 8);
			//H264_ON_ERR_RET(ret);

			/** 根据海思的说法，SEI序列为e0 01 01，其中第三个字节表示帧场模式 */
			//ret = bs_reader_skip_bits(&bs, 8);
			ret = bs_read_bits(&bs, &value, 8);
			H264_ON_ERR_RET(ret);

			if( (value & 0x1) == 0 )
			{
				/** 帧模式 */
				p_pack->sei.field_mode = H264_FRAME_MODE;
				//log_trace(LOG_NORMAL, _T("[h264_bs_parser::h264_demux_sei] frame mode\n"));
			}else
			{
				/** 场模式 */
				p_pack->sei.field_mode = H264_FIELD_MODE;
				//log_trace(LOG_NORMAL, _T("[h264_bs_parser::h264_demux_sei] field mode\n"));
			}
		}
		break;

	default:
		break;
	}

	bs_reader_get_use_bytes(&bs, p_sei_len);

	return 0;

ERR_FIN:
	*p_sei_len = H264_NALU_HEADER_SIZE;

	return -3;
}

/**
 * @brief 解析H264 SPS
 * @param [in] p_pack BS句柄
 * @param [in] p_sps SPS数据地址
 * @param [in/out] p_sps_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足
 * -3：数据格式错误
 */
static int32 h264_demux_sps(H264_BS_PACK *p_pack, uint8 *p_sps, int32 *p_sps_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_sps_len;

	int32 set0_flag;
	int32 set1_flag;
	int32 set2_flag;
	int32 set3_flag;
	int32 seq_scaling_matrix_present_flag;
	int32 b_seq_scaling_list_present;
	int32 pic_order_cnt_type;
	int32 ref_frames_order_cnt_cycle;
	int32 pic_width_in_mbs;
	int32 pic_height_in_map_units;

	int32 loop_time;
	int32 last_scale;
	int32 next_scale;
	int32 delta_scale;

	int32 i, j;
	int32 ret;

	*p_sps_len = 0;

	if( left_len < (H264_NALU_HEADER_SIZE + H264_SPS_HEADER_SIZE) )
	{
		return -2;
	}

	bs_reader_init(&bs, p_sps, left_len, GEN_BYTE_BIT);

	bs_reader_skip_bits(&bs, GEN_BYTE_BIT * H264_NALU_HEADER_SIZE);

	/** profile, 8 bit */
	ret = bs_read_bits(&bs, &p_pack->sps.profile, 8);

	/** constraint_set_flags, 4 bit */
	ret = bs_read_bits(&bs, &set0_flag, 1);
	ret = bs_read_bits(&bs, &set1_flag, 1);
	ret = bs_read_bits(&bs, &set2_flag, 1);
	ret = bs_read_bits(&bs, &set3_flag, 1);

	/** reserved_zero_4bits, 4 bit */
	ret = bs_reader_skip_bits(&bs, 4);

	/** level, 8 bit */
	ret = bs_read_bits(&bs, &p_pack->sps.level, 8);

	/** sps_id, var bit */
	ret = bs_read_uv_bits(&bs, &p_pack->sps.id);
	H264_ON_ERR_RET(ret);

	if( p_pack->sps.id > H264_SPS_ID_END )
	{
		goto ERR_FIN;
	}

	if( p_pack->sps.profile == H264_HIGH_PROFILE
		|| p_pack->sps.profile == H264_HIGH_10_PROFILE
		|| p_pack->sps.profile == H264_HIGH_422_PROFILE
		|| p_pack->sps.profile == H264_HIGH_444_PROFILE )
	{
		/** chroma_format_idc, var bit */
		ret = bs_read_uv_bits(&bs, &p_pack->sps.chroma);
		H264_ON_ERR_RET(ret);

		if( p_pack->sps.chroma > H264_CHROMA_FORMAT_444 )
		{
			goto ERR_FIN;
		}

		if( p_pack->sps.chroma == H264_CHROMA_FORMAT_444 )
		{
			/** residual_colour_transform_flag, 1 bit */
			ret = bs_reader_skip_bits(&bs, 1);
			H264_ON_ERR_RET(ret);
		}

		/** bit_depth_luma_minus8, var bit */
		ret = bs_read_uv_bits(&bs, &value);
		H264_ON_ERR_RET(ret);

		/** bit_depth_chroma_minus8, var bit */
		ret = bs_read_uv_bits(&bs, &value);
		H264_ON_ERR_RET(ret);

		/** qpprime_y_zero_transform_bypass_flag, 1 bit */
		ret = bs_reader_skip_bits(&bs, 1);
		H264_ON_ERR_RET(ret);

		/** seq_scaling_matrix_present_flag, 1 bit */
		ret = bs_read_bits(&bs, &seq_scaling_matrix_present_flag, 1);
		H264_ON_ERR_RET(ret);

		if( seq_scaling_matrix_present_flag )
		{
			for( i=0; i<8; i++ )
			{
				/** seq_scaling_list_present_flag, 1 bit */
				ret = bs_read_bits(&bs, &b_seq_scaling_list_present, 1);
				H264_ON_ERR_RET(ret);

				if( b_seq_scaling_list_present )
				{
					last_scale = 8;
					next_scale = 8;

					if( i < 6 )
					{
						loop_time = 16;

					}else
					{
						loop_time = 64;
					}

					for( j=0; j<loop_time; j++ )
					{
						if( next_scale != 0 )
						{
							/** delta_scale, var bit */
							ret = bs_read_uv_bits(&bs, &value);
							H264_ON_ERR_RET(ret);

							ret = bs_reader_calc_se_var(&bs, value, &delta_scale);
							next_scale = ( last_scale + delta_scale + 256 ) % 256;
							//useDefaultScalingMatrixFlag = ( j = = 0 && nextScale = = 0 )
							//scalingList[ j ] = ( nextScale = = 0 ) ? lastScale : nextScale
							//lastScale = scalingList[ j ]
						}
					}

				}// end if( b_seq_scaling_list_present )
			}
		}

	}else
	{
		p_pack->sps.chroma = H264_CHROMA_FORMAT_420;
	}

	/** log2_max_frame_num_minus4, var bit */
	//ret = bs_read_uv_bits(&bs, &p_pack->sps.log2_max_frame_num_minus4);
	ret = bs_read_uv_bits(&bs, &value);
	H264_ON_ERR_RET(ret);

	if( value > H264_LOG2_MAX_FRAME_NUM_MINUS4_END )
	{
		goto ERR_FIN;
	}
	p_pack->sps.log2_max_frame_num_minus4 = value;

	/** pic_order_cnt_type */
	ret = bs_read_uv_bits(&bs, &pic_order_cnt_type);
	H264_ON_ERR_RET(ret);

	if( pic_order_cnt_type > H264_PIC_ORDER_CNT_TYPE_END )
	{
		goto ERR_FIN;
	}

	if( pic_order_cnt_type == 0 )
	{
		/** log2_max_pic_order_cnt_lsb_minus4 */
		ret = bs_read_uv_bits(&bs, &value);
		H264_ON_ERR_RET(ret);

		if( value > H264_LOG2_MAX_FRAME_NUM_MINUS4_END )
		{
			goto ERR_FIN;
		}

	}else if( pic_order_cnt_type == 1 )
	{
		/** delta_pic_order_always_zero_flag, 1 bit */
		ret = bs_reader_skip_bits(&bs, 1);
		H264_ON_ERR_RET(ret);

		/** offset_for_non_ref_pic */
		ret = bs_read_uv_bits(&bs, &value);
		H264_ON_ERR_RET(ret);

		/** offset_for_top_to_bottom_field */
		ret = bs_read_uv_bits(&bs, &value);
		H264_ON_ERR_RET(ret);

		/** num_ref_frames_in_pic_order_cnt_cycle */
		ret = bs_read_uv_bits(&bs, &ref_frames_order_cnt_cycle);
		H264_ON_ERR_RET(ret);

		for( i=0; i<ref_frames_order_cnt_cycle; i++ )
		{
			ret = bs_read_uv_bits(&bs, &value);
			H264_ON_ERR_RET(ret);
		}
	}

	/** num_ref_frames */
	ret = bs_read_uv_bits(&bs, &value);
	H264_ON_ERR_RET(ret);

	/** gaps_in_frame_num_value_allowed_flag, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);
	H264_ON_ERR_RET(ret);

	/** pic_width_in_mbs_minus1 */
	ret = bs_read_uv_bits(&bs, &pic_width_in_mbs);
	H264_ON_ERR_RET(ret);

	pic_width_in_mbs++;
	p_pack->sps.width = pic_width_in_mbs * 16;

	/** pic_height_in_map_units_minus1 */
	ret = bs_read_uv_bits(&bs, &pic_height_in_map_units);
	H264_ON_ERR_RET(ret);

	pic_height_in_map_units++;

	/** frame_mbs_only_flag, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->sps.frame_mbs_only_flag, 1);
	H264_ON_ERR_RET(ret);

	p_pack->sps.height = (2 - p_pack->sps.frame_mbs_only_flag) * pic_height_in_map_units * 16;

	bs_reader_get_use_bytes(&bs, p_sps_len);

	return 0;

ERR_FIN:
	*p_sps_len = H264_NALU_HEADER_SIZE;

	return -3;
}

/**
 * @brief 解析H264 PPS
 * @param [in] p_pack BS句柄
 * @param [in] p_pps PPS数据地址
 * @param [in/out] p_pps_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足
 * -3：数据格式错误
 */
static int32 h264_demux_pps(H264_BS_PACK *p_pack, uint8 *p_pps, int32 *p_pps_len)
{
	BIT_STREAM_READER bs;

	int32 total_parse_len = 0;
	int32 left_len = *p_pps_len;

	int32 entropy_coding_mode_flag;
	int32 pic_order_present_flag;
	int32 num_slice_groups_minus1;
	int32 slice_group_map_type;

	int32 ret;

	*p_pps_len = 0;

	bs_reader_init(&bs, p_pps, left_len, GEN_BYTE_BIT);

	bs_reader_skip_bits(&bs, GEN_BYTE_BIT * H264_NALU_HEADER_SIZE);

	/** pps_id */
	ret = bs_read_uv_bits(&bs, &p_pack->pps.pps_id);
	H264_ON_ERR_RET(ret);

	/** sps_id */
	ret = bs_read_uv_bits(&bs, &p_pack->pps.sps_id);
	H264_ON_ERR_RET(ret);

	/** entropy_coding_mode_flag, 1 bit */
	ret = bs_read_bits(&bs, &entropy_coding_mode_flag, 1);
	H264_ON_ERR_RET(ret);

	/** pic_order_present_flag, 1 bit */
	ret = bs_read_bits(&bs, &pic_order_present_flag, 1);
	H264_ON_ERR_RET(ret);

	/** num_slice_groups_minus1 */
	ret = bs_read_uv_bits(&bs, &num_slice_groups_minus1);
	H264_ON_ERR_RET(ret);

	if( num_slice_groups_minus1 > 0 )
	{
		/** slice_group_map_type */
		ret = bs_read_uv_bits(&bs, &slice_group_map_type);
		H264_ON_ERR_RET(ret);
	}

	bs_reader_get_use_bytes(&bs, p_pps_len);

	return 0;
}

/**
 * @brief 解析H264 Slice头
 * @param [in] p_pack BS句柄
 * @param [in] p_slice 帧数据地址
 * @param [in/out] p_slice_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足
 * -3：数据格式错误
 */
static int32 h264_demux_slice_header(H264_BS_PACK *p_pack, uint8 *p_slice, int32 *p_slice_len)
{
	BIT_STREAM_READER bs;
	int32 read_bits;

	int32 total_parse_len = 0;
	int32 left_len = *p_slice_len;

	int32 ret;

	*p_slice_len = 0;

	if( !p_pack->b_sps )
	{
		goto ERR_FIN;
	}

	bs_reader_init(&bs, p_slice, left_len, GEN_BYTE_BIT);

	bs_reader_skip_bits(&bs, GEN_BYTE_BIT * H264_NALU_HEADER_SIZE);

	/** first_mb_in_slice, var bit */
	ret = bs_read_uv_bits(&bs, &p_pack->slice_header.first_mb_in_slice);
	H264_ON_ERR_RET(ret);

	/** slice type */
	ret = bs_read_uv_bits(&bs, &p_pack->slice_header.type);
	H264_ON_ERR_RET(ret);

	/** pic_parameter_set_id */
	ret = bs_read_uv_bits(&bs, &p_pack->slice_header.pps_id);
	H264_ON_ERR_RET(ret);

	/** frame_num */
	read_bits = p_pack->sps.log2_max_frame_num_minus4 + 4;
	ret = bs_read_bits(&bs, &p_pack->slice_header.frame_num, read_bits);
	H264_ON_ERR_RET(ret);
	//log_trace(LOG_NORMAL, _T("[h264_bs_parser::h264_demux_slice_header] frame num = %d\n"), p_pack->slice_header.frame_num);

	if( !p_pack->sps.frame_mbs_only_flag )
	{
		/** field_pic_flag, 1 bit */
		ret = bs_read_bits(&bs, &p_pack->slice_header.field_pic_flag, 1);
		H264_ON_ERR_RET(ret);

		if( p_pack->slice_header.field_pic_flag )
		{
			/** bottom_field_flag, 1 bit */
			ret = bs_read_bits(&bs, &p_pack->slice_header.bottom_field_flag, 1);
			H264_ON_ERR_RET(ret);
		}

	}else
	{
		/** 默认为0 */
		p_pack->slice_header.field_pic_flag = 0;
		p_pack->slice_header.bottom_field_flag = 0;
	}

	bs_reader_get_use_bytes(&bs, p_slice_len);

	return 0;

ERR_FIN:
	*p_slice_len = H264_NALU_HEADER_SIZE;

	return -3;
}


/**  
 * @brief 解析H264 Nalu
 * @param [in] p_pack BS句柄
 * @param [in] p_bs 数据地址
 * @param [in/out] p_bs_len 输入数据长度/实际解析数据长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足
 * -3：数据格式错误
 */
static int32 h264_demux_nalu(H264_BS_PACK *p_pack, uint8 *p_nalu, int32 *p_nalu_len)
{
	int32 total_parse_len = 0;
	int32 left_len = *p_nalu_len;
	int32 parse_len;

	int32 forbidden_zero;
	int32 nal_ref_idc;

	int32 ret;

	*p_nalu_len = 0;

	if( left_len < H264_NALU_HEADER_SIZE )
	{
		return -2;
	}

	//p_pack->p_start = p_nalu;

	/** forbidden_zero_bit, 1 bit */
	forbidden_zero = p_nalu[0] >> 7;
	if( forbidden_zero )
	{
		*p_nalu_len = H264_NALU_HEADER_SIZE;
		return -3;
	}

	/** nal_ref_idc, 2 bit */
	nal_ref_idc = (p_nalu[0] >> 5) & 0x3;

	/** nalu_type, 5 bit */
	p_pack->nalu_type = p_nalu[0] & 0x1F;
	//log_debug(_T("[h264_bs_parser::h264_demux_nalu] nalu type = %d\n"), p_pack->nalu_type);

	parse_len = left_len;
	switch( p_pack->nalu_type )
	{
	case H264_NALU_NON_IDR_SLICE:
	case H264_NALU_IDR_SLICE:
		{
			ret = h264_demux_slice_header(p_pack, p_nalu, &parse_len);
		}
		break;

	case H264_NALU_SEI:
		{
			ret = h264_demux_sei(p_pack, p_nalu, &parse_len);
		}
		break;

	case H264_NALU_SPS:
		{
			ret = h264_demux_sps(p_pack, p_nalu, &parse_len);
			if( ret == 0 )
			{
				p_pack->b_sps = 1;

			}else if( ret == -2 )
			{
				/** include some valid data */
				ret = -12;
			}
		}
		break;

	case H264_NALU_PPS:
		{
			ret = h264_demux_pps(p_pack, p_nalu, &parse_len);
		}
		break;

	case H264_NALU_AUD:
		{
			if( left_len < (H264_NALU_HEADER_SIZE + 1) )
			{
				parse_len = 0;
				ret = -2;

			}else
			{
				parse_len = H264_NALU_HEADER_SIZE + 1;
				ret = 0;
			}
		}
		break;

	default:
		{
			parse_len = H264_NALU_HEADER_SIZE;
			ret = -1;
		}
		break;
	}

	*p_nalu_len = parse_len;

	return ret;
}

////////////////////////////////外部接口///////////////////////////////////////
int32 h264_bs_init(H264_BS_PACK *p_pack)
{
	if( p_pack == NULL )
	{
		return -1;
	}

	memset(p_pack, 0, sizeof(H264_BS_PACK));

	return 0;
}

int32 h264_parse_bs(H264_BS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;
	int32 parse_len;

	int32 i;
	int32 ret;

	if( p_pack == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	i = 0;
	while( left_len >= (H264_BS_HEADER_SIZE + H264_NALU_HEADER_SIZE) )
	{
#if 0
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x00 && p_buf[i+3] == 0x01) )
		{
			i++;
			left_len--;
			continue;
		}

		p_pack->p_start = p_buf + i;
		i += H264_BS_HEADER_SIZE;
		left_len -= H264_BS_HEADER_SIZE;

#else

		if( p_buf[i] == 0x00 && p_buf[i+1] == 0x00 )
		{
			if( p_buf[i+2] == 0x01 )
			{
				/** short start code */
				p_pack->p_start = p_buf + i;
				i += 3;
				left_len -= 3;

			}else if( p_buf[i+2] == 0x00 && p_buf[i+3] == 0x01)
			{
				/** long start code */
				p_pack->p_start = p_buf + i;
				i += H264_BS_HEADER_SIZE;
				left_len -= H264_BS_HEADER_SIZE;

			}else
			{
				/** not start code */
				i++;
				left_len--;
				continue;
			}


		}else
		{
			i++;
			left_len--;
			continue;
		}

#endif

		parse_len = left_len;
		ret = h264_demux_nalu(p_pack, p_buf + i, &parse_len);
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			*p_parse_len = i;

			return 0;

		}else if( ret == -2 || ret == -12 )
		{
			break;
		}

		/** 其余情况下忽略该nalu，继续解析 */

	}

	*p_parse_len = i;

	return -2;
}

int32 h264_parse_bs2(H264_BS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;
	int32 parse_len;

	int32 i;
	int32 ret;

	if( p_pack == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	i = 0;
	while( left_len >= H264_NALU_HEADER_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x00 && p_buf[i+3] == 0x01) )
		{
			i++;
			left_len--;
			continue;
		}

		parse_len = left_len;
		ret = h264_demux_nalu(p_pack, p_buf+i, &parse_len);
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			*p_parse_len = i;

			return 0;

		}else if( ret == -2 )
		{
			break;

		}else if( ret == -12 )
		{
			*p_parse_len = i;

			return ret;
		}

		/** 其余情况下忽略该nalu，继续解析 */

	}

	*p_parse_len = i;

	ret = -2;
	switch( left_len )
	{
	case 4:
		{
			if( (p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x00 && p_buf[i+3] == 0x01) )
			{
				ret = -12;
			}
		}
		break;

	case 3:
		{
			if( (p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x00) )
			{
				ret = -12;
			}
		}
		break;

	case 2:
		{
			if( (p_buf[i] == 0x00 && p_buf[i+1] == 0x00) )
			{
				ret = -12;
			}
		}
		break;

	case 1:
		{
			if( (p_buf[i] == 0x00) )
			{
				ret = -12;
			}
		}
		break;

	default:
		{
		}
	}

	return ret;
}
