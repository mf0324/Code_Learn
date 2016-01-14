/**
 * @file net_sock_callback.h  Net Sock Callback
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.05.27
 *
 *
 */

#ifndef __NET_SOCK_CALLBACK_H__
#define __NET_SOCK_CALLBACK_H__

#include <public/gen_int.h>
#include <public/gen_buf_public.h>
#include "net_sock_public.h"

/**  
 * @brief accept notify
 * @param [in] listen_sock, listen sock
 * @param [in] accept_sock, accept sock
 * @param [in] p_info, accept sock info
 * @param [in] result, accept result
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*net_sock_accept_notify)(NET_SOCK_H listen_sock, NET_SOCK_H accept_sock, NET_SOCK_INFO* p_info, int32 result, void* p_custom_param);

/**  
 * @brief connect notify
 * @param [in] sock, sock
 * @param [in] result, connect result
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*net_sock_connect_notify)(NET_SOCK_H sock, int32 result, void* p_custom_param);

/**  
 * @brief disconnect notify
 * @param [in] sock, sock
 * @param [in] result, disconnect result
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*net_sock_disconnect_notify)(NET_SOCK_H sock, int32 result, void* p_custom_param);

/**  
 * @brief recv notify
 * @param [in] sock, sock
 * @param [in] result, recv result
 * @param [in] p_info, sock info
 * @param [in] p_buf, recv buf
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. when recv success, buf valid size means buf size, buf use size means recv data size, buf left size means buf unuse size
 */
typedef int32 (*net_sock_recv_notify)(NET_SOCK_H sock, int32 result, NET_SOCK_INFO* p_info, GEN_BUF* p_buf, void* p_custom_param);

/**  
 * @brief send notify
 * @param [in] sock, sock
 * @param [in] result, recv result
 * @param [in] p_sock_info, sock info
 * @param [in] p_buf, send buf
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. when send success, buf valid size means to send data size, buf use size means actual send data size, buf left size means unsend data size
 */
typedef int32 (*net_sock_send_notify)(NET_SOCK_H sock, int32 result, NET_SOCK_INFO* p_info, GEN_BUF* p_buf, void* p_custom_param);


/** sock callback */
typedef struct tag_net_sock_callback
{
	/**	accept */
	net_sock_accept_notify  accept;
	/**	connect */
	net_sock_connect_notify  connect;
	/**	disconnect */
	net_sock_disconnect_notify  disconnect;
	/**	recv */
	net_sock_recv_notify  recv;
	/**	send */
	net_sock_send_notify  send;

	/** custom param */
	void* p_custom_param;

} NET_SOCK_CALLBACK, *PNET_SOCK_CALLBACK;



#endif /// __NET_SOCK_CALLBACK_H__
