
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <util/log_agent.h>

#include "mp4_public.h"
#include "mp4_avc_demux.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 mp4_avc_dec_config_record_demux(AVC_DEC_CONFIG_RECORD *p_record, uint8 *p_buf, uint32 *p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;
	uint32 sps_count;
	uint16 sps_size;
	uint32 pps_count;
	uint16 pps_size;

	uint8* p_uint8 = NULL;

	uint32 i;

	if( p_record == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = MP4_AVC_DEC_CONFIG_RECORD_MIN_SIZE;
	if( left_size < need_size )
	{
		return -2;
	}

	p_uint8 = p_buf;
	total_size = 0;

	/** version + profile + compat + level */
	p_record->config_version = p_uint8[0];
	p_record->profile = p_uint8[1];
	p_record->profile_compat = p_uint8[2];
	p_record->level = p_uint8[3];
	p_uint8 += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	left_size -= MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;
	total_size += MP4_AVC_DEC_CONFIG_RECORD_HEADER_SIZE;

	/** length size minus 1, 2 bit */
	p_record->len_size_minus_1 = p_uint8[0] & 0x3;
	if( p_record->len_size_minus_1 != 3 )
	{
		*p_buf_size = total_size;
		return -3;
	}
	p_uint8 += 1;
	left_size -= 1;
	total_size += 1;

	/** sps count, 5 bit */
	sps_count = p_uint8[0] & 0x1F;
	p_uint8 += 1;
	left_size -= 1;
	total_size += 1;

	for( i = 0; i < sps_count; i++ )
	{
		if( left_size < MP4_AVC_SPS_RECORD_MIN_SIZE )
		{
			return -2;
		}

		sps_size = (((uint16)p_uint8[0]) << 8) | ((uint16)p_uint8[1]);
		p_uint8 += MP4_AVC_SPS_RECORD_MIN_SIZE;
		left_size -= MP4_AVC_SPS_RECORD_MIN_SIZE;
		total_size += MP4_AVC_SPS_RECORD_MIN_SIZE;

		if( left_size < sps_size )
		{
			return -2;
		}

		if( i < p_record->sps_count && p_record->p_sps )
		{
			p_record->p_sps[i].nalu_len = sps_size;
			p_record->p_sps[i].p_nalu = p_uint8;
		}

		p_uint8 += sps_size;
		left_size -= sps_size;
		total_size += sps_size;
	}

	/** pps count, 8 bit */
	pps_count = p_uint8[0];
	p_uint8 += 1;
	left_size -= 1;
	total_size += 1;

	for( i = 0; i < pps_count; i++ )
	{
		if( left_size < MP4_AVC_PPS_RECORD_MIN_SIZE )
		{
			return -2;
		}

		pps_size = (((uint16)p_uint8[0]) << 8) | ((uint16)p_uint8[1]);
		p_uint8 += MP4_AVC_PPS_RECORD_MIN_SIZE;
		left_size -= MP4_AVC_PPS_RECORD_MIN_SIZE;
		total_size += MP4_AVC_PPS_RECORD_MIN_SIZE;

		if( left_size < pps_size )
		{
			return -2;
		}

		if( i < p_record->pps_count && p_record->p_pps )
		{
			p_record->p_pps[i].nalu_len = pps_size;
			p_record->p_pps[i].p_nalu = p_uint8;
		}

		p_uint8 += pps_size;
		left_size -= pps_size;
		total_size += pps_size;
	}

	*p_buf_size = total_size;

	if( sps_count > p_record->sps_count || pps_count > p_record->pps_count )
	{
		p_record->sps_count = sps_count;
		p_record->pps_count = pps_count;
		return -4;
	}

	p_record->sps_count = sps_count;
	p_record->pps_count = pps_count;

	return 0;
}
