
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "mpeg2_public.h"
#include "mpeg2_dvb_parser.h"

#include <util/log_debug.h>


#define MPEG2_DVB_ON_ERR_RET(ERR)                          {if( ERR ) return ERR;}

///////////////////////////////////外部接口////////////////////////////////////
int32 mpeg2_dvb_demux_sdt(MPEG2_TS_PACK *p_pack, MPEG2_DVB_SDT *p_sdt)
{
	BIT_STREAM_READER bs;
	int32 use_byte;

	int32 left_len;
	int32 parse_len;
	int32 unparse_len;

	int32 pointer_field;
	int32 section_len;
	int32 stream_id;
	int32 section_number;
	int32 last_section_number;
	int32 original_network_id;
	int32 sds_num;
	int32 max_sds_num;
	int32 descriptor_data_size;

	SDS_SECTION* p_section = NULL;
	uint8* p_uint8 = NULL;

	int32 i;
	int32 ret;

	if( p_pack == NULL || p_sdt == NULL )
	{
		return -1;
	}

	bs_reader_init(&bs, p_pack->p_payload, p_pack->payload_len, 8);
	left_len = p_pack->payload_len;

	if( p_pack->b_payload_start )
	{
		/** pointer_field, 8 bit */
		bs_read_bits(&bs, &pointer_field, 8);
		left_len -= 1;

		ret = bs_reader_skip_bits(&bs, pointer_field * GEN_BYTE_BIT);
		if( ret )
		{
			return ret;
		}
		left_len -= pointer_field;
	}

	/** table_id, 8 bit */
	bs_read_bits(&bs, &p_sdt->table_id, 8);
	switch( p_sdt->table_id )
	{
	case MPEG2_DVB_SDS_ACTUAL_TABLE_ID:
		{
			/** sds actual */
			if( p_sdt->buf.p_data == NULL )
			{
				return -1;
			}

			if( left_len < MPEG2_DVB_SDS_MIN_SIZE )
			{
				return -3;
			}
			left_len -= 1;

			/** section_syntax_indicator + reserved_future_use + reserved, 1+1+2 bit */
			bs_reader_skip_bits(&bs, 1+1+2);

			/** section_length, 12 bit */
			bs_read_bits(&bs, &section_len, 12);
			left_len -= 2;

			if( section_len > left_len )
			{
				return -3;
			}

			/** transport_stream_id, 16 bit */
			bs_read_bits(&bs, &stream_id, 16);
			left_len -= 2;

			/** reserved + version_number + current_next_indicator, 2 + 5 + 1 bit */
			bs_reader_skip_bits(&bs, 2+5+1);
			left_len -= 1;

			/** section_number, 8 bit */
			bs_read_bits(&bs, &section_number, 8);
			left_len -= 1;

			/** last_section_number, 8 bit */
			bs_read_bits(&bs, &last_section_number, 8);
			left_len -= 1;

			/** original_network_id, 16 bit */
			bs_read_bits(&bs, &original_network_id, 16);
			left_len -= 2;

			/** reserved_future_use, 8 bit */
			bs_reader_skip_bits(&bs, 8);
			left_len -= 1;

			bs_reader_get_use_bytes(&bs, &use_byte);
			p_uint8 = p_pack->p_payload + use_byte;
			unparse_len = section_len - 12;

			max_sds_num = p_sdt->size.data_size / sizeof(SDS_SECTION);
			p_section = p_sdt->buf.p_sds_section;
			sds_num = 0;
			i = 0;
			parse_len = 0;
			while( unparse_len >= MPEG2_DVB_SUB_SDS_MIN_SIZE )
			{
				if( i < max_sds_num )
				{
					/** service_id, 16 bit */
					bs_read_bits(&bs, &p_section->service_id, 16);
					/** reserved_future_use, 6 bit */
					bs_reader_skip_bits(&bs, 6);
					/** eit_schedule_flag, 1 bit */
					bs_read_bits(&bs, &p_section->eit_schedule_flag, 1);
					/** eit_present_flag, 1 bit */
					bs_read_bits(&bs, &p_section->eit_present_flag, 1);
					/** running_status, 3 bit */
					bs_read_bits(&bs, &p_section->running_status, 3);
					/** free_CA_mode, 1 bit */
					bs_reader_skip_bits(&bs, 1);
					/** descriptor_length, 12 bit */
					bs_read_bits(&bs, &p_section->descriptor_data_size, 12);
					unparse_len -= MPEG2_DVB_SUB_SDS_MIN_SIZE;

					p_uint8 += MPEG2_DVB_SUB_SDS_MIN_SIZE;
					p_section->p_descriptor_data = p_uint8;

					/** descriptor */
					bs_reader_skip_bits(&bs, p_section->descriptor_data_size * GEN_BYTE_BIT);
					unparse_len -= p_section->descriptor_data_size;

					p_section++;
					i++;

				}else
				{
					bs_reader_skip_bits(&bs, 28);
					/** descriptor_length, 12 bit */
					bs_read_bits(&bs, &descriptor_data_size, 12);
					unparse_len -= MPEG2_DVB_SUB_SDS_MIN_SIZE;

					bs_reader_skip_bits(&bs, descriptor_data_size * GEN_BYTE_BIT);
					unparse_len -= descriptor_data_size;
				}

				sds_num++;
			}

			p_sdt->size.sds_section_num = sds_num;

			if( max_sds_num < sds_num )
			{
				p_sdt->size.data_size = sds_num * sizeof(SDS_SECTION);
				return -2;
			}
		}
		break;

	default:
		return -3;
	}

	return 0;
}

int32 mpeg2_dvb_demux_sit(MPEG2_TS_PACK *p_pack, MPEG2_DVB_SIT *p_sit)
{
	BIT_STREAM_READER bs;
	int32 use_byte;

	int32 left_len;
	//int32 parse_len;
	int32 unparse_len;

	int32 pointer_field;
	int32 section_len;
	int32 section_number;
	int32 last_section_number;
	int32 loop_length;
	int32 section_num;
	int32 max_section_num;
	int32 descriptor_data_size;

	SIT_SECTION* p_section = NULL;
	uint8* p_uint8 = NULL;

	int32 i;
	int32 ret;

	if( p_pack == NULL || p_sit == NULL )
	{
		return -1;
	}

	bs_reader_init(&bs, p_pack->p_payload, p_pack->payload_len, 8);
	left_len = p_pack->payload_len;

	if( p_pack->b_payload_start )
	{
		/** pointer_field, 8 bit */
		ret = bs_read_bits(&bs, &pointer_field, 8);
		MPEG2_DVB_ON_ERR_RET(ret);
		left_len -= 1;

		ret = bs_reader_skip_bits(&bs, pointer_field * GEN_BYTE_BIT);
		MPEG2_DVB_ON_ERR_RET(ret);
		left_len -= pointer_field;
	}

	/** table_id, 8 bit */
	ret = bs_read_bits(&bs, &p_sit->table_id, 8);
	MPEG2_DVB_ON_ERR_RET(ret);
	if( p_sit->table_id != MPEG2_DVB_SIT_TABLE_ID )
	{
		return -3;
	}
	//left_len -= 1;

	if( left_len < MPEG2_DVB_SIT_MIN_SIZE )
	{
		return -3;
	}
	left_len -= 1;

	/** section_syntax_indicator + reserved_future_use + reserved, 1+1+2 bit */
	bs_reader_skip_bits(&bs, 1+1+2);

	/** section_length, 12 bit */
	bs_read_bits(&bs, &section_len, 12);
	left_len -= 2;

	if( section_len > left_len )
	{
		return -3;
	}

	/** dvb_reserved_future_use, 16 bit */
	bs_reader_skip_bits(&bs, 16);
	left_len -= 2;

	/** reserved + version_number + current_next_indicator, 2 + 5 + 1 bit */
	bs_reader_skip_bits(&bs, 2+5+1);
	left_len -= 1;

	/** section_number, 8 bit */
	bs_read_bits(&bs, &section_number, 8);
	left_len -= 1;

	/** last_section_number, 8 bit */
	bs_read_bits(&bs, &last_section_number, 8);
	left_len -= 1;

	/** dvb_reserved_future_use, 4 bit */
	bs_reader_skip_bits(&bs, 4);

	/** transmission_info_loop_length, 12 bit */
	bs_read_bits(&bs, &loop_length, 12);
	left_len -= 2;

	if( left_len < loop_length || section_len < loop_length )
	{
		return -3;
	}

	/** descriptor */
	p_sit->descriptor_size = loop_length;
	if( loop_length > 0 )
	{
		bs_reader_get_use_bytes(&bs, &use_byte);
		p_sit->p_descriptor = p_pack->p_payload + use_byte;

	}else
	{
		p_sit->p_descriptor = NULL;
	}
	bs_reader_skip_bits(&bs, loop_length * GEN_BYTE_BIT);
	left_len -= loop_length;

	bs_reader_get_use_bytes(&bs, &use_byte);
	p_uint8 = p_pack->p_payload + use_byte;
	unparse_len = section_len - (2 + 1 + 1 + 1 + 2 + loop_length + MPEG2_PSI_CRC32_SIZE);

	max_section_num = p_sit->section_num;
	p_section = p_sit->p_section;
	section_num = 0;
	i = 0;
	//parse_len = 0;
	while( unparse_len >= MPEG2_DVB_SUB_SIS_MIN_SIZE )
	{
		if( i < max_section_num )
		{
			/** service_id, 16 bit */
			bs_read_bits(&bs, &p_section->service_id, 16);
			/** reserved_future_use, 1 bit */
			bs_reader_skip_bits(&bs, 1);
			/** running_status, 3 bit */
			bs_read_bits(&bs, &p_section->running_status, 3);
			/** service_loop_length, 12 bit */
			bs_read_bits(&bs, &p_section->descriptor_data_size, 12);
			unparse_len -= MPEG2_DVB_SUB_SIS_MIN_SIZE;
			p_uint8 += MPEG2_DVB_SUB_SIS_MIN_SIZE;

			/** descriptor */
			if( p_section->descriptor_data_size > 0 )
			{
				p_section->p_descriptor_data = p_uint8;

			}else
			{
				p_section->p_descriptor_data = NULL;
			}

			bs_reader_skip_bits(&bs, p_section->descriptor_data_size * GEN_BYTE_BIT);
			unparse_len -= p_section->descriptor_data_size;
			p_uint8 += p_section->descriptor_data_size;

			p_section++;
			i++;

		}else
		{
			/** service_id + reserved + running_status, 16 + 1 + 3 bit */
			bs_reader_skip_bits(&bs, 16+1+3);
			/** service_loop_length, 12 bit */
			bs_read_bits(&bs, &descriptor_data_size, 12);
			unparse_len -= MPEG2_DVB_SUB_SIS_MIN_SIZE;
			p_uint8 += MPEG2_DVB_SUB_SIS_MIN_SIZE;

			bs_reader_skip_bits(&bs, descriptor_data_size * GEN_BYTE_BIT);
			unparse_len -= descriptor_data_size;
			p_uint8 += descriptor_data_size;
		}

		section_num++;
	}

	p_sit->section_num = section_num;

	if( max_section_num < section_num )
	{
		return -2;
	}

	return 0;
}

int32 mpeg2_dvb_demux_service_descriptor(DVB_SERVICE_DESCRIPTOR *p_descriptor, uint8 *p_buf, int32 len)
{
	uint8* p_uint8 = p_buf;
	int32  left_len;

	if( p_descriptor == NULL || p_buf == NULL )
	{
		return -1;
	}

	left_len = len;
	if( left_len < MPEG2_DVB_SERVICE_DESCRIPTOR_MIN_SIZE )
	{
		return -2;
	}

	left_len -= 2;
	p_uint8 += 2;

	/** service type */
	p_descriptor->type = p_uint8[0];
	p_descriptor->provider_name_size = p_uint8[1];
	p_descriptor->p_provider_name = (int8*)(p_uint8 + 2);

	left_len -= (p_descriptor->provider_name_size + 2);
	p_uint8 += (p_descriptor->provider_name_size + 2);

	if( left_len < 1 )
	{
		return -2;
	}

	/** service name */
	p_descriptor->service_name_size = p_uint8[0];
	p_descriptor->p_service_name = (int8*)(p_uint8 + 1);

	return 0;
}
