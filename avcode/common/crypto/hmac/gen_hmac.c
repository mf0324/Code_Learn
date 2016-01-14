
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>

#include <crypto/sha/gen_sha256.h>
#include "gen_hmac_const.h"
#include "gen_hmac.h"

/** digest size */
#define GEN_SHA256_DIGEST_SIZE                                 (32)
/** block size */
#define GEN_SHA256_BLOCK_SIZE                                  (64)

/** max hash size */
#define GEN_HMAC_MAX_HASH_SIZE                                 (256)

/**  
 * @brief hash data
 * @param [in] p_state, hmac state
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_buf, output buf
 * @param [in/out] p_buf_size, output buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 gen_hmac_hash_data(GEN_HMAC_STATE* p_state, uint8* p_data, int32 data_size, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   mux_size;

	int32   ret;

	left_size = *p_buf_size;

	switch( p_state->sha_type )
	{
	case GEN_HMAC_SHA_TYPE_SHA256:
		{
			need_size = GEN_SHA256_DIGEST_SIZE;
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
		break;
	}

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	ret = gen_sha256_init(&p_state->sha.sha256);
	if( ret )
	{
		return ret;
	}

	ret = gen_sha256_process(&p_state->sha.sha256, p_data, data_size);
	if( ret )
	{
		return ret;
	}

	mux_size = left_size;
	ret = gen_sha256_done(&p_state->sha.sha256, p_buf, &mux_size);
	if( ret )
	{
		return ret;
	}

	*p_buf_size = mux_size;

	return GEN_S_OK;
}


/////////////////////////////// Outter Interface //////////////////////////////
int32 gen_hmac_setup(GEN_HMAC_STATE* p_state)
{
	if( p_state == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_state, 0, sizeof(*p_state));

	return GEN_S_OK;
}

int32 gen_hmac_init(GEN_HMAC_STATE* p_state, uint8* p_key, int32 key_size)
{
	uint8*  p_block_buf = NULL;
	int32   block_buf_size;

	int32   hash_size;
	int32   block_size;
	int32   mux_size;

	int32   b_error;
	int32   error_code;
	int32   i;
	int32   ret;

	if( p_state == NULL || p_key == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( key_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = GEN_S_OK;

	switch( p_state->sha_type )
	{
	case GEN_HMAC_SHA_TYPE_SHA256:
		{
			hash_size = GEN_SHA256_DIGEST_SIZE;
			block_size = GEN_SHA256_BLOCK_SIZE;

			p_state->hash_size = GEN_SHA256_DIGEST_SIZE;
			p_state->block_size = GEN_SHA256_BLOCK_SIZE;
			p_state->key_size = GEN_SHA256_BLOCK_SIZE;
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
		break;
	}

	p_block_buf = (uint8*)malloc(block_size);
	if( p_block_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	block_buf_size = block_size;

	/* (1) make sure we have a large enough key */
	if( key_size > block_size)
	{
		mux_size = block_size;
		ret = gen_hmac_hash_data(p_state, p_key, key_size, p_state->key, &mux_size);

		if( hash_size < block_size )
		{
			memset(p_state->key + hash_size, 0, block_size - hash_size);
		}
		key_size = hash_size;

	}else
	{
		if( key_size > 0 )
		{
			memcpy(p_state->key, p_key, key_size);
		}

		if( key_size < block_size )
		{
			memset(p_state->key + key_size, 0, block_size - key_size);
		}
	}

	/* Create the initial vector for step (3) */
	for( i = 0; i < block_size; i++ )
	{
		p_block_buf[i] = p_state->key[i] ^ 0x36;
	}

	/* Pre-pend that to the hash data */
	ret = gen_sha256_init(&p_state->sha.sha256);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	ret = gen_sha256_process(&p_state->sha.sha256, p_block_buf, block_size);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

FIN:

	if( p_block_buf )
	{
		free(p_block_buf);
		p_block_buf = NULL;
	}

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 gen_hmac_process(GEN_HMAC_STATE* p_state, uint8* p_data, int32 data_size)
{
	int32   ret;

	if( p_state == NULL || p_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	switch( p_state->sha_type )
	{
	case GEN_HMAC_SHA_TYPE_SHA256:
		{
			ret = gen_sha256_process(&p_state->sha.sha256, p_data, data_size);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
		break;
	}

	return ret;
}

int32 gen_hmac_done(GEN_HMAC_STATE* p_state, uint8* p_buf, int32* p_buf_size)
{
	uint8*  p_block_buf = NULL;
	int32   mux_size;

	uint8   hash_buf[GEN_HMAC_MAX_HASH_SIZE];
	int32   hash_size;

	int32   b_error;
	int32   error_code;
		
	int32   i;
	int32   ret;

	if( p_state == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	/* get the hash message digest size */
	hash_size = p_state->hash_size;

	if( *p_buf_size < hash_size )
	{
		*p_buf_size = hash_size;
		return GEN_E_NEED_MORE_BUF;
	}

	b_error = 0;
	error_code = GEN_S_OK;

	/* allocate buffers */
	p_block_buf = (uint8*)malloc(p_state->block_size);
	if( p_block_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	/* Get the hash of the first LTC_HMAC vector plus the data */
	mux_size = hash_size;
	ret = gen_sha256_done(&p_state->sha.sha256, hash_buf, &mux_size);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	/* Create the second LTC_HMAC vector vector for step (3) */
	for( i = 0; i < p_state->block_size; i++ )
	{
		p_block_buf[i] = p_state->key[i] ^ 0x5C;
	}

	/* Now calculate the "outer" hash for step (5), (6), and (7) */
	ret = gen_sha256_init(&p_state->sha.sha256);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	ret = gen_sha256_process(&p_state->sha.sha256, p_block_buf, p_state->block_size);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	ret = gen_sha256_process(&p_state->sha.sha256, hash_buf, hash_size);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	mux_size = hash_size;
	ret = gen_sha256_done(&p_state->sha.sha256, p_block_buf, &mux_size);
	if( ret )
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

	/* copy to output  */
	for( i = 0; i < hash_size; i++ )
	{
		p_buf[i] = p_block_buf[i];
	}

	*p_buf_size = hash_size;

FIN:

	if( p_block_buf )
	{
		free(p_block_buf);
		p_block_buf = NULL;
	}

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 gen_hmac_calc(GEN_HMAC_STATE* p_state, uint8* p_key, int32 key_size, uint8* p_data, int32 data_size, uint8* p_buf, int32* p_buf_size)
{
	int32   ret;

	if( p_state == NULL || p_key == NULL || p_data == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	ret = gen_hmac_init(p_state, p_key, key_size);
	ret = gen_hmac_process(p_state, p_data, data_size);
	ret = gen_hmac_done(p_state, p_buf, p_buf_size);

	return ret;
}