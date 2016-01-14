/**
 * @file bit_stream_reader.h  Bit Stream Reader
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.07.25
 *
 *
 */


#ifndef __BIT_STREAM_READER_H__
#define __BIT_STREAM_READER_H__

#include <public/gen_int.h>
#include <public/gen_def.h>

/** 每次最大可读取的比特数 */
#define BS_MAX_READ_BITS                           (32)

/** 比特流读取器 */
typedef struct tag_bit_stream_reader
{
	/** 缓冲区起始地址 */
	uint8  *p_buf;
	/** 缓冲区总长度，单位：字节 */
	int32  buf_size;
	/** 缓冲区总长度，单位：比特 */
	int32  total_bits;
	/** 缓冲区剩余长度，单位：比特 */
	int32  left_bits;

	/** 当前字节 */
	uint8  *p_cur;
	/** 当前比特位，1-8 */
	int32  cur_bit_pos;

} BIT_STREAM_READER, *PBIT_STREAM_READER;



#ifdef __cplusplus
extern "C"
{
#endif

/** bit mask table */
extern uint32 bs_reader_bm_table[];
/** se(v) table */
extern int32  bs_reader_se_table[];


/**  
 * @brief 初始化比特流
 *
 * @param [in] p_reader 流句柄
 * @param [in] p_buf 数据缓冲区
 * @param [in] size 缓冲区长度
 * @param [in] bit_pos 当前比特位，取值范围1-8
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_reader_init(BIT_STREAM_READER *p_reader, uint8 *p_buf, int32 size, int32 bit_pos);

/**  
 * @brief 从比特流读取比特
 *
 * @param [in] p_reader 流句柄
 * @param [out] p_value 输出数据
 * @param [in] bits 读取的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 * @remark
 * 1、读取的比特数不能超过32位!
 */
static inline int32 bs_read_bits(BIT_STREAM_READER *p_reader, int32 *p_value, int32 bits);

/**  
 * @brief 从比特流移除比特
 *
 * @param [in] p_reader 流句柄
 * @param [in] bits 读取的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 */
static inline int32 bs_reader_skip_bits(BIT_STREAM_READER *p_reader, int32 bits);

/**  
 * @brief 判断比特流是否满足所需的比特数
 *
 * @param [in] p_reader 流句柄
 * @param [in] bits 需要的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 */
static inline int32 bs_reader_need_bits(BIT_STREAM_READER *p_reader, int32 bits);

/**  
 * @brief 计算比特流当前位置到下一个1出现位置之间的0个数
 *
 * @param [in] p_reader 流句柄
 * @param [out] p_zero_bits 前缀0的个数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流直到结束未发现值为1的比特位
 */
static inline int32 bs_reader_count_zero_bits(BIT_STREAM_READER *p_reader, uint32 *p_zero_bits);

/**
 * @brief 从比特流读取无符号变长值
 *
 * @param [in] p_reader 流句柄
 * @param [out] p_value 输出数据
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 * @remark
 * 1、无符号变长值的含义参见H264文档说明
 */
static inline int32 bs_read_uv_bits(BIT_STREAM_READER *p_reader, int32 *p_value);

/**
 * @brief 计算se(v)值
 *
 * @param [in] p_reader 流句柄
 * @param [in] input_value 输入值
 * @param [out] p_value 输出值
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、se(v)值的含义参见H264标准里相关说明
 */
static inline int32 bs_reader_calc_se_var(BIT_STREAM_READER *p_reader, uint32 input_value, int32 *p_value);

/**  
 * @brief 将比特流当前比特对齐到字节边界
 *
 * @param [in] p_reader 流句柄
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_reader_align(BIT_STREAM_READER *p_reader);

/**  
 * @brief 当前比特流已消耗的字节数
 *
 * @param [in] p_reader 流句柄
 * @param [out] bytes 消耗的字节数
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_reader_get_use_bytes(BIT_STREAM_READER *p_reader, int32 *p_use_bytes);


static inline int32 bs_reader_init(BIT_STREAM_READER *p_reader, uint8 *p_buf, int32 size, int32 bit_pos)
{
	p_reader->p_buf = p_buf;
	p_reader->p_cur = p_buf;
	p_reader->buf_size = size;
	p_reader->total_bits = (size-1) * GEN_BYTE_BIT + bit_pos;
	p_reader->left_bits = p_reader->total_bits;
	p_reader->cur_bit_pos = bit_pos;

	return 0;
}

static inline int32 bs_read_bits(BIT_STREAM_READER *p_reader, int32 *p_value, int32 bits)
{
	int32 value = 0;
	/** 未读取的比特数 */
	int32 left_bits = bits;
	/** 数据位掩码 */
	//int32 bit_mask;

	*p_value = 0;
	if( p_reader->left_bits < bits )
	{
		return -2;
	}

	if( bits > BS_MAX_READ_BITS )
	{
		return -1;
	}

	while( left_bits > 0 )
	{
		if( p_reader->cur_bit_pos >= left_bits )
		{
			//bit_mask = 0xFF >> (GEN_BYTE_BIT - left_bits);
			//value |= ( p_reader->p_cur[0] >> (p_reader->cur_bit_pos - left_bits) ) & bit_mask;
			value |= ( p_reader->p_cur[0] >> (p_reader->cur_bit_pos - left_bits) ) & bs_reader_bm_table[left_bits];
			p_reader->cur_bit_pos -= left_bits;
			if( p_reader->cur_bit_pos == 0 )
			{
				p_reader->p_cur++;
				p_reader->cur_bit_pos = GEN_BYTE_BIT;
			}

			break;

		}else
		{
			//bit_mask = 0xFF >> (GEN_BYTE_BIT - p_reader->cur_bit_pos);
			left_bits -= p_reader->cur_bit_pos;

			//value |= ((p_reader->p_cur[0] & bit_mask) << left_bits);
			value |= ((p_reader->p_cur[0] & bs_reader_bm_table[p_reader->cur_bit_pos]) << left_bits);

			p_reader->p_cur++;
			p_reader->cur_bit_pos = GEN_BYTE_BIT;
		}
	}

	p_reader->left_bits -= bits;
	*p_value = value;

	return 0;
}

static inline int32 bs_reader_skip_bits(BIT_STREAM_READER *p_reader, int32 bits)
{
	/** 未读取的比特数 */
	int32 left_bits = bits;

	if( p_reader->left_bits < bits )
	{
		return -2;
	}

	while( left_bits > 0 )
	{
		if( p_reader->cur_bit_pos >= left_bits )
		{
			p_reader->cur_bit_pos -= left_bits;
			if( p_reader->cur_bit_pos == 0 )
			{
				p_reader->p_cur++;
				p_reader->cur_bit_pos = GEN_BYTE_BIT;
			}

			break;

		}else
		{
			left_bits -= p_reader->cur_bit_pos;

			p_reader->p_cur++;
			p_reader->cur_bit_pos = GEN_BYTE_BIT;
		}
	}

	p_reader->left_bits -= bits;

	return 0;
}

static inline int32 bs_reader_need_bits(BIT_STREAM_READER *p_reader, int32 bits)
{
	if( p_reader->left_bits < bits )
	{
		return -2;
	}

	return 0;
}

static inline int32 bs_reader_count_zero_bits(BIT_STREAM_READER *p_reader, uint32 *p_zero_bits)
{
	int32 value;
	/** 未读取的比特数 */
	int32 left_bits;
	/** 数据位掩码 */
	//int32 bit_mask;
	int32 zero_bits = 0;

	uint8 *p_cur;
	int32 cur_pos;
	int32 fore_pos;
	int32 b_find = 0;

	left_bits = p_reader->left_bits;
	p_cur = p_reader->p_cur;
	cur_pos = p_reader->cur_bit_pos;

	if( p_reader->left_bits < 1 )
	{
		return -2;
	}

	/** 取当前字节有效部分 */
	//bit_mask = 0xFF >> (GEN_BYTE_BIT - cur_pos);
	//value = p_cur[0] & bit_mask;
	value = p_cur[0] & bs_reader_bm_table[cur_pos];
	if( value != 0 )
	{
		/** 当前字节有效部分不为0 */
		cur_pos--;
		fore_pos = cur_pos;
		while( cur_pos >= 0 )
		{
			if( (value >> cur_pos) != 0 )
			{
				b_find = 1;
				break;
			}

			cur_pos--;
		}

		*p_zero_bits = (fore_pos - cur_pos);

		return 0;
	}

	p_cur++;
	left_bits -= cur_pos;
	zero_bits += cur_pos;

	while( left_bits > 0 )
	{
		/** 判断当前字节的值 */
		//value = p_cur[0];
		if( p_cur[0] != 0 )
		{
			/** 当前字节不为0 */
			cur_pos = GEN_BYTE_BIT - 1;
			fore_pos = cur_pos;
			while( cur_pos >= 0 )
			{
				if( (p_cur[0] >> cur_pos) != 0 )
				{
					b_find = 1;
					break;
				}

				cur_pos--;
			}

			if( b_find )
			{
				zero_bits += (fore_pos - cur_pos);
				*p_zero_bits = zero_bits;

				return 0;
			}
		}

		p_cur++;
		left_bits -= GEN_BYTE_BIT;
		zero_bits += GEN_BYTE_BIT;
	}

	*p_zero_bits = zero_bits;

	return -2;
}

static inline int32 bs_read_uv_bits(BIT_STREAM_READER *p_reader, int32 *p_value)
{
	int32  read_bits;
	uint32 zero_bits;

	int32 ret;

	ret = bs_reader_count_zero_bits(p_reader, &zero_bits);
	if( ret || zero_bits > (BS_MAX_READ_BITS-1)/2 )
	{
		return ret;
	}

	//read_bits = zero_bits * 2 + 1;
	read_bits = (zero_bits << 1) + 1;
	ret = bs_read_bits(p_reader, p_value, read_bits);

	if( ret )
	{
		return ret;
	}

	(*p_value)--;

	return 0;
}

static inline int32 bs_reader_calc_se_var(BIT_STREAM_READER *p_reader, uint32 input_value, int32 *p_value)
{
	if( input_value <= 6 )
	{
		*p_value = bs_reader_se_table[input_value];

	}else
	{
		if( input_value & 0x1 )
		{
			/** 奇数 */
			*p_value = input_value >> 1;

		}else
		{
			/** 偶数 */
			*p_value = (~(input_value >> 1) + 1);
		}
	}

	return 0;
}

static inline int32 bs_reader_align(BIT_STREAM_READER *p_reader)
{
	if( p_reader->cur_bit_pos != GEN_BYTE_BIT )
	{
		p_reader->p_cur++;
		p_reader->cur_bit_pos = GEN_BYTE_BIT;
	}

	return 0;
}

static inline int32 bs_reader_get_use_bytes(BIT_STREAM_READER *p_reader, int32 *p_use_bytes)
{
	*p_use_bytes = p_reader->p_cur - p_reader->p_buf;

	return 0;
}


#ifdef __cplusplus
}
#endif

#endif ///__BIT_STREAM_READER_H__
