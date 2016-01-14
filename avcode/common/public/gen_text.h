/**
 * @file gen_text.h  General Text Interface
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.12.26
 *
 *
 */

#ifndef __GEN_TEXT_H__
#define __GEN_TEXT_H__

#include <public/gen_int.h>
#include "gen_text_public.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief setup
 * @param [in] p_text, text
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_setup(GEN_TEXT* p_text);

/** 
 * @brief cleanup
 * @param [in] p_text, text
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_cleanup(GEN_TEXT* p_text);

/** 
 * @brief init
 * @param [in] p_text, text
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_init(GEN_TEXT* p_text);

/** 
 * @brief deinit
 * @param [in] p_text, text
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_deinit(GEN_TEXT* p_text);

/** 
 * @brief find char
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] p_char, char to find
 * @param [in] char_size, char size
 * @param [in] b_reverse, reverse find or not
 * @param [out] p_pos, find pos
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on success return, *p_pos indicate first find pos
 */
int32 gen_text_find_char(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_char, int32 char_size, int32 b_reverse, int32* p_pos);

/** 
 * @brief find string
 * @param [in] p_text, text
 * @param [in] p_str1, str1
 * @param [in] str1_size, str1 size
 * @param [in] p_str2, str2
 * @param [in] str2_size, str2 size
 * @param [in] b_reverse, reverse find or not
 * @param [out] p_pos, find pos
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on success return, *p_pos indicate first find pos
 */
int32 gen_text_find_str(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32 b_reverse, int32* p_pos);

/** 
 * @brief check decimal
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [out] p_b_decimal, is decimal or not
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_is_decimal(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32* p_b_decimal);

/** 
 * @brief check hex
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [out] p_b_hex, is hex or not
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_is_hex(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32* p_b_hex);

/** 
 * @brief transform hex string to binary data
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] p_data, output data buf
 * @param [in/out] p_data_size, data buf total size/actual transform size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_hex_to_bin(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_data, int32* p_data_size);

/** 
 * @brief transform string to uint32
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] radix, decimal or hex
 * @param [out] p_value, output value
 * @return
 * >= 0: success
 * < 0: fail
 * @remark
 * 1. the str must be decimal or hex string, else fail
 * 2. if str exceed 0xFFFFFFFF, it would cast to 0xFFFFFFFF
 */
int32 gen_text_str_to_uint32(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32 radix, uint32* p_value);

/** 
 * @brief check string is begin with sub string
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] p_sub_str, sub str
 * @param [in] sub_str_size, sub str size
 * @param [out] p_b_find, find or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if sub string is "" or sub string size is 0, the result is always true
 */
int32 gen_text_is_begin_with(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_sub_str, int32 sub_str_size, int32* p_b_find);

/** 
 * @brief check string is end with sub string
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] p_sub_str, sub str
 * @param [in] sub_str_size, sub str size
 * @param [out] p_b_find, find or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if sub string is "" or sub string size is 0, the result is always true
 */
int32 gen_text_is_end_with(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_sub_str, int32 sub_str_size, int32* p_b_find);

/** 
 * @brief compare string
 * @param [in] p_text, text
 * @param [in] p_str1, str1
 * @param [in] str1_size, str1 size
 * @param [in] p_str2, str2
 * @param [in] str2_size, str2 size
 * @param [out] p_result, compare result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. compare result as follow:
 *   str1 > str2:  1
 *   str1 = str2:  0
 *   str1 < str2:  -1
 */
int32 gen_text_strcmp(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32* p_result);

/** 
 * @brief compare string without case
 * @param [in] p_text, text
 * @param [in] p_str1, str1
 * @param [in] str1_size, str1 size
 * @param [in] p_str2, str2
 * @param [in] str2_size, str2 size
 * @param [out] p_result, compare result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. compare result as follow:
 *   str1 > str2:  1
 *   str1 = str2:  0
 *   str1 < str2:  -1
 */
int32 gen_text_stricmp(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32* p_result);

/** 
 * @brief make upper
 * @param [in] p_text, text
 * @param [in] p_str, str
 * @param [in] str_size, str size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_text_make_upper(GEN_TEXT* p_text, uint8* p_str, int32 str_size);

/** 
 * @brief make lower
 * @param [in] p_text, text
 * @param [in] p_str, str
 * @param [in] str_size, str size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_text_make_lower(GEN_TEXT* p_text, uint8* p_str, int32 str_size);

/** 
 * @brief trim blank
 * @param [in] p_text, text
 * @param [in] p_str, input str
 * @param [in] str_size, input str size
 * @param [in] p_blank, blank data
 * @param [in] blank_size, blank data size
 * @param [in] p_out, output buf
 * @param [in/out] p_out_size, output buf total size/output buf actual need size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. blank char indicate in blank data, eg. ' '(space) or '\t'(tab)
 * 2. if output buf can not hold whole output, GEN_E_NEED_MORE_BUF is return, with actual need size set properly
 */
int32 gen_text_trim_blank(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_blank, int32 blank_size, uint8* p_out, int32* p_out_size);

/** 
 * @brief replace str1 with str2
 * @param [in] p_text, text
 * @param [in] p_param, replace param
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on input, param's out_str_size must be specify, on output, it is set with actual output size; if output buf too small, GEN_E_NEED_MORE_BUF is return
 */
int32 gen_text_replace(GEN_TEXT* p_text, GEN_TEXT_REPLACE_PARAM* p_param);

/** 
 * @brief get line count
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_line_count, total line count
 * @return
 * 0: success
 * other: fail
 */
int32 gen_text_get_line_count(GEN_TEXT* p_text, uint8* p_data, int32 data_size, int32* p_line_count);

/** 
 * @brief get line size
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [in/out] p_text_line, text line
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on input, line number of p_text_line must be specify, which start from 1!
 * 2. line size including '\r\n', text size not involve
 */
int32 gen_text_get_line_size(GEN_TEXT* p_text, uint8* p_data, int32 data_size, GEN_TEXT_LINE* p_text_line);

/** 
 * @brief get max line size
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_text_line, max text line
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. one line seperate by '\r'¡¢'\n' or '\r\n'
 */
int32 gen_text_get_max_line_size(GEN_TEXT* p_text, uint8* p_data, int32 data_size, GEN_TEXT_LINE* p_text_line);

/** 
 * @brief get token
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [in] p_separator, separator buf
 * @param [in] separator_size, separator buf size
 * @param [in] token_number, token number
 * @param [out] p_token, token buf
 * @param [in/out] p_token_size, token buf total size/token buf actual size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. token separate by separator, which indicate in separator buf, eg. ' '(space) or '\t'(tab)
 * 2. token number start from 1
 * 3. if token buf less than token size + 4, GEN_E_NEED_MORE_BUF is return, with actual token size set properly
 */
int32 gen_text_get_token(GEN_TEXT* p_text, uint8* p_data, int32 data_size, uint8* p_separator, int32 separator_size, int32 token_number, uint8* p_token, int32* p_token_size);

/**
 * @brief get param pair
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_parse_size, parse size
 * @param [in] p_name, input name buf
 * @param [in/out] p_name_size, input name buf size/actual name size
 * @param [in] p_value, input value buf
 * @param [in/out] p_value_size, input value buf size/actual value size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. param in form of <name>=<value>, param token separate by separator, one line contain one param pair
 * 2. on success return, param name or value may be ""(null string), be careful!
 * 3. if line start with '#' or ';', this line treat as comment, would be ignore!
 */
int32 gen_text_get_param_pair(GEN_TEXT* p_text, uint8* p_data, int32 data_size, int32* p_parse_size, uint8* p_name, int32* p_name_size, uint8* p_value, int32* p_value_size);

/**
 * @brief get param value
 * @param [in] p_text, text
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [in] p_name, param name
 * @param [in] name_size, param name size
 * @param [out] p_value, param value buf
 * @param [in/out] p_value_size, param value buf total size/actual size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. param in form of <name>=<value>, param token separate by separator, one line contain one param pair
 * 2. if param not found, it would return GEN_E_NOT_FOUND
 * 3. on success return, param value may be ""(null string), be careful!
 * 4. if line start with '#' or ';', this line treat as comment, would be ignore!
 */
int32 gen_text_get_param_value(GEN_TEXT* p_text, uint8* p_data, int32 data_size, uint8* p_name, int32 name_size, uint8* p_value, int32* p_value_size);



#ifdef __cplusplus
}
#endif

#endif ///__GEN_TEXT_H__
