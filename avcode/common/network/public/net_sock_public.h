/**
 * @file net_sock_public.h  Net Sock Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.05.27
 *
 *
 */

#ifndef __NET_SOCK_PUBLIC_H__
#define __NET_SOCK_PUBLIC_H__

#include <public/gen_int.h>

/** net sock handle */
typedef void*  NET_SOCK_H;


/** sock addr */
typedef struct tag_net_sock_addr
{
	/** version */
	int32  version;

	/** addr */
	union
	{
		/** ip, network endian */
		uint32  ip;
		/** sub ip */
		uint8   sub_ip[4];
		/** ipv6 */
		uint8   ipv6[16];

	}addr;

	/** port, network endian */
	uint16  port;
	/** resv */
	uint16  resv;

} NET_SOCK_ADDR, *PNET_SOCK_ADDR;

/** sock info */
typedef struct tag_net_sock_info
{
	/** sock type */
	int32   sock_type;
	/** sock flag */
	uint32  sock_flag;

	/** local */
	NET_SOCK_ADDR  local;
	/** remote */
	NET_SOCK_ADDR  remote;

} NET_SOCK_INFO, *PNET_SOCK_INFO;


#endif /// __NET_SOCK_PUBLIC_H__
