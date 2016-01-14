
#ifndef CRC32_H
#define CRC32_H

#include "gen_int.h"
#include "gen_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern const uint32 crc32_table[256];

inline uint32 crc32(int8 *p_buf, int32 len)
{
	uint32 total_value;
	int32  i;

	total_value = 0xFFFFFFFF;
	for( i=0; i<len; i++ )
	{
		total_value = crc32_table[(total_value ^ (p_buf[i])) & 0xff] ^ (total_value >> 8);
	}

	return ~total_value;
}

#ifdef __cplusplus
}
#endif

#endif
