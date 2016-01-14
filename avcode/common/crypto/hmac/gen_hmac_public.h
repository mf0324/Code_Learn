/**
 * @file gen_hmac_public.h   HMAC Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.12
 *
 *
 */

#ifndef __GEN_HMAC_PUBLIC_H__
#define __GEN_HMAC_PUBLIC_H__


#include <public/gen_int.h>

#include <crypto/sha/gen_sha_public.h>

/** max key size */
#define GEN_HMAC_MAX_KEY_SIZE                                  (256)

/* hmac state */
typedef struct tag_gen_hmac_state
{
	/** sha type */
	int32   sha_type;

	/** hash size */
	int32   hash_size;
	/** block size */
	int32   block_size;

	/** key */
	uint8   key[GEN_HMAC_MAX_KEY_SIZE];
	/** key size */
	int32   key_size;

	/** sha */
	union
	{
		/** sha256 */
		GEN_SHA256_STATE sha256;

	} sha;

} GEN_HMAC_STATE, *PGEN_HMAC_STATE;


#endif //__GEN_HMAC_PUBLIC_H__
