/**
 * @file simple_sock_public.h  Simple Sock Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.11.25
 *
 *
 */

#ifndef __SIMPLE_SOCK_PUBLIC_H__
#define __SIMPLE_SOCK_PUBLIC_H__

#include <public/gen_int.h>
#include "net_sock_public.h"

/** simple sock */
typedef struct tag_net_simple_sock
{
	/** sock */
	int32  sock;

	/** multicast */
	int32  b_multicast;
	/** info */
	NET_SOCK_INFO  info;

} NET_SIMPLE_SOCK, *PNET_SIMPLE_SOCK;


#endif /// __SIMPLE_SOCK_PUBLIC_H__
