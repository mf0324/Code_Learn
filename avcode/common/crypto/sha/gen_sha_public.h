/**
 * @file gen_sha_public.h   SHA Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.10.12
 *
 *
 */

#ifndef __GEN_SHA_PUBLIC_H__
#define __GEN_SHA_PUBLIC_H__


#include <public/gen_int.h>


/* sha256 state */
typedef struct tag_gen_sha256_state
{
	/** length */
	uint64  length;
	/** state */
	uint32  state[8];
	/** current len */
	uint32  curlen;
	/** buf */
	uint8   buf[64];

} GEN_SHA256_STATE, *PGEN_SHA256_STATE;


#endif //__GEN_SHA_PUBLIC_H__
