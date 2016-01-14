

//#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_error.h>
#include <public/gen_text.h>
//#include <util/log_agent.h>

#include "http_const.h"
#include "http_mux.h"

/** max token size */
#define HTTP_DEMUX_MAX_TOKEN_SIZE                              (512)

/** lws */
static uint8 g_http_lws[] = 
{
	/** space */
	' ',
	/** tab */
	'\t',
	/** \r */
	'\r',
	/** \n */
	'\n'
};

/** separator */
static uint8 g_http_separator[] = 
{
	/** ( */
	'(',
	/** ) */
	')',
	/** < */
	'<',
	/** > */
	'>',
	/** @ */
	'@',
	/** , */
	',',
	/** ; */
	';',
	/** : */
	':',
	/** \ */
	'\\',
	/** " */
	'"',
	/** / */
	'/',
	/** [ */
	'[',
	/** ] */
	']',
	/** ? */
	'?',
	/** = */
	'=',
	/** { */
	'{',
	/** } */
	'}',
	/** space */
	' ',
	/** tab */
	'\t'
};

/////////////////////////////// Inner Interface ///////////////////////////////
/** 
 * @brief check string is lws
 * @param [in] p_text, text
 * @param [in] p_lws, lws buf
 * @param [in] lws_size, lws buf size
 * @param [in] p_char, char
 * @param [in] char_size, char size
 * @param [out] p_b_lws, lws or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 
 */
static inline int32 http_is_lws(GEN_TEXT* p_text, uint8* p_lws, int32 lws_size, uint8* p_char, int32 char_size, int32* p_b_lws)
{
	int32   i;

	*p_b_lws = 0;

	i = 0;
	while( i < lws_size )
	{
		if( p_lws[i] == p_char[0] )
		{
			*p_b_lws = 1;
			return GEN_S_OK;
		}

		i++;
	}

	return GEN_S_OK;
}

/** 
 * @brief find first not lws pos
 * @param [in] p_text, text lib
 * @param [in] p_lws, lws
 * @param [in] lws_size, lws size
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_pos, first not lws pos
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 http_find_first_not_lws_pos(GEN_TEXT* p_text, uint8* p_lws, int32 lws_size, uint8* p_data, int32 data_size, int32* p_pos)
{
	int32   b_find;
	int32   b_lws;

	int32   i;
	int32   ret;

	b_find = 0;
	i = 0;
	while( i < data_size )
	{
		ret = http_is_lws(p_text, g_http_lws, sizeof(g_http_lws), p_data + i, 1, &b_lws);
		if( b_lws )
		{

		}else
		{
			/** token */
			b_find = 1;
			*p_pos = i;
			break;
		}

		i++;
	}

	if( !b_find )
	{
		*p_pos = -1;
		return GEN_E_NOT_FOUND;
	}

	return GEN_S_OK;
}

/** 
 * @brief find last not lws pos
 * @param [in] p_text, text lib
 * @param [in] p_lws, lws
 * @param [in] lws_size, lws size
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_pos, first not lws pos
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 http_find_last_not_lws_pos(GEN_TEXT* p_text, uint8* p_lws, int32 lws_size, uint8* p_data, int32 data_size, int32* p_pos)
{
	int32   b_find;
	int32   b_lws;

	int32   i;
	int32   ret;

	b_find = 0;
	i = data_size - 1;
	while( i >= 0 )
	{
		ret = http_is_lws(p_text, g_http_lws, sizeof(g_http_lws), p_data + i, 1, &b_lws);
		if( b_lws )
		{

		}else
		{
			/** token */
			b_find = 1;
			*p_pos = i;
			break;
		}

		i--;
	}

	if( !b_find )
	{
		*p_pos = -1;
		return GEN_E_NOT_FOUND;
	}

	return GEN_S_OK;
}

/** 
 * @brief get method type
 * @param [in] p_text, text lib
 * @param [in] p_method_name, method name
 * @param [in] method_name_size, method name size
 * @param [out] p_method_type, method type
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 http_get_method_type(GEN_TEXT* p_text, uint8* p_method_name, int32 method_name_size, int32* p_method_type)
{
	int32   cmp_res;
	int32   ret;

	//gen_text_setup(&text);
	//gen_text_init(&text);

	/** get */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_GET, strlen(HTTP_METHOD_GET), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_GET;
		return GEN_S_OK;
	}

	/** post */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_POST, strlen(HTTP_METHOD_POST), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_POST;
		return GEN_S_OK;
	}

	/** connect */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_CONNECT, strlen(HTTP_METHOD_CONNECT), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_CONNECT;
		return GEN_S_OK;
	}

	/** delete */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_DELETE, strlen(HTTP_METHOD_DELETE), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_DELETE;
		return GEN_S_OK;
	}

	/** head */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_HEAD, strlen(HTTP_METHOD_HEAD), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_HEAD;
		return GEN_S_OK;
	}

	/** options */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_OPTIONS, strlen(HTTP_METHOD_OPTIONS), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_OPTIONS;
		return GEN_S_OK;
	}

	/** put */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_PUT, strlen(HTTP_METHOD_PUT), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_PUT;
		return GEN_S_OK;
	}

	/** trace */
	ret = gen_text_strcmp(p_text, p_method_name, method_name_size, HTTP_METHOD_TRACE, strlen(HTTP_METHOD_TRACE), &cmp_res);
	if( cmp_res == 0 )
	{
		*p_method_type = HTTP_METHOD_TYPE_TRACE;
		return GEN_S_OK;
	}

	return GEN_E_NOT_IMPLEMENT;
}

/** 
 * @brief demux req
 * @param [in] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_buf_size would always update
 */
static int32 http_demux_req_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	GEN_TEXT           text;
	//GEN_TEXT_LINE      text_line;
	GEN_TEXT_SEPARATOR text_separator;
	GEN_TEXT_TOKEN     text_token;

	int32   left_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;

	int32   ret;

	p_uint8 = p_buf;
	total_demux_size = 0;
	left_size = *p_buf_size;

	gen_text_setup(&text);
	gen_text_init(&text);

	text_separator.p_separator = gen_text_separator;
	text_separator.separator_size = sizeof(gen_text_separator);

	text_token.number = 1;

	/** method name */
	demux_size = left_size;
	text_token.p_token = p_line->msg.req.method_name;
	text_token.token_size = HTTP_MAX_METHOD_NAME_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_WRONG_FORMAT;
	}

	p_line->msg.req.method_name[text_token.token_size] = 0x00;
	p_line->msg.req.method_name_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	ret = http_get_method_type(&text, text_token.p_token, text_token.token_size, &p_line->msg.req.method_type);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	/** uri */
	demux_size = left_size;
	text_token.p_token = p_line->msg.req.uri;
	text_token.token_size = HTTP_MAX_URI_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	p_line->msg.req.uri[text_token.token_size] = 0x00;
	p_line->msg.req.uri_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** version */
	demux_size = left_size;
	text_token.p_token = p_line->msg.req.version;
	text_token.token_size = HTTP_MAX_VERSION_STR_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	p_line->msg.req.version[text_token.token_size] = 0x00;
	p_line->msg.req.version_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	*p_buf_size = total_demux_size;

	return GEN_S_OK;
}

/** 
 * @brief demux resp
 * @param [in] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size hold need size
 */
static int32 http_demux_resp_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	GEN_TEXT           text;
	GEN_TEXT_SEPARATOR text_separator;
	GEN_TEXT_TOKEN     text_token;

	int32   left_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;

	int32   ret;

	p_uint8 = p_buf;
	total_demux_size = 0;
	left_size = *p_buf_size;

	gen_text_setup(&text);
	gen_text_init(&text);

	text_separator.p_separator = gen_text_separator;
	text_separator.separator_size = sizeof(gen_text_separator);

	text_token.number = 1;

	/** version */
	demux_size = left_size;
	text_token.p_token = p_line->msg.resp.version;
	text_token.token_size = HTTP_MAX_VERSION_STR_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	p_line->msg.resp.version[text_token.token_size] = 0x00;
	p_line->msg.resp.version_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** status code */
	demux_size = left_size;
	text_token.p_token = p_line->msg.resp.status_code;
	text_token.token_size = HTTP_MAX_STATUS_CODE_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	p_line->msg.resp.status_code[text_token.token_size] = 0x00;
	p_line->msg.resp.status_code_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** status desc */
	demux_size = left_size;
	text_token.p_token = p_line->msg.resp.status_code_desc;
	text_token.token_size = HTTP_MAX_STATUS_CODE_DESC_SIZE;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_uint8, &demux_size);
	if( ret )
	{
		*p_buf_size = total_demux_size;
		return GEN_E_FAIL;
	}

	p_line->msg.resp.status_code_desc[text_token.token_size] = 0x00;
	p_line->msg.resp.status_code_desc_size = text_token.token_size;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	*p_buf_size = total_demux_size;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 http_demux_start_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	GEN_TEXT           text;
	GEN_TEXT_LINE      text_line;
	GEN_TEXT_SEPARATOR text_separator;
	GEN_TEXT_TOKEN     text_token;

	int32   left_size;
	int32   demux_size;

	uint8   token[HTTP_DEMUX_MAX_TOKEN_SIZE + 4];
	int32   token_size;

	int32   cmp_res;
	int32   ret;

	if( p_line == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	gen_text_setup(&text);
	gen_text_init(&text);

	text_line.number = 1;
	ret = gen_text_get_line_size(&text, p_buf, left_size, &text_line);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	text_separator.p_separator = gen_text_separator;
	text_separator.separator_size = sizeof(gen_text_separator);

	text_token.number = 1;
	text_token.p_token = token;
	text_token.token_size = HTTP_DEMUX_MAX_TOKEN_SIZE;

	demux_size = left_size;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_buf, &demux_size);
	if( ret )
	{
		*p_buf_size = demux_size;
		return GEN_E_FAIL;
	}

	ret = gen_text_strcmp(&text, text_token.p_token, text_token.token_size, HTTP_VERSION_1_1, strlen(HTTP_VERSION_1_1), &cmp_res);
	if( cmp_res == 0 )
	{
		/** version 1.1 */
		p_line->msg_type = HTTP_MSG_TYPE_RESP;

	}else
	{
		ret = gen_text_strcmp(&text, text_token.p_token, text_token.token_size, HTTP_VERSION_1_0, strlen(HTTP_VERSION_1_0), &cmp_res);
		if( cmp_res == 0 )
		{
			/** version 1.0 */
			p_line->msg_type = HTTP_MSG_TYPE_RESP;


		}else
		{
			/** request */
			p_line->msg_type = HTTP_MSG_TYPE_REQ;
		}
	}

	switch( p_line->msg_type )
	{
	case HTTP_MSG_TYPE_REQ:
		{
			demux_size = text_line.line_size;
			ret = http_demux_req_line(p_line, text_line.p_start, &demux_size);
		}
		break;

	case HTTP_MSG_TYPE_RESP:
		{
			demux_size = left_size;
			ret = http_demux_resp_line(p_line, text_line.p_start, &demux_size);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	*p_buf_size = text_line.line_size;

	return ret;
}

int32 http_demux_header(HTTP_HEADER* p_header, uint8* p_buf, int32* p_buf_size)
{
	GEN_TEXT           text;
	GEN_TEXT_LINE      text_line;
	GEN_TEXT_SEPARATOR text_separator;
	GEN_TEXT_TOKEN     text_token;

	int32   left_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;

	//uint8   token[HTTP_DEMUX_MAX_TOKEN_SIZE + 4];
	int32   token_size;

	int32   begin_pos;
	int32   end_pos;
	int32   ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_header->p_value == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	p_header->name[0] = 0x00;
	p_header->name_size = 0;
	p_header->p_value[0] = 0x00;
	p_header->value_size = 0;

	gen_text_setup(&text);
	gen_text_init(&text);

	text_line.number = 1;
	ret = gen_text_get_line_size(&text, p_buf, left_size, &text_line);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	if( text_line.text_size == 0 )
	{
		/** empty line */
		*p_buf_size = text_line.line_size;
		return GEN_S_OK;
	}

	p_uint8 = text_line.p_start;
	total_demux_size = 0;
	left_size = text_line.text_size;

	text_separator.p_separator = g_http_separator;
	text_separator.separator_size = sizeof(g_http_separator);

	/** name */
	text_token.number = 1;
	text_token.p_token = p_header->name;
	text_token.token_size = HTTP_MAX_HEADER_NAME_SIZE;

	demux_size = left_size;
	ret = gen_text_get_token(&text, &text_separator, &text_token, p_buf, &demux_size);
	if( ret )
	{
		*p_buf_size = text_line.line_size;
		return GEN_E_FAIL;
	}

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	p_header->name[text_token.token_size] = 0x00;
	p_header->name_size = text_token.token_size;

	if( left_size < 1 )
	{
		*p_buf_size = text_line.line_size;
		return GEN_E_FAIL;
	}

	if( p_uint8[0] != ':' )
	{
		*p_buf_size = text_line.line_size;
		return GEN_E_FAIL;
	}

	p_uint8 += 1;
	total_demux_size += 1;
	left_size -= 1;

	/** value */
	ret = http_find_first_not_lws_pos(&text, g_http_lws, sizeof(g_http_lws), p_uint8, left_size, &begin_pos);
	if( ret == GEN_S_OK )
	{
		/** not null */
		ret = http_find_last_not_lws_pos(&text, g_http_lws, sizeof(g_http_lws), p_uint8, left_size, &end_pos);
		token_size = end_pos - begin_pos + 1;

		if( token_size < p_header->value_size )
		{
			memcpy(p_header->p_value, p_uint8 + begin_pos, token_size);
			p_header->p_value[token_size] = 0x00;
			p_header->value_size = token_size;

		}else
		{
			p_header->value_size = token_size + 4;
			return GEN_E_NEED_MORE_BUF;
		}
	}

	*p_buf_size = text_line.line_size;

	return GEN_S_OK;
}
