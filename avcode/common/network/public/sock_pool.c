

#include <public/gen_platform.h>

#include <stdlib.h>
#include <string.h>

#if RUN_OS_WINDOWS

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <mswsock.h>

#else

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/in.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <errno.h>

#endif

#include <public/gen_endian.h>
#include <public/gen_list.h>
#include <public/gen_thread.h>
#include <public/thread_mutexer.h>
#include <public/msg_queue.h>
#include <public/gen_error.h>
#include <network/public/net_sock_const.h>
#include <util/log_agent.h>
#include "sock_pool_const.h"
#include "sock_pool.h"


/** max work thread */
#define SOCK_POOL_MAX_WORK_THREAD                          (16)
/** default work thread */
#define SOCK_POOL_DEFAULT_WORK_THREAD                      (4)
/** worker wait time, unit: ms */
#define SOCK_POOL_WORKER_WAIT_TIME                         (1000)

/** max accept buf size */
#define SOCK_POOL_MAX_ACCEPT_BUF_SIZE                      (128)

/** sleep time(second), unit: second */
#define SOCK_POOL_SLEEP_TIME_S                             (0)
/** sleep time(millisecond), unit: ms */
#define SOCK_POOL_SLEEP_TIME_MS                            (100)
/** max wait event */
#define SOCK_POOL_MAX_WAIT_EVENT                           (1024)
/** max wait event timeout, unit: ms */
#define SOCK_POOL_MAX_WAIT_EVENT_TIMEOUT                   (100)

typedef struct tag_sock_obj  SOCK_OBJ, *PSOCK_OBJ;
typedef struct tag_sock_worker SOCK_WORKER;
typedef struct tag_sock_pool SOCK_POOL;

/** sock tcp */
typedef struct tag_net_sock_tcp
{
	/** listen */
	int32  b_listen;
	/** readable */
	int32  b_readable;
	/** writeable */
	int32  b_writeable;
	/** connect status */
	int32  connect_status;

#if !RUN_OS_WINDOWS

	/** wait read */
	int32  b_wait_read;
	/** wait write */
	int32  b_wait_write;

#endif

} NET_SOCK_TCP, *PNET_SOCK_TCP;

/** sock buf */
typedef struct tag_net_sock_buf
{
#if RUN_OS_WINDOWS

	/** overlap */
	OVERLAPPED  overlap;
	/** op type */
	int32  op_type;
	/** sock */
	SOCK_OBJ*  p_sock;

#else

#endif

	/** buf */
	GEN_BUF  buf;

} NET_SOCK_BUF, *PNET_SOCK_BUF;

/** sock obj */
struct tag_sock_obj
{
	/** ref count */
	int32  ref_count;
	/** mutex */
	THREAD_MUTEXER  mutex;

	/** sock */
	int32  sock;
	/** origin sock */
	int32  origin_sock;
	/** listen sock */
	SOCK_OBJ* p_listen_sock;
	/** info */
	NET_SOCK_INFO  info;

	/** multicast */
	int32  b_multicast;
	/** tcp */
	NET_SOCK_TCP   tcp;
	/** remote addr */
	struct sockaddr_in remote;
	/** remote addr size */
	int32  remote_addr_size;

	/** node */
	LIST_NODE*  p_node;
	/** user data */
	void*  p_user_data;
	/** callback */
	//NET_SOCK_CALLBACK callback;

#if RUN_OS_WINDOWS
	/** accept buf */
	uint8  accept_buf[SOCK_POOL_MAX_ACCEPT_BUF_SIZE];
	/** accept */
	NET_SOCK_BUF  accept;

#else

	SOCK_WORKER*  p_worker;
	struct epoll_event  ep_event;

#endif

	/** recv buf */
	NET_SOCK_BUF  recv;
	/** send buf */
	NET_SOCK_BUF  send;

};

#if !RUN_OS_WINDOWS

typedef struct tag_sock_worker
{
	/** epoll fd */
	int32   epoll_fd;
	/** sock list */
	GEN_LIST  sock_list;
	/** send list */
	//GEN_LIST  send_list;

	/** msq */
	MSQ_H      msq;
	/** thread */
	GEN_THREAD work_thread;

	/** pool */
	SOCK_POOL*  p_pool;

} SOCK_WORKER, *PSOCK_WORKER;

#endif


/** sock pool */
typedef struct tag_sock_pool
{
	/** init */
	int32   b_init;

	/** mutex */
	THREAD_MUTEXER mutex;
	/** callback */
	NET_SOCK_CALLBACK callback;

#if RUN_OS_WINDOWS

	/** complete port */
	HANDLE  cp_h;
	/** work thread */
	GEN_THREAD work_thread[SOCK_POOL_MAX_WORK_THREAD];
	/** work thread num */
	int32   work_thread_num;

	/** sock list */
	GEN_LIST  sock_list;

#else

	/** main worker */
	SOCK_WORKER   main;
	/** sub worker */
	SOCK_WORKER   sub[SOCK_POOL_MAX_WORK_THREAD];
	/** work thread num */
	int32   work_thread_num;

	/** accept sock */
	SOCK_OBJ* p_accept_sock;

#endif

} SOCK_POOL, *PSOCK_POOL;


#if RUN_OS_WINDOWS

DWORD WINAPI sock_pool_worker(LPVOID p_param);

#else

void* sock_pool_main_worker(void* p_param);
void* sock_pool_sub_worker(void* p_param);

#endif

/////////////////////////////// Inner Interface ///////////////////////////////
#if RUN_OS_WINDOWS
/**  
 * @brief init cp
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_cp(SOCK_POOL* p_pool)
{
	p_pool->cp_h = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if( p_pool->cp_h == NULL )
	{
		return GEN_E_FAIL;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit cp
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_cp(SOCK_POOL* p_pool)
{
	if( p_pool->cp_h )
	{
		CloseHandle(p_pool->cp_h);
		p_pool->cp_h = NULL;
	}

	return GEN_S_OK;
}

#endif

/**  
 * @brief init work thread
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_work_thread(SOCK_POOL* p_pool)
{
	GEN_THREAD* p_thread = NULL;
	SYSTEM_INFO sys_info;

	int32   i;
	int32   ret;

	GetSystemInfo(&sys_info);
	p_pool->work_thread_num = sys_info.dwNumberOfProcessors * 2 + 2;
	if( p_pool->work_thread_num > SOCK_POOL_MAX_WORK_THREAD )
	{
		p_pool->work_thread_num = SOCK_POOL_MAX_WORK_THREAD;
	}

	for( i = 0; i < p_pool->work_thread_num; i++ )
	{
		p_thread = p_pool->work_thread + i;
		p_thread->p_user_data = p_pool;

		ret = gen_thread_init(p_thread);
		ret = gen_thread_open(p_thread, sock_pool_worker, p_thread);
		if( ret )
		{
			//break;
		}
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit work thread
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_work_thread(SOCK_POOL* p_pool)
{
	GEN_THREAD* p_thread = NULL;

	int32   i;
	//int32   ret;

	for( i = 0; i < p_pool->work_thread_num; i++ )
	{
		p_thread = p_pool->work_thread + i;
		p_thread->b_exit = 1;
	}

	for( i = 0; i < p_pool->work_thread_num; i++ )
	{
		p_thread = p_pool->work_thread + i;
		gen_thread_close(p_thread);
		gen_thread_deinit(p_thread);
	}

	return GEN_S_OK;
}

/**  
 * @brief init sock list
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_sock_list(SOCK_POOL* p_pool)
{
	gen_list_init(&p_pool->sock_list);

	return GEN_S_OK;
}

/**  
 * @brief deinit sock list
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_sock_list(SOCK_POOL* p_pool)
{
	SOCK_OBJ* p_sock = NULL;

	int32   ret;

	while( 1 )
	{
		ret = gen_list_get_front_data(&p_pool->sock_list, &p_sock);
		if( ret )
		{
			break;
		}

		gen_list_pop_front(&p_pool->sock_list);

		/** fuck my lazy */
		if( p_sock->sock != INVALID_SOCKET )
		{
			closesocket(p_sock->sock);
			p_sock->sock = INVALID_SOCKET;
		}

		free(p_sock);
	}


	gen_list_deinit(&p_pool->sock_list);

	return GEN_S_OK;
}

/**  
 * @brief add ref
 * @param [in] p_pool, pool
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
static inline int32 sock_pool_add_ref_sock(SOCK_POOL* p_pool, SOCK_OBJ* p_sock)
{
	thread_mutexer_lock(&p_sock->mutex);

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

	return GEN_S_OK;
}

/**  
 * @brief release
 * @param [in] p_pool, pool
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
static inline int32 sock_pool_release_sock(SOCK_POOL* p_pool, SOCK_OBJ* p_sock)
{
	thread_mutexer_lock(&p_sock->mutex);

	p_sock->ref_count--;

	if( p_sock->ref_count < 1 )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock %u release\n"), p_sock->origin_sock);

		thread_mutexer_unlock(&p_sock->mutex);
		thread_mutexer_cleanup(&p_sock->mutex);

		free(p_sock);

		return GEN_S_OK;
	}

	thread_mutexer_unlock(&p_sock->mutex);

	return GEN_S_OK;
}

/**  
 * @brief open sock
 * @param [in] p_pool, pool
 * @param [in] p_sock, sock
 * @param [in] p_info, sock info
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_open_sock_inner(SOCK_POOL* p_pool, SOCK_OBJ* p_sock, NET_SOCK_INFO* p_info)
{
	struct sockaddr_in local;
	int32  addr_len = sizeof(local);
	int32  sock_local;
	int32  option;

	int32  ret;


#if RUN_OS_WINDOWS

	p_sock->sock = INVALID_SOCKET;

#else

	p_sock->sock = -1;

	if( p_info->sock_flag & NET_SOCK_FLAG_PASSIVE )
	{
		/** passive */
		p_sock->info = *p_info;
		return GEN_S_OK;
	}

#endif

	switch( p_info->sock_type )
	{
	case NET_SOCK_TYPE_TCP:
		{
#if RUN_OS_WINDOWS
			sock_local = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
			sock_local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
		}
		break;

	case NET_SOCK_TYPE_UDP:
		{
#if RUN_OS_WINDOWS
			sock_local = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
			sock_local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
		}
		break;

	default:
		{
			//log_debug(_T("[net_sock::net_sock_open] unknow type = %d\n"), type);
			return GEN_E_NOT_SUPPORT;
		}
	}

#if RUN_OS_WINDOWS
	if( sock_local == INVALID_SOCKET )
#else
	if( sock_local == -1 )
#endif
	{
		return GEN_E_FAIL;
	}

	/** no block */
#if RUN_OS_WINDOWS

	option = 1;
	ret = ioctlsocket(sock_local, FIONBIO, (uint32*)&option);
	if( ret == SOCKET_ERROR )
	{
		goto ERROR_FIN;
	}

#else

	ret = fcntl(sock_local, F_SETFD, O_NONBLOCK);
	if( ret == -1 )
	{
		goto ERROR_FIN;
	}

#endif

	p_sock->sock = sock_local;
	p_sock->origin_sock = sock_local;
	p_sock->b_multicast = 0;
	p_sock->tcp.b_listen = 0;
	p_sock->tcp.b_writeable = 0;
	p_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_DISCONNECT;

#if !RUN_OS_WINDOWS
	p_sock->tcp.b_wait_read = 0;
	p_sock->tcp.b_wait_write = 0;
#endif

	p_sock->info = *p_info;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock %u open\n"), p_sock->sock);

	return GEN_S_OK;

ERROR_FIN:

#if RUN_OS_WINDOWS

	if( sock_local != INVALID_SOCKET )
	{
		closesocket(sock_local);
		sock_local = INVALID_SOCKET;
	}

#else

	if( sock_local != -1 )
	{
		close(sock_local);
		sock_local = -1;
	}

#endif

	return GEN_E_FAIL;
}

/**  
 * @brief close sock
 * @param [in] p_pool, pool
 * @param [in] p_sock, sock
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_close_sock_inner(SOCK_POOL* p_pool, SOCK_OBJ* p_sock)
{
#if RUN_OS_WINDOWS

	if( p_sock->sock != INVALID_SOCKET )
	{
		closesocket(p_sock->sock);
		p_sock->sock = INVALID_SOCKET;
	}

#else

	if( p_sock->sock != -1 )
	{
		close(p_sock->sock);
		p_sock->sock = -1;
	}

#endif

	return GEN_S_OK;
}

/**  
 * @brief init worker msq
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_worker_msq(SOCK_POOL* p_pool, SOCK_WORKER* p_worker)
{
	p_worker->msq = msq_create();
	if( p_worker->msq == NULL )
	{
		return GEN_E_FAIL;
	}

	msq_init(p_worker->msq);

	return GEN_S_OK;
}

/**  
 * @brief deinit worker msq
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_worker_msq(SOCK_POOL* p_pool, SOCK_WORKER* p_manager)
{
	if( p_manager->msq )
	{
		msq_destroy(p_manager->msq);
		p_manager->msq = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief setup worker
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_setup_worker(SOCK_POOL* p_pool, SOCK_WORKER* p_worker)
{
	p_worker->epoll_fd = -1;

	gen_list_setup(&p_worker->sock_list, NULL);

	p_worker->msq = NULL;

	gen_thread_setup(&p_worker->work_thread);
	p_worker->work_thread.p_user_data = p_worker;

	p_worker->p_pool = p_pool;

	return GEN_S_OK;
}

/**  
 * @brief setup main worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_setup_main_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = &p_pool->main;

	int32   ret;

	ret = sock_pool_setup_worker(p_pool, p_worker);

	return GEN_S_OK;
}

/**  
 * @brief init main worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_main_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = &p_pool->main;

	int32   ret;

	p_worker->epoll_fd = epoll_create(1024);
	if( p_worker->epoll_fd == -1 )
	{
		return GEN_E_FAIL;
	}

	ret = gen_list_init(&p_worker->sock_list);

	ret = sock_pool_init_worker_msq(p_pool, p_worker);

	ret = gen_thread_init(&p_worker->work_thread);
	ret = gen_thread_open(&p_worker->work_thread, sock_pool_main_worker, &p_worker->work_thread);

	return GEN_S_OK;
}

/**  
 * @brief deinit main worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_main_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = &p_pool->main;

	int32   ret;

	ret = gen_thread_close(&p_worker->work_thread);

	ret = sock_pool_deinit_worker_msq(p_pool, p_worker);

	/** fuck my lazy */
	ret = gen_list_deinit(&p_worker->sock_list);

	if( p_worker->epoll_fd > 0 )
	{
		close(p_worker->epoll_fd);
		p_worker->epoll_fd = -1;
	}

	return GEN_S_OK;
}

/**  
 * @brief setup sub worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_setup_sub_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = NULL;

	int32   i;
	int32   ret;

	p_pool->work_thread_num = 0;
	for( i = 0; i < SOCK_POOL_MAX_WORK_THREAD; i++ )
	{
		p_worker = p_pool->sub + i;
		ret = sock_pool_setup_worker(p_pool, p_worker);
	}

	return GEN_S_OK;
}

/**  
 * @brief init sub worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_init_sub_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = NULL;

	int32   i;
	int32   ret;

	p_pool->work_thread_num = SOCK_POOL_DEFAULT_WORK_THREAD;
	for( i = 0; i < p_pool->work_thread_num; i++ )
	{
		p_worker = p_pool->sub + i;

		p_worker->epoll_fd = epoll_create(1024);
		if( p_worker->epoll_fd == -1 )
		{
			continue;
		}

		ret = gen_list_init(&p_worker->sock_list);

		ret = sock_pool_init_worker_msq(p_pool, p_worker);

		ret = gen_thread_init(&p_worker->work_thread);
		ret = gen_thread_open(&p_worker->work_thread, sock_pool_sub_worker, &p_worker->work_thread);
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit sub worker
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_deinit_sub_worker(SOCK_POOL* p_pool)
{
	SOCK_WORKER* p_worker = NULL;

	int32   i;
	int32   ret;

	for( i = 0; i < p_pool->work_thread_num; i++ )
	{
		p_worker = p_pool->sub + i;

		ret = gen_thread_close(&p_worker->work_thread);

		ret = sock_pool_deinit_worker_msq(p_pool, p_worker);

		/** fuck my lazy */
		ret = gen_list_deinit(&p_worker->sock_list);

		if( p_worker->epoll_fd > 0 )
		{
			close(p_worker->epoll_fd);
			p_worker->epoll_fd = -1;
		}
	}

	return GEN_S_OK;
}

/**  
 * @brief add sock
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_sock, sock obj
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_add_sock2(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_sock)
{
	LIST_NODE*  p_node = NULL;

	int32   ret;

	//thread_mutexer_lock(&p_pool->mutex);

	ret = gen_list_push_back_data(&p_worker->sock_list, p_sock, &p_node);

	//thread_mutexer_unlock(&p_pool->mutex);

	if( ret == 0 )
	{
		p_sock->p_node = p_node;
	}

	p_sock->ep_event.events = EPOLLERR | EPOLLET;
	p_sock->ep_event.data.ptr = p_sock;
	ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_ADD, p_sock->sock, &event);

	return GEN_S_OK;
}

/**  
 * @brief delete sock
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_sock, sock obj
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_delete_sock2(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_sock)
{
	LIST_NODE*  p_node = NULL;

	int32   ret;

	p_node = p_sock->p_node;

	ret = gen_list_delete_node(&p_worker->sock_list, p_node);

	ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_DEL, p_sock->sock, NULL);

	ret = sock_pool_close_sock_inner(p_pool, p_sock);

	sock_pool_release_sock(p_pool, p_sock);

	return GEN_S_OK;
}

/**  
 * @brief accept sock
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_sock, sock obj
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_accept_sock2(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_sock)
{
	LIST_NODE*  p_node = NULL;

	int32   ret;

	//thread_mutexer_lock(&p_pool->mutex);

	ret = gen_list_push_back_data(&p_worker->sock_list, p_sock, &p_node);

	//thread_mutexer_unlock(&p_pool->mutex);

	if( ret == 0 )
	{
		p_sock->p_node = p_node;
	}

	p_sock->ep_event.events = EPOLLERR | EPOLLET;
	p_sock->ep_event.data.ptr = p_sock;
	ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_ADD, p_sock->sock, &event);

	if( p_sock->callback.accept )
	{
		ret = p_sock->callback.accept(p_sock->p_listen_sock, p_sock, &p_sock->info, GEN_S_OK, p_sock->callback.p_custom_param);
	}

	return GEN_S_OK;
}

/**  
 * @brief listen
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_sock, sock obj
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_listen2(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_sock)
{
	int32   error_code;
	int32   ret;

	ret = listen(p_sock->sock, 1024);
	if( ret == -1 )
	{
		error_code = errno;
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("listen2 error = %d\n"), error_code);
		return GEN_E_FAIL;
	}

	p_sock->ep_event.events |= EPOLLIN;
	ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);

	return GEN_S_OK;
}

/**  
 * @brief connect
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_sock, sock obj
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_connect2(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_sock)
{
	int32   error_code;
	int32   ret;

	//p_uint8 = (uint8*)&p_sock->addr.remote_addr;
	//log_debug(_T("[net_sock::net_sock_connect] connect to %u.%u.%u.%u:%d\n"), p_uint8[0], p_uint8[1], p_uint8[2], p_uint8[3], ntoh_u16(p_sock->addr.remote_port));

	p_sock->remote_addr_size = sizeof(p_sock->remote);

	ret = connect(p_sock->sock, (struct sockaddr*)&p_sock->remote, p_sock->remote_addr_size);
	if( ret == 0 )
	{
		p_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_CONNECT;
		p_sock->tcp.b_writeable = 1;
		p_sock->tcp.b_wait_read = 1;
		p_sock->tcp.b_wait_write = 0;

		if( p_sock->callback.connect )
		{
			ret = p_sock->callback.connect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
		}

		return GEN_S_OK;

	}else if( ret == -1 )
	{
		error_code = errno;

		if( error_code == EINPROGRESS )
		{
			p_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_ON_CONNECT;
			p_sock->tcp.b_writeable = 0;
			p_sock->tcp.b_wait_read = 1;
			p_sock->tcp.b_wait_write = 1;

			p_sock->ep_event.events |= EPOLLOUT;
			ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);

		}else
		{
			//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("connect2 error = %d\n"), error_code);

			if( p_sock->callback.connect )
			{
				ret = p_sock->callback.connect(p_sock, GEN_E_FAIL, p_sock->callback.p_custom_param);
			}
		}
	}

	return GEN_S_OK;
}

/**  
 * @brief accept
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_listen_sock, listen sock obj
 * @param [in] accept_sock, accept sock
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_accept_connection(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, SOCK_OBJ* p_listen_sock, int32 accept_sock)
{
	SOCK_OBJ*  p_new_sock = NULL;

	MSQ_MSG  msg;
	SOCK_WORKER* p_worker = NULL;
	SOCK_WORKER* p_min_worker = NULL;
	int32  min_sock_num;

	int32   error_code;
	int32   i;
	int32   ret;

	if( p_pool->p_accept_sock == NULL )
	{
		return GEN_E_FAIL;
	}

	p_new_sock = p_pool->p_accept_sock;
	p_pool->p_accept_sock = NULL;

	p_new_sock->sock = accept_sock;
	p_new_sock->origin_sock = accept_sock;
	p_new_sock->p_listen_sock = p_listen_sock;

	p_new_sock->info.local = p_listen_sock->info.local;
	p_new_sock->info.remote.addr.ip = p_listen_sock->remote.sin_addr.s_addr;
	p_new_sock->info.remote.port = p_listen_sock->remote.sin_port;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u local host %d.%d.%d.%d:%d accept from %d.%d.%d.%d:%d\n"),
		p_new_sock->sock,
		p_new_sock->info.local.addr.sub_ip[0], p_new_sock->info.local.addr.sub_ip[1], p_new_sock->info.local.addr.sub_ip[2], p_new_sock->info.local.addr.sub_ip[3], hton_u16(p_new_sock->info.local.port),
		p_new_sock->info.remote.addr.sub_ip[0], p_new_sock->info.remote.addr.sub_ip[1], p_new_sock->info.remote.addr.sub_ip[2], p_new_sock->info.remote.addr.sub_ip[3], hton_u16(p_new_sock->info.remote.port));

	p_new_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_CONNECT;
	p_new_sock->tcp.b_listen = 0;
	p_new_sock->tcp.b_writeable = 1;
	p_new_sock->tcp.b_wait_read = 1;
	p_new_sock->tcp.b_wait_write = 0;

	/** dispatch sock to worker thread */
	min_sock_num = p_pool->sub[0].sock_list.list_size;
	p_min_worker = &p_pool->sub[0];
	for( i = 1; i < p_pool->work_thread_num; i++ )
	{
		p_worker = p_pool->sub + i;

		if( p_worker->sock_list.list_size < min_sock_num )
		{
			min_sock_num = p_worker->sock_list.list_size;
			p_min_worker = p_worker;
		}
	}

	p_new_sock->p_worker = p_min_worker;

	msg.id = SOCK_POOL_MSG_ACCEPT_SOCK;
	msg.param1 = p_new_sock;
	msg.param2 = NULL;

	ret = msq_send_msg(p_min_worker->msq, &msg, 0);

	return GEN_S_OK;
}

/**  
 * @brief handle msg
 * @param [in] p_pool, pool
 * @param [in] p_worker, worker
 * @param [in] p_msg, msg
 * @param [in] b_sync, sync or not
 * @return
 * 0: success
 * other: fail
 */
static int32 sock_pool_handle_worker_msg(SOCK_POOL* p_pool, SOCK_WORKER* p_worker, MSQ_MSG* p_msg, int32 b_sync)
{
	SOCK_OBJ*  p_sock = NULL;

	int32   error_code;
	int32   ret;

	error_code = 0;

	switch( p_msg->id )
	{
	case SOCK_POOL_MSG_ADD_SOCK:
		{
			p_sock = (SOCK_OBJ*)p_msg->param1;
			error_code = sock_pool_add_sock2(p_pool, p_worker, p_sock);
		}
		break;

	case SOCK_POOL_MSG_DEL_SOCK:
		{
			p_sock = (SOCK_OBJ*)p_msg->param1;
			error_code = sock_pool_delete_sock2(p_pool, p_worker, p_sock);
		}
		break;

	case SOCK_POOL_MSG_ACCEPT_SOCK:
		{
			p_sock = (SOCK_OBJ*)p_msg->param1;
			error_code = sock_pool_accept_sock2(p_pool, p_worker, p_sock);
		}
		break;

	case SOCK_POOL_MSG_LISTEN:
		{
			error_code = sock_pool_listen2(p_pool, p_worker, p_sock);
		}
		break;

	case SOCK_POOL_MSG_CONNECT:
		{
			error_code = sock_pool_connect2(p_pool, p_worker, p_sock);
		}
		break;

	case SOCK_POOL_MSG_DISCONNECT:
		{
			//error_code = sock_pool_disconnect2(p_pool, p_worker, p_sock);
		}
		break;

	default:
		{
			error_code = GEN_E_NOT_SUPPORT;
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("unknown msg id = %d\n"), p_msg->id);
		}
		break;
	}

	//if( b_sync )
	//{
	//	SetEvent(p_engine->event);
	//}

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
SOCK_POOL_H sock_pool_create()
{
	SOCK_POOL* p_pool = NULL;

	int32   i;

	p_pool = (SOCK_POOL*)malloc( sizeof(SOCK_POOL) );
	if( p_pool == NULL )
	{
		return NULL;
	}

	thread_mutexer_setup(&p_pool->mutex);

#if RUN_OS_WINDOWS

	p_pool->cp_h = NULL;

	p_pool->work_thread_num = 0;
	for( i = 0; i < SOCK_POOL_MAX_WORK_THREAD; i++ )
	{
		gen_thread_setup(p_pool->work_thread + i);
	}

	gen_list_setup(&p_pool->sock_list, NULL);


#else

	sock_pool_setup_main_worker(p_pool);
	sock_pool_setup_sub_worker(p_pool);

	p_pool->p_accept_sock = NULL;

#endif

	p_pool->b_init = 0;

	return p_pool;
}

int32 sock_pool_destroy(SOCK_POOL_H h)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;

	//int32   ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_pool->b_init )
	{
		sock_pool_deinit(h);
	}

	free(p_pool);

	return GEN_S_OK;
}

int32 sock_pool_init(SOCK_POOL_H h)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;

#if RUN_OS_WINDOWS
	WSADATA wsd;
#endif

	int32   ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_pool->b_init )
	{
		return GEN_S_OK;
	}

	ret = thread_mutexer_init(&p_pool->mutex);
	memset(&p_pool->callback, 0, sizeof(p_pool->callback));

#if RUN_OS_WINDOWS

	WSAStartup(0x0202, &wsd);

	ret = sock_pool_init_cp(p_pool);
	ret = sock_pool_init_work_thread(p_pool);
	ret = sock_pool_init_sock_list(p_pool);

#else

	ret = sock_pool_init_main_worker(p_pool);
	ret = sock_pool_init_sub_worker(p_pool);

	p_pool->p_accept_sock = NULL;

#endif


	p_pool->b_init = 1;

	return GEN_S_OK;
}

int32 sock_pool_deinit(SOCK_POOL_H h)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;

	int32   ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	ret = sock_pool_deinit_work_thread(p_pool);
	ret = sock_pool_deinit_sock_list(p_pool);
	ret = sock_pool_deinit_cp(p_pool);

	if( p_pool->b_init )
	{
		WSACleanup();
	}

#else

	ret = sock_pool_deinit_sub_worker(p_pool);
	ret = sock_pool_deinit_main_worker(p_pool);

#endif

	ret = thread_mutexer_deinit(&p_pool->mutex);

	p_pool->b_init = 0;

	return GEN_S_OK;
}

int32 sock_pool_set_callback(SOCK_POOL_H h, NET_SOCK_CALLBACK* p_callback)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;

	//int32   ret;

	if( p_pool == NULL || p_callback == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_pool->callback = *p_callback;

	return GEN_S_OK;
}

//int32 sock_pool_open_sock(SOCK_POOL_H h, NET_SOCK_H* p_net_sock, NET_SOCK_INFO* p_info, NET_SOCK_CALLBACK* p_callback)
int32 sock_pool_open_sock(SOCK_POOL_H h, NET_SOCK_H* p_sock, NET_SOCK_INFO* p_info, void* p_custom_param)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_temp = NULL;

#if RUN_OS_WINDOWS

	HANDLE  h_cp = NULL;

	LIST_NODE* p_node = NULL;

#else

	MSQ_MSG  msg;
	SOCK_WORKER* p_worker = NULL;
	SOCK_WORKER* p_min_worker = NULL;
	int32  min_sock_num;

#endif

	int32   i;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_info == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_sock = NULL;

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_temp = (SOCK_OBJ*)malloc( sizeof(SOCK_OBJ) );
	if( p_temp == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	memset(p_temp, 0, sizeof(*p_temp));
	ret = sock_pool_open_sock_inner(p_pool, p_temp, p_info);
	if( ret )
	{
		free(p_temp);
		p_temp = NULL;

		return GEN_E_FAIL;
	}

	p_temp->p_user_data = p_custom_param;

	thread_mutexer_setup(&p_temp->mutex);
	//thread_mutexer_init(&p_temp->mutex);
	p_temp->ref_count = 1;

#if RUN_OS_WINDOWS

	p_temp->accept.p_sock = p_temp;
	p_temp->recv.p_sock = p_temp;
	p_temp->send.p_sock = p_temp;

	/** dispatch sock to completion port */
	h_cp = CreateIoCompletionPort((HANDLE)p_temp->sock, p_pool->cp_h, (ULONG_PTR)p_temp, 0);
	if( h_cp == NULL )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("create io cp error = %d\n"), GetLastError());
	}

	thread_mutexer_lock(&p_pool->mutex);

	ret = gen_list_push_back_data(&p_pool->sock_list, p_temp, &p_node);

	thread_mutexer_unlock(&p_pool->mutex);

	if( ret == 0 )
	{
		p_temp->p_node = p_node;
	}

#else

	if( p_info->sock_flag & NET_SOCK_FLAG_PASSIVE )
	{

	}else
	{
		/** dispatch sock to worker thread */
		min_sock_num = p_pool->sub[0].sock_list.list_size;
		p_min_worker = &p_pool->sub[0];
		for( i = 1; i < p_pool->work_thread_num; i++ )
		{
			p_worker = p_pool->sub + i;

			if( p_worker->sock_list.list_size < min_sock_num )
			{
				min_sock_num = p_worker->sock_list.list_size;
				p_min_worker = p_worker;
			}
		}

		p_temp->p_worker = p_min_worker;

		msg.id = SOCK_POOL_MSG_ADD_SOCK;
		msg.param1 = p_temp;
		msg.param2 = NULL;

		ret = msq_send_msg(p_min_worker->msq, &msg, 0);
	}

#endif

	*p_sock = p_temp;

	return GEN_S_OK;
}

int32 sock_pool_close_sock(SOCK_POOL_H h, NET_SOCK_H net_sock)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;
	LIST_NODE* p_node = NULL;

#if !RUN_OS_WINDOWS

	MSQ_MSG  msg;
	SOCK_WORKER* p_worker = NULL;

#endif

	int32   ret;

	if( p_pool == NULL || p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	p_node = p_sock->p_node;

	thread_mutexer_lock(&p_pool->mutex);

	ret = gen_list_delete_node(&p_pool->sock_list, p_node);

	thread_mutexer_unlock(&p_pool->mutex);

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock %u close\n"), p_sock->sock);

	ret = sock_pool_close_sock_inner(p_pool, p_sock);

	sock_pool_release_sock(p_pool, p_sock);

#else

	p_worker = p_sock->p_worker;

	if( p_worker )
	{
		msg.id = SOCK_POOL_MSG_DEL_SOCK;
		msg.param1 = p_sock;
		msg.param2 = NULL;

		ret = msq_send_msg(p_worker->msq, &msg, 0);

	}else
	{
		ret = sock_pool_close_sock_inner(p_pool, p_sock);

		sock_pool_release_sock(p_pool, p_sock);
	}

#endif

	return GEN_S_OK;
}

int32 sock_pool_set_sock_option(SOCK_POOL_H h, NET_SOCK_H net_sock, int32 option, uint8* p_data, int32 data_size)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

	struct sockaddr_in local;
	int32  addr_len = sizeof(local);
	//int32  option;

	int32   b_error;
	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	thread_mutexer_lock(&p_sock->mutex);

	b_error = 0;
	error_code = 0;
	switch( option )
	{
	case NET_SOCK_OPTION_RECV_BUF_SIZE:
		{
			ret = setsockopt(p_sock->sock, SOL_SOCKET, SO_RCVBUF, p_data, data_size);
			if( ret == SOCKET_ERROR )
			{
				b_error = 1;
				error_code = GEN_E_FAIL;
				log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("set recv buf size error = %d!\n"), WSAGetLastError());
			}
		}
		break;

	case NET_SOCK_OPTION_SEND_BUF_SIZE:
		{
			ret = setsockopt(p_sock->sock, SOL_SOCKET, SO_SNDBUF, p_data, data_size);
			if( ret == SOCKET_ERROR )
			{
				b_error = 1;
				error_code = GEN_E_FAIL;
				log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("set send buf size error = %d!\n"), WSAGetLastError());
			}
		}
		break;

	default:
		{
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
		}
		break;
	}

	thread_mutexer_unlock(&p_sock->mutex);

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 sock_pool_get_sock_info(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_INFO* p_info)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

	if( p_pool == NULL || p_sock == NULL || p_info == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*p_info = p_sock->info;

	return GEN_S_OK;
}

int32 sock_pool_join_multicast_group(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_INFO* p_info)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

	struct ip_mreq mreq;

	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_info == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	mreq.imr_interface.s_addr = p_info->local.addr.ip;
	mreq.imr_multiaddr.s_addr = p_info->remote.addr.ip;

	thread_mutexer_lock(&p_sock->mutex);

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (uint8*)&mreq, sizeof(mreq));
	if( ret == SOCKET_ERROR )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("join group fail = %d!\n"), WSAGetLastError());
		thread_mutexer_unlock(&p_sock->mutex);
		return GEN_E_FAIL;
	}

	thread_mutexer_unlock(&p_sock->mutex);

	return GEN_S_OK;
}

int32 sock_pool_leave_multicast_group(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_INFO* p_info)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

	struct ip_mreq mreq;

	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_info == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	mreq.imr_interface.s_addr = p_info->local.addr.ip;
	mreq.imr_multiaddr.s_addr = p_info->remote.addr.ip;

	thread_mutexer_lock(&p_sock->mutex);

	ret = setsockopt(p_sock->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (uint8*)&mreq, sizeof(mreq));
	if( ret == SOCKET_ERROR )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("leave group fail = %d!\n"), WSAGetLastError());
		thread_mutexer_unlock(&p_sock->mutex);
		return GEN_E_FAIL;
	}

	thread_mutexer_unlock(&p_sock->mutex);

	return GEN_S_OK;
}

int32 sock_pool_bind_sock(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_ADDR* p_addr)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

	struct sockaddr_in local;
	int32  addr_len = sizeof(local);
	int32  option;

	//int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_addr == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** reuse addr */
	option = 1;
	ret = setsockopt(p_sock->sock, SOL_SOCKET, SO_REUSEADDR, (uint8*)&option, sizeof(option));
	if( ret == SOCKET_ERROR )
	{
		//log_debug(_T("[net_sock::net_sock_open] set reuse addr error = %d!\n"), WSAGetLastError());
	}

	local.sin_family = AF_INET;
	local.sin_port = p_addr->port;
	local.sin_addr.s_addr = p_addr->addr.ip;

	ret = bind(p_sock->sock, (struct sockaddr*)&local, addr_len);
	if( ret == -1 )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("bind to %d.%d.%d.%d:%d error = %d\n"),
			p_addr->addr.sub_ip[0], p_addr->addr.sub_ip[1], p_addr->addr.sub_ip[2], p_addr->addr.sub_ip[3], ntoh_u16(p_addr->port), WSAGetLastError());
		return GEN_E_FAIL;
	}

	p_sock->info.local.addr.ip = p_addr->addr.ip;
	p_sock->info.local.port = p_addr->port;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u bind to %d.%d.%d.%d:%d ok\n"),
		p_sock->sock, p_addr->addr.sub_ip[0], p_addr->addr.sub_ip[1], p_addr->addr.sub_ip[2], p_addr->addr.sub_ip[3], ntoh_u16(p_addr->port), WSAGetLastError());

	return GEN_S_OK;
}

int32 sock_pool_listen_sock(SOCK_POOL_H h, NET_SOCK_H net_sock)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if !RUN_OS_WINDOWS

	SOCK_WORKER* p_worker = NULL;
	MSQ_MSG  msg;
	//struct epoll_event event;

#endif

	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_sock->tcp.b_listen )
	{
		return GEN_S_OK;
	}

#if RUN_OS_WINDOWS

	ret = listen(p_sock->sock, SOMAXCONN);
	if( ret == SOCKET_ERROR )
	{
		error_code = WSAGetLastError();
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("listen error = %d\n"), error_code);
		return GEN_E_FAIL;
	}

#else


	p_worker = p_sock->p_worker;

	if( p_worker )
	{
		msg.id = SOCK_POOL_MSG_LISTEN;
		msg.param1 = p_sock;
		msg.param2 = NULL;

		ret = msq_send_msg(p_worker->msq, &msg, 0);

	}else
	{
		return GEN_E_FAIL;
	}


#endif

	p_sock->tcp.b_listen = 1;

	return GEN_S_OK;
}

int32 sock_pool_accept_sock(SOCK_POOL_H h, NET_SOCK_H listen_sock, NET_SOCK_H accept_sock)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_listen_sock = (SOCK_OBJ*)listen_sock;
	SOCK_OBJ*  p_accept_sock = (SOCK_OBJ*)accept_sock;

#if RUN_OS_WINDOWS

	LPFN_ACCEPTEX AcceptEx;
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD   dwBytes;

#else

#endif

	int32   b_ok;
	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_listen_sock == NULL || p_accept_sock == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_listen_sock->tcp.b_listen )
	{
		return GEN_E_FAIL;
	}

#if RUN_OS_WINDOWS

	ret = WSAIoctl(p_listen_sock->sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &AcceptEx, sizeof(AcceptEx), &dwBytes, NULL, NULL);
	if( ret == SOCKET_ERROR )
	{
		return GEN_E_FAIL;
	}

	p_accept_sock->accept.op_type = NET_SOCK_OP_ACCEPT;
	p_accept_sock->accept.buf.p_buf = p_accept_sock->accept_buf;
	p_accept_sock->accept.buf.buf_total_size = SOCK_POOL_MAX_ACCEPT_BUF_SIZE;
	p_accept_sock->accept.buf.buf_valid_size = p_accept_sock->accept.buf.buf_total_size;
	p_accept_sock->accept.buf.buf_use_size = 0;
	p_accept_sock->accept.buf.buf_left_size = p_accept_sock->accept.buf.buf_valid_size;
	p_accept_sock->p_listen_sock = p_listen_sock;

	memset(&p_accept_sock->accept.overlap, 0, sizeof(p_accept_sock->accept.overlap));

	thread_mutexer_lock(&p_accept_sock->mutex);

	b_ok = AcceptEx(p_listen_sock->sock, p_accept_sock->sock, p_accept_sock->accept_buf, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &dwBytes, (OVERLAPPED*)&p_accept_sock->accept);
	if( !b_ok )
	{
		error_code = WSAGetLastError();
		if( error_code != ERROR_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("accept error = %d\n"), error_code);
			thread_mutexer_unlock(&p_accept_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_accept_sock->ref_count++;

	thread_mutexer_unlock(&p_accept_sock->mutex);

#else

	if( p_pool->p_accept_sock != NULL )
	{
		return GEN_E_ALREADY_EXIST;
	}

	p_pool->p_accept_sock = p_accept_sock;


#endif

	return GEN_S_OK;
}

int32 sock_pool_connect_sock(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_ADDR* p_remote)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if RUN_OS_WINDOWS

	LPFN_CONNECTEX ConnectEx;
	GUID guidConnectEx = WSAID_CONNECTEX;
	struct sockaddr_in  remote;
	DWORD   dwBytes;

#else

	SOCK_WORKER* p_worker = NULL;
	MSQ_MSG  msg;

#endif

	int32   b_ok;
	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_remote == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	ret = WSAIoctl(p_sock->sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &ConnectEx, sizeof(ConnectEx), &dwBytes, NULL, NULL);
	if( ret == SOCKET_ERROR )
	{
		return GEN_E_FAIL;
	}

	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_port = p_remote->port;
	remote.sin_addr.s_addr = p_remote->addr.ip;

	p_sock->info.remote.addr.ip = p_remote->addr.ip;
	p_sock->info.remote.port = p_remote->port;

	p_sock->recv.op_type = NET_SOCK_OP_CONNECT;

	memset(&p_sock->recv.overlap, 0, sizeof(p_sock->recv.overlap));

	thread_mutexer_lock(&p_sock->mutex);

	b_ok = ConnectEx(p_sock->sock, (struct sockaddr*)&remote, sizeof(remote), NULL, 0, NULL, (OVERLAPPED*)&p_sock->recv);
	if( !b_ok )
	{
		error_code = WSAGetLastError();
		if( error_code != ERROR_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("connect error = %d\n"), error_code);
			thread_mutexer_unlock(&p_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

#else

	memset(&p_sock->remote, 0, sizeof(p_sock->remote));
	p_sock->remote.sin_family = AF_INET;
	p_sock->remote.sin_port = p_remote->port;
	p_sock->remote.sin_addr.s_addr = p_remote->addr.ip;

	p_sock->info.remote.addr.ip = p_remote->addr.ip;
	p_sock->info.remote.port = p_remote->port;

	p_worker = p_sock->p_worker;

	msg.id = SOCK_POOL_MSG_CONNECT;
	msg.param1 = p_sock;
	msg.param2 = NULL;

	ret = msq_send_msg(p_worker->msq, &msg, 0);

#endif

	return GEN_S_OK;
}

int32 sock_pool_recv_data(SOCK_POOL_H h, NET_SOCK_H net_sock, GEN_BUF* p_buf)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if RUN_OS_WINDOWS

	WSABUF  wsa_buf;
	DWORD   recv_size;
	DWORD   flag;

#else

	uint8*  p_uint8 = NULL;

	int32   recv_size;
	int32   total_recv_size;
	int32   left_size;

#endif

	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	p_sock->recv.op_type = NET_SOCK_OP_RECV;
	p_sock->recv.buf = *p_buf;

	wsa_buf.buf = p_buf->p_buf;
	wsa_buf.len = p_buf->buf_valid_size;
	flag = 0;

	memset(&p_sock->recv.overlap, 0, sizeof(p_sock->recv.overlap));

	thread_mutexer_lock(&p_sock->mutex);

	ret = WSARecv(p_sock->sock, &wsa_buf, 1, &recv_size, &flag, (WSAOVERLAPPED*)&p_sock->recv.overlap, NULL);
	if( ret == SOCKET_ERROR )
	{
		error_code = WSAGetLastError();
		if( error_code != WSA_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u recv error = %d\n"), p_sock->sock, error_code);
			thread_mutexer_unlock(&p_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

#else

	total_recv_size = 0;
	left_size = p_buf->buf_valid_size;
	p_uint8 = p_buf->p_buf;

	ret = recv(p_sock->sock, p_uint8, left_size, 0);
	if( ret < 0 )
	{
		error_code = errno;
		if( error_code == EAGAIN )
		{
			if( !p_sock->tcp.b_wait_read )
			{
				p_sock->tcp.b_wait_read = 1;

				p_sock->ep_event.events |= EPOLLIN;
				ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
			}

			p_sock->recv.buf = *p_buf;

			return GEN_E_OPERATION_PENDING;
		}

		return GEN_E_INNER_ERROR;

	}else if( ret == 0 )
	{
		if( p_sock->callback.disconnect )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u recv disconnect!\n"), p_sock->sock);
			ret = p_sock->callback.disconnect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
		}

		return GEN_E_INNER_ERROR;

	}else
	{
		p_buf->buf_use_size = ret;
		p_buf->buf_left_size = p_buf->buf_valid_size - p_buf->buf_use_size;
	}


#endif

	return GEN_S_OK;
}

int32 sock_pool_send_data(SOCK_POOL_H h, NET_SOCK_H net_sock, GEN_BUF* p_buf)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if RUN_OS_WINDOWS

	WSABUF wsa_buf;
	DWORD  send_size;

#else

	uint8*  p_uint8 = NULL;

	int32   send_size;
	int32   total_send_size;
	int32   left_size;

#endif

	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	p_sock->send.op_type = NET_SOCK_OP_SEND;
	p_sock->send.buf = *p_buf;
	wsa_buf.buf = p_buf->p_buf;
	wsa_buf.len = p_buf->buf_valid_size;

	memset(&p_sock->send.overlap, 0, sizeof(p_sock->send.overlap));

	thread_mutexer_lock(&p_sock->mutex);

	ret = WSASend(p_sock->sock, &wsa_buf, 1, &send_size, 0, (WSAOVERLAPPED*)&p_sock->send.overlap, NULL);
	if( ret == SOCKET_ERROR )
	{
		error_code = WSAGetLastError();
		if( error_code != WSA_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u send error = %d\n"), p_sock->sock, error_code);
			thread_mutexer_unlock(&p_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

#else

	total_send_size = 0;
	left_size = p_buf->buf_valid_size;
	p_uint8 = p_buf->p_buf;

	ret = send(p_sock->sock, p_uint8, left_size, 0);
	if( ret < 0 )
	{
		error_code = errno;
		if( error_code == EAGAIN )
		{
			if( !p_sock->tcp.b_wait_write )
			{
				p_sock->tcp.b_wait_write = 1;

				p_sock->ep_event.events |= EPOLLOUT;
				ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
			}

			p_sock->send.buf = *p_buf;

			return GEN_E_OPERATION_PENDING;
		}

		return GEN_E_INNER_ERROR;

	}else
	{
		p_buf->buf_use_size = ret;
		p_buf->buf_left_size = p_buf->buf_valid_size - p_buf->buf_use_size;
	}

#endif

	return GEN_S_OK;
}

int32 sock_pool_recvfrom_data(SOCK_POOL_H h, NET_SOCK_H net_sock, GEN_BUF* p_buf)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if RUN_OS_WINDOWS

	WSABUF wsa_buf;
	DWORD  recv_size;
	DWORD  flag;

#else

#endif

	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	p_sock->recv.op_type = NET_SOCK_OP_RECVFROM;
	p_sock->recv.buf = *p_buf;

	wsa_buf.buf = p_buf->p_buf;
	wsa_buf.len = p_buf->buf_valid_size;
	flag = 0;

	memset(&p_sock->recv.overlap, 0, sizeof(p_sock->recv.overlap));
	p_sock->remote_addr_size = sizeof(p_sock->remote);

	thread_mutexer_lock(&p_sock->mutex);

	ret = WSARecvFrom(p_sock->sock, &wsa_buf, 1, &recv_size, &flag, (struct sockaddr*)&p_sock->remote, &p_sock->remote_addr_size, (WSAOVERLAPPED*)&p_sock->recv.overlap, NULL);
	if( ret == SOCKET_ERROR )
	{
		error_code = WSAGetLastError();
		if( error_code != WSA_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u recvfrom error = %d\n"), p_sock->sock, error_code);
			thread_mutexer_unlock(&p_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

#else

#endif

	return GEN_S_OK;
}

int32 sock_pool_sendto_data(SOCK_POOL_H h, NET_SOCK_H net_sock, NET_SOCK_INFO* p_info, GEN_BUF* p_buf)
{
	SOCK_POOL* p_pool = (SOCK_POOL*)h;
	SOCK_OBJ*  p_sock = (SOCK_OBJ*)net_sock;

#if RUN_OS_WINDOWS

	WSABUF wsa_buf;
	DWORD  send_size;

#else

#endif

	int32   error_code;
	int32   ret;

	if( p_pool == NULL || p_sock == NULL || p_info == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	p_sock->send.op_type = NET_SOCK_OP_SENDTO;
	p_sock->send.buf = *p_buf;
	wsa_buf.buf = p_buf->p_buf;
	wsa_buf.len = p_buf->buf_valid_size;

	memset(&p_sock->send.overlap, 0, sizeof(p_sock->send.overlap));
	p_sock->remote.sin_family = AF_INET;
	p_sock->remote.sin_port = p_info->remote.port;
	p_sock->remote.sin_addr.s_addr = p_info->remote.addr.ip;

	thread_mutexer_lock(&p_sock->mutex);

	ret = WSASendTo(p_sock->sock, &wsa_buf, 1, &send_size, 0, (struct sockaddr*)&p_sock->remote, sizeof(p_sock->remote), (WSAOVERLAPPED*)&p_sock->send.overlap, NULL);
	if( ret == SOCKET_ERROR )
	{
		error_code = WSAGetLastError();
		if( error_code != WSA_IO_PENDING )
		{
			log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sendto error = %d\n"), error_code);
			thread_mutexer_unlock(&p_sock->mutex);
			return GEN_E_FAIL;
		}

	}else
	{
	}

	p_sock->ref_count++;

	thread_mutexer_unlock(&p_sock->mutex);

#else

#endif

	return GEN_S_OK;
}

/////////////////////////////// Work Thread ////////////////////////////////////////
#if RUN_OS_WINDOWS

DWORD WINAPI sock_pool_worker(LPVOID p_param)
{
	GEN_THREAD* p_thread = (GEN_THREAD*)p_param;
	SOCK_POOL* p_pool = (SOCK_POOL*)p_thread->p_user_data;
	SOCK_OBJ*  p_sock = NULL;
	NET_SOCK_BUF*  p_sock_buf = NULL;
	NET_SOCK_INFO sock_info;

	int32   b_ok;
	DWORD   complete_size;
	DWORD   dwKey;

	struct sockaddr_in  local;
	struct sockaddr_in  remote;
	struct sockaddr_in* p_local = NULL;
	struct sockaddr_in* p_remote = NULL;
	int32   local_addr_size;
	int32   remote_addr_size;

	LPFN_GETACCEPTEXSOCKADDRS  GetAcceptExSockAddrs;
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD   dwBytes;

	int32   error_code;
	int32   ret;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock thread id = 0x%x start\n"), p_thread->id);

	while( !p_thread->b_exit )
	{
		p_sock_buf = NULL;
		b_ok = GetQueuedCompletionStatus(p_pool->cp_h, &complete_size, &dwKey, (OVERLAPPED**)&p_sock_buf, SOCK_POOL_WORKER_WAIT_TIME);

		if( !b_ok )
		{
			error_code = GetLastError();
			if( error_code != WAIT_TIMEOUT )
			{
				if( p_sock_buf == NULL )
				{
					log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("get cp status error = %d\n"), error_code);

				}else
				{
					p_sock = (SOCK_OBJ*)p_sock_buf->p_sock;
					log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u op = %d get cp status error = %d\n"), p_sock->origin_sock, p_sock_buf->op_type, error_code);

					if( p_sock_buf->op_type == NET_SOCK_OP_CONNECT )
					{
						/** connect fail */
						if( p_sock->callback.connect )
						{
							ret = p_sock->callback.connect(p_sock, GEN_E_FAIL, p_sock->callback.p_custom_param);
						}
					}

					sock_pool_release_sock(p_pool, p_sock);
				}
			}

			continue;
		}

		if( p_sock_buf == NULL )
		{
			continue;
		}

		p_sock = (SOCK_OBJ*)p_sock_buf->p_sock;

		switch( p_sock_buf->op_type )
		{
		case NET_SOCK_OP_ACCEPT:
			{
				ret = WSAIoctl(p_sock->sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidGetAcceptExSockAddrs, sizeof(guidGetAcceptExSockAddrs), &GetAcceptExSockAddrs, sizeof(GetAcceptExSockAddrs), &dwBytes, NULL, NULL);
				if( ret == SOCKET_ERROR )
				{
					log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("wsaioctl error = %d\n"), WSAGetLastError());
					break;
				}

				GetAcceptExSockAddrs(p_sock_buf->buf.p_buf, 0, sizeof(local) + 16, sizeof(remote) + 16, (LPSOCKADDR*)&p_local, &local_addr_size, (LPSOCKADDR*)&p_remote, &remote_addr_size);
				p_sock->info.local.addr.ip = p_local->sin_addr.s_addr;
				p_sock->info.local.port = p_local->sin_port;
				p_sock->info.remote.addr.ip = p_remote->sin_addr.s_addr;
				p_sock->info.remote.port = p_remote->sin_port;

				log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u local host %d.%d.%d.%d:%d accept from %d.%d.%d.%d:%d\n"),
					p_sock->sock,
					p_sock->info.local.addr.sub_ip[0], p_sock->info.local.addr.sub_ip[1], p_sock->info.local.addr.sub_ip[2], p_sock->info.local.addr.sub_ip[3], hton_u16(p_sock->info.local.port),
					p_sock->info.remote.addr.sub_ip[0], p_sock->info.remote.addr.sub_ip[1], p_sock->info.remote.addr.sub_ip[2], p_sock->info.remote.addr.sub_ip[3], hton_u16(p_sock->info.remote.port));

				if( p_sock->callback.accept )
				{
					ret = p_sock->callback.accept(p_sock->p_listen_sock, p_sock, &p_sock->info, GEN_S_OK, p_sock->callback.p_custom_param);
				}
			}
			break;

		case NET_SOCK_OP_CONNECT:
			{
				if( p_sock->callback.connect )
				{
					ret = p_sock->callback.connect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
				}
			}
			break;

		case NET_SOCK_OP_RECV:
			{
				p_sock_buf->buf.buf_use_size = complete_size;
				p_sock_buf->buf.buf_left_size = p_sock_buf->buf.buf_valid_size - p_sock_buf->buf.buf_use_size;

				if( complete_size == 0 )
				{
					/** remote disconnect */
					if( p_sock->callback.disconnect )
					{
						log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u disconnect!\n"), p_sock->sock);
						ret = p_sock->callback.disconnect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
					}

				}else
				{
					if( p_sock->callback.recv )
					{
						ret = p_sock->callback.recv(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->buf, p_sock->callback.p_custom_param);
					}
				}
			}
			break;

		case NET_SOCK_OP_SEND:
			{
				p_sock_buf->buf.buf_use_size = complete_size;
				p_sock_buf->buf.buf_left_size = p_sock_buf->buf.buf_valid_size - p_sock_buf->buf.buf_use_size;

				if( p_sock->callback.send )
				{
					//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u send %u byte\n"), p_sock->sock, complete_size);
					ret = p_sock->callback.send(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->buf, p_sock->callback.p_custom_param);
				}
			}
			break;

		case NET_SOCK_OP_RECVFROM:
			{
				p_sock_buf->buf.buf_use_size = complete_size;
				p_sock_buf->buf.buf_left_size = p_sock_buf->buf.buf_valid_size - p_sock_buf->buf.buf_use_size;

				if( p_sock->callback.recv )
				{
					sock_info = p_sock->info;
					local_addr_size = sizeof(local);
					ret = getsockname(p_sock->sock, (struct sockaddr*)&local, &local_addr_size);
					if( ret == 0 )
					{
						sock_info.local.port = local.sin_port;
						sock_info.local.addr.ip = local.sin_addr.s_addr;
					}

					sock_info.remote.port = p_sock->remote.sin_port;
					sock_info.remote.addr.ip = p_sock->remote.sin_addr.s_addr;;
					ret = p_sock->callback.recv(p_sock, GEN_S_OK, &sock_info, &p_sock_buf->buf, p_sock->callback.p_custom_param);
				}
			}
			break;

		case NET_SOCK_OP_SENDTO:
			{
				p_sock_buf->buf.buf_use_size = complete_size;
				p_sock_buf->buf.buf_left_size = p_sock_buf->buf.buf_valid_size - p_sock_buf->buf.buf_use_size;

				if( p_sock->callback.send )
				{
					sock_info = p_sock->info;
					ret = getsockname(p_sock->sock, (struct sockaddr*)&local, &local_addr_size);
					if( ret == 0 )
					{
						sock_info.local.port = local.sin_port;
						sock_info.local.addr.ip = local.sin_addr.s_addr;
					}

					sock_info.remote.port = p_sock->remote.sin_port;
					sock_info.remote.addr.ip = p_sock->remote.sin_addr.s_addr;
					ret = p_sock->callback.send(p_sock, GEN_S_OK, &sock_info, &p_sock_buf->buf, p_sock->callback.p_custom_param);
				}
			}
			break;

		default:
			{
			}
			break;
		}

		sock_pool_release_sock(p_pool, p_sock);		
	}

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock thread id = 0x%x stop\n"), p_thread->id);

	return GEN_S_OK;
}

#else

void* sock_pool_main_worker(void* p_param)
{
	GEN_THREAD* p_thread = (GEN_THREAD*)p_param;
	SOCK_WORKER* p_worker = (SOCK_WORKER*)p_thread->p_user_data;
	SOCK_POOL* p_pool = p_worker->p_pool;
	//SOCK_WORKER* p_sub_manager = NULL;

	MSQ_MSG  msg;
	//MSQ_MSG  temp_msg;
	struct timespec sleep_time;

	struct epoll_event ep_event[SOCK_POOL_MAX_WAIT_EVENT];
	int32   ep_event_count;

	SOCK_OBJ*  p_sock = NULL;
	NET_SOCK_BUF*  p_sock_buf = NULL;
	NET_SOCK_INFO sock_info;
	int32   new_sock;

	struct sockaddr_in  local;
	struct sockaddr_in  remote;
	struct sockaddr_in* p_local = NULL;
	struct sockaddr_in* p_remote = NULL;
	int32   local_addr_size;
	int32   remote_addr_size;

	int32   min_sock_count;
	int32   loop_count;

	int32   i;
	int32   j;
	int32   error_code;
	int32   ret;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("main worker thread id = %d start\n"), p_thread->pid);

	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = SOCK_POOL_SLEEP_TIME_MS * 1000 * 1000;

	loop_count = 0;

	while( !p_thread->b_exit )
	{
		if( loop_count > 0 )
		{
			ret = msq_get_msg(p_worker->msq, &msg, 0);
			if( ret == 0 )
			{
				/** handle msg */
				ret = sock_pool_handle_worker_msg(p_pool, p_worker, &msg, 0);
			}

			loop_count = 0;
		}

		loop_count++;

		ret = epoll_wait(p_worker->epoll_fd, ep_event, SOCK_POOL_MAX_WAIT_EVENT, SOCK_POOL_MAX_WAIT_EVENT_TIMEOUT);
		if( ret < 0 )
		{
			nanosleep(&sleep_time, NULL);
			continue;

		}else if( ret == 0 )
		{
			continue;
		}

		ep_event_count = ret;
		for( i = 0; i < ep_event_count; i++ )
		{
			p_sock = (SOCK_OBJ*)ep_event[i].data.ptr;

			if( ep_event[i].events & EPOLLIN )
			{
				if( p_sock->tcp.b_listen )
				{
					/** listening sock */
					p_sock->remote_addr_size = sizeof(p_sock->remote);
					ret = accept(p_sock->sock, (struct sockaddr*)&p_sock->remote, (socklen_t*)&p_sock->remote_addr_size);
					if( ret < 0 )
					{
						error_code = errno;
						log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("accept error = %d\n"), error_code);
						continue;

					}else
					{
						/** accept connection */
						new_sock = ret;
						ret = sock_pool_accept_connection(p_pool, p_worker, p_sock, new_sock);
						if( ret )
						{
							close(new_sock);
						}
					}

				}else
				{
					/** recv data */
					p_sock->tcp.b_wait_read = 0;

					ret = recv(p_sock->sock, p_sock->recv.buf.p_buf, p_sock->recv.buf.buf_valid_size, 0);
					if( ret < 0 )
					{
						error_code = errno;
						if( error_code == EAGAIN )
						{
							if( !p_sock->tcp.b_wait_read )
							{
								p_sock->tcp.b_wait_read = 1;

								p_sock->ep_event.events |= EPOLLIN;
								ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
							}

							//p_sock->recv.buf = *p_buf;

							continue;
						}

						continue;

					}else if( ret == 0 )
					{
						if( p_sock->callback.disconnect )
						{
							log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u recv disconnect!\n"), p_sock->sock);
							ret = p_sock->callback.disconnect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
						}

						continue;

					}else
					{
						p_sock->recv.buf.buf_use_size = ret;
						p_sock->recv.buf.buf_left_size = p_sock->recv.buf.buf_valid_size - p_sock->recv.buf.buf_use_size;

						if( p_sock->callback.recv )
						{
							ret = p_sock->callback.recv(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->recv.buf, p_sock->callback.p_custom_param);
						}
					}
				}

			}
			
			if( ep_event[i].events & EPOLLOUT )
			{
				if( p_sock->tcp.connect_status == NET_SOCK_TCP_STATUS_ON_CONNECT )
				{
					/** connect */
					p_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_CONNECT;

					if( p_sock->callback.connect )
					{
						ret = p_sock->callback.connect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
					}

				}else
				{
					/** send data */
					p_sock->tcp.b_wait_write = 0;

					ret = send(p_sock->sock, p_sock->send.buf.p_buf, p_sock->send.buf.buf_valid_size, 0);
					if( ret < 0 )
					{
						error_code = errno;
						if( error_code == EAGAIN )
						{
							if( !p_sock->tcp.b_wait_write )
							{
								p_sock->tcp.b_wait_write = 1;

								p_sock->ep_event.events |= EPOLLOUT;
								ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
							}

							continue;
						}

						continue;

					}else
					{
						p_sock->send.buf.buf_use_size = ret;
						p_sock->send.buf.buf_left_size = p_sock->send.buf.buf_valid_size - p_sock->send.buf.buf_use_size;

						if( p_sock->callback.send )
						{
							ret = p_sock->callback.send(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->send.buf, p_sock->callback.p_custom_param);
						}
					}
				}
			}
			
			if( ep_event[i].events & EPOLLERR )
			{
			}

		}
	}

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("main worker thread id = %d stop\n"), p_thread->pid);

	return GEN_S_OK;
}

void* sock_pool_sub_worker(void* p_param)
{
	GEN_THREAD* p_thread = (GEN_THREAD*)p_param;
	SOCK_WORKER* p_worker = (SOCK_WORKER*)p_thread->p_user_data;
	SOCK_POOL* p_pool = p_worker->p_pool;

	MSQ_MSG  msg;
	struct timespec sleep_time;

	struct epoll_event ep_event[SOCK_POOL_MAX_WAIT_EVENT];
	int32   ep_event_count;

	SOCK_OBJ*  p_sock = NULL;
	NET_SOCK_BUF*  p_sock_buf = NULL;
	NET_SOCK_INFO sock_info;
	//int32   new_sock;

	struct sockaddr_in  local;
	struct sockaddr_in  remote;
	struct sockaddr_in* p_local = NULL;
	struct sockaddr_in* p_remote = NULL;
	int32   local_addr_size;
	int32   remote_addr_size;

	int32   min_sock_count;
	int32   loop_count;

	int32   i;
	int32   j;
	int32   error_code;
	int32   ret;

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sub worker thread id = %d start\n"), p_thread->pid);

	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = SOCK_POOL_SLEEP_TIME_MS * 1000 * 1000;

	loop_count = 0;

	while( !p_thread->b_exit )
	{
		if( loop_count > 0 )
		{
			ret = msq_get_msg(p_worker->msq, &msg, 0);
			if( ret == 0 )
			{
				/** handle msg */
				ret = sock_pool_handle_worker_msg(p_pool, p_worker, &msg, 0);
			}

			loop_count = 0;
		}

		loop_count++;

		ret = epoll_wait(p_worker->epoll_fd, ep_event, SOCK_POOL_MAX_WAIT_EVENT, SOCK_POOL_MAX_WAIT_EVENT_TIMEOUT);
		if( ret < 0 )
		{
			nanosleep(&sleep_time, NULL);
			continue;

		}else if( ret == 0 )
		{
			continue;
		}

		ep_event_count = ret;
		for( i = 0; i < ep_event_count; i++ )
		{
			p_sock = (SOCK_OBJ*)ep_event[i].data.ptr;

			if( ep_event[i].events & EPOLLIN )
			{
				/** recv data */
				p_sock->tcp.b_wait_read = 0;

				ret = recv(p_sock->sock, p_sock->recv.buf.p_buf, p_sock->recv.buf.buf_valid_size, 0);
				if( ret < 0 )
				{
					error_code = errno;
					if( error_code == EAGAIN )
					{
						if( !p_sock->tcp.b_wait_read )
						{
							p_sock->tcp.b_wait_read = 1;

							p_sock->ep_event.events |= EPOLLIN;
							ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
						}

						//p_sock->recv.buf = *p_buf;

						continue;
					}

					continue;

				}else if( ret == 0 )
				{
					if( p_sock->callback.disconnect )
					{
						log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock = %u recv disconnect!\n"), p_sock->sock);
						ret = p_sock->callback.disconnect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
					}

					continue;

				}else
				{
					p_sock->recv.buf.buf_use_size = ret;
					p_sock->recv.buf.buf_left_size = p_sock->recv.buf.buf_valid_size - p_sock->recv.buf.buf_use_size;

					if( p_sock->callback.recv )
					{
						ret = p_sock->callback.recv(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->recv.buf, p_sock->callback.p_custom_param);
					}
				}

			}

			if( ep_event[i].events & EPOLLOUT )
			{
				if( p_sock->tcp.connect_status == NET_SOCK_TCP_STATUS_ON_CONNECT )
				{
					/** connect */
					p_sock->tcp.connect_status = NET_SOCK_TCP_STATUS_CONNECT;

					if( p_sock->callback.connect )
					{
						ret = p_sock->callback.connect(p_sock, GEN_S_OK, p_sock->callback.p_custom_param);
					}

				}else
				{
					/** send data */
					p_sock->tcp.b_wait_write = 0;

					ret = send(p_sock->sock, p_sock->send.buf.p_buf, p_sock->send.buf.buf_valid_size, 0);
					if( ret < 0 )
					{
						error_code = errno;
						if( error_code == EAGAIN )
						{
							if( !p_sock->tcp.b_wait_write )
							{
								p_sock->tcp.b_wait_write = 1;

								p_sock->ep_event.events |= EPOLLOUT;
								ret = epoll_ctl(p_worker->epoll_fd, EPOLL_CTL_MOD, p_sock->sock, &p_sock->ep_event);
							}

							continue;
						}

						continue;

					}else
					{
						p_sock->send.buf.buf_use_size = ret;
						p_sock->send.buf.buf_left_size = p_sock->send.buf.buf_valid_size - p_sock->send.buf.buf_use_size;

						if( p_sock->callback.send )
						{
							ret = p_sock->callback.send(p_sock, GEN_S_OK, &p_sock->info, &p_sock_buf->send.buf, p_sock->callback.p_custom_param);
						}
					}
				}
			}

			if( ep_event[i].events & EPOLLERR )
			{
			}

		}
	}

	log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sub worker thread id = %d stop\n"), p_thread->pid);

	return GEN_S_OK;
}

#endif
