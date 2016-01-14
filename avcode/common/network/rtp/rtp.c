
#include <string.h>

#include "../public/gen_endian.h"
#include "rtp.h"

#define RTP_HEADER_MIN_SIZE                        (sizeof(RTP_HEADER))
#define RTP_EXT_HEADER_MIN_SIZE                    (sizeof(RTP_EXT_HEADER))

/**
 * @brief 生成RTP头
 * @param [in] p_param 参数句柄
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_buf_len 缓冲区总长度/实际生成长度
 * @return
 * 0：成功
 * -1：失败
 * -2：缓冲区长度不足
 * -3：RTP头参数无效
 */
static int32 rtp_make_header(RTP_HEADER_PARAM *p_param, int8 *p_buf, int32 *p_buf_len)
{
	RTP_HEADER *p_header = (RTP_HEADER*)p_buf;
	int32 need_len;

	need_len = sizeof(RTP_HEADER) + p_param->csrc_num * RTP_CSRC_SIZE;
	if( *p_buf_len < need_len )
	{
		/** 长度不足 */
		*p_buf_len = need_len;
		return -2;
	}

	if( p_param->version != RTP_VERSION )
	{
		return -3;
	}

	if( p_param->csrc_num > RTP_MAX_CSRC_NUM )
	{
		return -3;
	}

	/** 版本 */
	p_header->flag = 0;
	p_header->flag |= ((p_param->version & 0x3) << 6);

	/** 填充标志 */
	p_header->flag |= ((p_param->b_pad & 0x1) << 5 );

	/** 扩展标志 */
	p_header->flag |= ((p_param->b_ext & 0x1) << 4 );

	/** CSRC个数 */
	p_header->flag |= (p_param->csrc_num & 0xF);

	/** 掩码标志 */
	p_header->mark_pt = ((p_param->b_mark & 0x1) << 7 );

	/** 载荷类型 */
	p_header->mark_pt |= (p_param->payload_type & 0x7F);

	/** 序列号 */
	p_header->seq = hton_u16(p_param->seq);

	/** 时间戳 */
	p_header->timestamp = hton_u32(p_param->timestamp);

	/** SSRC */
	p_header->ssrc = hton_u32(p_param->ssrc);

	if( p_param->csrc_num > 0 )
	{
		/** 复制CSRC列表 */
		if( p_param->p_csrc_data == NULL )
		{
			return -1;
		}

		memcpy(p_buf + sizeof(RTP_HEADER), p_param->p_csrc_data, p_param->csrc_num * RTP_CSRC_SIZE);
	}

	*p_buf_len = need_len;
	
	return 0;
}

/**
 * @brief 生成RTP扩展头
 * @param [in] p_ext_param 扩展参数句柄
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_buf_len 缓冲区总长度/实际生成长度
 * @return
 * 0：成功
 * -1：失败
 * -2：缓冲区长度不足
 * -3：参数无效
 */
static int32 rtp_make_ext_header(RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len)
{
	RTP_EXT_HEADER *p_ext_header = (RTP_EXT_HEADER*)p_buf;
	int32 need_len;

	need_len = sizeof(RTP_EXT_HEADER) + p_ext_param->ext_data_len * RTP_CSRC_SIZE;
	if( *p_buf_len < need_len )
	{
		*p_buf_len = need_len;
		return -2;
	}

	/** Profile */
	p_ext_header->profile = hton_u16(p_ext_param->profile);

	/** 扩展长度 */
	p_ext_header->ext_data_len = hton_u16(p_ext_param->ext_data_len);

	if( p_ext_param->ext_data_len > 0 )
	{
		if( p_ext_param->p_ext_data == NULL )
		{
			return -3;
		}

		memcpy(p_buf + sizeof(RTP_EXT_HEADER), p_ext_param->p_ext_data, p_ext_param->ext_data_len * RTP_CSRC_SIZE);
	}

	*p_buf_len = need_len;
	
	return 0;
}

/**
 * @brief 解析RTP头
 * @param [out] p_param 参数句柄
 * @param [in] p_buf 数据地址
 * @param [in/out] p_buf_len 数据总长度/已解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：输入数据不足以解析
 * -3：参数无效
 * @remark
 * 1、无论解析成功与否，p_buf_len均会返回已解析长度
 */
static int32 rtp_parse_header(RTP_HEADER_PARAM *p_param, int8 *p_buf, int32 *p_buf_len)
{
	RTP_HEADER *p_header = (RTP_HEADER*)p_buf;
	int32 left_len;
	int32 need_len;

	left_len = *p_buf_len;
	*p_buf_len = 0;
	if( left_len < RTP_HEADER_MIN_SIZE )
	{
		return -2;
	}

	/** CSRC个数 */
	p_param->csrc_num = (p_header->flag & 0xF);
	need_len = RTP_HEADER_MIN_SIZE + p_param->csrc_num * RTP_CSRC_SIZE;
	if( left_len < need_len )
	{
		return -2;
	}
	
	/** 版本 */
	p_param->version = ((p_header->flag & 0xC0) >> 6 );
	if( p_param->version != RTP_VERSION )
	{
		*p_buf_len = 1;
		return -3;
	}

	/** 填充标志 */
	p_param->b_pad = ((p_header->flag & 0x20) >> 5 );

	/** 扩展标志 */
	p_param->b_ext = ((p_header->flag & 0x10) >> 4 );
	
	/** 掩码标志 */
	p_param->b_mark = ((p_header->mark_pt & 0x80) >> 7 );
	
	/** 载荷类型 */
	p_param->payload_type = (p_header->mark_pt & 0x7F);
	
	/** 序列号 */
	p_param->seq = ntoh_u16(p_header->seq);

	/** 时间戳 */
	p_param->timestamp = ntoh_u32(p_header->timestamp);
	
	/** SSRC */
	p_param->ssrc = ntoh_u32(p_header->ssrc);
	
	if( p_param->csrc_num > 0 )
	{
		p_param->p_csrc_data = p_buf + sizeof(RTP_HEADER);

	}else
	{
		p_param->p_csrc_data = NULL;
	}

	*p_buf_len = need_len;

	return 0;
}

/**
 * @brief 解析RTP扩展头
 * @param [out] p_ext_param 扩展头参数句柄
 * @param [in] p_buf 输入数据地址
 * @param [in/out] p_buf_len 数据总长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：输入数据不足以解析
 * -3：RTP头格式错误
 */
static int32 rtp_parse_ext_header(RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len)
{
	RTP_EXT_HEADER *p_ext_header = (RTP_EXT_HEADER*)p_buf;
	int32 left_len;
	int32 need_len;

	left_len = *p_buf_len;
	*p_buf_len = 0;
	if( left_len < RTP_EXT_HEADER_MIN_SIZE )
	{
		return -2;
	}

	/** 扩展数据长度 */
	p_ext_param->ext_data_len = ntoh_u16(p_ext_header->ext_data_len);
	need_len = RTP_EXT_HEADER_MIN_SIZE + p_ext_param->ext_data_len * RTP_CSRC_SIZE;
	if( left_len < need_len )
	{
		return -2;
	}

	/** Profile */
	p_ext_param->profile = ntoh_u16(p_ext_header->profile);

	if( p_ext_param->ext_data_len > 0 )
	{
		p_ext_param->p_ext_data = p_buf + sizeof(RTP_EXT_HEADER);

	}else
	{
		p_ext_param->p_ext_data = NULL;
	}

	*p_buf_len = need_len;
	
	return 0;
}

///////////////////////////////////外部接口////////////////////////////////////////
int32 rtp_mux(RTP_HEADER_PARAM *p_param, RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len)
{
	int32 header_len;
	int32 left_len;
	int32 need_len;
	int32 pack_len;

	int32 ret;

	if( p_param == NULL || p_ext_param == NULL || p_buf == NULL || p_buf_len == NULL )
	{
		return -1;
	}

	need_len = RTP_HEADER_MIN_SIZE + p_param->csrc_num * RTP_CSRC_SIZE;
	if( p_param->b_ext )
	{
		need_len += (RTP_EXT_HEADER_MIN_SIZE + p_ext_param->ext_data_len * RTP_CSRC_SIZE);
	}

	left_len = *p_buf_len;
	if( left_len < need_len )
	{
		*p_buf_len = need_len;
		return -2;
	}

	header_len = 0;
	pack_len = left_len;
	ret = rtp_make_header(p_param, p_buf, &pack_len);
	if( ret )
	{
		return ret;
	}

	/** 更新缓冲区长度 */
	header_len += pack_len;
	left_len -= pack_len;

	if( p_param->b_ext )
	{
		pack_len = left_len;
		ret = rtp_make_ext_header(p_ext_param, p_buf + header_len, &pack_len);
		if( ret )
		{
			return ret;
		}

		header_len += pack_len;
	}

	*p_buf_len = header_len;

	return 0;
}

int32 rtp_demux(RTP_HEADER_PARAM *p_param, RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len)
{
	int32 header_len = 0;
	int32 left_len;
	int32 parse_len;

	int32 ret;

	if( p_param == NULL || p_ext_param == NULL || p_buf == NULL || p_buf_len == NULL )
	{
		return -1;
	}

	left_len = *p_buf_len;
	parse_len = *p_buf_len;
	ret = rtp_parse_header(p_param, p_buf, &parse_len);
	if( ret )
	{
		*p_buf_len = parse_len;
		return ret;
	}

	/** 更新缓冲区长度 */
	header_len += parse_len;
	left_len -= parse_len;

	if( p_param->b_ext )
	{
		/** 解析扩展头 */
		parse_len = left_len;
		ret = rtp_parse_ext_header(p_ext_param, p_buf + header_len, &parse_len);
		if( ret )
		{
			*p_buf_len = header_len + parse_len;
			return ret;
		}

		header_len += parse_len;
	}

	*p_buf_len = header_len;

	return 0;
}
