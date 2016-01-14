

#include <stdlib.h>
#include <string.h>

#include <public/gen_platform.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>

#if RUN_OS_WINDOWS

/** for winsock exclude */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

#include <util/log_agent.h>

#include "net_sock_const.h"
#include "simple_sock.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 simple_sock_setup(NET_SIMPLE_SOCK* p_sock)
{
	memset(p_sock, 0, sizeof(*p_sock));

	p_sock->sock = -1;

	return GEN_S_OK;
}

int32 simple_sock_open(NET_SIMPLE_SOCK* p_sock, int32 type, int32 b_block)
{
	struct sockaddr_in local;
	int32 addr_len = sizeof(local);
	int32 sock_local;
	int32 option;

	int32 ret;

	if( p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

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
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("open unknow type = %d\n"), type);
			return GEN_E_INVALID_PARAM;
		}
	}

	if( sock_local == -1 )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock create error!\n"));
		return GEN_E_FAIL;
	}

	if( !b_block )
	{
		/** no block */
#if RUN_OS_WINDOWS

		option = 1;
		ret = ioctlsocket(sock_local, FIONBIO, (uint32*)&option);
		if( ret == SOCKET_ERROR )
		{
			closesocket(sock_local);
			return GEN_E_FAIL;
		}

#else

		ret = fcntl(sock_local, F_SETFD, O_NONBLOCK);
		if( ret == -1 )
		{
			close(sock_local);
			return GEN_E_FAIL;
		}

#endif
	}

	if( p_sock->info.local.port != 0 )
	{
		/** reuse addr */
		option = 1;
		ret = setsockopt(sock_local, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, sizeof(option));
		if( ret == SOCKET_ERROR )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("set reuse addr error = %d!\n"), WSAGetLastError());
		}

		memset(&local, 0, sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = p_sock->info.local.port;
		local.sin_addr.s_addr = p_sock->info.local.addr.ip;

		ret = bind(sock_local, (struct sockaddr*)&local, addr_len);
		if( ret == -1 )
		{
#if RUN_OS_WINDOWS
			closesocket(sock_local);
#else
			close(sock_local);
#endif
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("bind to local port = %d error\n"), ntoh_u16(p_sock->info.local.port));
			return GEN_E_FAIL;
		}

		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("bind to %x:%d\n"), ntoh_u32(p_sock->info.local.addr.ip), ntoh_u16(p_sock->info.local.port));
	}

	p_sock->sock = sock_local;

	return GEN_S_OK;
}

int32 simple_sock_close(NET_SIMPLE_SOCK* p_sock)
{
	if( p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_sock->sock > 0 )
	{
#if RUN_OS_WINDOWS

		closesocket(p_sock->sock);
		p_sock->sock = INVALID_SOCKET;

#else

		close(p_sock->sock);
		p_sock->sock = -1;

#endif
	}

	return GEN_S_OK;
}

int32 simple_sock_join_group(NET_SIMPLE_SOCK* p_sock)
{
	struct ip_mreq mreq;

	int32 ret;

	if( p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	mreq.imr_interface.s_addr = p_sock->info.local.addr.ip;
	mreq.imr_multiaddr.s_addr = p_sock->info.remote.addr.ip;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (int8*)&mreq, sizeof(mreq));

	return ret;
}

int32 simple_sock_leave_group(NET_SIMPLE_SOCK* p_sock)
{
	struct ip_mreq mreq;

	int32 ret;

	if( p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	mreq.imr_interface.s_addr = p_sock->info.local.addr.ip;
	mreq.imr_multiaddr.s_addr = p_sock->info.remote.addr.ip;

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (int8*)&mreq, sizeof(mreq));

	return ret;
}

int32 simple_sock_listen(NET_SIMPLE_SOCK* p_sock, int32 max_connection)
{
	int32 ret;

	if( p_sock == NULL )
	{
		return -1;
	}

	//ret = listen(p_sock->sock, max_connection);

	return GEN_E_NOT_IMPLEMENT;
}

int32 simple_sock_accept(NET_SIMPLE_SOCK* p_sock, NET_SIMPLE_SOCK* p_accept)
{
	struct sockaddr_in remote;
	int32 addr_len = sizeof(remote);

	int32 ret;

	if( p_sock == NULL || p_accept == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if 0
	ret = accept(p_sock->sock, (struct sockaddr*)&remote, (socklen_t*)&addr_len);
	if( ret == -1 )
	{
		return -1;
	}

	p_accept->sock = ret;
	p_accept->addr.remote_addr = remote.sin_addr.s_addr;
	p_accept->addr.remote_port = remote.sin_port;
#endif

	return GEN_E_NOT_IMPLEMENT;
}

int32 simple_sock_connect(NET_SIMPLE_SOCK* p_sock, int32* p_error_code)
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

#if 0

	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_port = p_sock->addr.remote_port;
	remote.sin_addr.s_addr = p_sock->addr.remote_addr;

	p_uint8 = (uint8*)&p_sock->addr.remote_addr;
	log_debug(_T("[net_sock::net_sock_connect] connect to %u.%u.%u.%u:%d\n"), p_uint8[0], p_uint8[1], p_uint8[2], p_uint8[3], ntoh_u16(p_sock->addr.remote_port));


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

#endif

	return GEN_E_NOT_IMPLEMENT;
}
