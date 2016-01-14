

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <public/gen_platform.h>

#if RUN_OS_WINDOWS

/** for winsock exclude */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

#else

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

/** for pthread */
#ifndef  _GNU_SOURCE
#define  _GNU_SOURCE
#endif
#include <pthread.h>

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#endif

#include <public/gen_def.h>
#include <public/gen_error.h>
#include <public/gen_dir.h>

#include "log_agent_const.h"
#include "log_agent.h"

/** split time, unit: second */
#define LA_LOG_SPLIT_TIME                                  (60 * 60)
/** split size, unit: byte */
#define LA_LOG_SPLIT_SIZE                                  (1024 * 1024)

#ifndef LOG_AGENT_NO_DEBUG

LOG_AGENT  g_log_agent;
LOG_AGENT* g_p_log_agent = NULL;

/////////////////////////////// Inner Interface ///////////////////////////////
#if LA_ENABLE_NETWORK

/**  
 * @brief open sock
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_open_sock(LOG_AGENT* p_agent)
{
	struct sockaddr_in local;
	int32 addr_len = sizeof(local);
	int32 sock_local;
	int32 option;

	int32 ret;

	sock_local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( sock_local == -1 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("sock create error!\n"));
		return GEN_E_FAIL;
	}

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

	if( p_agent->sock_info.local.port != 0 )
	{
		/** reuse addr */
		option = 1;
		ret = setsockopt(sock_local, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, sizeof(option));
#if RUN_OS_WINDOWS
		if( ret == SOCKET_ERROR )
#else
		if( ret == -1 )
#endif
		{
			//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("set reuse addr error = %d!\n"), WSAGetLastError());
		}

		memset(&local, 0, sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = p_agent->sock_info.local.port;
		local.sin_addr.s_addr = p_agent->sock_info.local.addr.ip;

		ret = bind(sock_local, (struct sockaddr*)&local, addr_len);
		if( ret == -1 )
		{
#if RUN_OS_WINDOWS
			closesocket(sock_local);
#else
			close(sock_local);
#endif
			//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("bind to local port = %d error\n"), ntoh_u16(p_sock->info.local.port));
			return GEN_E_FAIL;
		}

		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("bind to %x:%d\n"), ntoh_u32(p_sock->info.local.addr.ip), ntoh_u16(p_sock->info.local.port));
	}

	p_agent->sock = sock_local;

	return GEN_S_OK;
}

/**  
 * @brief sendto sock
 * @param [in] p_agent, agent
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_sendto_sock(LOG_AGENT* p_agent, int8* p_data, int32 data_size)
{
	struct sockaddr_in remote;

	int32  ret;

	remote.sin_family = AF_INET;
	remote.sin_port = p_agent->sock_info.remote.port;
	remote.sin_addr.s_addr = p_agent->sock_info.remote.addr.ip;

	ret = sendto(p_agent->sock, p_data, data_size, 0, (struct sockaddr*)&remote, sizeof(remote));

	return GEN_S_OK;
}

#endif

/**  
 * @brief open file
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_open_fileA(LOG_AGENT* p_agent)
{
	int8    str_buf[LA_MAX_LOG_FILE_DIR_SIZE + 64];
	int32   str_buf_left_size;
	int32   str_buf_size;

	int8*   p_int8 = NULL;

	int32   ret;

	/** create dir */
	p_int8 = str_buf;
	str_buf_left_size = LA_MAX_LOG_FILE_DIR_SIZE + 64;
	str_buf_size = 0;

#if RUN_OS_WINDOWS
	ret = sprintf_s(p_int8, str_buf_left_size, "%s%04d%02d%02d\\",
		p_agent->file_dir, p_agent->now_time_year, p_agent->now_time_month, p_agent->now_time_day);
#else
	ret = snprintf(p_int8, str_buf_left_size, "%s%04d%02d%02d/",
		p_agent->file_dir, p_agent->now_time_year, p_agent->now_time_month, p_agent->now_time_day);
#endif

	p_int8 += ret;
	str_buf_left_size -= ret;
	str_buf_size += ret;

	//printf("dir = %s\n", str_buf);
	ret = gen_dir_createA(str_buf, str_buf_size);

	/** open file */
#if RUN_OS_WINDOWS

	ret = sprintf_s(p_int8, str_buf_left_size, "%d.log", p_agent->split_file_seq);

	p_agent->h = CreateFileA(
		str_buf,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( p_agent->h == INVALID_HANDLE_VALUE )
	{
		//log_debug(_T("[gen_file::gen_file_openA] file open error = %d\n"), GetLastError());
		return GEN_E_INNER_ERROR;
	}

#else

	ret = snprintf(p_int8, str_buf_left_size, "%d.log", p_agent->split_file_seq);

	p_agent->fd = open(str_buf, O_RDWR | O_CREAT, 00777);
	if( p_agent->fd < 0 )
	{
		printf("[log_agent::log_agent_open_fileA] file open error = %d\n", errno);
		return GEN_E_INNER_ERROR;
	}

#endif

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS
/**  
 * @brief open file(unicode)
 * @param [in] p_agent, agent
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_open_fileW(LOG_AGENT* p_agent)
{
	int8    str_buf[LA_MAX_LOG_FILE_DIR_SIZE + 64];
	int32   str_buf_left_size;
	int32   str_buf_size;

	wchar_t* p_char = NULL;
	int32   char_buf_left_size;
	int32   char_buf_size;

	/*
	 * bom
	 * utf-16, le: 0xFF, 0xFE
	 * utf-16, be: 0xFE, 0xFF
	 * utf-8: 0xEF, 0xBB, 0xBF
	 * utf-32, le: 0xFF, 0xFE, 0x00, 0x00
	 * utf-32, be: 0x00, 0x00, 0xFF, 0xFE
	 */
	uint8   bom_buf[2] = {0xFF, 0xFE};

	uint32  write_size;
	int64   file_size;

	int32   ret;

	/** create dir */
	p_char = (wchar_t*)str_buf;
	str_buf_left_size = LA_MAX_LOG_FILE_DIR_SIZE + 64;
	str_buf_size = 0;
	char_buf_left_size = str_buf_left_size / sizeof(wchar_t);
	char_buf_size = 0;

	ret = swprintf_s(p_char, char_buf_left_size, L"%s%04d%02d%02d\\",
		(wchar_t*)p_agent->file_dir, p_agent->now_time_year, p_agent->now_time_month, p_agent->now_time_day);

	if( ret < 0 )
	{
		return GEN_E_FAIL;
	}

	p_char += ret;
	char_buf_left_size -= ret;
	char_buf_size += ret;
	str_buf_left_size -= (ret * sizeof(wchar_t));
	str_buf_size += (ret * sizeof(wchar_t));

	ret = gen_dir_createW((wchar_t*)str_buf, str_buf_size);

	/** open file */
	ret = swprintf_s(p_char, char_buf_left_size, L"%d.log", p_agent->split_file_seq);

	p_agent->h = CreateFileW(
		(wchar_t*)str_buf,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( p_agent->h == INVALID_HANDLE_VALUE )
	{
		//OutputDebugString(_T("[log_agent::log_agent_open_fileW] file open error = %d\n"), GetLastError());
		return GEN_E_INNER_ERROR;
	}

	ret = GetFileSizeEx(p_agent->h, (LARGE_INTEGER*)&file_size);
	if( file_size == 0 )
	{
		/** new file */
		ret = WriteFile(p_agent->h, bom_buf, sizeof(bom_buf), (DWORD*)&write_size, NULL);
	}

	return GEN_S_OK;
}
#endif

/**  
 * @brief output
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @param [in] var_str, var string
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_outputA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_format, va_list var_str)
{
	int8*  p_int8 = NULL;
	int32  buf_left_size;
	int32  total_str_size;

#if RUN_OS_WINDOWS

	SYSTEMTIME sys_time;
	struct tm  now_time;
	uint32 proc_id;
	uint32 thread_id;

	LARGE_INTEGER seek_pos;
	int64  file_pos;

#else

	struct timeval sys_time;
	struct tm  now_time;
	pid_t  thread_id;

#endif

	int64  now;
	int32  left_value;
	int32  b_need_open;
	uint32 write_size;

	int32  ret;

#if RUN_OS_WINDOWS

	_time64(&now);
	ret = _localtime64_s(&now_time, &now);
	GetLocalTime(&sys_time);
	proc_id = GetCurrentProcessId();
	thread_id = GetCurrentThreadId();

	EnterCriticalSection(&p_agent->cs);

	p_agent->now_time_year = sys_time.wYear;
	p_agent->now_time_month = sys_time.wMonth;
	p_agent->now_time_day = sys_time.wDay;
	p_agent->now_time_hour = sys_time.wHour;

#else

	now = time(NULL);
	ret = gettimeofday(&sys_time, NULL);
	localtime_r(&sys_time.tv_sec, &now_time);
	//thread_id = gettid();
	thread_id = syscall(SYS_gettid);

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->now_time_year = now_time.tm_year + 1900;
	p_agent->now_time_month = now_time.tm_mon + 1;
	p_agent->now_time_day = now_time.tm_mday;
	p_agent->now_time_hour = now_time.tm_hour;

#endif

	p_int8 = p_agent->str_buf;
	buf_left_size = LA_MAX_LOG_STRING_SIZE;
	total_str_size = 0;


	switch( level )
	{
	case LA_LOG_LEVEL_FATAL:
		{
			if( p_agent->level & LA_LOG_LEVEL_FATAL )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Fatal]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Fatal]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ERROR:
		{
			if( p_agent->level & LA_LOG_LEVEL_ERROR )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Error]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Error]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_WARN:
		{
			if( p_agent->level & LA_LOG_LEVEL_WARN )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Warn]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Warn]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_INFO:
		{
			if( p_agent->level & LA_LOG_LEVEL_INFO )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Info]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Info]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_DEBUG:
		{
			if( p_agent->level & LA_LOG_LEVEL_DEBUG )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Debug]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Debug]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ACCESS:
		{
			if( p_agent->level & LA_LOG_LEVEL_ACCESS )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Access]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Access]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	default:
		{
			goto FIN;
		}
	}

#if RUN_OS_WINDOWS

	ret = sprintf_s(p_int8, buf_left_size, "[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u:%u] ",
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds,
		p_src_file, src_line, proc_id, thread_id);

#else

	ret = snprintf(p_int8, buf_left_size, "[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u] ",
		now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec, (int)sys_time.tv_usec / 1000,
		p_src_file, src_line, thread_id);

#endif

	p_int8 += ret;
	buf_left_size -= ret;
	total_str_size += ret;

#if RUN_OS_WINDOWS

	ret = vsprintf_s(p_int8, buf_left_size, p_format, var_str);

#else

	ret = vsnprintf(p_int8, buf_left_size, p_format, var_str);

#endif

	if( ret < 0 )
	{
		goto FIN;
	}

	p_int8 += ret;
	buf_left_size -= ret;
	total_str_size += ret;

	p_agent->str_buf_size = total_str_size;
	p_agent->str_buf[total_str_size] = 0x00;

	/** output to stdout */
	if( p_agent->type & LA_LOG_TYPE_STDOUT )
	{
#if RUN_OS_WINDOWS
		//OutputDebugStringA(p_agent->str_buf);
#else
		printf(p_agent->str_buf);
#endif
	}

	/** output to file */
	if( p_agent->type & LA_LOG_TYPE_FILE )
	{
		b_need_open = 0;

#if RUN_OS_WINDOWS

		if( p_agent->h != INVALID_HANDLE_VALUE )
		{
			/** already open */
			if( p_agent->split_file_rule == LA_SPLIT_FILE_BY_TIME )
			{
				/** split by time */
				if( now >= p_agent->next_split_time )
				{
					CloseHandle(p_agent->h);
					p_agent->h = INVALID_HANDLE_VALUE;

					p_agent->split_file_seq++;
					p_agent->next_split_time += p_agent->split_time;
					b_need_open = 1;
				}

			}else
			{
				/** split by size */
			}

		}else
		{
			/** not open */
			if( p_agent->split_time < 60 )
			{
				/** split < 1 minute */
				p_agent->split_file_seq = sys_time.wSecond / p_agent->split_time;
				left_value = sys_time.wSecond % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else if( p_agent->split_time < (60 * 60) )
			{
				/** split < 1 hour */
				p_agent->split_file_seq = (sys_time.wMinute * 60 + sys_time.wSecond) / p_agent->split_time;
				left_value = (sys_time.wMinute * 60 + sys_time.wSecond) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else
			{
				/** split >= 1 hour */
				p_agent->split_file_seq = (sys_time.wHour * 60 * 60 + sys_time.wMinute * 60 + sys_time.wSecond) / p_agent->split_time;
				left_value = (sys_time.wHour * 60 * 60 + sys_time.wMinute * 60 + sys_time.wSecond) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;
			}

			b_need_open = 1;
		}

		if( b_need_open )
		{
			/** open file */
			ret = log_agent_open_fileA(p_agent);
			if( ret )
			{
				goto FIN;
			}

			seek_pos.QuadPart = 0;
			ret = SetFilePointerEx(p_agent->h, seek_pos, (LARGE_INTEGER*)&file_pos, FILE_END);
		}

		ret = WriteFile(p_agent->h, p_agent->str_buf, p_agent->str_buf_size, (DWORD*)&write_size, NULL);

#else

		if( p_agent->fd > 0 )
		{
			/** already open */
			if( p_agent->split_file_rule == LA_SPLIT_FILE_BY_TIME )
			{
				if( now >= p_agent->next_split_time )
				{
					close(p_agent->fd);
					p_agent->fd = -1;

					p_agent->split_file_seq++;
					p_agent->next_split_time += p_agent->split_time;
					b_need_open = 1;
				}
			}

		}else
		{
			if( p_agent->split_time < 60 )
			{
				/** split < 1 minute */
				p_agent->split_file_seq = now_time.tm_sec / p_agent->split_time;
				left_value = now_time.tm_sec % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else if( p_agent->split_time < (60 * 60) )
			{
				/** split < 1 hour */
				p_agent->split_file_seq = (now_time.tm_min * 60 + now_time.tm_sec) / p_agent->split_time;
				left_value = (now_time.tm_min * 60 + now_time.tm_sec) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else
			{
				/** split >= 1 hour */
				p_agent->split_file_seq = (now_time.tm_hour * 60 * 60 + now_time.tm_min * 60 + now_time.tm_sec) / p_agent->split_time;
				left_value = (now_time.tm_hour * 60 * 60 + now_time.tm_min * 60 + now_time.tm_sec) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;
			}

			b_need_open = 1;
		}

		if( b_need_open )
		{
			/** open file */
			ret = log_agent_open_fileA(p_agent);
			if( ret )
			{
				goto FIN;
			}

			ret = lseek(p_agent->fd, 0, SEEK_END);
		}

		ret = write(p_agent->fd, p_agent->str_buf, p_agent->str_buf_size);
		//printf("fd = %d, write ret = %d\n", p_agent->fd, ret);

#endif
	}

	/** output to sock */
	if( p_agent->type & LA_LOG_TYPE_SOCK )
	{
#if LA_ENABLE_NETWORK

		if( p_agent->sock == -1 )
		{
			ret = log_agent_open_sock(p_agent);
			if( ret )
			{
				goto FIN;
			}
		}

		ret = log_agent_sendto_sock(p_agent, p_agent->str_buf, p_agent->str_buf_size);

#endif
	}

FIN:

#if RUN_OS_WINDOWS

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

/**  
 * @brief output to buf
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_buf, output buf
 * @param [in/out] buf_size, output buf size/actual use size
 * @param [in] p_format, format string
 * @param [in] var_str, var string
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_output2bufA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, int8* p_format, va_list var_str)
{
	int8*  p_int8 = NULL;
	int32  buf_left_size;
	int32  total_str_size;

#if RUN_OS_WINDOWS

	SYSTEMTIME sys_time;
	struct tm  now_time;
	uint32 proc_id;
	uint32 thread_id;

	LARGE_INTEGER seek_pos;
	int64  file_pos;

#else

	struct timeval sys_time;
	struct tm  now_time;
	pid_t  thread_id;

#endif

	int64  now;
	int32  left_value;
	int32  b_need_open;
	uint32 write_size;

	int32  ret;

#if RUN_OS_WINDOWS

	_time64(&now);
	ret = _localtime64_s(&now_time, &now);
	GetLocalTime(&sys_time);
	proc_id = GetCurrentProcessId();
	thread_id = GetCurrentThreadId();

	EnterCriticalSection(&p_agent->cs);

	p_agent->now_time_year = sys_time.wYear;
	p_agent->now_time_month = sys_time.wMonth;
	p_agent->now_time_day = sys_time.wDay;
	p_agent->now_time_hour = sys_time.wHour;

#else

	now = time(NULL);
	ret = gettimeofday(&sys_time, NULL);
	localtime_r(&sys_time.tv_sec, &now_time);
	//thread_id = gettid();
	thread_id = syscall(SYS_gettid);

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->now_time_year = now_time.tm_year + 1900;
	p_agent->now_time_month = now_time.tm_mon + 1;
	p_agent->now_time_day = now_time.tm_mday;
	p_agent->now_time_hour = now_time.tm_hour;

#endif

	p_int8 = p_buf;
	buf_left_size = *p_buf_size;
	total_str_size = 0;


	switch( level )
	{
	case LA_LOG_LEVEL_FATAL:
		{
			if( p_agent->level & LA_LOG_LEVEL_FATAL )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Fatal]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Fatal]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ERROR:
		{
			if( p_agent->level & LA_LOG_LEVEL_ERROR )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Error]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Error]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_WARN:
		{
			if( p_agent->level & LA_LOG_LEVEL_WARN )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Warn]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Warn]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_INFO:
		{
			if( p_agent->level & LA_LOG_LEVEL_INFO )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Info]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Info]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_DEBUG:
		{
			if( p_agent->level & LA_LOG_LEVEL_DEBUG )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Debug]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Debug]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ACCESS:
		{
			if( p_agent->level & LA_LOG_LEVEL_ACCESS )
			{
#if RUN_OS_WINDOWS

				ret = sprintf_s(p_int8, buf_left_size, "[Access]");

#else

				ret = snprintf(p_int8, buf_left_size, "[Access]");

#endif

				p_int8 += ret;
				buf_left_size -= ret;
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	default:
		{
			goto FIN;
		}
	}

#if RUN_OS_WINDOWS

	ret = sprintf_s(p_int8, buf_left_size, "[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u:%u] ",
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds,
		p_src_file, src_line, proc_id, thread_id);

#else

	ret = snprintf(p_int8, buf_left_size, "[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u] ",
		now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec, (int)sys_time.tv_usec / 1000,
		p_src_file, src_line, thread_id);

#endif

	p_int8 += ret;
	buf_left_size -= ret;
	total_str_size += ret;

#if RUN_OS_WINDOWS

	ret = vsprintf_s(p_int8, buf_left_size, p_format, var_str);

#else

	ret = vsnprintf(p_int8, buf_left_size, p_format, var_str);

#endif

	if( ret < 0 )
	{
		goto FIN;
	}

	p_int8 += ret;
	buf_left_size -= ret;
	total_str_size += ret;

FIN:

#if RUN_OS_WINDOWS

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	*p_buf_size = total_str_size;
	p_buf[total_str_size] = 0x00;

	return GEN_S_OK;
}



#if RUN_OS_WINDOWS

/**  
 * @brief output(unicode)
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_format, format string
 * @param [in] var_str, var string
 * @return
 * 0: success
 * other: fail
 */
static inline int32 log_agent_outputW(LOG_AGENT* p_agent, uint32 level, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, va_list var_str)
{
	//int8*  p_int8 = NULL;
	wchar_t*  p_char = NULL;
	int32  buf_left_size;
	int32  char_buf_left_size;
	int32  total_str_size;


	SYSTEMTIME sys_time;
	struct tm  now_time;
	uint32 proc_id;
	uint32 thread_id;

	LARGE_INTEGER seek_pos;
	int64  file_pos;

	int64  now;
	int32  left_value;
	int32  b_need_open;
	uint32 write_size;
	int32  ret;

	_time64(&now);
	ret = _localtime64_s(&now_time, &now);
	GetLocalTime(&sys_time);
	proc_id = GetCurrentProcessId();
	thread_id = GetCurrentThreadId();

	EnterCriticalSection(&p_agent->cs);

	p_agent->now_time_year = sys_time.wYear;
	p_agent->now_time_month = sys_time.wMonth;
	p_agent->now_time_day = sys_time.wDay;
	p_agent->now_time_hour = sys_time.wHour;

	p_char = (wchar_t*)p_agent->str_buf;
	buf_left_size = LA_MAX_LOG_STRING_SIZE;
	char_buf_left_size = buf_left_size / sizeof(wchar_t);
	total_str_size = 0;

	switch( level )
	{
	case LA_LOG_LEVEL_FATAL:
		{
			if( p_agent->level & LA_LOG_LEVEL_FATAL )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Fatal]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ERROR:
		{
			if( p_agent->level & LA_LOG_LEVEL_ERROR )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Error]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_WARN:
		{
			if( p_agent->level & LA_LOG_LEVEL_WARN )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Warn]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_INFO:
		{
			if( p_agent->level & LA_LOG_LEVEL_INFO )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Info]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_DEBUG:
		{
			if( p_agent->level & LA_LOG_LEVEL_DEBUG )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Debug]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ACCESS:
		{
			if( p_agent->level & LA_LOG_LEVEL_ACCESS )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Access]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	default:
		{
			goto FIN;
		}
	}

	ret = swprintf_s(p_char, char_buf_left_size, L"[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u:%u] ",
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds,
		p_src_file, src_line, proc_id, thread_id);

	p_char += ret;
	char_buf_left_size -= ret;
	buf_left_size -= (ret * sizeof(wchar_t));
	total_str_size += ret;

	ret = vswprintf_s(p_char, char_buf_left_size, p_format, var_str);

	if( ret < 0 )
	{
		goto FIN;
	}

	p_char += ret;
	char_buf_left_size -= ret;
	buf_left_size -= (ret * sizeof(wchar_t));
	total_str_size += ret;

	p_agent->str_buf_size = total_str_size * sizeof(wchar_t);
	p_char = (wchar_t*)p_agent->str_buf;
	p_char[total_str_size] = 0x0;

	/** output to stdout */
	if( p_agent->type & LA_LOG_TYPE_STDOUT )
	{
		//OutputDebugStringW((wchar_t*)p_agent->str_buf);
	}

	/** output to file */
	if( p_agent->type & LA_LOG_TYPE_FILE )
	{
		b_need_open = 0;

		if( p_agent->h != INVALID_HANDLE_VALUE )
		{
			/** already open */
			if( p_agent->split_file_rule == LA_SPLIT_FILE_BY_TIME )
			{
				if( now >= p_agent->next_split_time )
				{
					CloseHandle(p_agent->h);
					p_agent->h = INVALID_HANDLE_VALUE;

					p_agent->split_file_seq++;
					p_agent->next_split_time += p_agent->split_time;
					b_need_open = 1;
				}

			}else
			{
			}

		}else
		{
			/** not open */
			if( p_agent->split_time < 60 )
			{
				/** split < 1 minute */
				p_agent->split_file_seq = sys_time.wSecond / p_agent->split_time;
				left_value = sys_time.wSecond % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else if( p_agent->split_time < (60 * 60) )
			{
				/** split < 1 hour */
				p_agent->split_file_seq = (sys_time.wMinute * 60 + sys_time.wSecond) / p_agent->split_time;
				left_value = (sys_time.wMinute * 60 + sys_time.wSecond) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;

			}else
			{
				/** split >= 1 hour */
				p_agent->split_file_seq = (sys_time.wHour * 60 * 60 + sys_time.wMinute * 60 + sys_time.wSecond) / p_agent->split_time;
				left_value = (sys_time.wHour * 60 * 60 + sys_time.wMinute * 60 + sys_time.wSecond) % p_agent->split_time;
				p_agent->next_split_time = now - left_value + p_agent->split_time;
			}

			b_need_open = 1;
		}

		if( b_need_open )
		{
			/** open file */
			ret = log_agent_open_fileW(p_agent);
			if( ret )
			{
				goto FIN;
			}

			seek_pos.QuadPart = 0;
			ret = SetFilePointerEx(p_agent->h, seek_pos, (LARGE_INTEGER*)&file_pos, FILE_END);
		}

		ret = WriteFile(p_agent->h, p_agent->str_buf, p_agent->str_buf_size, (DWORD*)&write_size, NULL);
	}

	/** output to sock */
	if( p_agent->type & LA_LOG_TYPE_SOCK )
	{
#if LA_ENABLE_NETWORK

		if( p_agent->sock == -1 )
		{
			ret = log_agent_open_sock(p_agent);
			if( ret )
			{
				goto FIN;
			}
		}

		ret = log_agent_sendto_sock(p_agent, p_agent->str_buf, p_agent->str_buf_size);

#endif
	}

FIN:

	LeaveCriticalSection(&p_agent->cs);

	return GEN_S_OK;
}

/**  
 * @brief output to buf(unicode)
 * @param [in] p_agent, agent
 * @param [in] level, log level
 * @param [in] p_src_file, src file that call this func
 * @param [in] src_line, src line that call this func
 * @param [in] p_buf, output buf
 * @param [in/out] p_buf_size, output buf size/actual use size
 * @param [in] p_format, format string
 * @param [in] var_str, var string
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. buf size count in bytes!
 */
static inline int32 log_agent_output2bufW(LOG_AGENT* p_agent, uint32 level, wchar_t* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, wchar_t* p_format, va_list var_str)
{
	wchar_t*  p_char = NULL;
	int32  buf_left_size;
	int32  char_buf_left_size;
	int32  total_str_size;


	SYSTEMTIME sys_time;
	struct tm  now_time;
	uint32 proc_id;
	uint32 thread_id;

	LARGE_INTEGER seek_pos;
	int64  file_pos;

	int64  now;
	int32  left_value;
	int32  b_need_open;
	uint32 write_size;
	int32  ret;

	_time64(&now);
	ret = _localtime64_s(&now_time, &now);
	GetLocalTime(&sys_time);
	proc_id = GetCurrentProcessId();
	thread_id = GetCurrentThreadId();

	EnterCriticalSection(&p_agent->cs);

	p_agent->now_time_year = sys_time.wYear;
	p_agent->now_time_month = sys_time.wMonth;
	p_agent->now_time_day = sys_time.wDay;
	p_agent->now_time_hour = sys_time.wHour;

	p_char = (wchar_t*)p_buf;
	buf_left_size = *p_buf_size;
	char_buf_left_size = buf_left_size / sizeof(wchar_t);
	total_str_size = 0;

	switch( level )
	{
	case LA_LOG_LEVEL_FATAL:
		{
			if( p_agent->level & LA_LOG_LEVEL_FATAL )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Fatal]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ERROR:
		{
			if( p_agent->level & LA_LOG_LEVEL_ERROR )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Error]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_WARN:
		{
			if( p_agent->level & LA_LOG_LEVEL_WARN )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Warn]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_INFO:
		{
			if( p_agent->level & LA_LOG_LEVEL_INFO )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Info]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_DEBUG:
		{
			if( p_agent->level & LA_LOG_LEVEL_DEBUG )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Debug]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	case LA_LOG_LEVEL_ACCESS:
		{
			if( p_agent->level & LA_LOG_LEVEL_ACCESS )
			{
				ret = swprintf_s(p_char, char_buf_left_size, L"[Access]");

				p_char += ret;
				char_buf_left_size -= ret;
				buf_left_size -= (ret * sizeof(wchar_t));
				total_str_size += ret;

			}else
			{
				goto FIN;
			}
		}
		break;

	default:
		{
			goto FIN;
		}
	}

	ret = swprintf_s(p_char, char_buf_left_size, L"[%04d-%02d-%02d %02d:%02d:%02d %03d][%s:%d][%u:%u] ",
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds,
		p_src_file, src_line, proc_id, thread_id);

	p_char += ret;
	char_buf_left_size -= ret;
	buf_left_size -= (ret * sizeof(wchar_t));
	total_str_size += ret;

	ret = vswprintf_s(p_char, char_buf_left_size, p_format, var_str);

	if( ret < 0 )
	{
		goto FIN;
	}

	p_char += ret;
	char_buf_left_size -= ret;
	buf_left_size -= (ret * sizeof(wchar_t));
	total_str_size += ret;

FIN:

	LeaveCriticalSection(&p_agent->cs);

	*p_buf_size = total_str_size * sizeof(wchar_t);
	p_char = (wchar_t*)p_buf;
	p_char[total_str_size] = 0x00;

	return GEN_S_OK;
}

#endif

/////////////////////////////// Outter Interface //////////////////////////////
int32 log_agent_set_global_agent(LOG_AGENT* p_agent)
{
	g_p_log_agent = p_agent;

	return GEN_S_OK;
}

int32 log_agent_setup(LOG_AGENT* p_agent)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_agent, 0, sizeof(*p_agent));

#if LA_ENABLE_NETWORK
	p_agent->sock = -1;
#endif

	return GEN_S_OK;
}

int32 log_agent_cleanup(LOG_AGENT* p_agent)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_agent->b_init )
	{
		log_agent_deinit(p_agent);
	}

	return GEN_S_OK;
}

int32 log_agent_init(LOG_AGENT* p_agent)
{
#if LA_ENABLE_NETWORK

#if RUN_OS_WINDOWS
	WSADATA wsd;
#endif

#endif

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_agent->b_init )
	{
		return GEN_S_OK;
	}

	p_agent->type = LA_LOG_TYPE_STDOUT;
	p_agent->level = LA_LOG_LEVEL_FATAL | LA_LOG_LEVEL_ERROR;

	p_agent->split_file_rule = LA_SPLIT_FILE_BY_TIME;
	p_agent->split_time = LA_LOG_SPLIT_TIME;
	p_agent->split_size = LA_LOG_SPLIT_SIZE;

#if RUN_OS_WINDOWS

	InitializeCriticalSection(&p_agent->cs);
	p_agent->h = INVALID_HANDLE_VALUE;

#else

	pthread_mutexattr_init(&p_agent->mutex_attr);
	pthread_mutexattr_settype(&p_agent->mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&p_agent->mutex, &p_agent->mutex_attr);

	p_agent->fd = -1;

#endif

	p_agent->b_init_mutex = 1;

	p_agent->file_dir[0] = 0x00;
	p_agent->file_dir_size = 0;

	p_agent->str_buf[0] = 0x00;
	p_agent->str_buf_size = 0;

#if LA_ENABLE_NETWORK

	p_agent->sock = -1;
	memset(&p_agent->sock_info, 0, sizeof(p_agent->sock_info));

#if RUN_OS_WINDOWS
	if( !p_agent->b_init_winsock )
	{
		WSAStartup(0x0202, &wsd);
		p_agent->b_init_winsock = 1;
	}
#endif

#endif

	p_agent->b_init = 1;

	return GEN_S_OK;
}

int32 log_agent_deinit(LOG_AGENT* p_agent)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	if( p_agent->b_init_mutex )
	{
		DeleteCriticalSection(&p_agent->cs);
		p_agent->b_init_mutex = 0;
	}

	if( p_agent->h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(p_agent->h);
		p_agent->h = INVALID_HANDLE_VALUE;
	}

#else

	if( p_agent->b_init_mutex )
	{
		pthread_mutex_destroy(&p_agent->mutex);
		pthread_mutexattr_destroy(&p_agent->mutex_attr);
	}

	if( p_agent->fd > 0 )
	{
		close(p_agent->fd);
		p_agent->fd = -1;
	}

#endif

#if LA_ENABLE_NETWORK

	if( p_agent->sock != -1 )
	{
#if RUN_OS_WINDOWS
		closesocket(p_agent->sock);
#else
		close(p_agent->sock);
#endif
		p_agent->sock = -1;
	}

#if RUN_OS_WINDOWS
	if( p_agent->b_init_winsock )
	{
		WSACleanup();
		p_agent->b_init_winsock = 0;
	}
#endif

#endif

	p_agent->b_init = 0;

	return GEN_S_OK;
}

int32 log_agent_set_log_level(LOG_AGENT* p_agent, uint32 level)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	p_agent->level = level;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->level = level;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

int32 log_agent_set_log_type(LOG_AGENT* p_agent, uint32 type)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	p_agent->type = type;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->type = type;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

int32 log_agent_set_log_file_dirA(LOG_AGENT* p_agent, int8* p_dir, int32 dir_size)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( dir_size < 1 || dir_size > LA_MAX_LOG_FILE_DIR_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	memcpy(p_agent->file_dir, p_dir, dir_size);
	p_agent->file_dir_size = dir_size;

	if( p_agent->file_dir[dir_size - 1] != '\\' )
	{
		p_agent->file_dir[dir_size] = '\\';
		p_agent->file_dir_size += sizeof(int8);
	}

	p_agent->file_dir[p_agent->file_dir_size] = 0x00;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	memcpy(p_agent->file_dir, p_dir, dir_size);
	p_agent->file_dir_size = dir_size;

	if( p_agent->file_dir[dir_size - 1] != '/' )
	{
		p_agent->file_dir[dir_size] = '/';
		p_agent->file_dir_size += sizeof(int8);
	}

	p_agent->file_dir[p_agent->file_dir_size] = 0x00;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS

int32 log_agent_set_log_file_dirW(LOG_AGENT* p_agent, wchar_t* p_dir, int32 dir_size)
{
	wchar_t* p_char = NULL;
	int32  char_dir_size;

	if( p_agent == NULL || p_dir == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( dir_size < 1 || dir_size > LA_MAX_LOG_FILE_DIR_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	EnterCriticalSection(&p_agent->cs);

	memcpy(p_agent->file_dir, p_dir, dir_size);
	p_agent->file_dir_size = dir_size;

	p_char = (wchar_t*)p_agent->file_dir;
	char_dir_size = dir_size / sizeof(wchar_t);
	if( p_char[char_dir_size - 1] != L'\\' )
	{
		p_char[char_dir_size] = L'\\';
		char_dir_size += 1;
		p_agent->file_dir_size += sizeof(wchar_t);
	}

	p_char[char_dir_size] = 0x00;

	LeaveCriticalSection(&p_agent->cs);

	return GEN_S_OK;
}

#endif

int32 log_agent_set_split_file_rule(LOG_AGENT* p_agent, int32 split_rule)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	p_agent->split_file_rule = split_rule;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->split_file_rule = split_rule;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

int32 log_agent_set_log_file_split_time(LOG_AGENT* p_agent, int32 split_time)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	p_agent->split_time = split_time;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->split_time = split_time;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

int32 log_agent_set_log_file_split_size(LOG_AGENT* p_agent, uint32 split_size)
{
	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_agent->cs);

	p_agent->split_size = split_size;

	LeaveCriticalSection(&p_agent->cs);

#else

	pthread_mutex_lock(&p_agent->mutex);

	p_agent->split_size = split_size;

	pthread_mutex_unlock(&p_agent->mutex);

#endif

	return GEN_S_OK;
}

#if LA_ENABLE_NETWORK
int32 log_agent_set_log_sock_info(LOG_AGENT* p_agent, NET_SOCK_INFO* p_info)
{
	if( p_agent == NULL || p_info == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	memcpy(&p_agent->sock_info, p_info, sizeof(*p_info));

	return GEN_S_OK;
}
#endif

int32 log_agent_traceA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputA(p_agent, level, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_trace2bufA(LOG_AGENT* p_agent, uint32 level, int8* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL || p_src_file == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_output2bufA(p_agent, level, p_src_file, src_line, p_buf, p_buf_size, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_debugA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputA(p_agent, LA_LOG_LEVEL_DEBUG, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_infoA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputA(p_agent, LA_LOG_LEVEL_INFO, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_warnA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputA(p_agent, LA_LOG_LEVEL_WARN, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_errorA(LOG_AGENT* p_agent, int8* p_src_file, int32 src_line, int8* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputA(p_agent, LA_LOG_LEVEL_ERROR, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS

int32 log_agent_traceW(LOG_AGENT* p_agent, int32 level, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputW(p_agent, level, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_trace2bufW(LOG_AGENT* p_agent, int32 level, wchar_t* p_src_file, int32 src_line, int8* p_buf, int32* p_buf_size, wchar_t* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL || p_src_file == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_output2bufW(p_agent, level, p_src_file, src_line, p_buf, p_buf_size, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_debugW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputW(p_agent, LA_LOG_LEVEL_DEBUG, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_infoW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t* p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputW(p_agent, LA_LOG_LEVEL_INFO, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_warnW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t *p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputW(p_agent, LA_LOG_LEVEL_WARN, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

int32 log_agent_errorW(LOG_AGENT* p_agent, wchar_t* p_src_file, int32 src_line, wchar_t *p_format, ...)
{
	va_list args;

	int32   ret;

	if( p_agent == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_agent->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	va_start(args, p_format);

	ret = log_agent_outputW(p_agent, LA_LOG_LEVEL_ERROR, p_src_file, src_line, p_format, args);

	va_end(args);

	return GEN_S_OK;
}

#endif

#endif// LOG_AGENT_NO_DEBUG
