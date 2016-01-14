

#include "bit_stream_writer.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint32 bs_writer_low_bm_table[] =
{
	0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF
};

uint32 bs_writer_high_bm_table[] =
{
	0, 0xFFFFFFFE, 0xFFFFFFFC, 0xFFFFFFF8, 0xFFFFFFF0, 0xFFFFFFE0, 0xFFFFFFC0, 0xFFFFFF80, 0xFFFFFF00
};

#ifdef __cplusplus
}
#endif
