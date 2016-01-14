

//#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>

//#include <util/log_agent.h>

#include "http_const.h"
#include "http_mux.h"


/////////////////////////////// Inner Interface ///////////////////////////////
/** 
 * @brief mux req
 * @param [in] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size hold need size
 */
static int32 http_mux_req_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;

	int32   ret;

	left_size = *p_buf_size;
	need_size = p_line->msg.req.method_name_size + p_line->msg.req.uri_size + p_line->msg.req.version_size + 4;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_mux_size = 0;

	/** method */
	memcpy(p_uint8, p_line->msg.req.method_name, p_line->msg.req.method_name_size);
	p_uint8 += p_line->msg.req.method_name_size;
	total_mux_size += p_line->msg.req.method_name_size;
	left_size -= p_line->msg.req.method_name_size;

	p_uint8[0] = ' ';
	p_uint8 += 1;
	total_mux_size += 1;
	left_size -= 1;

	/** uri */
	memcpy(p_uint8, p_line->msg.req.uri, p_line->msg.req.uri_size);
	p_uint8 += p_line->msg.req.uri_size;
	total_mux_size += p_line->msg.req.uri_size;
	left_size -= p_line->msg.req.uri_size;

	p_uint8[0] = ' ';
	p_uint8 += 1;
	total_mux_size += 1;
	left_size -= 1;

	/** version */
	memcpy(p_uint8, p_line->msg.req.version, p_line->msg.req.version_size);
	p_uint8 += p_line->msg.req.version_size;
	total_mux_size += p_line->msg.req.version_size;
	left_size -= p_line->msg.req.version_size;

	p_uint8[0] = 0xD;
	p_uint8[1] = 0xA;
	p_uint8 += 2;
	total_mux_size += 2;
	left_size -= 2;

	*p_buf_size = total_mux_size;

	return GEN_S_OK;
}

/** 
 * @brief mux resp
 * @param [in] p_line, start line
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, input buf size/actual mux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if input buf not enough, p_buf_size hold need size
 */
static int32 http_mux_resp_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;

	int32   ret;

	left_size = *p_buf_size;
	need_size = p_line->msg.req.version_size + HTTP_STATUS_CODE_SIZE + p_line->msg.resp.status_code_desc_size + 4;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_mux_size = 0;

	/** version */
	memcpy(p_uint8, p_line->msg.resp.version, p_line->msg.resp.version_size);
	p_uint8 += p_line->msg.resp.version_size;
	total_mux_size += p_line->msg.resp.version_size;
	left_size -= p_line->msg.resp.version_size;

	p_uint8[0] = ' ';
	p_uint8 += 1;
	total_mux_size += 1;
	left_size -= 1;

	/** status code */
	memcpy(p_uint8, p_line->msg.resp.status_code, p_line->msg.resp.status_code_size);
	p_uint8 += p_line->msg.resp.status_code_size;
	total_mux_size += p_line->msg.resp.status_code_size;
	left_size -= p_line->msg.resp.status_code_size;

	p_uint8[0] = ' ';
	p_uint8 += 1;
	total_mux_size += 1;
	left_size -= 1;

	/** status desc */
	memcpy(p_uint8, p_line->msg.resp.status_code_desc, p_line->msg.resp.status_code_desc_size);
	p_uint8 += p_line->msg.resp.status_code_desc_size;
	total_mux_size += p_line->msg.resp.status_code_desc_size;
	left_size -= p_line->msg.resp.status_code_desc_size;

	p_uint8[0] = 0xD;
	p_uint8[1] = 0xA;
	p_uint8 += 2;
	total_mux_size += 2;
	left_size -= 2;

	*p_buf_size = total_mux_size;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
int32 http_mux_start_line(HTTP_START_LINE* p_line, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   mux_size;

	int32   ret;

	if( p_line == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	switch( p_line->msg_type )
	{
	case HTTP_MSG_TYPE_REQ:
		{
			mux_size = left_size;
			ret = http_mux_req_line(p_line, p_buf, &mux_size);
		}
		break;

	case HTTP_MSG_TYPE_RESP:
		{
			mux_size = left_size;
			ret = http_mux_resp_line(p_line, p_buf, &mux_size);
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	*p_buf_size = mux_size;

	return ret;
}

int32 http_mux_header(HTTP_HEADER* p_header, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   mux_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;

	int32   ret;

	if( p_header == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	if( p_header->name_size > 0 )
	{
		need_size = p_header->name_size + HTTP_MIN_LWS_SIZE * 2 + p_header->value_size + HTTP_CRLF_SIZE;

	}else
	{
		need_size = HTTP_CRLF_SIZE;
	}

	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_mux_size = 0;

	if( p_header->name_size > 0 )
	{
		/** name */
		memcpy(p_uint8, p_header->name, p_header->name_size);
		p_uint8 += p_header->name_size;
		total_mux_size += p_header->name_size;
		left_size -= p_header->name_size;

		p_uint8[0] = ':';
		p_uint8[1] = ' ';
		p_uint8 += 2;
		total_mux_size += 2;
		left_size -= 2;

		/** value */
		if( p_header->p_value )
		{
			memcpy(p_uint8, p_header->p_value, p_header->value_size);
			p_uint8 += p_header->value_size;
			total_mux_size += p_header->value_size;
			left_size -= p_header->value_size;
		}
	}

	p_uint8[0] = '\r';
	p_uint8[1] = '\n';
	p_uint8 += 2;
	total_mux_size += 2;
	left_size -= 2;

	*p_buf_size = total_mux_size;

	return GEN_S_OK;
}
