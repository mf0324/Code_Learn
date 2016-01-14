/**
 * @file http_public.h  HTTP Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.23
 *
 *
 */

#ifndef __HTTP_PUBLIC_H__
#define __HTTP_PUBLIC_H__

#include <public/gen_int.h>


/** max version str size */
#define HTTP_MAX_VERSION_STR_SIZE                              (32)
/** max method name size */
#define HTTP_MAX_METHOD_NAME_SIZE                              (64)
/** max status code size */
#define HTTP_MAX_STATUS_CODE_SIZE                              (16)
/** max status code desc size */
#define HTTP_MAX_STATUS_CODE_DESC_SIZE                         (64)
/** max uri size */
#define HTTP_MAX_URI_SIZE                                      (16 * 1024)
/** max header name size */
#define HTTP_MAX_HEADER_NAME_SIZE                              (64)


/** req */
typedef struct tag_http_req
{
	/** method type */
	int32   method_type;

	/** method name */
	uint8   method_name[HTTP_MAX_METHOD_NAME_SIZE + 4];
	/** method name size */
	int32   method_name_size;

	/** uri */
	uint8   uri[HTTP_MAX_URI_SIZE + 4];
	/** uri size */
	int32   uri_size;

	/** version */
	uint8   version[HTTP_MAX_VERSION_STR_SIZE + 4];
	/** version size */
	int32   version_size;

} HTTP_REQ, *PHTTP_REQ;

/** resp */
typedef struct tag_http_resp
{
	/** version */
	uint8   version[HTTP_MAX_VERSION_STR_SIZE + 4];
	/** version size */
	int32   version_size;

	/** status code */
	uint8   status_code[HTTP_MAX_STATUS_CODE_SIZE + 4];
	/** status code size */
	int32   status_code_size;

	/** status code desc */
	uint8   status_code_desc[HTTP_MAX_STATUS_CODE_DESC_SIZE + 4];
	/** status code desc size */
	int32   status_code_desc_size;

} HTTP_RESP, *PHTTP_RESP;

/** header */
typedef struct tag_http_header
{
	/** header name */
	uint8   name[HTTP_MAX_HEADER_NAME_SIZE + 4];
	/** header name size */
	int32   name_size;

	/** value */
	uint8*  p_value;
	/** value size */
	int32   value_size;

} HTTP_HEADER, *PHTTP_HEADER;

/** start line */
typedef struct tag_http_start_line
{
	/** msg type */
	int32   msg_type;

	/** msg */
	union
	{
		/** req */
		HTTP_REQ   req;
		/** resp */
		HTTP_RESP  resp;

	} msg;

} HTTP_START_LINE, *PHTTP_START_LINE;



#endif /// __HTTP_PUBLIC_H__
