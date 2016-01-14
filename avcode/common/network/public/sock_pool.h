/**
 * @file sock_pool.h   Sock  Pool
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.05.27
 *
 *
 */

#ifndef __SOCK_POOL_H__
#define __SOCK_POOL_H__


#include <public/gen_int.h>
#include <public/gen_platform.h>

#include "net_sock_public.h"
#include "net_sock_callback.h"


/** sock pool handle */
typedef void*  SOCK_POOL_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not NULL: success
 * NULL: fail
 */
SOCK_POOL_H sock_pool_create();

/**  
 * @brief destroy
 * @param [in] h, sock pool
 * @return
 * 0: success
 * other: fail
 */
int32 sock_pool_destroy(SOCK_POOL_H h);

/**  
 * @brief init
 * @param [in] h, sock pool
 * @return
 * 0: success
 * other: fail
 */
int32 sock_pool_init(SOCK_POOL_H h);

/**  
 * @brief deinit
 * @param [in] h, sock pool
 * @return
 * 0: success
 * other: fail
 */
int32 sock_pool_deinit(SOCK_POOL_H h);

/**  
 * @brief set callback
 * @param [in] h, sock pool
 * @param [in] p_callback, sock callback
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_set_callback(SOCK_POOL_H h, NET_SOCK_CALLBACK* p_callback);

/**  
 * @brief open sock
 * @param [in] h, sock pool
 * @param [out] p_sock, net sock
 * @param [in] p_info, sock info
 * @param [in] p_custom_param, sock custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. p_info member must be specify
 *    sock_type
 *    sock_flag
 */
int32 sock_pool_open_sock(SOCK_POOL_H h, NET_SOCK_H* p_sock, NET_SOCK_INFO* p_info, void* p_custom_param);

/**  
 * @brief close sock
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_close_sock(SOCK_POOL_H h, NET_SOCK_H sock);

/**  
 * @brief set sock option
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] option, sock option
 * @param [in] p_data, option data
 * @param [in] data_size, option data size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_set_sock_option(SOCK_POOL_H h, NET_SOCK_H sock, int32 option, uint8* p_data, int32 data_size);

/**  
 * @brief get sock info
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [out] p_info, sock info
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_get_sock_info(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_INFO* p_info);

/**  
 * @brief join multicast group
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_info, sock info
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_join_multicast_group(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_INFO* p_info);

/**  
 * @brief leave multicast group
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_info, sock info
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_leave_multicast_group(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_INFO* p_info);

/**  
 * @brief bind sock
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_addr, local addr
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if p_addr's port is 0, it would bind to random port
 */
int32 sock_pool_bind_sock(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_ADDR* p_addr);

/**  
 * @brief listen sock
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 sock_pool_listen_sock(SOCK_POOL_H h, NET_SOCK_H sock);

/**  
 * @brief accept sock
 * @param [in] h, sock pool
 * @param [in] listen_sock, listen sock
 * @param [in] accept_sock, accept sock
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. accept result notify through callback
 */
int32 sock_pool_accept_sock(SOCK_POOL_H h, NET_SOCK_H listen_sock, NET_SOCK_H accept_sock);

/**  
 * @brief connect sock
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_remote, remote addr
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. connect result notify through callback
 * 2. p_remote's member must be specify
 *    ip
 *    port
 */
int32 sock_pool_connect_sock(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_ADDR* p_remote);

/**  
 * @brief recv data
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_buf, data buf
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. recv result notify through callback
 * 2. before recv, p_buf->buf_valid_size means buf size, after recv, p_buf->buf_use_size indicate recv size
 */
int32 sock_pool_recv_data(SOCK_POOL_H h, NET_SOCK_H sock, GEN_BUF* p_buf);

/**  
 * @brief send data
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_buf, data buf
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. send result notify through callback
 * 2. before send, p_buf->buf_valid_size means to send data size, after send, p_buf->buf_use_size indicate send size
 */
int32 sock_pool_send_data(SOCK_POOL_H h, NET_SOCK_H sock, GEN_BUF* p_buf);

/**  
 * @brief recvfrom data
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_buf, data buf
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. recvfrom result notify through callback
 * 2. before recv, p_buf->buf_valid_size means buf size, after recv, p_buf->buf_use_size indicate recv size
 */
int32 sock_pool_recvfrom_data(SOCK_POOL_H h, NET_SOCK_H sock, GEN_BUF* p_buf);

/**  
 * @brief sendto data
 * @param [in] h, sock pool
 * @param [in] sock, net sock
 * @param [in] p_info, sock info
 * @param [in] p_buf, data buf
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. sendto result notify through callback
 * 2. before send, p_buf->buf_valid_size means to send data size, after send, p_buf->buf_use_size indicate send size
 */
int32 sock_pool_sendto_data(SOCK_POOL_H h, NET_SOCK_H sock, NET_SOCK_INFO* p_info, GEN_BUF* p_buf);


#ifdef __cplusplus
}
#endif

#endif ///__SOCK_POOL_H__
