

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)

#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/in.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <errno.h>

#endif

#include <public/gen_endian.h>
#include <util/log_debug.h>
#include "net_sock.h"


int32 net_sock_open(NET_SOCK* p_sock, int32 type, int32 b_block)
{
	struct sockaddr_in local;
	int32 addr_len = sizeof(local);
	int32 sock_local;
	int32 option;

	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	//memset(p_sock, 0, sizeof(NET_SOCK));
	p_sock->sock = -1;

	switch( type )
	{
	case NET_SOCK_TYPE_TCP:
		{
			sock_local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		break;

	case NET_SOCK_TYPE_UDP:
		{
			sock_local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		}
		break;

	default:
		{
			log_debug(_T("[net_sock::net_sock_open] unknow type = %d\n"), type);
			return -1;
		}
	}

	if( sock_local == -1 )
	{
		log_debug(_T("[net_sock::net_sock_open] sock create error"));
		return -1;
	}

	if( !b_block )
	{
		/** no block */
#if defined(_WIN32)

		option = 1;
		if( ioctlsocket(sock_local, FIONBIO, (uint32*)&option) == SOCKET_ERROR )
		{
			closesocket(sock_local);
			return -1;
		}

#else

		if( fcntl(sock_local, F_SETFD, O_NONBLOCK) == -1 )
		{
			close(sock_local);
			return -1;
		}

#endif
	}

	if( p_sock->addr.local_port != 0 )
	{
		/** reuse addr */
		option = 1;
		ret = setsockopt(sock_local, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, sizeof(option));
		if( ret == SOCKET_ERROR )
		{
			log_debug(_T("[net_sock::net_sock_open] set reuse addr error = %d!\n"), WSAGetLastError());
		}

		local.sin_family = AF_INET;
		local.sin_port = p_sock->addr.local_port;
		local.sin_addr.s_addr = p_sock->addr.local_addr;

		ret = bind(sock_local, (struct sockaddr*)&local, addr_len);
		if( ret == -1 )
		{
#if defined(_WIN32)
			closesocket(sock_local);
#else
			close(sock_local);
#endif
			log_debug(_T("[net_sock::net_sock_open] bind to local port = %d error\n"), ntohs(p_sock->addr.local_port));
			return -1;
		}

		log_debug(_T("[net_sock::net_sock_open] bind to %x:%d\n"), ntohl(p_sock->addr.local_addr), ntohs(p_sock->addr.local_port));
	}

	p_sock->sock = sock_local;

	return 0;
}

int32 net_sock_close(NET_SOCK* p_sock)
{
	if( p_sock == NULL )
	{
		return -1;
	}

	if( p_sock->sock > 0 )
	{
#if defined(_WIN32)

		closesocket(p_sock->sock);
		p_sock->sock = INVALID_SOCKET;

#else

		close(p_sock->sock);
		p_sock->sock = -1;

#endif
	}

	return 0;
}

int32 net_sock_join_group(NET_SOCK* p_sock)
{
	struct ip_mreq mreq;

	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	mreq.imr_interface.s_addr = p_sock->addr.local_addr;
	mreq.imr_multiaddr.s_addr = p_sock->addr.remote_addr;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (int8*)&mreq, sizeof(mreq));

	return ret;
}

int32 net_sock_leave_group(NET_SOCK* p_sock)
{
	struct ip_mreq mreq;

	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	mreq.imr_interface.s_addr = p_sock->addr.local_addr;
	mreq.imr_multiaddr.s_addr = p_sock->addr.remote_addr;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (int8*)&mreq, sizeof(mreq));

	return ret;
}

#if 0
int32 net_sock_join_src_group(NET_SOCK* p_sock, uint32 src_ip)
{
	struct ip_mreq_source mreq_source;

	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	mreq_source.imr_interface.s_addr = p_sock->addr.local_addr;
	mreq_source.imr_multiaddr.s_addr = p_sock->addr.remote_addr;
	mreq_source.imr_sourceaddr.s_addr = src_ip;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (int8*)&mreq_source, sizeof(mreq_source));

	return ret;
}

int32 net_sock_leave_src_group(NET_SOCK* p_sock, uint32 src_ip)
{
	struct ip_mreq_source mreq_source;

	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	mreq_source.imr_interface.s_addr = p_sock->addr.local_addr;
	mreq_source.imr_multiaddr.s_addr = p_sock->addr.remote_addr;
	mreq_source.imr_sourceaddr.s_addr = src_ip;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (int8*)&mreq_source, sizeof(mreq_source));

	return ret;
}
#endif

int32 net_sock_listen(NET_SOCK* p_sock, int32 max_connect)
{
	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	ret = listen(p_sock->sock, max_connect);

	log_debug(_T("[net_sock::net_sock_listen] start listen on %x:%d\n"), ntohl(p_sock->addr.local_addr), ntohs(p_sock->addr.local_port));

	return ret;
}

int32 net_sock_accept(NET_SOCK* p_sock, NET_SOCK* p_accept)
{
	struct sockaddr_in remote;
	int32 addr_len = sizeof(remote);

	int32 ret;

	if( p_sock == NULL || p_accept == NULL )
	{
		return -1;
	}

	ret = accept(p_sock->sock, (struct sockaddr*)&remote, (socklen_t*)&addr_len);
	if( ret == -1 )
	{
		return -1;
	}

	p_accept->sock = ret;
	p_accept->addr.remote_addr = remote.sin_addr.s_addr;
	p_accept->addr.remote_port = remote.sin_port;

	return 0;
}

int32 net_sock_connect(NET_SOCK* p_sock, int32* p_error_code)
{
	struct sockaddr_in remote;
	int32 addr_len = sizeof(remote);
	//int32 error_code;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_sock == NULL || p_error_code == NULL )
	{
		return -1;
	}

	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_port = p_sock->addr.remote_port;
	remote.sin_addr.s_addr = p_sock->addr.remote_addr;

#if 1
	p_uint8 = (uint8*)&p_sock->addr.remote_addr;
	log_debug(_T("[net_sock::net_sock_connect] connect to %u.%u.%u.%u:%d\n"), p_uint8[0], p_uint8[1], p_uint8[2], p_uint8[3], ntoh_u16(p_sock->addr.remote_port));
#endif

	ret = connect(p_sock->sock, (struct sockaddr*)&remote, addr_len);
	if( ret == 0 )
	{
		*p_error_code = 0;

	}else
	{
#if defined(_WIN32)

		*p_error_code = WSAGetLastError();

#else

		*p_error_code = errno;

#endif
	}

	return ret;
}
