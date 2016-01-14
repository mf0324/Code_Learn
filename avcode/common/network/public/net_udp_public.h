/**
 * @file net_sock_udp_public.h  Net Sock UDP Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.07.01
 *
 *
 */

#ifndef __NET_SOCK_UDP_PUBLIC_H__
#define __NET_SOCK_UDP_PUBLIC_H__

#include <public/gen_int.h>
#include <public/gen_buf.h>
#include "net_sock_public.h"

/** net sock udp */
typedef struct tag_net_sock_udp
{
	/** sock */
	NET_SOCK_H  sock_h;

	/** info */
	NET_SOCK_INFO  info;

	/** recv */
	GEN_BUF  recv;
	/** send */
	GEN_BUF  send;

	/** param */
	void*    p_param;

} NET_SOCK_UDP, *PNET_SOCK_UDP;


#endif /// __NET_SOCK_UDP_PUBLIC_H__
