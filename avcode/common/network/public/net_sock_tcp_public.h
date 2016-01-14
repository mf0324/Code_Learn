/**
 * @file net_sock_tcp_public.h  Net Sock TCP Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.07.01
 *
 *
 */

#ifndef __NET_SOCK_TCP_PUBLIC_H__
#define __NET_SOCK_TCP_PUBLIC_H__

#include <public/gen_int.h>
#include <public/gen_buf.h>
#include "net_sock_public.h"

/** net sock tcp */
typedef struct tag_net_sock_tcp
{
	/** sock */
	int32  sock;

	/** listen */
	int32  b_listen;
	/** can read */
	int32  b_can_read;
	/** can write */
	int32  b_can_write;
	/** connect status */
	int32  connect_status;

	/** info */
	NET_SOCK_INFO  info;

	/** recv */
	GEN_BUF  recv;
	/** send */
	GEN_BUF  send;
	/** send size */
	int32  send_size;
	/** send left size */
	int32  send_left_size;

	/** param */
	void*    p_param;

} NET_SOCK_TCP, *PNET_SOCK_TCP;


#endif /// __NET_SOCK_TCP_PUBLIC_H__
