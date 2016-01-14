/**
 * @file net_sock_const.h   Net Sock Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.06.03
 *
 *
 */

#ifndef __NET_SOCK_CONST_H__
#define __NET_SOCK_CONST_H__

/** 
 * @name sock version
 * @{
 */

/** unknown */
#define NET_SOCK_VERSION_UNKNOWN                           (0)
/** IPV4 */
#define NET_SOCK_VERSION_IPV4                              (4)
/** IPV6 */
#define NET_SOCK_VERSION_IPV6                              (6)

/** 
 * @}
 */

/** 
 * @name sock type
 * @{
 */

/** unknown */
#define NET_SOCK_TYPE_UNKNOWN                              (0)
/** TCP */
#define NET_SOCK_TYPE_TCP                                  (1)
/** UDP */
#define NET_SOCK_TYPE_UDP                                  (2)
/** SCTP */
#define NET_SOCK_TYPE_SCTP                                 (3)

/** 
 * @}
 */

/** 
 * @name sock flag
 * @{
 */

/** passive */
#define NET_SOCK_FLAG_PASSIVE                              (0x1)


/** 
 * @}
 */

/** 
 * @name sock option
 * @{
 */

/** recv buf size */
#define NET_SOCK_OPTION_RECV_BUF_SIZE                      (1)
/** send buf size */
#define NET_SOCK_OPTION_SEND_BUF_SIZE                      (2)


/** 
 * @}
 */

/** 
 * @name op type
 * @{
 */

/** unknown */
#define NET_SOCK_OP_UNKNOWN                                (0)
/** accept */
#define NET_SOCK_OP_ACCEPT                                 (1)
/** connect */
#define NET_SOCK_OP_CONNECT                                (2)
/** recv */
#define NET_SOCK_OP_RECV                                   (3)
/** send */
#define NET_SOCK_OP_SEND                                   (4)
/** recvfrom */
#define NET_SOCK_OP_RECVFROM                               (5)
/** sendto */
#define NET_SOCK_OP_SENDTO                                 (6)

/** 
 * @}
 */

/** 
 * @name tcp status
 * @{
 */

/** unknown */
#define NET_SOCK_TCP_STATUS_UNKNOWN                        (0)
/** connect */
#define NET_SOCK_TCP_STATUS_CONNECT                        (1)
/** on connect */
#define NET_SOCK_TCP_STATUS_ON_CONNECT                     (2)
/** disconnect */
#define NET_SOCK_TCP_STATUS_DISCONNECT                     (3)


/** 
 * @}
 */


#endif ///__NET_SOCK_CONST_H__
