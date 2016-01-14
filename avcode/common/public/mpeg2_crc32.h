/**
 * @file mpeg2_crc32.h  MPEG2 CRC32
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.08.31
 *
 *
 */


#ifndef __MPEG2_CRC32_H__
#define __MPEG2_CRC32_H__

#include <public/gen_int.h>
#include <public/gen_def.h>


#ifdef __cplusplus
extern "C"
{
#endif

extern uint32 mpeg2_crc32_le_table[256];
extern uint32 mpeg2_crc32_be_table[256];

static inline uint32 mpeg2_crc32(uint8 *p_buf, int32 len)
{
	uint32 total_value;
	int32  i;

	total_value = 0xFFFFFFFF;

	for( i = 0; i < len; i++ )
	{
		total_value = (total_value << 8) ^ mpeg2_crc32_be_table[((total_value >> 24) ^ *p_buf++) &0xff];
	}

	return total_value;
}

#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_CRC32_H__
