/**
 * @file bit_stream_writer.h  Bit Stream Writer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.08.25
 *
 *
 */


#ifndef __BIT_STREAM_WRITER_H__
#define __BIT_STREAM_WRITER_H__

#include <public/gen_int.h>
#include <public/gen_def.h>

/** 每次最大可写入的比特数 */
#define BS_MAX_WRITE_BITS                          (32)

/** 比特流写入器 */
typedef struct tag_bit_stream_writer
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

} BIT_STREAM_WRITER, *PBIT_STREAM_WRITER;


#ifdef __cplusplus
extern "C"
{
#endif

/** bit mask table(low bit) */
extern uint32 bs_writer_low_bm_table[];
/** bit mask table(high bit) */
extern uint32 bs_writer_high_bm_table[];

/**  
 * @brief 初始化比特流
 *
 * @param [in] p_writer 流句柄
 * @param [in] p_buf 数据缓冲区
 * @param [in] size 缓冲区长度
 * @param [in] bit_pos 当前比特位，取值范围1-8
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_writer_init(BIT_STREAM_WRITER *p_writer, uint8 *p_buf, int32 size, int32 bit_pos);

/**  
 * @brief 向比特流写入比特
 *
 * @param [in] p_writer 流句柄
 * @param [in] value 写入的值
 * @param [in] bits 写入的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 * @remark
 * 1、每次写入的比特数不能超过32位!
 */
static inline int32 bs_write_bits(BIT_STREAM_WRITER *p_writer, uint32 value, int32 bits);

/**  
 * @brief 从比特流跳过若干比特
 *
 * @param [in] p_writer 流句柄
 * @param [in] bits 跳过的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 */
static inline int32 bs_writer_skip_bits(BIT_STREAM_WRITER *p_writer, int32 bits);

/**  
 * @brief 判断剩下比特流是否满足所需的比特数
 *
 * @param [in] p_writer 流句柄
 * @param [in] bits 需要的比特数
 * @return
 * 0：成功
 * -1：失败
 * -2：比特流数据不足
 */
static inline int32 bs_writer_need_bits(BIT_STREAM_WRITER *p_writer, int32 bits);

/**  
 * @brief 将当前比特流位置对齐到字节边界
 *
 * @param [in] p_writer 流句柄
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_writer_align(BIT_STREAM_WRITER *p_writer);

/**  
 * @brief 当前比特流已消耗的字节数
 *
 * @param [in] p_stream 流句柄
 * @param [out] bytes 消耗的字节数
 * @return
 * 0：成功
 * -1：失败
 */
static inline int32 bs_writer_get_use_bytes(BIT_STREAM_WRITER *p_writer, int32 *p_use_bytes);


static inline int32 bs_writer_init(BIT_STREAM_WRITER *p_writer, uint8 *p_buf, int32 size, int32 bit_pos)
{
	p_writer->p_buf = p_buf;
	p_writer->p_cur = p_buf;
	p_writer->buf_size = size;
	p_writer->total_bits = (size-1) * GEN_BYTE_BIT + bit_pos;
	p_writer->left_bits = p_writer->total_bits;
	p_writer->cur_bit_pos = bit_pos;

	return 0;
}

static inline int32 bs_write_bits(BIT_STREAM_WRITER *p_writer, uint32 value, int32 bits)
{
	int32  left_bits;

	//uint32 temp_value;

	if( bits < 0 || bits > BS_MAX_WRITE_BITS )
	{
		return -1;
	}

	if( p_writer->left_bits < bits )
	{
		return -2;
	}

	left_bits = bits;
	while( left_bits > 0 )
	{
		if( p_writer->cur_bit_pos >= left_bits )
		{
			/** 直接写入 */
			//bit_mask = 0xFF >> (GEN_BYTE_BIT - left_bits);
			//temp_value = value & bit_mask;
			//p_writer->p_cur[0] =
			//	(p_writer->p_cur[0] & (0xFF << p_writer->cur_bit_pos)) | (temp_value << (p_writer->cur_bit_pos - left_bits));
			p_writer->p_cur[0] =
				(p_writer->p_cur[0] & bs_writer_high_bm_table[p_writer->cur_bit_pos]) | ((value & bs_writer_low_bm_table[left_bits]) << (p_writer->cur_bit_pos - left_bits));

			p_writer->cur_bit_pos -= left_bits;
			if( p_writer->cur_bit_pos == 0 )
			{
				p_writer->p_cur++;
				p_writer->cur_bit_pos = GEN_BYTE_BIT;
			}

			break;

		}else
		{
			//bit_mask = 0xFF >> (GEN_BYTE_BIT - p_writer->cur_bit_pos);
			//p_writer->p_cur[0] =
			//	(p_writer->p_cur[0] & (0xFF << p_writer->cur_bit_pos)) | ((value >> (left_bits - p_writer->cur_bit_pos)) & bit_mask);
			p_writer->p_cur[0] =
				(p_writer->p_cur[0] & bs_writer_high_bm_table[p_writer->cur_bit_pos]) | ((value >> (left_bits - p_writer->cur_bit_pos)) & bs_writer_low_bm_table[p_writer->cur_bit_pos]);

			left_bits -= p_writer->cur_bit_pos;
			p_writer->p_cur++;
			p_writer->cur_bit_pos = GEN_BYTE_BIT;
		}
	}

	p_writer->left_bits -= bits;

	return 0;
}

static inline int32 bs_writer_skip_bits(BIT_STREAM_WRITER *p_writer, int32 bits)
{
	int32 left_bits = bits;

	if( p_writer->left_bits < bits )
	{
		return -2;
	}

	while( left_bits > 0 )
	{
		if( p_writer->cur_bit_pos >= left_bits )
		{
			p_writer->cur_bit_pos -= left_bits;
			if( p_writer->cur_bit_pos == 0 )
			{
				p_writer->p_cur++;
				p_writer->cur_bit_pos = GEN_BYTE_BIT;
			}

			break;

		}else
		{
			left_bits -= p_writer->cur_bit_pos;

			p_writer->p_cur++;
			p_writer->cur_bit_pos = GEN_BYTE_BIT;
		}
	}

	p_writer->left_bits -= bits;

	return 0;
}

static inline int32 bs_writer_need_bits(BIT_STREAM_WRITER *p_writer, int32 bits)
{
	if( p_writer->left_bits < bits )
	{
		return -2;
	}

	return 0;
}

static inline int32 bs_writer_align(BIT_STREAM_WRITER *p_writer)
{
	if( p_writer->cur_bit_pos != GEN_BYTE_BIT )
	{
		p_writer->p_cur++;
		p_writer->cur_bit_pos = GEN_BYTE_BIT;
	}

	return 0;
}

static inline int32 bs_writer_get_use_bytes(BIT_STREAM_WRITER *p_writer, int32 *p_use_bytes)
{
	*p_use_bytes = p_writer->p_cur - p_writer->p_buf;

	return 0;
}

#ifdef __cplusplus
}
#endif

#endif ///__BIT_STREAM_WRITER_H__
