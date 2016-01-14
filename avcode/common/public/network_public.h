/**
 * @file network_public.h  Network Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.06.04
 *
 *
 */

#ifndef __NETWORK_PUBLIC_H__
#define __NETWORK_PUBLIC_H__

#include <public/gen_int.h>



/** net addr */
typedef struct tag_net_addr
{
	/** local port */
	uint16  local_port;
	/** remote port */
	uint16  remote_port;

	/** local addr */
	uint32  local_addr;
	/** remote addr */
	uint32  remote_addr;

} NET_ADDR, *PNET_ADDR;

/** net sock */
typedef struct tag_net_sock
{
	/** sock */
	int32  sock;

	/** multicast */
	int32  b_multicast;
	/** addr */
	NET_ADDR  addr;

} NET_SOCK, *PNET_SOCK;

/** udp recv */
typedef struct tag_net_udp_recv
{
	/** buf */
	uint8* p_buf;
	/** buf size */
	int32  buf_size;
	/** use size */
	int32  use_size;
	/** left size */
	int32  left_size;

} NET_UDP_RECV, *PNET_UDP_RECV;

/** udp send */
typedef struct tag_net_udp_send
{
	/** buf */
	uint8* p_buf;
	/** buf size */
	int32  buf_size;
	/** use size */
	int32  use_size;
	/** left size */
	int32  left_size;
	/** send size */
	int32  send_size;
	/** unsend size */
	int32  unsend_size;

} NET_UDP_SEND, *PNET_UDP_SEND;

/** net udp */
typedef struct tag_net_udp
{
	/** sock */
	NET_SOCK  sock;

	/** recv */
	NET_UDP_RECV*  p_recv;
	/** send */
	NET_UDP_SEND*  p_send;

} NET_UDP, *PNET_UDP;

/** tcp recv */
typedef struct tag_net_tcp_recv
{
	/** buf */
	uint8* p_buf;
	/** buf size */
	int32  buf_size;
	/** use size */
	int32  use_size;
	/** left size */
	int32  left_size;

} NET_TCP_RECV, *PNET_TCP_RECV;

/** tcp send */
typedef struct tag_net_tcp_send
{
	/** buf */
	uint8* p_buf;
	/** buf size */
	int32  buf_size;
	/** use size */
	int32  use_size;
	/** left size */
	int32  left_size;
	/** send size */
	int32  send_size;
	/** send left size */
	int32  send_left_size;

} NET_TCP_SEND, *PNET_TCP_SEND;

/** net tcp */
typedef struct tag_net_tcp
{
	/** connect status */
	int32  connect_status;
	/** writeable */
	int32  b_writeable;

	/** sock */
	NET_SOCK  sock;

	/** recv */
	NET_TCP_RECV* p_recv;
	/** send */
	NET_TCP_SEND* p_send;

} NET_TCP, *PNET_TCP;


#endif /// __NETWORK_PUBLIC_H__
