
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_error.h>
#include <public/bit_stream_writer.h>
#include <util/log_debug.h>

#include "mpeg2_aac_const.h"
#include "mpeg2_aac_mux.h"


/**  
 * @brief build adts header
 * @param [in] p_header, aac adts header
 * @param [in] p_buf, output buf
 * @param [in/out] p_buf_size, output buf total size/actually mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if return value is GEN_E_NEED_MORE_BUF, *p_buf_size means actually need size
 */
static int32 mpeg2_build_aac_adts_header(MPEG2_AAC_ADTS_HEADER* p_header, uint8* p_buf, int32* p_buf_size)
{
	BIT_STREAM_WRITER bs;

	int32  left_size;

	//int32  ret;

	left_size = *p_buf_size;
	if( left_size < MPEG2_AAC_ADTS_HEADER_SIZE )
	{
		*p_buf_size = MPEG2_AAC_ADTS_HEADER_SIZE;
		return GEN_E_NEED_MORE_BUF;
	}

	bs_writer_init(&bs, p_buf, left_size, GEN_BYTE_BIT);

	/** syncword, 12 bit */
	bs_write_bits(&bs, 0xFFF, 12);

	/** id, 1 bit */
	bs_write_bits(&bs, MPEG2_AAC_MPEG2_ID, 1);

	/** layer, 2 bit */
	bs_write_bits(&bs, 0x0, 2);

	/** protection_absent, 1 bit */
	bs_write_bits(&bs, p_header->protection_absent, 1);

	/** profile, 2 bit */
	bs_write_bits(&bs, p_header->profile, 2);

	/** sample_freq_index, 4 bit */
	bs_write_bits(&bs, p_header->sample_freq_index, 4);

	/** private_bit, 1 bit */
	bs_write_bits(&bs, p_header->b_private, 1);

	/** channel_configuration, 3 bit */
	bs_write_bits(&bs, p_header->chn_config, 3);

	/** original_copy, 1 bit */
	bs_write_bits(&bs, p_header->b_original, 1);

	/** home, 1 bit */
	bs_write_bits(&bs, p_header->home, 1);

	/** copyright_id_bit, 1 bit */
	bs_write_bits(&bs, p_header->b_copyright_id, 1);

	/** copyright_id_start, 1 bit */
	bs_write_bits(&bs, p_header->b_copyright_id_start, 1);

	/** aac_frame_len, 13 bit */
	bs_write_bits(&bs, p_header->frame_len, 13);

	/** adts_buffer_fullness, 11 bit */
	bs_write_bits(&bs, p_header->buf_fullness, 11);

	/** number_of_raw_data_blocks, 2 bit */
	bs_write_bits(&bs, p_header->raw_data_block, 2);

	*p_buf_size = MPEG2_AAC_ADTS_HEADER_SIZE;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 mpeg2_aac_adts_header_mux(MPEG2_AAC_ADTS_HEADER* p_header, uint8* p_buf, int32* p_buf_size)
{
	int32 ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return -1;
	}

	ret = mpeg2_build_aac_adts_header(p_header, p_buf, p_buf_size);

	return ret;
}
