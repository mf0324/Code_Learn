
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "mpeg4_ves_parser.h"

//#include <util/log_debug.h>

/**  
 * @brief 解析MPEG4 VOS
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_vos 输入数据地址
 * @param [in/out] p_vos_len 输入数据长度/已解析数据长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg4_demux_vos_header(MPEG4_VES_STATE *p_state, uint8 *p_vos, int32 *p_vos_len)
{
	int32 total_parse_len = 0;
	int32 left_len = *p_vos_len;

	*p_vos_len = 0;

	if( left_len < MPEG4_VOS_HEADER_SIZE )
	{
		return -2;
	}

	/** profile_and_level_indication, 8 bit */
	p_state->vos.profile = p_vos[4];

	total_parse_len += MPEG4_VOS_HEADER_SIZE;

	*p_vos_len = total_parse_len;

	return 0;
}

/**  
 * @brief 解析MPEG4 Visual Object
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_vo 输入数据地址
 * @param [in/out] p_vos_len 输入数据长度/已解析数据长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg4_demux_vo(MPEG4_VES_STATE *p_state, uint8 *p_vo, int32 *p_vo_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_vo_len;

	int32 b_is_vo_id;
	int32 b_video_signal_type;
	int32 b_color_desc;

	int32 ret;

	*p_vo_len = 0;

	if( left_len < MPEG4_VISUAL_OBJECT_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_vo, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** is_visual_object_identifier, 1 bit */
	ret = bs_read_bits(&bs, &b_is_vo_id, 1);
	p_state->vo.is_visual_obj = b_is_vo_id;

	if( b_is_vo_id )
	{
		/** visual_object_verid + visual_object_priority, 4 + 3 bit */
		ret = bs_reader_skip_bits(&bs, 4+3);
	}

	/** visual_object_type, 4 bit */
	ret = bs_read_bits(&bs, &value, 4);
	if( ret )
	{
		return ret;
	}
	p_state->vo.type = value;

	if( p_state->vo.type == MPEG4_VO_VIDEO_ID || p_state->vo.type == MPEG4_VO_STILL_TEXTURE_ID )
	{
		ret = bs_read_bits(&bs, &b_video_signal_type, 1);
		if( ret )
		{
			return ret;
		}
		p_state->vo.signal_type = b_video_signal_type;

		if( b_video_signal_type )
		{
			ret = bs_reader_need_bits(&bs, 5);
			if( ret )
			{
				return ret;
			}

			/** video_format, 3 bit */
			ret = bs_read_bits(&bs, &value, 3);
			p_state->vo.video_format = value;
			/** video_range, 1 bit */
			ret = bs_reader_skip_bits(&bs, 1);
			/** color_description, 1 bit */
			ret = bs_read_bits(&bs, &b_color_desc, 1);
			p_state->vo.color_desc = b_color_desc;

			if( b_color_desc )
			{
				/** color_primaries + transfer_character + matrix_coefficient, 8+8+8 bit */
				ret = bs_reader_skip_bits(&bs, 8+8+8);
				if( ret )
				{
					return ret;
				}
			}

		}
	}

	ret = bs_reader_align(&bs);
	ret = bs_reader_get_use_bytes(&bs, p_vo_len);

	return 0;
}

/**  
 * @brief 解析MPEG4 VOL
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_vol 输入数据地址
 * @param [in] p_vol_len 输入数据长度/已解析数据长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg4_demux_vol(MPEG4_VES_STATE *p_state, uint8 *p_vol, int32 *p_vol_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 total_parse_len = 0;
	int32 left_len = *p_vol_len;

	int32 b_object_layer_id;
	int32 vol_verid = 0;
	int32 b_vol_ctrl_param;
	int32 b_vbv_param;
	int32 b_fix_vop_rate;

	int32 ret;

	*p_vol_len = 0;

	if( left_len < MPEG4_VOL_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_vol, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	ret = bs_reader_skip_bits(&bs, 32);

	/** random_accessible_vol, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** video_object_type_indication, 8 bit */
	ret = bs_reader_skip_bits(&bs, 8);

	/** is_object_layer_identifier, 1 bit */
	ret = bs_read_bits(&bs, &b_object_layer_id, 1);
	if( b_object_layer_id )
	{
		/** vol_verid + vol_priority, 4+3 bit */
		ret = bs_reader_need_bits(&bs, 4+3);
		if( ret )
		{
			return ret;
		}

		ret = bs_read_bits(&bs, &vol_verid, 4);
		ret = bs_reader_skip_bits(&bs, 3);
	}

	/** aspect_ratio_info, 4 bit */
	ret = bs_read_bits(&bs, &value, 4);
	if( ret )
	{
		return ret;
	}
	p_state->vol.aspect_ratio = value;

	if( p_state->vol.aspect_ratio == MPEG4_PAR_EXTEND )
	{
		/** par_width + par_height, 8+8 bit */
		ret = bs_reader_skip_bits(&bs, 8+8);
		if( ret )
		{
			return ret;
		}

		//ret = bs_read_bits(&bs, &p_state->head.vol.par_width, 8);
		//ret = bs_read_bits(&bs, &p_state->head.vol.par_height, 8);
	}

	/** vol_control_param, 1 bit */
	ret = bs_read_bits(&bs, &b_vol_ctrl_param, 1);
	if( ret )
	{
		return ret;
	}

	if( b_vol_ctrl_param )
	{
		ret = bs_reader_need_bits(&bs, 2+1+1);
		if( ret )
		{
			return ret;
		}

		/** chroma_format, 2 bit */
		ret = bs_read_bits(&bs, &value, 2);
		p_state->vol.chroma_format = value;
		/** low_delay, 1 bit */
		ret = bs_reader_skip_bits(&bs, 1);
		/** vbv_parameters, 1 bit */
		ret = bs_read_bits(&bs, &b_vbv_param, 1);
		if( b_vbv_param )
		{
			/** misc parameters, 15+1+15+1+15+1+3+11+1+15+1 bit */
			ret = bs_reader_skip_bits(&bs, 15+1+15+1+15+1+3+11+1+15+1);
			if( ret )
			{
				return ret;
			}
		}
	}

	/** video_object_layer_shape, 2 bit */
	ret = bs_read_bits(&bs, &value, 2);
	if( ret )
	{
		return ret;
	}
	p_state->vol.shape = value;

	if( p_state->vol.shape == MPEG4_VOL_SHAPE_GRAYSCALE && vol_verid != 0x1 )
	{
		/** vol_shape_ext, 4 bit */
		ret = bs_reader_skip_bits(&bs, 4);
		if( ret )
		{
			return ret;
		}
	}

	/** marker + vop_time_inc_resolution + marker, 1+16+1 bit */
	ret = bs_reader_skip_bits(&bs, 1+16+1);
	if( ret )
	{
		return ret;
	}

	/** fixed_vop_rate, 1 bit */
	ret = bs_read_bits(&bs, &b_fix_vop_rate, 1);
	if( ret )
	{
		return ret;
	}

	if( b_fix_vop_rate )
	{
		/** fixed_vop_time_increment, 1-16 bit */
		ret = bs_reader_skip_bits(&bs, 4);
		if( ret )
		{
			return ret;
		}
	}

	if( p_state->vol.shape != MPEG4_VOL_SHAPE_BINARY_ONLY )
	{
		/** rectangular */
		if( p_state->vol.shape == MPEG4_VOL_SHAPE_RECT )
		{
			/** marker + vol_width + marker + vol_height + marker, 1+13+1+13+1 bit */
			ret = bs_reader_need_bits(&bs, 1+13+1+13+1);
			if( ret )
			{
				return ret;
			}

			ret = bs_reader_skip_bits(&bs, 1);
			ret = bs_read_bits(&bs, &value, 13);
			p_state->vol.width = value;
			ret = bs_reader_skip_bits(&bs, 1);
			ret = bs_read_bits(&bs, &value, 13);
			p_state->vol.height = value;
			ret = bs_reader_skip_bits(&bs, 1);
		}

		/** interlaced, 1 bit */
		ret = bs_read_bits(&bs, &p_state->vol.b_interlace, 1);
		if( ret )
		{
			return ret;
		}
	}

	/** 已解析的长度 */
	ret = bs_reader_align(&bs);
	ret = bs_reader_get_use_bytes(&bs, p_vol_len);

	return 0;
}

/**  
 * @brief 解析MPEG4 GOP
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_gop 输入数据地址
 * @param [in/out] p_gop_len 输入数据长度/已解析数据长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg4_demux_gop(MPEG4_VES_STATE *p_state, uint8 *p_gop, int32 *p_gop_len)
{
	//BIT_STREAM_READER  bs;

	int32 left_len = *p_gop_len;

	//int32 ret;

	*p_gop_len = 0;

	if( left_len < MPEG4_GOP_HEADER_SIZE )
	{
		return -2;
	}

#if 0
	bs_reader_init(&bs, p_gop, left_len, 8);
	bs_reader_skip_bits(&bs, 32);

	/** time_code+closed_gov+broken_link, 18+1+1 bit */
	ret = bs_reader_skip_bits(&bs, 18+1+1);
	if( ret )
	{
		return ret;
	}

	/** 已解析的长度 */
	ret = bs_reader_align(&bs);
	ret = bs_reader_get_use_bytes(&bs, p_gop_len);
#endif

	*p_gop_len = MPEG4_GOP_HEADER_SIZE;

	return 0;
}

/**  
 * @brief 解析MPEG4 VOP
 *
 * @param [in] p_state 视频ES句柄
 * @param [in] p_vop 输入数据地址
 * @param [in] p_vop_len 输入数据长度/已解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：数据不足
 */
static int32 mpeg4_demux_vop(MPEG4_VES_STATE *p_state, uint8 *p_vop, int32 *p_vop_len)
{
	BIT_STREAM_READER  bs;

	int32 left_len = *p_vop_len;

	int32 ret;

	*p_vop_len = 0;

	if( left_len < MPEG4_VOP_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_vop, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** vop_coding_type, 2 bit */
	ret = bs_read_bits(&bs, &p_state->vop.frame_type, 2);
	if( ret )
	{
		return ret;
	}

	/** 已解析的长度 */
	//ret = bs_reader_align(&bs);
	//ret = bs_reader_get_use_bytes(&bs, p_vop_len);
	*p_vop_len = MPEG4_VOP_HEADER_SIZE;

	return 0;
}

/////////////////////////////////////外部接口//////////////////////////////////
int32 mpeg4_ves_init(MPEG4_VES_STATE *p_state)
{
	if( p_state == NULL )
	{
		return -1;
	}

	memset(p_state, 0, sizeof(MPEG4_VES_STATE));

	return 0;
}

int32 mpeg4_ves_parse(MPEG4_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
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

	while( left_len > MPEG4_VES_START_CODE_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			left_len--;
			continue;
		}

		p_state->p_start = p_buf + i;
		parse_len = left_len;

		/** 解析起始码 */
		switch( p_buf[i+3] )
		{
		case MPEG4_VOP_START_CODE:
			{
				ret = mpeg4_demux_vop(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vop = %d\n"), p_state->vop.frame_type);
			}
			break;

		case MPEG4_VOS_START_CODE:
			{
				ret = mpeg4_demux_vos_header(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vos\n"));
			}
			break;

		case MPEG4_VISUAL_OBJECT_START_CODE:
			{
				ret = mpeg4_demux_vo(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser][mpeg4_ves_parse] visual object\n"));
			}
			break;

		case MPEG4_GOP_START_CODE:
			{
				ret = mpeg4_demux_gop(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] gop\n"));
			}
			break;

		case MPEG4_USER_DATA_START_CODE:
			{
				ret = -1;
				parse_len = MPEG4_VES_START_CODE_SIZE;
				//log_debug(_T("[mpeg4_ves_parser][mpeg4_ves_parse] user data\n"));
			}
			break;

		default:
			{
				if( p_buf[i+3] >= MPEG4_VOL_START_CODE_BEGIN
					&& p_buf[i+3] <= MPEG4_VOL_START_CODE_END )
				{
					/** VOL */
					ret = mpeg4_demux_vol(p_state, p_buf+i, &parse_len);
					//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vol\n"));
					
				}else if( p_buf[i+3] <= MPEG4_VIDEO_OBJECT_START_CODE_END )
				{
					/** video object */
					ret = -1;
					parse_len = MPEG4_VES_START_CODE_SIZE;
				}else
				{
					/** 未知起始码 */
					//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] unknow start code = 0x%x\n"), p_buf[i+3]);
					ret = -1;
					parse_len = MPEG4_VES_START_CODE_SIZE;
				}
			}
		}

		p_state->start_code = p_buf[i+3];
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			/** 解析成功 */
			*p_parse_len = i;
			
			return 0;

		}else if( ret == -2 || ret == -12 )
		{
			break;
		}
		
		/** 其余情况下忽略该起始码，继续解析 */
	}

	*p_parse_len = i;

	return -2;
}

int32 mpeg4_ves_parse2(MPEG4_VES_STATE *p_state, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;
	int32 parse_len = 0;

	int32 ret;

	int32 i;

	if( p_state == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	i = 0;
	while( left_len > MPEG4_VES_START_CODE_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			left_len--;
			continue;
		}

		p_state->p_start = p_buf + i;
		parse_len = left_len;

		/** 解析起始码 */
		switch( p_buf[i+3] )
		{
		case MPEG4_VOP_START_CODE:
			{
				ret = mpeg4_demux_vop(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vop = %d\n"), p_state->vop.frame_type);
			}
			break;

		case MPEG4_VOS_START_CODE:
			{
				ret = mpeg4_demux_vos_header(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vos\n"));
			}
			break;

		case MPEG4_VISUAL_OBJECT_START_CODE:
			{
				ret = mpeg4_demux_vo(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser][mpeg4_ves_parse] visual object\n"));
			}
			break;

		case MPEG4_GOP_START_CODE:
			{
				ret = mpeg4_demux_gop(p_state, p_buf+i, &parse_len);
				//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] gop\n"));
			}
			break;

		case MPEG4_USER_DATA_START_CODE:
			{
				ret = -1;
				parse_len = MPEG4_VES_START_CODE_SIZE;
				//log_debug(_T("[mpeg4_ves_parser][mpeg4_ves_parse] user data\n"));
			}
			break;

		default:
			{
				if( p_buf[i+3] >= MPEG4_VOL_START_CODE_BEGIN
					&& p_buf[i+3] <= MPEG4_VOL_START_CODE_END )
				{
					/** VOL */
					ret = mpeg4_demux_vol(p_state, p_buf+i, &parse_len);
					//log_debug(_T("[mpeg4_ves_parser::mpeg4_ves_parse] vol\n"));

				}else if( p_buf[i+3] <= MPEG4_VIDEO_OBJECT_START_CODE_END )
				{
					/** video object */
					ret = -1;
					parse_len = MPEG4_VES_START_CODE_SIZE;
				}else
				{
					/** 未知起始码 */
					//log_warn(_T("[mpeg4_ves_parser::mpeg4_ves_parse] unknow start code = 0x%x\n"), p_buf[i+3]);
					ret = -1;
					parse_len = MPEG4_VES_START_CODE_SIZE;
				}
			}
		}

		p_state->start_code = p_buf[i+3];
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			/** 解析成功 */
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

		/** 其余情况下忽略该起始码，继续解析 */
	}

	*p_parse_len = i;

	ret = -2;
	switch( left_len )
	{
	case 4:
	case 3:
		{
			if( (p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
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
