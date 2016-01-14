/**
 * @file simple_sock.h  Simple Sock
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.25
 *
 *
 */

#ifndef __SIMPLE_SOCK_H__
#define __SIMPLE_SOCK_H__

#include <public/gen_int.h>
#include "simple_sock_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief setup
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be call at first!
 */
int32 simple_sock_setup(NET_SIMPLE_SOCK* p_sock);

/** 
 * @brief open
 * @param [in/out] p_sock, sock
 * @param [in] type, sock type
 * @param [in] b_block, block or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. without mention elsewhere, p_sock addr always network endian!
 * 2. addr local_addr means local ip to bind, if not, it would use 0.0.0.0, local_port means port to bind, 0 means not binding
 */
int32 simple_sock_open(NET_SIMPLE_SOCK* p_sock, int32 type, int32 b_block);

/** 
 * @brief close
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
int32 simple_sock_close(NET_SIMPLE_SOCK* p_sock);

/** 
 * @brief join multicast group
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
int32 simple_sock_join_group(NET_SIMPLE_SOCK* p_sock);

/** 
 * @brief leave multicast group
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
int32 simple_sock_leave_group(NET_SIMPLE_SOCK* p_sock);

/** 
 * @brief listen
 * @param [in] p_sock, sock
 * @param [in] max_connection, max connection
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. before listen, it must be bind to one port!
 */
int32 simple_sock_listen(NET_SIMPLE_SOCK* p_sock, int32 max_connection);

/** 
 * @brief accept
 * @param [in] p_sock, sock
 * @param [in] p_accept, acceptor
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be listen before aceept
 */
int32 simple_sock_accept(NET_SIMPLE_SOCK* p_sock, NET_SIMPLE_SOCK* p_accept);

/** 
 * @brief connect
 * @param [in] p_sock, sock
 * @param [out] p_error_code, error code
 * @return
 * 0: success
 * other: fail
 */
int32 simple_sock_connect(NET_SIMPLE_SOCK* p_sock, int32* p_error_code);


#ifdef __cplusplus
}
#endif

#endif ///__SIMPLE_SOCK_H__
