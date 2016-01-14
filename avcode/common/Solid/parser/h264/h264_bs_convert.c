

#include <stdlib.h>
#include <string.h>

#include <public/gen_endian.h>
//#include <util/log_debug.h>

#include "h264_public.h"
#include "h264_bs_convert.h"


////////////////////////////////外部接口///////////////////////////////////////
int32 h264_bs_to_mp4_format(uint8* p_data, int32 data_size)
{
	int32 left_size;
	int32 b_find;
	int32 fore_pos;
	int32 nalu_count;
	int32 nalu_size;
	
	int32 i;
	int32 ret;

	if( p_data == NULL )
	{
		return -1;
	}

	if( data_size <= H264_BS_HEADER_SIZE )
	{
		return -2;
	}

	left_size = data_size;
	b_find = 0;
	fore_pos = 0;
	i = 0;
	nalu_count = 0;

	while( left_size > H264_BS_HEADER_SIZE )
	{
		if( !(p_data[i] == 0x00 && p_data[i+1] == 0x00 && p_data[i+2] == 0x00 && p_data[i+3] == 0x01) )
		{
			i++;
			left_size--;
			continue;
		}

		if( b_find )
		{
			nalu_size = i - fore_pos - 4;
			if( nalu_size < 0 )
			{
				nalu_size = 0;
			}

			//log_debug(_T("[mg3500_qbox_parser::mg3500_qbox2nalu] nalu %d size = %d\n"), nalu_count, nalu_size);
			//nalu_count++;

			/** change '00 00 00 01' to 4 byte size */
			nalu_size = hton_u32(nalu_size);
			memcpy(p_data + fore_pos, &nalu_size, 4);
		}

		b_find = 1;
		fore_pos = i;

		i += H264_BS_HEADER_SIZE;
		left_size -= H264_BS_HEADER_SIZE;
	}

	if( b_find )
	{
		nalu_size = data_size - fore_pos - 4;
		if( nalu_size < 0 )
		{
			nalu_size = 0;
		}

		//log_debug(_T("[mg3500_qbox_parser::mg3500_qbox2nalu] nalu %d size = %d\n"), nalu_count, nalu_size);
		//nalu_count++;

		/** change '00 00 00 01' to 4 byte size */
		nalu_size = hton_u32(nalu_size);
		memcpy(p_data + fore_pos, &nalu_size, 4);
	}

	return 0;
}

int32 h264_mp4_format_to_bs(uint8* p_data, int32 data_size)
{
	int32 left_size;
	int32 total_parse_size;
	int32 nalu_size;

	uint8* p_uint8 = NULL;

	//int32 i;
	int32 ret;

	if( p_data == NULL )
	{
		return -1;
	}

	if( data_size < H264_NALU_LENGTH_SIZE )
	{
		return -2;
	}

	p_uint8 = p_data;
	left_size = data_size;
	total_parse_size = 0;

	//i = 0;
	while( left_size > H264_NALU_LENGTH_SIZE )
	{
		memcpy(&nalu_size, p_uint8, H264_NALU_LENGTH_SIZE);
		nalu_size = ntoh_u32(nalu_size);

		if( left_size < (nalu_size + H264_NALU_LENGTH_SIZE) )
		{
			return -2;
		}

		/** 4 byte size to 00 00 00 01 */
		p_uint8[0] = 0x00;
		p_uint8[1] = 0x00;
		p_uint8[2] = 0x00;
		p_uint8[3] = 0x01;

		p_uint8 += (nalu_size + H264_NALU_LENGTH_SIZE);
		total_parse_size += (nalu_size + H264_NALU_LENGTH_SIZE);
		left_size -= (nalu_size + H264_NALU_LENGTH_SIZE);

		//log_debug(_T("[mg3500_qbox_parser::mg3500_qbox2nalu] nalu %d size = %d\n"), i, nalu_size);
		//i++;
	}

	return 0;
}
