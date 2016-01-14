/**
 * @file gen_buf_public.h  Gen Buf Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.05.04
 *
 *
 */

#ifndef __GEN_BUF_PUBLIC_H__
#define __GEN_BUF_PUBLIC_H__

#include <public/gen_int.h>



/** buf */
typedef struct tag_gen_buf
{
	/** buf */
	uint8*  p_buf;

#if !RUN_OS_64

	/** buf total size */
	int32   buf_total_size;
	/** buf valid size */
	int32   buf_valid_size;
	/** buf use size */
	int32   buf_use_size;
	/** buf left size */
	int32   buf_left_size;

#else

	/** buf total size */
	int64   buf_total_size;
	/** buf valid size */
	int64   buf_valid_size;
	/** buf use size */
	int64   buf_use_size;
	/** buf left size */
	int64   buf_left_size;

#endif

} GEN_BUF, *PGEN_BUF;

/** round buf */
typedef struct tag_gen_round_buf
{
	/** buf */
	GEN_BUF  buf;
	/** begin */
	uint8*   p_begin;
	/** end */
	uint8*   p_end;

} GEN_ROUND_BUF, *PGEN_ROUND_BUF;

#endif ///__GEN_BUF_PUBLIC_H__
