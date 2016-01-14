

#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "mpeg2_ves_parser.h"
#include <util/log_debug.h>

#define MPEG2_VES_ON_ERR_RET(ERR)                  {if( ERR ) return ERR;}

/**  
 * @brief 解析MPEG2视频序列头
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_seq_start 输入数据地址
 * @param [in/out] p_seq_len 输入数据长度/已解析的长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_seq_header(MPEG2_VES_STATE *p_state, uint8 *p_seq_start, int32 *p_seq_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_seq_len;

	int32 b_intra_matrix;
	int32 b_non_intra_matrix;

	int32 ret;

	*p_seq_len = 0;

	if( left_len < MPEG2_SEQ_START_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_seq_start, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** horizontal_size_value, low 12 bit(total 14 bit) */
	ret = bs_read_bits(&bs, &p_state->seq.width, 12);

	/** vertical_size_value, low 12 bit(total 14 bit) */
	ret = bs_read_bits(&bs, &p_state->seq.height, 12);

	/** aspect_ratio_information, 4 bit */
	ret = bs_read_bits(&bs, &p_state->seq.aspect_ratio, 4);

	/** frame_rate_code, 4 bit */
	ret = bs_read_bits(&bs, &p_state->seq.frame_rate, 4);

	/** bit_rate_value, low 18 bit(total 30 bit) */
	ret = bs_read_bits(&bs, &p_state->seq.bit_rate, 18);

	/** marker_bit, 1 bit*/
	ret = bs_read_bits(&bs, &value, 1);

	/** vbv_buffer_size_value, low 10 bit(total 18 bit) */
	ret = bs_read_bits(&bs, &p_state->seq.vbv_buf_size, 10);

	/** constrained_parameters_flag, 1 bit */
	bs_reader_skip_bits(&bs, 1);

	total_parse_len += MPEG2_SEQ_START_HEADER_SIZE;

	/** load_intra_quantiser_matrix, 1 bit */
	ret = bs_read_bits(&bs, &b_intra_matrix, 1);
	if( b_intra_matrix )
	{
		/** intra quantiser matrix, 64 byte */
		ret = bs_reader_need_bits(&bs, GEN_BYTE_BIT*64);
		if( ret < 0 )
		{
			return -2;
		}

		ret = bs_reader_skip_bits(&bs, GEN_BYTE_BIT*64);
		total_parse_len += 64;
	}

	/** load_non_intra_quantiser_matrix, 1 bit */
	ret = bs_read_bits(&bs, &b_non_intra_matrix, 1);
	if( b_non_intra_matrix )
	{
		ret = bs_reader_need_bits(&bs, GEN_BYTE_BIT*64);
		if( ret < 0 )
		{
			return -2;
		}

		ret = bs_reader_skip_bits(&bs, GEN_BYTE_BIT*64);
		total_parse_len += 64;
	}

	*p_seq_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG2视频序列扩展头
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_ext_start 输入数据地址
 * @param [in/out] p_ext_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_seq_ext(MPEG2_VES_STATE *p_state, uint8 *p_ext_start, int32 *p_ext_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_ext_len;

	int32 frame_rate_value;
	int32 frame_rate_n;
	int32 frame_rate_d;

	int32 ret;

	*p_ext_len = 0;

	if( left_len < MPEG2_SEQ_EXT_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_ext_start, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** extension_start_code_id, 4 bit */
	bs_reader_skip_bits(&bs, 4);

	/** profile_and_level_indication, 8 bit */
	ret = bs_read_bits(&bs, &value, 8);
	p_state->seq.profile = (value >> 4) & 0x7;
	p_state->seq.level = value & 0xF;

	/** progressive_sequence, 1 bit */
	ret = bs_read_bits(&bs, &p_state->seq.progs_seq, 1);

	/** chroma_format, 2 bit */
	ret = bs_read_bits(&bs, &p_state->seq.chroma_format, 2);

	/** horizontal_size, high 2 bit(total 14 bit) */
	ret = bs_read_bits(&bs, &value, 2);
	p_state->seq.width |= ( value << 12 );
	/** 样本数转为像素 */
	//p_state->seq.width = (p_state->seq.width + 15)/16;

	/** vertical_size, high 2 bit(total 14 bit) */
	ret = bs_read_bits(&bs, &value, 2);
	p_state->seq.height |= ( value << 12 );

	/** bit_rate, high 12 bit(total 30 bit) */
	ret = bs_read_bits(&bs, &value, 12);
	p_state->seq.bit_rate |= ( value << 18 );
	/** 400bit/s转为bps */
	p_state->seq.bit_rate = (p_state->seq.bit_rate * 400);
	//p_state->seq.bit_rate = (p_state->seq.bit_rate * 400)/1000;

	/** marker_bit, 1 bit */
	bs_reader_skip_bits(&bs, 1);

	/** vbv_buffer_size, high 8 bit(total 18 bit) */
	ret = bs_read_bits(&bs, &value, 8);
	p_state->seq.vbv_buf_size |= ( value << 10 );

	/** low_delay, 1 bit */
	ret = bs_read_bits(&bs, &p_state->seq.b_low_delay, 1);

	/** frame_rate_n, 2 bit */
	ret = bs_read_bits(&bs, &frame_rate_n, 2);

	/** frame_rate_d, 5 bit */
	ret = bs_read_bits(&bs, &frame_rate_d, 5);

	switch( p_state->seq.frame_rate )
	{
	case MPEG2_VIDEO_FRAME_RATE_23:
		{
			frame_rate_value = 23;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_24:
		{
			frame_rate_value = 24;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_25:
		{
			frame_rate_value = 25;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_29:
		{
			frame_rate_value = 29;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_30:
		{
			frame_rate_value = 30;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_50:
		{
			frame_rate_value = 50;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_59:
		{
			frame_rate_value = 59;
		}
		break;

	case MPEG2_VIDEO_FRAME_RATE_60:
		{
			frame_rate_value = 23;
		}
		break;

	default:
		{
			frame_rate_value = 25;
		}
		break;
	}

	if( frame_rate_n == 0 && frame_rate_d == 0 )
	{
		p_state->seq.frame_rate = frame_rate_value;
	}else
	{
		p_state->seq.frame_rate = frame_rate_value * (frame_rate_n + 1) + (frame_rate_d + 1);
	}
	//cp_trace(_T("[mpeg2_demux_seq_ext] frame dim = %dx%d, bit rate = %d kbps, vbv buf = %d, frame rate = %d\n"),
	//	p_state->seq.width, p_state->seq.height, p_state->seq.bit_rate, p_state->seq.vbv_buf_size, p_state->seq.frame_rate);

	total_parse_len += MPEG2_SEQ_EXT_HEADER_SIZE;

	*p_ext_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG2视频序列显示扩展
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_ext_start 输入数据地址
 * @param [in/out] p_ext_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_seq_display_ext(MPEG2_VES_STATE *p_state, uint8 *p_ext_start, int32 *p_ext_len)
{
	BIT_STREAM_READER bs;
	//int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_ext_len;

	int32 b_color_desc;

	int32 ret;

	*p_ext_len = 0;

	if( left_len < MPEG2_SEQ_DISPLAY_EXT_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_ext_start, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** extension_start_code_id, 4 bit */
	bs_reader_skip_bits(&bs, 4);

	/** video_format, 3 bit */
	ret = bs_read_bits(&bs, &p_state->seq.video_format, 3);

	/** colour_description, 1 bit */
	ret = bs_read_bits(&bs, &b_color_desc, 1);

	total_parse_len += 1;

	if( b_color_desc )
	{
		ret = bs_reader_skip_bits(&bs, 24);
		total_parse_len += 3;
	}

	/** display_horizontal_size, 14 bit */
	ret = bs_read_bits(&bs, &p_state->seq.display_width, 14);
	/** 样本数转换为像素 */
	//p_state->seq.display_width = (p_state->seq.display_width + 15)/16;

	/** marker bit, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** display_vertical_size, 14 bit */
	ret = bs_read_bits(&bs, &p_state->seq.display_height, 14);

	total_parse_len += 4;

	*p_ext_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG2视频GOP头
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_gop_start 输入数据地址
 * @param [in] p_gop_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_gop_header(MPEG2_VES_STATE *p_state, uint8 *p_gop_start, int32 *p_gop_len)
{
	int32 total_parse_len = 0;
	int32 left_len = *p_gop_len;

	//int32 b_closed_gop;
	//int32 b_broken_link;

	*p_gop_len = 0;

	if( left_len < MPEG2_GOP_HEADER_SIZE )
	{
		return -2;
	}

	/** time_code, 25 bit */

	/** closed_gop, 1 bit */
//	b_closed_gop = (p_gop_start[7] >> 6) & 0x1;

	/** broken_link, 1 bit */
//	b_broken_link = (p_gop_start[7] >> 5) & 0x1;

	total_parse_len += MPEG2_GOP_HEADER_SIZE;

	*p_gop_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG2视频图象头
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_buf 输入数据地址
 * @param [in/out] p_pic_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_pic_header(MPEG2_VES_STATE *p_state, uint8 *p_pic_start, int32 *p_pic_len)
{
	BIT_STREAM_READER bs;
	//int32 value;

	int32 left_len = *p_pic_len;
	int32 full_pel_forward_vector;
	int32 forward_f_code;
	int32 full_pel_backward_vector;
	int32 backward_f_code;

	int32 ret;

	*p_pic_len = 0;

	if( left_len < MPEG2_VIDEO_PIC_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_pic_start, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** temporal reference, 10 bit */
	bs_read_bits(&bs, &p_state->pic.temp_ref, 10);

	/** picture_coding_type, 3 bit */
	ret = bs_read_bits(&bs, &p_state->pic.type, 3);
	if( p_state->pic.type == 0 || p_state->pic.type >= 0x4 )
	{
		goto ERR_FIN;
	}

	/** vbv_delay, 16 bit */
	bs_reader_skip_bits(&bs, 16);

	if( p_state->pic.type == MPEG2_P_FRAME || p_state->pic.type == MPEG2_B_FRAME )
	{
		/** full_pel_forward_vector, 1 bit */
		ret = bs_read_bits(&bs, &full_pel_forward_vector, 1);
		MPEG2_VES_ON_ERR_RET(ret);

		if( full_pel_forward_vector != 0 )
		{
			goto ERR_FIN;
		}

		/** forward_f_code, 3 bit */
		ret = bs_read_bits(&bs, &forward_f_code, 3);
		MPEG2_VES_ON_ERR_RET(ret);

		if( forward_f_code != 0x7 )
		{
			goto ERR_FIN;
		}
	}

	if( p_state->pic.type == MPEG2_B_FRAME )
	{
		/** full_pel_backward_vector, 1 bit */
		ret = bs_read_bits(&bs, &full_pel_backward_vector, 1);
		MPEG2_VES_ON_ERR_RET(ret);

		if( full_pel_backward_vector != 0 )
		{
			goto ERR_FIN;
		}

		/** backward_f_code, 3 bit */
		ret = bs_read_bits(&bs, &backward_f_code, 3);
		MPEG2_VES_ON_ERR_RET(ret);

		if( backward_f_code != 0x7 )
		{
			goto ERR_FIN;
		}
	}

	bs_reader_get_use_bytes(&bs, p_pic_len);

	return 0;

ERR_FIN:
	*p_pic_len = MPEG2_VES_START_CODE_SIZE;
	return -3;
}

/**  
 * @brief 解析MPEG2视频图像编码扩展
 *
 * @param [in] p_state 视频ES信息句柄
 * @param [in] p_ext_start 输入数据地址
 * @param [in/out] p_ext_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_pic_coding_ext(MPEG2_VES_STATE *p_state, uint8 *p_ext_start, int32 *p_ext_len)
{
	BIT_STREAM_READER bs;
	//int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_ext_len;

	int32 ret;

	*p_ext_len = 0;

	if( left_len < MPEG2_VIDEO_PIC_CODING_EXT_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_ext_start, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** extension_start_code_id, 4 bit */
	bs_reader_skip_bits(&bs, 4);

	/** f_code[s][t], 4*4 bit */
	ret = bs_reader_skip_bits(&bs, 4*4);

	/** intra_dc_precision, 2 bit */
	ret = bs_read_bits(&bs, &p_state->pic.dc_precision, 2);

	/** picture_structure, 2 bit */
	ret = bs_read_bits(&bs, &p_state->pic.pic_struct, 2);

	/** misc flags, 8 bit */
	ret = bs_reader_skip_bits(&bs, 8);

	/** progressive_frame, 1 bit */
	ret = bs_read_bits(&bs, &p_state->pic.b_progressive, 1);

	total_parse_len += MPEG2_VIDEO_PIC_CODING_EXT_SIZE;

	*p_ext_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG2视频扩展头
 *
 * @param [in] p_state 视频ES信息句柄
 * @param [in] p_ext_start 输入数据地址
 * @param [in/out] p_ext_len 输入数据长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg2_demux_ext_header(MPEG2_VES_STATE *p_state, uint8 *p_ext_start, int32 *p_ext_len)
{
	int32 left_len = *p_ext_len;

	int32 ret;

	//*p_ext_len = 0;

	if( left_len < MPEG2_VIDEO_EXT_HEADER_SIZE )
	{
		*p_ext_len = 0;
		return -2;
	}

	/** extension_start_code_identifier, 4 bit */
	p_state->ext_id = (p_ext_start[4] >> 4) & 0xF;

	switch( p_state->ext_id )
	{
	case MPEG2_SEQ_EXT_ID:
		{
			/** 序列扩展 */
			ret = mpeg2_demux_seq_ext(p_state, p_ext_start, p_ext_len);
		}
		break;

	case MPEG2_SEQ_DISPLAY_EXT_ID:
		{
			/** 序列显示扩展 */
			ret = mpeg2_demux_seq_display_ext(p_state, p_ext_start, p_ext_len);
		}
		break;

	case MPEG2_PIC_CODING_EXT_ID:
		{
			/** 图像编码扩展 */
			ret = mpeg2_demux_pic_coding_ext(p_state, p_ext_start, p_ext_len);
		}
		break;

	case MPEG2_SEQ_SCALABLE_EXT_ID:
		{
			ret = -1;
		}

	default:
		{
			*p_ext_len = MPEG2_VIDEO_EXT_HEADER_SIZE;
			ret = -1;
		}
	}

	return ret;
}

/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg2_ves_init(MPEG2_VES_STATE *p_state)
{
	if( p_state == NULL )
	{
		return -1;
	}

	memset(p_state, 0, sizeof(MPEG2_VES_STATE));

	return 0;
}

int32 mpeg2_ves_parse(MPEG2_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;
	int32 parse_len = 0;

	int32 ret;

	int32 i = 0;

	if( p_state == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	while( left_len > MPEG2_VES_START_CODE_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			left_len--;
			continue;
		}

		p_state->p_start = &p_buf[i];
		parse_len = left_len;

		/** 解析起始码 */
		switch( p_buf[i+3] )
		{
		case MPEG2_PICTURE_START_CODE:
			{
				ret = mpeg2_demux_pic_header(p_state, p_buf+i, &parse_len);
			}
			break;

		case MPEG2_EXT_START_CODE:
			{
				ret = mpeg2_demux_ext_header(p_state, p_buf+i, &parse_len);
			}
			break;

		case MPEG2_SEQ_START_CODE:
			{
				ret = mpeg2_demux_seq_header(p_state, p_buf+i, &parse_len);
			}
			break;

		case MPEG2_GROUP_START_CODE:
			{
				ret = mpeg2_demux_gop_header(p_state, p_buf+i, &parse_len);
			}
			break;

		case MPEG2_USER_DATA_START_CODE:
			{
				parse_len = MPEG2_VES_START_CODE_SIZE;
				ret = -1;
				//cp_trace(_T("[mpeg2_ves_parser][mpeg2_ves_parse] user data!\n"));
			}
			break;

		default:
			{
				//ATLTRACE("unknow start code = 0x%x!!\n", p_buf[i+3]);
				parse_len = MPEG2_VES_START_CODE_SIZE;
				ret = -1;
			}
		}

		p_state->start_code = p_buf[i+3];
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			/** 找到start code并且解析成功 */
			*p_parse_len = i;

			return 0;

		}else if( ret == -2 )
		{
			break;
		}

		/** 其余情况下忽略该起始码，继续解析 */
	}

	*p_parse_len = i;

	return -2;
}
