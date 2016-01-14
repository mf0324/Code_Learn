
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_error.h>
//#include "../../public/bit_stream_reader.h"
#include "mpeg2_public.h"
#include "mpeg2_audio_demux.h"

/** bit rate table */
static const int32 mpeg2_audio_bit_rate[2][3][16] =
{
	/** mpeg1 */
	{
		/** layer1 */
		{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 },
		/** layer2 */
		{ 0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
		/** layer3 */
		{ 0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0 }
	},

	/** mpeg2 */
	{
		/** layer1 */
		{ 0, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0 },
		/** layer2 */
		{ 0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 },
		/** layer3 */
		{ 0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }
	}
};

/** sample rate */
static const int32 mpeg2_audio_sample_rate[2][4] =
{
	/** mpeg1 */
	{ 44100, 48000, 32000, 0 },
	/** mpeg2 */
	{ 22050, 24000, 16000, 0 }
};


/**  
 * @brief demux audio header
 * @param [in] p_header, header
 * @param [in] p_buf, input data
 * @param [in] p_buf_len, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 */
static int32 mpeg2_demux_audio_header(MPEG2_AUDIO_HEADER* p_header, uint8* p_buf, int32* p_buf_len)
{
	int32  total_parse_len = 0;
	int32  left_len = *p_buf_len;

	uint32 audio_header;

	int32  b_mpeg2_5;
	int32  version;
	int32  layer_index;
	int32  bit_rate_index;
	int32  sample_rate_index;

	//int32 ret;

	*p_buf_len = 0;

	audio_header = ((uint32)p_buf[0] << 24) | ((uint32)p_buf[1] << 16) | ((uint32)p_buf[2] << 8) | (p_buf[3]);

	/** syncword, 11 bit */

	/** b_mpeg2_5, 1 bit */
	b_mpeg2_5 = 1 - ((audio_header & 0x100000) >> 20);

	/** version, 1 bit */
	version = 1 - ((audio_header & 0x80000) >> 19);

	if( b_mpeg2_5 )
	{
		/** mpeg2.5 */
		p_header->version = MPEG2_AUDIO_VERSION_MPEG2_5;

	}else if( version == 0 )
	{
		/** mpeg1 */
		p_header->version = MPEG2_AUDIO_VERSION_MPEG1;

	}else
	{
		/** mpeg2 */
		p_header->version = MPEG2_AUDIO_VERSION_MPEG2;
	}

	/** layer, 2 bit */
	layer_index = (audio_header >> 17) & 0x3;
	if( layer_index == 0x0 )
	{
		goto ERR_FIN;
	}
	p_header->layer = 4 - layer_index;

	if( p_header->layer == 1 )
	{
		goto ERR_FIN;
	}

	/** protection_bit, 1 bit */
	p_header->b_crc = 1 - ((audio_header >> 16) & 0x1);

	/** bitrate_index, 4 bit */
	bit_rate_index = (audio_header >> 12) & 0xF;
	if( bit_rate_index == 0xF )
	{
		goto ERR_FIN;
	}

	p_header->bit_rate = mpeg2_audio_bit_rate[version][p_header->layer - 1][bit_rate_index];
	p_header->max_bit_rate = mpeg2_audio_bit_rate[version][p_header->layer - 1][14];

	/** sampling_frequency, 2 bit */
	sample_rate_index = (audio_header >> 10) & 0x3;
	if( sample_rate_index == 0x3 )
	{
		goto ERR_FIN;
	}
	p_header->sample_rate = mpeg2_audio_sample_rate[version][sample_rate_index];

	if( b_mpeg2_5 )
	{
		p_header->sample_rate /= 2;
	}

	/** sample bit */
	p_header->sample_bit = 16;

	/** padding_bit, 1 bit */
	p_header->b_padding = (audio_header >> 9) & 0x1;

	//if( p_header->sample_rate == 44100 && !p_header->b_padding )
	{
		//goto ERR_FIN;
	}

	/** private_bit, 1 bit */

	/** mode, 2 bit */
	p_header->mode = (audio_header >> 6) & 0x3;
	switch( p_header->mode )
	{
	case MPEG2_AUDIO_STEREO:
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
		{
		}
		break;
	}

	/** mode_extension, 2 bit */
	p_header->mode_ext = (audio_header >> 4) & 0x3;

	/** copyright, 1 bit */
	p_header->b_copyright = (audio_header >> 3) & 0x1;

	/** original/home, 1 bit */
	p_header->b_origin = (audio_header >> 2) & 0x1;

	/** emphasis, 2 bit */
	p_header->emphasis = audio_header & 0x3;
	if( p_header->emphasis == 0x2 )
	{
		goto ERR_FIN;
	}

	total_parse_len += MPEG2_AES_HEADER_SIZE;
	*p_buf_len = total_parse_len;

	return GEN_S_OK;

ERR_FIN:

	/** 0xFF + 0xE0 */
	*p_buf_len = 1;

	return GEN_E_WRONG_FORMAT;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg2_audio_header_init(MPEG2_AUDIO_HEADER *p_header)
{
	if( p_header == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_header, 0, sizeof(MPEG2_AUDIO_HEADER));

	return GEN_S_OK;
}

int32 mpeg2_audio_header_demux(MPEG2_AUDIO_HEADER* p_header, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32  left_size = data_size;
	int32  demux_size;

	int32 i;
	int32 ret;

	if( p_header == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	i = 0;
	while( left_size >= MPEG2_AES_HEADER_SIZE )
	{
		if( !(p_data[i] == 0xFF && (p_data[i+1] & 0xE0) == 0xE0) )
		{
			i++;
			left_size--;
			continue;
		}

		p_header->p_start = p_data + i;
		demux_size = left_size;

		ret = mpeg2_demux_audio_header(p_header, p_data + i, &demux_size);
		i += demux_size;
		left_size -= demux_size;

		if( ret == 0 )
		{
			*p_demux_size = i;

			return GEN_S_OK;

		}else if( ret == GEN_E_NEED_MORE_DATA )
		{
			break;
		}
	}

	*p_demux_size = i;

	return GEN_E_NEED_MORE_DATA;
}
