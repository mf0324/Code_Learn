/**
 * @file gen_md5_public.h   MD5 Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.11.20
 *
 *
 */

#ifndef __GEN_MD5_PUBLIC_H__
#define __GEN_MD5_PUBLIC_H__


#include "gen_int.h"


/* Data structure for MD5 (Message-Digest) computation */
typedef struct tag_gen_md5_ctx
{
	/* number of _bits_ handled mod 2^64 */
	uint32  i[2];
	/* scratch buffer */
	uint32  buf[4];
	/* input buffer */
	uint8   in[64];
	/* actual digest after MD5Final call */
	uint8   digest[16];

} GEN_MD5_CTX, *PGEN_MD5_CTX;


#endif //__GEN_MD5_PUBLIC_H__
