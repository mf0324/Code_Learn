/**
 * @file gen_text_public.h  General Text Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.11
 *
 *
 */

#ifndef __GEN_TEXT_PUBLIC_H__
#define __GEN_TEXT_PUBLIC_H__

#include <public/gen_int.h>


/** text line */
typedef struct tag_gen_text_line
{
	/** line number */
	int32  number;
	/** start */
	uint8* p_start;
	/** line size(including CRLF) */
	int32  line_size;
	/** text size */
	int32  text_size;

} GEN_TEXT_LINE, *PGEN_TEXT_LINE;

/** replace param */
typedef struct tag_gen_text_replace_param
{
	/** origin str */
	uint8* p_orgin;
	/** origin str size */
	int32  origin_size;

	/** str1 */
	uint8* p_str1;
	/** str1 size */
	int32  str1_size;
	/** str2 */
	uint8* p_str2;
	/** str2 size */
	int32  str2_size;

	/** output str */
	uint8* p_out_str;
	/** output str size */
	int32  out_str_size;

} GEN_TEXT_REPLACE_PARAM, *PGEN_TEXT_REPLACE_PARAM;


/** text */
typedef struct tag_gen_text
{
	/** init */
	int32  b_init;

	/** encoding */
	int32  encoding;

} GEN_TEXT, *PGEN_TEXT;


#endif ///__GEN_TEXT_PUBLIC_H__
