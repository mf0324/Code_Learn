
#include <string.h>

#include <public/gen_error.h>
#include <public/gen_platform.h>
#include "log_agent_const.h"
#include "log_agent_util.h"

/** max log level desc string size, unit: byte */
#define LA_MAX_LOG_LEVEL_DESC_STRING_SIZE                  (16)

/** level entry */
typedef struct tag_log_level_entry
{
	/** level */
	uint32 level;
	/** desc */
	int8   desc[LA_MAX_LOG_LEVEL_DESC_STRING_SIZE];

} LOG_LEVEL_ENTRY, *PLOG_LEVEL_ENTRY;

/** level desc table */
static LOG_LEVEL_ENTRY log_level_desc_table[] = 
{
	{ LA_LOG_LEVEL_FATAL,  "Fatal"  },
	{ LA_LOG_LEVEL_ERROR,  "Error"  },
	{ LA_LOG_LEVEL_WARN,   "Warn"   },
	{ LA_LOG_LEVEL_NORMAL, "Normal" },
	{ LA_LOG_LEVEL_INFO,   "Info"   },
	{ LA_LOG_LEVEL_DEBUG,  "Debug"  },
	{ LA_LOG_LEVEL_ACCESS, "Access" },
	{ 0,  "\0"  }
};

/** white space */
static int8  text_whitespace[5] = " \t\r\n";

/** 
 * @brief trim whitespace
 * @param [in] p_str, input string
 * @param [in] p_str_size, input string size
 * @param [out] p_out, output buf
 * @param [in/out] p_out_size, output buf size/actual string size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. trim front and back whitespace
 */
static int32 gen_text_trim_whitespace(int8* p_str, int32 str_size, int8* p_out, int32* p_out_size)
{
	int32  input_str_size;
	int32  output_buf_size;
	int32  token_size;

	int32  i;
	int32  j;
	int32  ret;

	if( p_str == NULL || p_out == NULL || p_out_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	input_str_size = str_size;
	output_buf_size = *p_out_size;

	p_out[0] = 0x00;
	*p_out_size = 0;

	if( str_size < 1 )
	{
		return GEN_S_OK;
	}

	/** trim front */
	i = 0;
	while( ( p_str[i] == text_whitespace[0]
			|| p_str[i] == text_whitespace[1]
			|| p_str[i] == text_whitespace[2]
			|| p_str[i] == text_whitespace[3] )
			&& i < input_str_size )
	{
		i++;
	}

	if( i == input_str_size )
	{
		return GEN_S_OK;
	}

	/** trim back */
	j = input_str_size - 1;
	while( ( p_str[j] == text_whitespace[0]
			|| p_str[j] == text_whitespace[1]
			|| p_str[j] == text_whitespace[2]
			|| p_str[j] == text_whitespace[3] )
			&& j > i )
	{
		j--;
	}

	/** copy token */
	token_size = j - i + 1;
	if( token_size >= output_buf_size )
	{
		*p_out_size = token_size;
		return GEN_E_NEED_MORE_BUF;
	}

	memcpy(p_out, p_str + i, token_size);
	p_out[token_size] = 0x00;

	*p_out_size = token_size;

	return GEN_S_OK;
}

/** 
 * @brief get token
 * @param [in] token_number, token number
 * @param [in] p_str, input string
 * @param [in/out] p_str_size, input string size/actual parse string size
 * @param [in] delimeter, delimeter of tokens
 * @param [out] p_token, output token buf
 * @param [in/out] p_token_size, token buf size/actual token size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. token number count from 1
 */
static int32 gen_text_get_token(int32 token_number, int8* p_str, int32* p_str_size, int8 delimeter, int8* p_token, int32* p_token_size)
{
	int32  token_count;
	int32  token_buf_size;
	//int32  token_size;
	int32  token_pos;
	int32  str_buf_size;
	//int32  str_pos;

	int32  i;
	int32  j;
	int32  ret;

	if( p_str == NULL || p_str_size == NULL || p_token == NULL || p_token_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( token_number < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	str_buf_size = *p_str_size;
	token_buf_size = *p_token_size;
	//token_size = 0;

	p_token[0] = 0x00;
	*p_token_size = 0;

	/** skip first delimeter */
	i = 0;
	while( p_str[i] == delimeter && i < str_buf_size )
	{
		i++;
	}

	if( i == str_buf_size )
	{
		return GEN_E_NOT_FOUND;
	}

	/** find specify token */
	token_count = 1;
	while( token_count < token_number && i < str_buf_size )
	{
		if( p_str[i] == delimeter )
		{
			token_count++;
		}

		i++;
	}

	if( token_count < token_number || i == str_buf_size )
	{
		return GEN_E_NOT_FOUND;
	}

	/** copy token */
	token_pos = i;
	j = 0;
	while( i < str_buf_size )
	{
		if( p_str[i] == delimeter )
		{
			break;
		}

		//p_token[j] = p_str[i];

		i++;
		j++;
	}

	if( j >= token_buf_size )
	{
		*p_token_size = j;
		return GEN_E_NEED_MORE_BUF;
	}

	memcpy(p_token, p_str + token_pos, j);
	p_token[j] = 0x00;

	*p_token_size = j;
	*p_str_size = i;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 log_agent_trans_str_to_level(int8* p_str, int32 str_size, uint32* p_level)
{
	int8   token_buf[LA_MAX_LOG_LEVEL_DESC_STRING_SIZE];
	int32  token_size;

	int8   str_buf[1024];
	int32  str_buf_size;

	int8*  p_int8 = NULL;
	int32  parse_size;
	int32  str_unparse_size;
	int32  total_parse_size;

	uint32 level;
	int32  entry_count;

	int32  i;
	int32  ret;

	if( p_str == NULL || p_level == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_int8 = p_str;
	str_unparse_size = str_size;
	total_parse_size = 0;

	level = 0;
	entry_count = sizeof(log_level_desc_table) / sizeof(log_level_desc_table[0]);

	while( str_unparse_size > 0 )
	{
		parse_size = str_unparse_size;
		str_buf_size = 1024;
		ret = gen_text_get_token(1, p_int8, &parse_size, '|', str_buf, &str_buf_size);

		if( ret )
		{
			break;
		}

		p_int8 += parse_size;
		str_unparse_size -= parse_size;
		total_parse_size += parse_size;

		token_size = LA_MAX_LOG_LEVEL_DESC_STRING_SIZE;
		ret = gen_text_trim_whitespace(str_buf, str_buf_size, token_buf, &token_size);
		if( ret == 0 && token_size > 0 )
		{
			for( i = 0; i < entry_count - 1; i++ )
			{
#if RUN_OS_WINDOWS
				if( _stricmp(token_buf, log_level_desc_table[i].desc) == 0 )
#else
				if( strcasecmp(token_buf, log_level_desc_table[i].desc) == 0 )
#endif
				{
					level |= log_level_desc_table[i].level;
					break;
				}
			}
		}
	}

	*p_level = level;

	return GEN_S_OK;
}
