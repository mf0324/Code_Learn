

#include "bit_stream_reader.h"

uint32 bs_reader_bm_table[] =
{
	0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF
};

int32 bs_reader_se_table[] = 
{
	0, 1, -1, 2, -2, 3, -3
};
