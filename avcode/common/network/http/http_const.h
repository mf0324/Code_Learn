/**
 * @file http_const.h   HTTP Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.09.23
 *
 *
 */

#ifndef __HTTP_CONST_H__
#define __HTTP_CONST_H__

/** 
 * @name http
 * @{
 */
/** default port */
#define HTTP_PORT                                              (80)
/** request */
#define HTTP_MSG_TYPE_REQ                                      (1)
/** resp */
#define HTTP_MSG_TYPE_RESP                                     (2)
/** status code size */
#define HTTP_STATUS_CODE_SIZE                                  (3)
/** version size */
#define HTTP_VERSION_SIZE                                      (8)
/** version 1.0 */
#define HTTP_VERSION_1_0                                       "HTTP/1.0"
/** version 1.1 */
#define HTTP_VERSION_1_1                                       "HTTP/1.1"

/** min lws size */
#define HTTP_MIN_LWS_SIZE                                      (1)
/** crlf size */
#define HTTP_CRLF_SIZE                                         (2)

/** 
 * @}
 */


/** 
 * @name status code
 * @{
 */
/** 100 */
#define HTTP_STATUS_CODE_100                                   "100"
/** 200 */
#define HTTP_STATUS_CODE_200                                   "200"
/** 201 */
#define HTTP_STATUS_CODE_201                                   "201"
/** 202 */
#define HTTP_STATUS_CODE_202                                   "202"
/** 203 */
#define HTTP_STATUS_CODE_203                                   "203"
/** 300 */
#define HTTP_STATUS_CODE_300                                   "300"
/** 301 */
#define HTTP_STATUS_CODE_301                                   "301"
/** 302 */
#define HTTP_STATUS_CODE_302                                   "302"
/** 303 */
#define HTTP_STATUS_CODE_303                                   "303"
/** 304 */
#define HTTP_STATUS_CODE_304                                   "304"
/** 305 */
#define HTTP_STATUS_CODE_305                                   "305"
/** 307 */
#define HTTP_STATUS_CODE_307                                   "307"
/** 400 */
#define HTTP_STATUS_CODE_400                                   "400"
/** 401 */
#define HTTP_STATUS_CODE_401                                   "401"
/** 402 */
#define HTTP_STATUS_CODE_402                                   "402"
/** 403 */
#define HTTP_STATUS_CODE_403                                   "403"
/** 404 */
#define HTTP_STATUS_CODE_404                                   "404"
/** 405 */
#define HTTP_STATUS_CODE_405                                   "405"
/** 406 */
#define HTTP_STATUS_CODE_406                                   "406"
/** 407 */
#define HTTP_STATUS_CODE_407                                   "407"
/** 408 */
#define HTTP_STATUS_CODE_408                                   "408"
/** 409 */
#define HTTP_STATUS_CODE_409                                   "409"
/** 410 */
#define HTTP_STATUS_CODE_410                                   "410"
/** 500 */
#define HTTP_STATUS_CODE_500                                   "500"
/** 501 */
#define HTTP_STATUS_CODE_501                                   "501"
/** 502 */
#define HTTP_STATUS_CODE_502                                   "502"
/** 503 */
#define HTTP_STATUS_CODE_503                                   "503"
/** 504 */
#define HTTP_STATUS_CODE_504                                   "504"

/** 
 * @}
 */

/** 
 * @name method
 * @{
 */

/** CONNECT */
#define HTTP_METHOD_CONNECT                                    "CONNECT"
/** DELETE */
#define HTTP_METHOD_DELETE                                     "DELETE"
/** GET */
#define HTTP_METHOD_GET                                        "GET"
/** HEAD */
#define HTTP_METHOD_HEAD                                       "HEAD"
/** OPTIONS */
#define HTTP_METHOD_OPTIONS                                    "OPTIONS"
/** POST */
#define HTTP_METHOD_POST                                       "POST"
/** PUT */
#define HTTP_METHOD_PUT                                        "PUT"
/** TRACE */
#define HTTP_METHOD_TRACE                                      "TRACE"

/** CONNECT */
#define HTTP_METHOD_TYPE_CONNECT                               (1)
/** DELETE */
#define HTTP_METHOD_TYPE_DELETE                                (2)
/** GET */
#define HTTP_METHOD_TYPE_GET                                   (3)
/** HEAD */
#define HTTP_METHOD_TYPE_HEAD                                  (4)
/** OPTIONS */
#define HTTP_METHOD_TYPE_OPTIONS                               (5)
/** POST */
#define HTTP_METHOD_TYPE_POST                                  (6)
/** PUT */
#define HTTP_METHOD_TYPE_PUT                                   (7)
/** TRACE */
#define HTTP_METHOD_TYPE_TRACE                                 (8)

/** 
 * @}
 */

/** 
 * @name header
 * @{
 */

/** accept */
#define HTTP_HEADER_ACCEPT                                     "Accept"
/** accept-charset */
#define HTTP_HEADER_ACCEPT_CHARSET                             "Accept-Charset"
/** accept-encoding */
#define HTTP_HEADER_ACCEPT_ENCODING                            "Accept-Encoding"
/** accept-language */
#define HTTP_HEADER_ACCEPT_LANGUAGE                            "Accept-Language"
/** allow */
#define HTTP_HEADER_ALLOW                                      "Allow"
/** authorization */
#define HTTP_HEADER_AUTHORIZATION                              "Authorization"
/** cache-control */
#define HTTP_HEADER_CACHE_CONTROL                              "Cache-Control"
/** connection */
#define HTTP_HEADER_CONNECTION                                 "Connection"
/** content-encoding */
#define HTTP_HEADER_CONTENT_ENCODING                           "Content-Encoding"
/** content-language */
#define HTTP_HEADER_CONTENT_LANGUAGE                           "Content-Language"
/** content-length */
#define HTTP_HEADER_CONTENT_LENGTH                             "Content-Length"
/** content-location */
#define HTTP_HEADER_CONTENT_LOCATION                           "Content-Location"
/** content-type */
#define HTTP_HEADER_CONTENT_TYPE                               "Content-Type"
/** date */
#define HTTP_HEADER_DATE                                       "Date"
/** expect */
#define HTTP_HEADER_EXPECT                                     "Expect"
/** expires */
#define HTTP_HEADER_EXPIRES                                    "Expires"
/** from */
#define HTTP_HEADER_FROM                                       "From"
/** host */
#define HTTP_HEADER_HOST                                       "Host"
/** if-match */
#define HTTP_HEADER_IF_MATCH                                   "If-Match"
/** if-modified-since */
#define HTTP_HEADER_IF_MODIFIED_SINCE                          "If-Modified-Since"
/** if-none-match */
#define HTTP_HEADER_IF_NONE_MATCH                              "If-None-Match"
/** if-unmodified-since */
#define HTTP_HEADER_IF_UNMODIFIED_SINCE                        "If-Unmodified-Since"
/** last-modified */
#define HTTP_HEADER_LAST_MODIFIED                              "Last-Modified"
/** location */
#define HTTP_HEADER_LOCATION                                   "Location"
/** max-forwards */
#define HTTP_HEADER_MAX_FORWARDS                               "Max-Forwards"
/** pragma */
#define HTTP_HEADER_PRAGMA                                     "Pragma"
/** proxy-authenticate */
#define HTTP_HEADER_PROXY_AUTHENTICATE                         "Proxy-Authenticate"
/** proxy-authorization */
#define HTTP_HEADER_PROXY_AUTHORIZATION                        "Proxy-Authorization"
/** referer */
#define HTTP_HEADER_REFERER                                    "Referer"
/** retry-after */
#define HTTP_HEADER_RETRY_AFTER                                "Retry-After"
/** server */
#define HTTP_HEADER_SERVER                                     "Server"
/** te */
#define HTTP_HEADER_TE                                         "TE"
/** trailer */
#define HTTP_HEADER_TRAILER                                    "Trailer"
/** transfer-encoding */
#define HTTP_HEADER_TRANSER_ENCODING                           "Transfer-Encoding"
/** user-agent */
#define HTTP_HEADER_USER_AGENT                                 "User-Agent"
/** via */
#define HTTP_HEADER_VIA                                        "Via"
/** warning */
#define HTTP_HEADER_WARNING                                    "Warning"
/** www-authenticate */
#define HTTP_HEADER_WWW_AUTHENTICATE                           "WWW-Authenticate"



/** 
 * @}
 */


#endif ///__HTTP_CONST_H__
