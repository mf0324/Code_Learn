
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "mpeg2_aac_parser.h"

/** adts sampling frequency */
static int32 aac_sample_freq_table[16] =
{
	96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
	16000, 12000, 11025, 8000,  0, 0, 0, 0
};

/**  
 * @brief 解析ADTS包头
 *
 * @param [in] p_pack ADTS包句柄
 * @param [in] p_adts 输入数据地址
 * @param [in] p_adts_len 输入数据长度/已解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 */
static int32 adts_demux_header(ADTS_PACK *p_pack, uint8 *p_adts, int32 *p_adts_len)
{
	BIT_STREAM_READER bs;

	int32 left_len = *p_adts_len;

	int32 id;
	int32 layer;
	int32 sample_freq_index;

	int32 ret;

	*p_adts_len = 0;

	if( left_len < MPEG2_AAC_ADTS_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_adts, left_len, GEN_BYTE_BIT);

	/** fix header */
	/** syncword, 12 bit */
	ret = bs_reader_skip_bits(&bs, 12);

	/** id, 1 bit */
	ret = bs_read_bits(&bs, &id, 1);

	/** layer, 2 bit */
	ret = bs_read_bits(&bs, &layer, 2);
	if( layer != MPEG2_AAC_ADTS_LAYER )
	{
		goto ERR_FIN;
	}

	/** protection_absent, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->fix_header.b_protection_absent, 1);

	/** profile, 2 bit */
	ret = bs_read_bits(&bs, &p_pack->fix_header.profile, 2);
	if( p_pack->fix_header.profile == MPEG2_AAC_RESV_PROFILE )
	{
		goto ERR_FIN;
	}

	/** sample_freq_index, 4 bit */
	ret = bs_read_bits(&bs, &sample_freq_index, 4);
	p_pack->fix_header.sample_freq = aac_sample_freq_table[sample_freq_index];
	if( p_pack->fix_header.sample_freq == 0 )
	{
		goto ERR_FIN;
	}

	/** private_bit, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** channel_configuration, 3 bit */
	ret = bs_read_bits(&bs, &p_pack->fix_header.chn_config, 3);

	/** original+home, 1+1 bit */
	ret = bs_reader_skip_bits(&bs, 2);

	/** var header */
	/** copyright_identification_bit, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->var_header.b_copyright, 1);

	/** copyright_identification_start, 1 bit */
	ret = bs_read_bits(&bs, &p_pack->var_header.b_copyright_start, 1);

	/** aac_frame_length, 13 bit */
	ret = bs_read_bits(&bs, &p_pack->var_header.frame_len, 13);

	/** adts_buffer_fullness, 11 bit */
	ret = bs_read_bits(&bs, &p_pack->var_header.buf_fullness, 11);

	/** number_of_raw_data_blocks_in_frame, 2 bit */
	ret = bs_read_bits(&bs, &p_pack->var_header.rdb_num, 2);
	//p_pack->var_header.rdb_num++;

	*p_adts_len = MPEG2_AAC_ADTS_HEADER_SIZE;

	return 0;

ERR_FIN:
	bs_reader_get_use_bytes(&bs, p_adts_len);
	return -3;
}

///////////////////////////////////外部接口////////////////////////////////////
int32 aac_adts_init(ADTS_PACK *p_pack)
{
	if( p_pack == NULL )
	{
		return -1;
	}

	memset(p_pack, 0, sizeof(ADTS_PACK));

	return 0;
}

int32 aac_adts_parse(ADTS_PACK *p_pack, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;

	int32 parse_len;
	int32 i;
	int32 ret;

	if( p_pack == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	i = 0;
	while( left_len >= MPEG2_AAC_ADTS_HEADER_SIZE )
	{
		if( !( p_buf[i] == 0xFF && (p_buf[i+1] & 0xF0) == 0xF0 ) )
		{
			i++;
			left_len--;
			continue;
		}

		p_pack->p_start = p_buf + i;
		parse_len = left_len;

		ret = adts_demux_header(p_pack, p_buf+i, &parse_len);
		i += parse_len;
		left_len -= parse_len;

		if( ret == 0 )
		{
			*p_parse_len = i;

			return 0;

		}else if( ret == -2 )
		{
			break;
		}
	}

	*p_parse_len = i;

	return -2;
}

int32 aac_get_sample_freq_index(int32 sample_freq, int32 *p_index)
{
	int32 b_find;
	int32 i;

	if( p_index == NULL )
	{
		return -1;
	}

	b_find = 0;
	for( i = 0; i < sizeof(aac_sample_freq_table)/sizeof(int32); i++ )
	{
		if( aac_sample_freq_table[i] == sample_freq )
		{
			b_find = 1;
			*p_index = i;
			break;
		}
	}

	if( b_find )
	{
		return 0;

	}else
	{
		return -3;
	}
}