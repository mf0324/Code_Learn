
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/bit_stream_reader.h>
#include "mpeg2_aes_parser.h"

/** audio layer I bitrate index table */
static int32 mpeg2_lay1_bitrate[16] =
{
	/** 0-7 */
	0,   32,  64,  96,  128, 160, 192, 224,
	/** 8-15 */
	256, 288, 320, 352, 384, 416, 448, 0
};

/** audio layer II bitrate index table */
static int32 mpeg2_lay2_bitrate[16] =
{
	/** 0-7 */
	0,   32,  48,  56,  64,  80,  96, 112,
	/** 8-15 */
	128, 160, 192, 224, 256, 320, 384, 0
};

/** audio layer III bitrate index table */
static int32 mpeg2_lay3_bitrate[16] =
{
	/** 0-7 */
	0,   32,  40,  48,  56,  64,  80,  96,
	/** 8-15 */
	112, 128, 160, 192, 224, 256, 320, 0
};

/** audio sampling frequency */
static int32 mpeg2_audio_sample[4] =
{
	44100, 48000, 32000, 0
};

/**  
 * @brief 解析MPEG2音频头
 *
 * @param [in] p_header 音频头句柄
 * @param [in] p_aes 输入数据地址
 * @param [in] p_aes_len 输入数据长度/已解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：数据不足以解析
 */
static int32 mpeg2_demux_audio_header(MPEG2_AES_HEADER *p_header, uint8 *p_aes, int32 *p_aes_len)
{
	BIT_STREAM_READER bs;

	int32 total_parse_len = 0;
	int32 left_len = *p_aes_len;

	int32 id;
	int32 protection_bit;
	int32 bit_rate_index;
	int32 sample_freq;
	int32 mode;
	int32 mode_ext;
	int32 emphasis;

	int32 ret;

	*p_aes_len = 0;

	if( left_len < MPEG2_AES_HEADER_SIZE )
	{
		return -2;
	}

	bs_reader_init(&bs, p_aes, left_len, GEN_BYTE_BIT);

	/** syncword, 12 bit */
	ret = bs_reader_skip_bits(&bs, 12);

	/** id, 1 bit */
	ret = bs_read_bits(&bs, &id, 1);
	if( id == 0 )
	{
		goto ERR_FIN;
	}

	/** layer, 2 bit */
	ret = bs_read_bits(&bs, &p_header->layer, 2);
	if( p_header->layer == 0x0 )
	{
		goto ERR_FIN;
	}

	/** protection_bit, 1 bit */
	ret = bs_read_bits(&bs, &protection_bit, 1);
#if 0
	if( protection_bit == 0x0 )
	{
		goto ERR_FIN;
	}
#endif

	/** bitrate_index, 4 bit */
	ret = bs_read_bits(&bs, &bit_rate_index, 4);
	if( bit_rate_index == 0xF )
	{
		goto ERR_FIN;
	}

	switch( p_header->layer )
	{
	case MPEG2_AUDIO_LAYER_I:
		{
			p_header->bit_rate = mpeg2_lay1_bitrate[bit_rate_index];
		}
		break;

	case MPEG2_AUDIO_LAYER_II:
		{
			p_header->bit_rate = mpeg2_lay2_bitrate[bit_rate_index];
		}
		break;

	case MPEG2_AUDIO_LAYER_III:
		{
			p_header->bit_rate = mpeg2_lay3_bitrate[bit_rate_index];
		}
		break;

	default:
		{
		}
		break;
	}

	if( p_header->bit_rate == 0 )
	{
		goto ERR_FIN;
	}

	/** sampling_frequency, 2 bit */
	ret = bs_read_bits(&bs, &sample_freq, 2);
	if( sample_freq == 0x3 )
	{
		goto ERR_FIN;
	}
	p_header->freq = mpeg2_audio_sample[sample_freq];

	/** 采样位宽恒为16 */
	p_header->bits = 16;

	/** padding_bit, 1 bit */
	ret = bs_read_bits(&bs, &p_header->b_padding, 1);
	if( p_header->b_padding && p_header->freq != 44100 )
	{
		goto ERR_FIN;
	}

	/** private_bit, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** mode, 2 bit */
	ret = bs_read_bits(&bs, &mode, 2);
	switch( mode )
	{
	case MPEG2_AUDIO_STEREO:
		{
			p_header->chn = 2;
		}
		break;

	case MPEG2_AUDIO_JOINT_STEREO:
		{
			p_header->chn = 2;
		}
		break;

	case MPEG2_AUDIO_DUAL_CHANNEL:
		{
			p_header->chn = 2;
		}
		break;

	case MPEG2_AUDIO_SINGLE_CHANNEL:
		{
			p_header->chn = 1;
		}
		break;

	default:
		break;
	}

	/** mode_extension, 2 bit */
	ret = bs_read_bits(&bs, &mode_ext, 2);

	/** copyright, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** original/home, 1 bit */
	ret = bs_reader_skip_bits(&bs, 1);

	/** emphasis, 2 bit */
	ret = bs_read_bits(&bs, &emphasis, 2);
	if( emphasis == 0x2 )
	{
		goto ERR_FIN;
	}

	total_parse_len += MPEG2_AES_HEADER_SIZE;
	*p_aes_len = total_parse_len;

	return 0;

ERR_FIN:
	bs_reader_get_use_bytes(&bs, p_aes_len);
	return -3;
}

///////////////////////////////////外部接口////////////////////////////////////
int32 mpeg2_aes_init(MPEG2_AES_HEADER *p_header)
{
	if( p_header == NULL )
	{
		return -1;
	}

	memset(p_header, 0, sizeof(MPEG2_AES_HEADER));

	return 0;
}

int32 mpeg2_aes_parse(MPEG2_AES_HEADER *p_header, uint8 *p_buf, int32 buf_len, int32 *p_parse_len)
{
	int32 left_len = buf_len;

	int32 parse_len;
	int32 i;
	int32 ret;

	if( p_header == NULL || p_buf == NULL || p_parse_len == NULL )
	{
		return -1;
	}

	*p_parse_len = 0;

	i = 0;
	while( left_len >= MPEG2_AES_HEADER_SIZE )
	{
		if( p_buf[i] != 0xFF || (p_buf[i+1] & 0xF0) != 0xF0 )
		{
			i++;
			left_len--;
			continue;
		}

		p_header->p_start = p_buf + i;
		parse_len = left_len;

		ret = mpeg2_demux_audio_header(p_header, p_buf+i, &parse_len);
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
