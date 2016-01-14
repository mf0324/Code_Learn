
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include <public/gen_error.h>
#include <util/log_agent.h>

#include "mpeg2_pes_parser.h"




/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg2_pes_find_pes_header(MPEG2_PES_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_demux_size)
{
	int32  left_size = buf_size;
	//int32  parse_size;

	int32  i;
	int32  ret;


	if( p_pack == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;
	i = 0;

	while( left_size >= MPEG2_PES_HEADER_MIN_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			left_size--;
			continue;
		}

		if( p_buf[i+3] >= MPEG2_PES_SID_START )
		{
			/** pes header */
			p_pack->p_start = p_buf + i;
			p_pack->start_code = p_buf[i+3];
			/** pes_packet_length, 16 bit */
			p_pack->pes_pack_len = (((uint16)p_buf[i+4]) << 8) | ((uint16)p_buf[i+5]);

			if( p_pack->pes_pack_len > 0 )
			{
				p_pack->p_data = &p_buf[i+6];

			}else
			{
				p_pack->p_data = NULL;
			}

			*p_demux_size = i;

			return GEN_S_OK;
		}

		i += MPEG2_PES_START_CODE_SIZE;
		left_size -= MPEG2_PES_START_CODE_SIZE;

		/** ignore other start code */
	}

	*p_demux_size = i;

	return GEN_E_NEED_MORE_DATA;
}

int32 mpeg2_parse_pes(MPEG2_PES_PACK* p_pack, uint8* p_buf, int32 buf_size, int32* p_demux_size)
{
	int32  left_size = buf_size;
	int32  parse_size;

	int32  i;
	int32  ret;


	if( p_pack == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;
	i = 0;

	while( left_size >= MPEG2_PES_HEADER_MIN_SIZE )
	{
		if( !(p_buf[i] == 0x00 && p_buf[i+1] == 0x00 && p_buf[i+2] == 0x01) )
		{
			i++;
			left_size--;
			continue;
		}

		if( p_buf[i + 3] >= MPEG2_PES_SID_START )
		{
			parse_size = 0;
			p_pack->p_start = &p_buf[i];
			p_pack->start_code = p_buf[i+3];
			/** pes_packet_length, 16 bit */
			p_pack->pes_pack_len = p_buf[i+4];
			p_pack->pes_pack_len = (p_pack->pes_pack_len << 8) | p_buf[i+5];

			if( left_size > 0 )
			{
				p_pack->p_data = &p_buf[i+6];

			}else
			{
				p_pack->p_data = NULL;
			}

			*p_demux_size = i;

			return GEN_S_OK;
		}

		i += MPEG2_PES_START_CODE_SIZE;
		left_size -= MPEG2_PES_START_CODE_SIZE;

		/** ignore other start code */
	}

	*p_demux_size = i;

	return GEN_E_NEED_MORE_DATA;
}

int32 mpeg2_pes_demux_pes_header(MPEG2_PES_HEADER* p_header, uint8* p_data, int32 *p_data_size)
{
	BIT_STREAM_READER  bs;
	int32   value;

	int32   left_size;

	//int32   pes_header_data_size;

	//int32 ret;

	if( p_header == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	*p_data_size = 0;

	if( left_size < MPEG2_PES_HEADER_SIZE )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_header->p_start = p_data;
	p_header->start_code = p_data[3];

	bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** PES packet length, 16 bit */
	bs_read_bits(&bs, &p_header->pes_pack_len, 16);
	//if( (pes_pack_len > 0) && (pes_pack_len+MPEG2_PES_HEADER_MIN_SIZE) > left_len )
	{
		//	return -2;
	}

	/** misc flag, 2+2+1+1+1+1 bit */
	bs_reader_skip_bits(&bs, 2+2+1+1+1+1);

	/** pts dts flag, 2 bit */
	bs_read_bits(&bs, &p_header->pts_dts_flag, 2);

	/** misc flag, 6 bit */
	bs_reader_skip_bits(&bs, 6);

	/** pes header data len, 8 bit */
	bs_read_bits(&bs, &p_header->header_data_len, 8);
	if( left_size < (p_header->header_data_len + MPEG2_PES_HEADER_SIZE) )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_header->p_pack_data = p_data + (MPEG2_PES_HEADER_SIZE + p_header->header_data_len);
	p_header->pack_data_len = (MPEG2_PES_HEADER_MIN_SIZE + p_header->pes_pack_len) - (MPEG2_PES_HEADER_SIZE + p_header->header_data_len);
	if( p_header->pack_data_len < 0 )
	{
		p_header->pack_data_len = left_size - (MPEG2_PES_HEADER_SIZE + p_header->header_data_len);
	}

	switch( p_header->pts_dts_flag )
	{
	case 0x0:
		{
		}
		break;

	case 0x1:
		{
			*p_data_size = MPEG2_PES_HEADER_SIZE;
			return GEN_E_WRONG_FORMAT;
		}

	case 0x2:
		{
			/** pts, 33 bit */
			p_header->pts = 0;
			/** [32..30] */
			bs_reader_skip_bits(&bs, 4);
			bs_read_bits(&bs, &value, 3);
			p_header->pts = (p_header->pts | value) << 30;
			bs_reader_skip_bits(&bs, 1);

			/** [29..15] */
			bs_read_bits(&bs, &value, 15);
			p_header->pts |= (value << 15);
			bs_reader_skip_bits(&bs, 1);

			/** [14..0] */
			bs_read_bits(&bs, &value, 15);
			p_header->pts |= value;
			bs_reader_skip_bits(&bs, 1);
		}
		break;

	case 0x3:
		{
			/** pts, 33 bit */
			p_header->pts = 0;
			/** [32..30] */
			bs_reader_skip_bits(&bs, 4);
			bs_read_bits(&bs, &value, 3);
			p_header->pts = (p_header->pts | value) << 30;
			bs_reader_skip_bits(&bs, 1);

			/** [29..15] */
			bs_read_bits(&bs, &value, 15);
			p_header->pts |= (value << 15);
			bs_reader_skip_bits(&bs, 1);

			/** [14..0] */
			bs_read_bits(&bs, &value, 15);
			p_header->pts |= value;
			bs_reader_skip_bits(&bs, 1);

			/** dts, 33 bit */
			p_header->dts = 0;
			/** [32..30] */
			bs_reader_skip_bits(&bs, 4);
			bs_read_bits(&bs, &value, 3);
			p_header->dts = (p_header->dts | value) << 30;
			bs_reader_skip_bits(&bs, 1);

			/** [29..15] */
			bs_read_bits(&bs, &value, 15);
			p_header->dts |= (value << 15);
			bs_reader_skip_bits(&bs, 1);

			/** [14..0] */
			bs_read_bits(&bs, &value, 15);
			p_header->dts |= value;
			bs_reader_skip_bits(&bs, 1);
		}
		break;

	default:
		{
		}
		break;
	}

	*p_data_size = MPEG2_PES_HEADER_SIZE + p_header->header_data_len;

	return GEN_S_OK;
}

int32 mpeg2_pes_demux_ps_map(MPEG2_PS_MAP* p_ps_map, uint8* p_data, int32* p_data_size)
{
	BIT_STREAM_READER  bs;

	int32  left_size;
	int32  use_bytes;

	int32  ps_map_len;
	int32  ps_info_len;
	int32  es_map_len;

	//int32 ret;

	if( p_ps_map == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_data_size;
	*p_data_size = 0;

	if( left_size < MPEG2_PS_MAP_MIN_SIZE )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** ps map length, 16 bit */
	bs_read_bits(&bs, &ps_map_len, 16);
	if( left_size < (ps_map_len + MPEG2_PS_MAP_MIN_SIZE) )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	if( ps_map_len > MPEG2_PS_MAP_MAX_LEN )
	{
		*p_data_size = MPEG2_PS_MAP_MIN_SIZE;
		return GEN_E_WRONG_FORMAT;
	}

	/** indicator + reserved + version + reserved + marker, 1+2+5+7+1 bit */
	bs_reader_skip_bits(&bs, 1+2+5+7+1);

	/** ps info length, 16 bit */
	bs_read_bits(&bs, &ps_info_len, 16);
	if( ps_info_len > 0 )
	{
		bs_reader_get_use_bytes(&bs, &use_bytes);
		p_ps_map->p_descriptor = p_data + use_bytes;
		p_ps_map->descriptor_len = ps_info_len;

		bs_reader_skip_bits(&bs, ps_info_len * GEN_BYTE_BIT);

	}else
	{
		p_ps_map->p_descriptor = NULL;
		p_ps_map->descriptor_len = 0;
	}

	/** es map length, 16 bit */
	bs_read_bits(&bs, &es_map_len, 16);
	if( es_map_len > 0 )
	{
		bs_reader_get_use_bytes(&bs, &use_bytes);
		p_ps_map->p_es_map = p_data + use_bytes;
		p_ps_map->es_map_len = es_map_len;

		bs_reader_skip_bits(&bs, es_map_len * GEN_BYTE_BIT);

	}else
	{
		p_ps_map->p_es_map = NULL;
		p_ps_map->es_map_len = 0;
	}

	/** crc_32, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	*p_data_size = ps_map_len + MPEG2_PS_MAP_MIN_SIZE;

	return GEN_S_OK;
}

int32 mpeg2_pes_demux_es_map(MPEG2_ES_STREAM* p_stream, int32* p_stream_size, uint8* p_data, int32* p_data_size)
{
	//BIT_STREAM_READER  bs;

	int32   left_size;
	//int32   use_bytes;

	int32   stream_count;
	int32   max_stream_count;
	int32   desc_size;

	int32   i;
	int32   ret;

	if( p_stream == NULL || p_stream_size == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	max_stream_count = *p_stream_size;

	left_size = *p_data_size;
	//*p_data_size = 0;

	if( left_size < MPEG2_ES_MAP_MIN_SIZE )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	//bs_reader_init(&bs, p_data, left_size, GEN_BYTE_BIT);

	stream_count = 0;
	i = 0;
	while( left_size >= MPEG2_ES_MAP_MIN_SIZE )
	{
		desc_size = (((uint16)p_data[i+2]) << 8) | (p_data[i+3]);
		if( stream_count < max_stream_count )
		{
			p_stream[stream_count].stream_type = p_data[i];
			p_stream[stream_count].es_id = p_data[i+1];
			p_stream[stream_count].descriptor_size = desc_size;
			if( desc_size > 0 )
			{
				p_stream[stream_count].p_descriptor = p_data + i + 4;

			}else
			{
				p_stream[stream_count].p_descriptor = NULL;
			}
		}

		stream_count++;
		i += (MPEG2_ES_MAP_MIN_SIZE + desc_size);
		left_size -= (MPEG2_ES_MAP_MIN_SIZE + desc_size);
	}

	if( stream_count > max_stream_count )
	{
		*p_stream_size = stream_count;
		return GEN_E_NEED_MORE_BUF;
	}

	*p_stream_size = stream_count;

	return GEN_S_OK;
}

int32 mpeg2_demux_ps_dir(MPEG2_PS_DIR *p_ps_dir, uint8 *p_buf, int32 *p_buf_len)
{
	BIT_STREAM_READER bs;
	int32 value;

	int32 left_len;
	int32 use_bytes;

	int32 ps_dir_len;
	int32 au_num;
	//int32 au_len;

	//int32 ret;

	if( p_ps_dir == NULL || p_buf == NULL || p_buf_len == NULL )
	{
		return -1;
	}

	left_len = *p_buf_len;
	*p_buf_len = 0;

	if( left_len < MPEG2_PS_DIRECTORY_MIN_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_buf, left_len, GEN_BYTE_BIT);

	/** start code, 32 bit */
	bs_reader_skip_bits(&bs, 32);

	/** PS directory length, 16 bit */
	bs_read_bits(&bs, &ps_dir_len, 16);
	if( (ps_dir_len + MPEG2_PS_DIRECTORY_MIN_SIZE) > left_len )
	{
		return -2;
	}

	/** au num, 15 bit */
	bs_read_bits(&bs, &au_num, 15);
	p_ps_dir->au_num = au_num;
	bs_reader_skip_bits(&bs, 1);

	/** prev dir offset, 45 bit */
	p_ps_dir->prev_dir_offset = 0;
	/** [44..30] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->prev_dir_offset = (p_ps_dir->prev_dir_offset | value) << 30;
	bs_reader_skip_bits(&bs, 1);

	/** [29..15] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->prev_dir_offset |= (value << 15);
	bs_reader_skip_bits(&bs, 1);

	/** [14..0] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->prev_dir_offset |= value;
	bs_reader_skip_bits(&bs, 1);

	/** next dir offset, 45 bit */
	p_ps_dir->next_dir_offset = 0;
	/** [44..30] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->next_dir_offset = (p_ps_dir->next_dir_offset | value) << 30;
	bs_reader_skip_bits(&bs, 1);

	/** [29..15] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->next_dir_offset |= (value << 15);
	bs_reader_skip_bits(&bs, 1);

	/** [14..0] */
	bs_read_bits(&bs, &value, 15);
	p_ps_dir->next_dir_offset |= value;
	bs_reader_skip_bits(&bs, 1);

	/** au */
	if( au_num > 0 )
	{
		bs_reader_get_use_bytes(&bs, &use_bytes);
		p_ps_dir->p_au = p_buf + use_bytes;
		p_ps_dir->au_len = au_num * MPEG2_PS_DIRECTORY_AU_SIZE;

		bs_reader_skip_bits(&bs, au_num * MPEG2_PS_DIRECTORY_AU_SIZE * GEN_BYTE_BIT);
	}

	*p_buf_len = ps_dir_len + MPEG2_PS_DIRECTORY_MIN_SIZE;

	return 0;
}

int32 mpeg2_demux_descriptor(MPEG2_DESCRIPTOR *p_descriptor, uint8 *p_buf, int32 *p_buf_len)
{
	int32 left_len;

	int32 i;
	int32 ret;

	if( p_descriptor == NULL || p_buf == NULL || p_buf_len == NULL )
	{
		return -1;
	}

	i = 0;
	left_len = *p_buf_len;
	if( left_len < 2 )
	{
		return -2;
	}
	
	i += 2;
	left_len -= 2;

	p_descriptor->tag = p_buf[0];
	p_descriptor->data_size = p_buf[1];
	if( left_len < p_descriptor->data_size )
	{
		return -2;
	}

	p_descriptor->p_data = p_buf + 2;

	i += p_descriptor->data_size;
	*p_buf_len = i;
	
	return 0;
}
