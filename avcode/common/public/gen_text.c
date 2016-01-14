
#include <stdio.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_error.h>

#include "gen_text_const.h"
#include "gen_text.h"


/** space */
#define GEN_TEXT_SPACE                                     ' '
/** tab */
#define GEN_TEXT_TAB                                       '\t'
/** CR */
#define GEN_TEXT_CR                                        '\r'
/** LF */
#define GEN_TEXT_LF                                        '\n'
/** path delimeter */
#define GEN_TEXT_DIR_DELIMETER                             '/'

/** start line number */
#define GEN_TEXT_START_LINE_NUMBER                         (1)

/** delimeter */
static uint8 gen_text_delimeter[] =
{
	' ',
	'\t',
	'\r',
	'\n'
};

/** separator */
static uint8 gen_text_separator[] = 
{
	/** space */
	' ',
	/** tab */
	'\t',
	/** carriage return */
	'\r',
	/** line feed */
	'\n'
};


/** hex string table */
static uint8 gen_text_hex_table[] = 
{
	/** 0 - 15 */
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 48 - 63 */
	1, 1, 1, 1, 1, 1, 1, 1,    1, 1, 0, 0, 0, 0, 0, 0,
	/** 64 - 79 */
	0, 1, 1, 1, 1, 1, 1, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 96 - 112 */
	0, 1, 1, 1, 1, 1, 1, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 128+ */
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0
};

/** hex to bin tabale */
static uint8 gen_text_hex_to_bin_table[] = 
{
	/** 0 - 15 */
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 48 - 63 */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,    0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/** 64 - 79 */
	0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00,    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 96 - 112 */
	0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00,    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	/** 128+ */
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0
};

/** 
 * @brief check string is separator
 * @param [in] p_text, text
 * @param [in] p_separator, separator buf
 * @param [in] separator_size, separator buf size
 * @param [in] p_char, char
 * @param [in] char_size, char size
 * @param [out] p_b_separator, separator or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 
 */
static inline int32 gen_text_is_separator(GEN_TEXT* p_text, uint8* p_separator, int32 separator_size, uint8* p_char, int32 char_size, int32* p_b_separator)
{
	int32   i;

	*p_b_separator = 0;

	i = 0;
	while( i < separator_size )
	{
		if( p_separator[i] == p_char[0] )
		{
			*p_b_separator = 1;
			return GEN_S_OK;
		}

		i++;
	}

	return GEN_S_OK;
}

/** 
 * @brief check string is blank
 * @param [in] p_text, text
 * @param [in] p_blank, blank buf
 * @param [in] blank_size, blank buf size
 * @param [in] p_char, char
 * @param [in] char_size, char size
 * @param [out] p_b_blank, blank or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 
 */
static inline int32 gen_text_is_blank(GEN_TEXT* p_text, uint8* p_blank, int32 blank_size, uint8* p_char, int32 char_size, int32* p_b_blank)
{
	int32   i;

	*p_b_blank = 0;

	i = 0;
	while( i < blank_size )
	{
		if( p_blank[i] == p_char[0] )
		{
			*p_b_blank = 1;
			return GEN_S_OK;
		}

		i++;
	}

	return GEN_S_OK;
}

/** 
 * @brief compare string(ignore case)
 * @param [in] p_text, text
 * @param [in] p_str1, str1
 * @param [in] p_str2, str2
 * @param [in] str_size, str size
 * @param [out] p_result, compare result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 
 */
static inline int32 gen_text_ignore_case_cmp(GEN_TEXT* p_text, uint8* p_str1, uint8* p_str2, int32 str_size, int32* p_result)
{
	uint8   data1;
	uint8   data2;
	int32   i;

	i = 0;
	while( i < str_size )
	{
		if( p_str1[i] != p_str2[i] )
		{
			data1 = p_str1[i];
			data2 = p_str2[i];

			/** check 'A'-> 'Z' */
			if( data1 >= 0x41 && data1 <= 0x5A )
			{
				/** lower case */
				data1 += 0x20;
			}

			if( data1 > data2 )
			{
				*p_result = 1;
				return GEN_S_OK;

			}else if( data1 < data2 )
			{
				*p_result = -1;
				return GEN_S_OK;

			}else
			{
				/** equal */
			}
		}

		i++;
	}

	*p_result = 0;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 gen_text_setup(GEN_TEXT* p_text)
{
	if( p_text == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_text, 0, sizeof(*p_text));

	return GEN_S_OK;
}

int32 gen_text_cleanup(GEN_TEXT* p_text)
{
	if( p_text == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_text->b_init )
	{
		gen_text_deinit(p_text);
	}

	return GEN_S_OK;
}

int32 gen_text_init(GEN_TEXT* p_text)
{
	if( p_text == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_text->b_init )
	{
		return GEN_S_OK;
	}

	p_text->encoding = GEN_TEXT_ENCODING_ASCII;

	p_text->b_init = 1;

	return GEN_S_OK;
}

int32 gen_text_deinit(GEN_TEXT* p_text)
{
	if( p_text == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_text->b_init = 0;

	return GEN_S_OK;
}

int32 gen_text_find_char(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_char, int32 char_size, int32 b_reverse, int32* p_pos)
{
	int32   end_pos;
	int32   i;
	int32   ret;

	if( p_text == NULL || p_str == NULL || p_char == NULL || p_pos == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_pos = -1;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( str_size < 0 || char_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( char_size > str_size )
	{
		return GEN_E_NOT_FOUND;

	}else if( char_size == str_size )
	{
		ret = memcmp(p_str, p_char, char_size);
		if( ret == 0 )
		{
			*p_pos = 0;
			return GEN_S_OK;

		}else
		{
			*p_pos = -1;
			return GEN_E_NOT_FOUND;
		}

	}else
	{
		/** char size < str size */
		if( b_reverse )
		{
			/** reverse find */
			end_pos = 0;
			i = str_size - char_size;

			while( i >= end_pos )
			{
				if( p_str[i] == p_char[0] )
				{
					ret = memcmp(p_str + i, p_char, char_size);
					if( ret == 0 )
					{
						*p_pos = i;
						return GEN_S_OK;
					}
				}

				i--;
			}

		}else
		{
			/** sequence find */
			end_pos = str_size - char_size;
			i = 0;

			while( i <= end_pos )
			{
				if( p_str[i] == p_char[0] )
				{
					ret = memcmp(p_str + i, p_char, char_size);
					if( ret == 0 )
					{
						*p_pos = i;
						return GEN_S_OK;
					}
				}

				i++;
			}
		}

	}

	return GEN_E_NOT_FOUND;
}

int32 gen_text_find_str(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32 b_reverse, int32* p_pos)
{
	int32   end_pos;
	int32   i;

	int32   ret;

	if( p_text == NULL || p_str1 == NULL || p_str2 == NULL || p_pos == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_pos = -1;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( str1_size < 0 || str2_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( str2_size > str1_size )
	{
		return GEN_E_NOT_FOUND;

	}else if( str2_size == str1_size )
	{
		ret = memcmp(p_str1, p_str2, str2_size);
		if( ret == 0 )
		{
			*p_pos = 0;
			return GEN_S_OK;

		}else
		{
			*p_pos = -1;
			return GEN_E_NOT_FOUND;
		}

	}else
	{
		/** str2 size < str1 size */
		if( b_reverse )
		{
			/** reverse find */
			end_pos = 0;
			i = str1_size - str2_size;

			while( i >= end_pos )
			{
				if( p_str1[i] == p_str2[0] )
				{
					ret = memcmp(p_str1 + i, p_str2, str2_size);
					if( ret == 0 )
					{
						*p_pos = i;
						return GEN_S_OK;
					}
				}

				i--;
			}

		}else
		{
			/** sequence find */
			end_pos = str1_size - str2_size;
			i = 0;

			while( i <= end_pos )
			{
				if( p_str1[i] == p_str2[0] )
				{
					ret = memcmp(p_str1 + i, p_str2, str2_size);
					if( ret == 0 )
					{
						*p_pos = i;
						return GEN_S_OK;
					}
				}

				i++;
			}
		}
	}

	return GEN_E_NOT_FOUND;
}

int32 gen_text_is_decimal(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32* p_b_decimal)
{
	int32   i;
	//int32   ret;

	if( p_text == NULL || p_str == NULL || p_b_decimal == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_b_decimal = 0;

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( str_size == 0 )
	{
		return GEN_S_OK;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	i = 0;
	while( i < str_size )
	{
		/** '0'->0x30, '9'->0x39 */
		if( p_str[i] < 0x30 || p_str[i] > 0x39 )
		{
			*p_b_decimal = 0;
			return GEN_S_OK;
		}

		i++;
	}

	*p_b_decimal = 1;

	return GEN_S_OK;
}

int32 gen_text_is_hex(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32* p_b_hex)
{
	int32   i;

	if( p_text == NULL || p_str == NULL || p_b_hex == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_b_hex = 0;

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( str_size == 0 )
	{
		return GEN_S_OK;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	i = 0;
	while( i < str_size )
	{
		if( !gen_text_hex_table[p_str[i]] )
		{
			*p_b_hex = 0;
			return GEN_S_OK;
		}

		i++;
	}

	*p_b_hex = 1;

	return GEN_S_OK;
}

int32 gen_text_hex_to_bin(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_data, int32* p_data_size)
{
	int32   data_buf_size;
	int32   value_size;
	int32   b_low;
	int32   i;
	int32   j;

	if( p_text == NULL || p_str == NULL || p_data == NULL || p_data_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	data_buf_size = *p_data_size;
	*p_data_size = 0;

	if( str_size <= 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	/** two hex -> one byte */
	value_size = (str_size + 1) / 2;
	if( data_buf_size < value_size )
	{
		*p_data_size = value_size;
		return GEN_E_NEED_MORE_BUF;
	}

	/** prevent hex str size is odd  */
	p_data[0] = 0;

	i = str_size - 1;
	j = value_size - 1;
	b_low = 1;

	while( i >= 0 )
	{
		if( b_low )
		{
			p_data[j] = gen_text_hex_to_bin_table[p_str[i]];
			b_low = 0;

		}else
		{
			p_data[j] |= (gen_text_hex_to_bin_table[p_str[i]] << 4 );
			b_low = 1;

			j--;
		}

		i--;
	}

	*p_data_size = value_size;

	return GEN_S_OK;
}

int32 gen_text_str_to_uint32(GEN_TEXT* p_text, uint8* p_str, int32 str_size, int32 radix, uint32* p_value)
{
	int32   begin_pos;
	int32   pos;
	int32   b_negative;
	uint32  base;
	uint32  total;

	int32   i;
	int32   j;
	//int32   result;

	if( p_text == NULL || p_str == NULL || p_value == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_value = 0;

	if( str_size <= 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	switch( radix )
	{
	case GEN_TEXT_RADIX_DECIMAL:
		{
			/** decimal */
			if( p_str[0] == '-' )
			{
				/** negative */
				begin_pos = 1;
				b_negative = 1;

				//str_size -= 1;

			}else
			{
				/** positive */
				begin_pos = 0;
				b_negative = 0;
			}

			total = 0;
			for( i = str_size - 1; i >= begin_pos; i-- )
			{
				base = 1;
				j = str_size - 1;
				while( j > i )
				{
					base = base * 10;
					j--;
				}

				total += (base * (p_str[i] - '0'));
			}

			if( b_negative )
			{
				/** negative number */
				total = ~total + 1;
			}
		}
		break;

	case GEN_TEXT_RADIX_HEX:
		{
			/** hex */
			if( str_size > 8 )
			{
				total = UINT32_MAX;
				break;
			}

			total = 0;
			pos = 0;
			for( i = str_size - 1; i >= 0; i-- )
			{
				if( p_str[i] >= 'a' && p_str[i] <= 'f' )
				{
					j = 10 + p_str[i] - 'a';

				}else if( p_str[i] >= 'A' && p_str[i] <= 'F' )
				{
					j = 10 + p_str[i] - 'A';

				}else
				{
					j = p_str[i] - '0';
				}

				total = total | (j << pos);
				pos += 4;
			}
		}
		break;

	default:
		{
			return GEN_E_NOT_IMPLEMENT;
		}
	}

	*p_value = total;

	return GEN_S_OK;
}

int32 gen_text_is_begin_with(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_sub_str, int32 sub_str_size, int32* p_b_find)
{
	//int32   i;
	int32   ret;

	if( p_text == NULL || p_str == NULL || p_sub_str == NULL || p_b_find == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_b_find = 0;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( sub_str_size == 0 )
	{
		/** ""(empty str) */
		*p_b_find = 1;
		return GEN_S_OK;
	}

	if( sub_str_size > str_size )
	{
		*p_b_find = 0;
		return GEN_S_OK;
	}

	ret = memcmp(p_str, p_sub_str, sub_str_size);
	if( ret == 0 )
	{
		*p_b_find = 1;

	}else
	{
		*p_b_find = 0;
	}

	return GEN_S_OK;
}

int32 gen_text_is_end_with(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_sub_str, int32 sub_str_size, int32* p_b_find)
{
	int32   pos;
	int32   ret;

	if( p_text == NULL || p_str == NULL || p_sub_str == NULL || p_b_find == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_b_find = 0;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( sub_str_size == 0 )
	{
		/** ""(empty str) */
		*p_b_find = 1;
		return GEN_S_OK;
	}

	if( sub_str_size > str_size )
	{
		*p_b_find = 0;
		return GEN_S_OK;
	}

	pos = str_size - sub_str_size;
	ret = memcmp(p_str + pos, p_sub_str, sub_str_size);
	if( ret == 0 )
	{
		*p_b_find = 1;

	}else
	{
		*p_b_find = 0;
	}

	return GEN_S_OK;
}

int32 gen_text_strcmp(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32* p_result)
{
	int32   ret;

	if( p_text == NULL || p_str1 == NULL || p_str2 == NULL || p_result == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_result = -1;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( str1_size < 0 || str2_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( str1_size == 0 && str2_size == 0 )
	{
		*p_result = 0;
		return GEN_S_OK;
	}

	if( str1_size > str2_size )
	{
		ret = memcmp(p_str1, p_str2, str2_size);
		if( ret == 0 )
		{
			*p_result = 1;

		}else
		{
			*p_result = ret;
		}

	}else if( str1_size < str2_size )
	{
		ret = memcmp(p_str1, p_str2, str1_size);
		if( ret == 0 )
		{
			*p_result = -1;

		}else
		{
			*p_result = ret;
		}

	}else
	{
		ret = memcmp(p_str1, p_str2, str1_size);
		*p_result = ret;
	}

	return GEN_S_OK;
}

int32 gen_text_stricmp(GEN_TEXT* p_text, uint8* p_str1, int32 str1_size, uint8* p_str2, int32 str2_size, int32* p_result)
{
	int32   compare_result;
	int32   ret;

	if( p_text == NULL || p_str1 == NULL || p_str2 == NULL || p_result == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_result = -1;

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	if( str1_size < 0 || str2_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( str1_size == 0 && str2_size == 0 )
	{
		*p_result = 0;
		return GEN_S_OK;
	}

	if( str1_size > str2_size )
	{
		ret = gen_text_ignore_case_cmp(p_text, p_str1, p_str2, str2_size, &compare_result);
		if( compare_result == 0 )
		{
			/** str1 longer than str2 */
			*p_result = 1;

		}else
		{
			*p_result = compare_result;
		}

	}else if( str1_size < str2_size )
	{
		ret = gen_text_ignore_case_cmp(p_text, p_str1, p_str2, str1_size, &compare_result);
		if( compare_result == 0 )
		{
			/** str1 short than str2 */
			*p_result = -1;

		}else
		{
			*p_result = compare_result;
		}

	}else
	{
		ret = gen_text_ignore_case_cmp(p_text, p_str1, p_str2, str1_size, &compare_result);
		*p_result = compare_result;
	}

	return GEN_S_OK;
}

int32 gen_text_make_upper(GEN_TEXT* p_text, uint8* p_str, int32 str_size)
{
	int32   i;

	if( p_text == NULL || p_str == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	i = 0;
	while( i < str_size )
	{
		/** check 'a'-> 'z' */
		if( p_str[i] >= 0x61 && p_str[i] <= 0x7A )
		{
			/** upper case */
			p_str[i] -= 0x20;
		}

		i++;
	}

	return GEN_S_OK;
}

int32 gen_text_make_lower(GEN_TEXT* p_text, uint8* p_str, int32 str_size)
{
	int32   i;

	if( p_text == NULL || p_str == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	if( str_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	i = 0;
	while( i < str_size )
	{
		/** A -> Z */
		if( p_str[i] >= 0x41 && p_str[i] <= 0x5A )
		{
			/** lower case */
			p_str[i] += 0x20;
		}

		i++;
	}

	return GEN_S_OK;
}

int32 gen_text_trim_blank(GEN_TEXT* p_text, uint8* p_str, int32 str_size, uint8* p_blank, int32 blank_size, uint8* p_out, int32* p_out_size)
{
	int32   out_buf_size;
	int32   token_size;
	int32   begin_pos;
	int32   end_pos;

	int32   b_blank;
	int32   left_size;

	int32   ret;
	int32   i;

	if( p_text == NULL || p_str == NULL || p_blank == NULL || p_out == NULL || p_out_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	out_buf_size = *p_out_size;

	p_out[0] = 0x00;
	*p_out_size = 0;

	if( str_size < 0 || blank_size < 1 || out_buf_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	/** trim left */
	i = 0;

	while( i < str_size )
	{
		ret = gen_text_is_blank(p_text, p_blank, blank_size, p_str + i, 1, &b_blank);
		if( !b_blank )
		{
			break;
		}

		i++;
	}

	begin_pos = i;
	left_size = str_size - i;

	if( left_size < 1 )
	{
		/** all blank */
		return GEN_S_OK;
	}

	/** trim right */
	i = str_size - 1;

	while( i > begin_pos )
	{
		ret = gen_text_is_blank(p_text, p_blank, blank_size, p_str + i, 1, &b_blank);
		if( !b_blank )
		{
			break;
		}

		i--;
	}

	end_pos = i;

	token_size = end_pos - begin_pos + 1;

	if( out_buf_size >= (token_size + 4) )
	{
		memcpy(p_out, p_str + begin_pos, token_size);
		p_out[token_size] = 0x00;
		p_out[token_size + 1] = 0x00;

		*p_out_size = token_size;
		return GEN_S_OK;

	}else
	{
		*p_out_size = token_size;
		return GEN_E_NEED_MORE_BUF;
	}
}

int32 gen_text_replace(GEN_TEXT* p_text, GEN_TEXT_REPLACE_PARAM* p_param)
{
	int32   out_buf_size;
	int32   out_str_size;
	uint8*  p_src = NULL;
	uint8*  p_dest = NULL;

	int32   b_error;
	int32   error_code;

	int32   i;
	int32   ret;

	if( p_text == NULL || p_param == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	if( p_param->p_orgin == NULL || p_param->p_str1 == NULL || p_param->p_str2 == NULL || p_param->p_out_str == NULL || out_buf_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	out_buf_size = p_param->out_str_size;

	p_param->p_out_str[0] = 0x00;
	p_param->out_str_size = 0;

	if( p_param->str1_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_param->str2_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_param->str1_size > p_param->origin_size )
	{
		/** str1 not found */
		if( out_buf_size < (p_param->origin_size + 4) )
		{
			p_param->out_str_size = p_param->origin_size + 4;
			return GEN_E_NEED_MORE_BUF;
		}

		memcpy(p_param->p_out_str, p_param->p_orgin, p_param->origin_size);
		p_param->p_out_str[p_param->origin_size] = 0x00;
		p_param->p_out_str[p_param->origin_size + 1] = 0x00;
		p_param->out_str_size = p_param->origin_size;

		return GEN_S_OK;
	}

	b_error = 0;
	error_code = 0;

	i = 0;
	out_str_size = 0;
	p_src = p_param->p_orgin;
	p_dest = p_param->p_out_str;

	while( i < p_param->origin_size )
	{
		if( p_src[i] == p_param->p_str1[0] )
		{
			ret = memcmp(p_src + i, p_param->p_str1, p_param->str1_size);
			if( ret == 0 )
			{
				/** find str1 */
				if( p_param->str2_size > 0 )
				{
					if( (out_str_size + p_param->str2_size) < (out_buf_size - 4) )
					{
						memcpy(p_dest + out_str_size, p_param->p_str2, p_param->str2_size);

					}else
					{
						//b_error = 1;
						//error_code = GEN_E_NEED_MORE_BUF;
						p_param->out_str_size = out_str_size + p_param->str2_size + 4;

						return GEN_E_NEED_MORE_BUF;
					}

				}

				i += p_param->str1_size;
				out_str_size += p_param->str2_size;
			}
			
		}else
		{
			p_dest[out_str_size] = p_src[i];
			i++;
			out_str_size++;
		}
	}

	p_dest[out_str_size] = 0x00;
	p_dest[out_str_size + 1] = 0x00;
	p_param->out_str_size = out_str_size;

	return GEN_S_OK;
}

int32 gen_text_get_line_count(GEN_TEXT* p_text, uint8* p_data, int32 data_size, int32* p_line_count)
{
	int32   line_count;
	int32   b_cr;
	int32   b_lf;

	int32   i;

	if( p_text == NULL || p_data == NULL || p_line_count == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_line_count = 0;

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size == 0 )
	{
		/** empty string */
		return GEN_S_OK;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	line_count = 0;
	b_cr = 0;
	b_lf = 0;
	i = 0;

	while( i < data_size )
	{
		if( p_data[i] == GEN_TEXT_CR )
		{
			b_cr = 1;
			b_lf = 0;

			line_count++;

		}else if( p_data[i] == GEN_TEXT_LF )
		{
			b_lf = 1;

			if( b_cr )
			{
				b_cr = 0;

			}else
			{
				line_count++;
			}

		}else
		{
			b_cr = 0;
			b_lf = 0;
		}

		i++;
	}

	if( !b_cr && !b_lf )
	{
		/** not end with CR or LF */
		line_count++;
	}

	*p_line_count = line_count;

	return GEN_S_OK;
}

int32 gen_text_get_line_size(GEN_TEXT* p_text, uint8* p_data, int32 data_size, GEN_TEXT_LINE* p_text_line)
{
	int32   line_number;
	int32   line_size;
	int32   text_size;
	uint8*  p_uint8 = NULL;
	int32   b_cr;
	int32   b_lf;

	int32   i;
	//int32   j;

	if( p_text == NULL || p_data == NULL || p_text_line == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_text_line->line_size = 0;
	p_text_line->text_size = 0;
	p_text_line->p_start = NULL;

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size == 0 )
	{
		/** empty string */
		//p_text_line->line_size = 0;
		//p_text_line->text_size = 0;
		//p_text_line->p_start = NULL;

		return GEN_S_OK;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	if( p_text_line->number < GEN_TEXT_START_LINE_NUMBER )
	{
		return GEN_E_INVALID_PARAM;
	}

	/** find line */
	line_number = GEN_TEXT_START_LINE_NUMBER;
	b_cr = 0;
	b_lf = 0;
	i = 0;

	while( line_number < p_text_line->number && i < data_size )
	{
		if( p_data[i] == GEN_TEXT_CR )
		{
			b_cr = 1;
			b_lf = 0;

			line_number++;

		}else if( p_data[i] == GEN_TEXT_LF )
		{
			b_lf = 1;

			if( b_cr )
			{
				b_cr = 0;

			}else
			{
				line_number++;
			}

		}else
		{
			b_cr = 0;
			b_lf = 0;
		}

		i++;
	}

	if( line_number < p_text_line->number )
	{
		/** request line exceed max line */
		return GEN_E_NOT_FOUND;
	}

	if( b_cr )
	{
		/** \r\n */
		if( i < data_size )
		{
			if( p_data[i] == GEN_TEXT_LF )
			{
				b_cr = 0;
				b_lf = 1;
				i++;
			}

		}else
		{
			/** null line */
			return GEN_S_OK;
		}
	}

	/** get line size */
	line_size = 0;
	text_size = 0;
	p_uint8 = p_data + i;

	while( i < data_size )
	{
		if( p_data[i] == GEN_TEXT_CR )
		{
			line_size++;
			i++;

			if( i < data_size && p_data[i] == GEN_TEXT_LF )
			{
				line_size++;
				i++;
			}

			break;

		}else if( p_data[i] == GEN_TEXT_LF )
		{
			line_size++;
			i++;
			break;
		}

		line_size++;
		text_size++;
		i++;
	}

	p_text_line->line_size = line_size;
	p_text_line->text_size = text_size;

	if( line_size > 0 )
	{
		p_text_line->p_start = p_uint8;

	}else
	{
		p_text_line->p_start = NULL;
	}

	return GEN_S_OK;
}

int32 gen_text_get_max_line_size(GEN_TEXT* p_text, uint8* p_data, int32 data_size, GEN_TEXT_LINE* p_text_line)
{
	int32   line_number;
	int32   line_size;
	int32   text_size;
	int32   max_line_number;
	int32   max_line_size;
	int32   max_text_size;
	uint8*  p_uint8 = NULL;
	uint8*  p_temp = NULL;

	int32   b_cr;
	int32   b_lf;

	int32   i;

	if( p_text == NULL || p_data == NULL || p_text_line == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_text_line->number = 0;
	p_text_line->line_size = 0;
	p_text_line->text_size = 0;
	p_text_line->p_start = NULL;

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size == 0 )
	{
		/** empty string */
		return GEN_S_OK;
	}

	if( !p_text->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_text->encoding != GEN_TEXT_ENCODING_ASCII && p_text->encoding != GEN_TEXT_ENCODING_UTF_8 )
	{
		return GEN_E_NOT_IMPLEMENT;
	}

	/** find line */
	line_number = GEN_TEXT_START_LINE_NUMBER;
	line_size = 0;
	text_size = 0;
	max_line_number = 0;
	max_line_size = 0;
	max_text_size = 0;
	p_uint8 = p_data;
	p_temp = p_data;

	b_cr = 0;
	b_lf = 0;
	i = 0;

	while( i < data_size )
	{
		if( p_data[i] == GEN_TEXT_CR )
		{
			/** '\r' */
			b_cr = 1;
			b_lf = 0;

			line_size++;

			if( (i + 1) < data_size )
			{
				/** check '\n' */
				if( p_data[i+1] == GEN_TEXT_LF )
				{
					line_size++;
					i++;
				}
			}

			if( line_size > max_line_size )
			{
				max_line_number = line_number;
				max_line_size = line_size;
				max_text_size = text_size;
				p_uint8 = p_temp;
			}

			line_number++;

		}else if( p_data[i] == GEN_TEXT_LF )
		{
			/** '\n' */
			b_lf = 1;

			line_size++;

			if( line_size > max_line_size )
			{
				max_line_number = line_number;
				max_line_size = line_size;
				max_text_size = text_size;
				p_uint8 = p_temp;
			}

			if( b_cr )
			{
				b_cr = 0;

			}else
			{
				line_number++;
			}

		}else
		{
			/** text */
			if( b_cr || b_lf )
			{
				b_cr = 0;
				b_lf = 0;

				line_size = 0;
				text_size = 0;
				p_temp = p_data + i;
			}

			line_size++;
			text_size++;
		}

		i++;
	}

	if( !b_cr && !b_lf )
	{
		/** not end with CRLF */
		if( line_size > max_line_size )
		{
			max_line_number = line_number;
			max_line_size = line_size;
			max_text_size = text_size;
			p_uint8 = p_temp;
		}
	}

	p_text_line->number = max_line_number;
	p_text_line->line_size = max_line_size;
	p_text_line->text_size = max_text_size;
	p_text_line->p_start = p_uint8;

	return GEN_S_OK;
}

int32 gen_text_get_token(
						 GEN_TEXT* p_text,
						 uint8* p_data, int32 data_size,
						 uint8* p_separator, int32 separator_size,
						 int32 token_number, uint8* p_token, int32* p_token_size)
{
	int32   token_index;
	int32   token_buf_size;
	int32   token_size;
	int32   token_begin;

	int32   b_inside_token;
	int32   b_separator;

	int32   ret;
	int32   i;

	if( p_text == NULL || p_data == NULL || p_separator == NULL || p_token == NULL || p_token_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	token_buf_size = *p_token_size;

	p_token[0] = 0x00;
	*p_token_size = 0;

	if( data_size < 0 || separator_size < 1 || token_number < GEN_TEXT_START_LINE_NUMBER || token_buf_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	token_index = 0;
	b_inside_token = 0;
	i = 0;

	while( token_index < token_number && i < data_size )
	{
		ret = gen_text_is_separator(p_text, p_separator, separator_size, p_data + i, 1, &b_separator);
		if( b_separator )
		{
			/** separator */
			if( b_inside_token )
			{
				b_inside_token = 0;
			}

		}else
		{
			/** token */
			if( !b_inside_token )
			{
				b_inside_token = 1;
				token_index++;

				if( token_index == token_number )
				{
					break;
				}
			}
		}

		i++;
	}

	if( token_index < token_number )
	{
		/** not found */
		return GEN_E_NOT_FOUND;
	}

	token_size = 0;
	token_begin = i;
	while( i < data_size )
	{
		ret = gen_text_is_separator(p_text, p_separator, separator_size, p_data + i, 1, &b_separator);
		if( b_separator )
		{
			/** separator */
			break;

		}else
		{
			/** token */
			token_size++;
		}

		i++;
	}

	if( token_buf_size >= (token_size + 4) )
	{
		memcpy(p_token, p_data + token_begin, token_size);
		p_token[token_size] = 0x00;
		p_token[token_size + 1] = 0x00;

		*p_token_size = token_size;
		return GEN_S_OK;

	}else
	{
		*p_token_size = token_size;
		return GEN_E_NEED_MORE_BUF;
	}
}

int32 gen_text_get_param_pair(GEN_TEXT* p_text, uint8* p_data, int32 data_size, int32* p_parse_size, uint8* p_name, int32* p_name_size, uint8* p_value, int32* p_value_size)
{
	GEN_TEXT_LINE  text_line;

	int32   b_continue;
	int32   total_parse_size;
	int32   unparse_size;
	int32   left_size;

	int32   b_get_param;
	int32   b_separator;
	int32   text_size;
	int32   token_size;

	int32   name_size;
	int32   name_buf_size;
	int32   value_size;
	int32   value_buf_size;
	int32   sign_pos;
	int32   begin_pos;
	int32   end_pos;
	int32   end_pos2;

	uint8*  p_uint8 = NULL;
	uint8*  p_start = NULL;
	uint8*  p_begin = NULL;
	uint8*  p_end = NULL;
	uint8*  p_begin2 = NULL;
	uint8*  p_end2 = NULL;

	int32   b_error;
	int32   error_code;

	int32   i;
	int32   ret;

	if( p_text == NULL || p_data == NULL || p_parse_size == NULL || p_name == NULL || p_name_size == NULL || p_value == NULL || p_value_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size < 0 || (*p_name_size < 1) || (*p_value_size < 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = 0;

	name_size = 0;
	name_buf_size = *p_name_size;
	p_name[0] = 0x00;
	*p_name_size = 0;

	value_size = 0;
	value_buf_size = *p_value_size;
	p_value[0] = 0x00;
	*p_value_size = 0;

	b_get_param = 0;

	b_continue = 1;
	unparse_size = data_size;
	total_parse_size = 0;
	p_uint8 = p_data;

	text_line.number = 1;

	while( b_continue && total_parse_size < data_size )
	{
		/** fetch one line */
		ret = gen_text_get_line_size(p_text, p_uint8, unparse_size, &text_line);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		/** trim left */
		p_begin = p_uint8;
		i = 0;

		while( i < text_line.line_size )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_uint8 + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i++;
		}

		if( i == text_line.line_size )
		{
			/** line is null */
			goto NEXT;
		}

		p_begin += i;
		begin_pos = i;

		/** trim right */
		i = text_line.line_size - 1;

		while( i > begin_pos )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_uint8 + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i--;
		}

		p_end = p_uint8 + i;
		end_pos = i;

		text_size = end_pos - begin_pos + 1;

		/** check comment line */
		if( p_begin[0] == '#' || p_begin[0] == ';' )
		{
			goto NEXT;
		}

		/** find '=' */
		ret = gen_text_find_char(p_text, p_begin, text_size, "=", strlen("="), 0, &sign_pos);
		if( ret )
		{
			/** not find */
			goto NEXT;
		}

		if( sign_pos == 0 )
		{
			/** token is null */
			goto NEXT;
		}

		/** token trim right */
		i = sign_pos - 1;

		while( i > 0 )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_begin + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i--;
		}

		p_end2 = p_begin + i;
		end_pos2 = i;

		token_size = end_pos2 + 1;

		/** compare token size with input name size */
		if( token_size >= name_buf_size )
		{
			b_error = 1;
			error_code = GEN_E_NEED_MORE_BUF;

			*p_name_size = token_size + 4;
			break;
		}

		b_continue = 0;

		/** store name */
		memcpy(p_name, p_begin, token_size);
		p_name[token_size] = 0x00;
		name_size = token_size;
		*p_name_size = token_size;

		/** get param value */
		/** value trim left */
		p_start = p_begin + sign_pos + strlen("=");
		p_begin = p_start;
		left_size = text_size - (sign_pos + strlen("="));
		i = 0;

		while( i < left_size )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_start + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i++;
		}

		if( i == left_size )
		{
			/** value is null */
			break;
		}

		p_begin += i;
		begin_pos = i;

		/** value trim right */
		i = left_size - 1;

		while( i > begin_pos )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_start + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i--;
		}

		end_pos = i;

		value_size = end_pos - begin_pos + 1;

		if( value_size > 0 )
		{
			if( value_size >= value_buf_size )
			{
				b_error = 1;
				error_code = GEN_E_NEED_MORE_BUF;
				*p_value_size = value_size + 4;
				break;
			}

			memcpy(p_value, p_begin, value_size);
			p_value[value_size] = 0x00;
			*p_value_size = value_size;
		}

NEXT:

		/** next loop */
		total_parse_size += text_line.line_size;
		unparse_size -= text_line.line_size;
		p_uint8 += text_line.line_size;
	}

	*p_parse_size = total_parse_size;

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 gen_text_get_param_value(GEN_TEXT* p_text, uint8* p_data, int32 data_size, uint8* p_name, int32 name_size, uint8* p_value, int32* p_value_size)
{
	GEN_TEXT_LINE  text_line;

	int32   total_parse_size;
	int32   unparse_size;
	int32   left_size;

	int32   b_get_param;
	int32   b_separator;
	int32   text_size;
	int32   token_size;
	int32   value_size;
	int32   value_buf_size;
	int32   sign_pos;
	int32   begin_pos;
	int32   end_pos;
	int32   end_pos2;

	uint8*  p_uint8 = NULL;
	uint8*  p_start = NULL;
	uint8*  p_begin = NULL;
	uint8*  p_end = NULL;
	uint8*  p_end2 = NULL;

	int32   i;
	int32   ret;

	if( p_text == NULL || p_data == NULL || p_name == NULL || p_value == NULL || p_value_size == NULL || (*p_value_size) < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	value_buf_size = *p_value_size;

	p_value[0] = 0x00;
	*p_value_size = 0;

	b_get_param = 0;

	unparse_size = data_size;
	total_parse_size = 0;
	p_uint8 = p_data;

	text_line.number = 1;

	while( total_parse_size < data_size )
	{
		/** fetch one line */
		ret = gen_text_get_line_size(p_text, p_uint8, unparse_size, &text_line);
		if( ret )
		{
			break;
		}

		/** trim left */
		p_begin = p_uint8;
		i = 0;

		while( i < text_line.line_size )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_uint8 + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i++;
		}

		if( i == text_line.line_size )
		{
			/** line is null */
			goto NEXT;
		}

		p_begin += i;
		begin_pos = i;

		/** trim right */
		i = text_line.line_size - 1;

		while( i > begin_pos )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_uint8 + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i--;
		}

		p_end = p_uint8 + i;
		end_pos = i;

		text_size = end_pos - begin_pos + 1;

		/** check comment line */
		if( p_begin[0] == '#' || p_begin[0] == ';' )
		{
			goto NEXT;
		}

		/** find '=' */
		ret = gen_text_find_char(p_text, p_begin, text_size, "=", strlen("="), 0, &sign_pos);
		if( ret )
		{
			/** not find */
			goto NEXT;
		}

		if( sign_pos == 0 )
		{
			/** token is null */
			goto NEXT;
		}

		/** token trim right */
		i = sign_pos - 1;

		while( i > 0 )
		{
			ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_begin + i, sizeof(uint8), &b_separator);
			if( ret == 0 && !b_separator )
			{
				break;
			}

			i--;
		}

		p_end2 = p_begin + i;
		end_pos2 = i;

		token_size = end_pos2 + 1;

		/** compare token with param name */
		if( token_size != name_size )
		{
			goto NEXT;
		}

		ret = memcmp(p_begin, p_name, name_size);
		if( ret == 0 )
		{
			/** get param value */
			b_get_param = 1;

			/** value trim left */
			p_start = p_begin + sign_pos + strlen("=");
			p_begin = p_start;
			left_size = text_size - (sign_pos + strlen("="));
			i = 0;

			while( i < left_size )
			{
				ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_start + i, sizeof(uint8), &b_separator);
				if( ret == 0 && !b_separator )
				{
					break;
				}

				i++;
			}

			if( i == left_size )
			{
				/** value is null */
				break;
			}

			p_begin += i;
			begin_pos = i;

			/** value trim right */
			i = left_size - 1;

			while( i > begin_pos )
			{
				ret = gen_text_is_separator(p_text, gen_text_separator, sizeof(gen_text_separator), p_start + i, sizeof(uint8), &b_separator);
				if( ret == 0 && !b_separator )
				{
					break;
				}

				i--;
			}

			end_pos = i;

			value_size = end_pos - begin_pos + 1;

			if( value_size > 0 )
			{
				if( value_buf_size < (value_size + 4) )
				{
					*p_value_size = value_size + 4;
					return GEN_E_NEED_MORE_BUF;
				}

				memcpy(p_value, p_begin, value_size);
				p_value[value_size] = 0x00;
				*p_value_size = value_size;
			}

			break;
		}

NEXT:

		/** next loop */
		total_parse_size += text_line.line_size;
		unparse_size -= text_line.line_size;
		p_uint8 += text_line.line_size;
	}

	if( !b_get_param )
	{
		return GEN_E_NOT_FOUND;
	}

	return GEN_S_OK;
}
