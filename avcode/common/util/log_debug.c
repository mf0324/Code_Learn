
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_debug.h"

#if RUN_OS_WINDOWS

#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

#endif

/** 默认级别 */
#define LOG_DEFAULT_LEVEL                                  (LOG_NORMAL_LEVEL)

/** max string size */
#define LOG_MAX_STRING_SIZE                                (2 * 1024)
/** max line size */
#define LOG_MAX_LINE_SIZE                                  (16)


static int32 b_init_log = 0;
static int32 log_level = LOG_DEFAULT_LEVEL;
static int32 b_output_log = 0;
static int32 log_sock = -1;
static struct sockaddr_in remote;


#if RUN_OS_WINDOWS
static HANDLE log_file = INVALID_HANDLE_VALUE;
#else
static int32 log_file = -1;
#endif

int32 log_outputA(int8 *p_str, int32 str_size);
#if RUN_OS_WINDOWS
int32 log_outputW(uint16 *p_str, int32 str_size);
#endif

int32 log_init()
{
#if RUN_OS_WINDOWS
	WSADATA wsd;
#endif

	//int32 ret;

	if( b_init_log )
	{
		return 0;
	}

	log_level = LOG_DEFAULT_LEVEL;

	b_output_log = 0;

#if RUN_OS_WINDOWS
	WSAStartup(0x0202, &wsd);
#endif

	memset(&remote, 0, sizeof(remote));
	log_sock = -1;


#if RUN_OS_WINDOWS
	log_file = INVALID_HANDLE_VALUE;
#else
	log_file = -1;
#endif

	b_init_log = 1;

	return 0;
}

int32 log_deinit()
{
	if( !b_init_log )
	{
		return 0;
	}

#if RUN_OS_WINDOWS

	if( log_file != INVALID_HANDLE_VALUE )
	{
		CloseHandle(log_file);
		log_file = INVALID_HANDLE_VALUE;
	}

#else

	if( log_file > 0 )
	{
		close(log_file);
		log_file = -1;
	}

#endif

#if RUN_OS_WINDOWS

	if( log_sock != -1 )
	{
		closesocket(log_sock);
	}
	WSACleanup();

#else

	if( log_sock > 0 )
	{
		close(log_sock);
	}

#endif

	log_sock = -1;

	b_init_log = 0;

	return 0;
}


int32 log_set_level(int32 level)
{
	log_level = level;

	return 0;
}

int32 log_set_output(int32 b_output)
{
	b_output_log = b_output;

	return 0;
}

int32 log_set_remote(uint32 ip, uint16 port)
{
	int32 option;
	int32 ret;

	if( log_sock < 0 )
	{
		log_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		/** 设置为非阻塞模式 */
		if( log_sock > 0 )
		{
#if RUN_OS_WINDOWS
			option = 1;
			ret = ioctlsocket(log_sock, FIONBIO, (DWORD*)&option);
#else
			ret = fcntl(log_sock, F_SETFD, O_NONBLOCK);
#endif
		}
	}

	remote.sin_family = AF_INET;
	remote.sin_port = port;
	remote.sin_addr.s_addr = ip;

	return 0;
}

int32 log_set_fileA(int8 *p_path, int32 path_len, uint32 file_len)
{
	uint32 file_size;
	int32  ret;

#if RUN_OS_WINDOWS
	if( log_file != INVALID_HANDLE_VALUE )
	{
		CloseHandle(log_file);
		log_file = INVALID_HANDLE_VALUE;
	}

	log_file = CreateFileA(
		p_path,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( log_file == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	file_size = GetFileSize(log_file, NULL);
	if( file_size >= file_len )
	{
		/** 清空文件 */
		ret = SetFilePointer(log_file, 0, NULL, FILE_BEGIN);
		ret = SetEndOfFile(log_file);
	}

	ret = SetFilePointer(log_file, 0, NULL, FILE_END);

#else

	if( log_file > 0 )
	{
		close(log_file);
		log_file = -1;
	}

	log_file = open(p_path, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if( log_file == -1 )
	{
		printf("[log_debug][log_set_fileA] open file %s error = %d\n", p_path, errno);
		log_file = open(p_path, O_WRONLY);
		if( log_file == -1 )
		{
			return -1;
		}
	}

	file_size = lseek(log_file, 0, SEEK_END);
	if( file_size > file_len )
	{
		/** 清空文件 */
		close(log_file);
		log_file = open(p_path, O_WRONLY | O_TRUNC);
	}

#endif

	return 0;
}

#if RUN_OS_WINDOWS
int32 log_set_fileW(wchar_t *p_path, int32 path_len, uint32 file_len)
{
	uint32 file_size;
	int32  ret;

	if( log_file != INVALID_HANDLE_VALUE )
	{
		CloseHandle(log_file);
		log_file = INVALID_HANDLE_VALUE;
	}
	
	log_file = CreateFileW(
		p_path,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( log_file == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	
	file_size = GetFileSize(log_file, NULL);
	if( file_size >= file_len )
	{
		/** 清空文件 */
		ret = SetFilePointer(log_file, 0, NULL, FILE_BEGIN);
		ret = SetEndOfFile(log_file);
	}
	
	ret = SetFilePointer(log_file, 0, NULL, FILE_END);

	return 0;
}
#endif

int32 log_output_buf_data(int8* p_buf, int32 buf_size)
{
	uint8* p_uint8 = NULL;

	int8   str_buf[LOG_MAX_STRING_SIZE];
	int32  str_buf_size;

	int32  line_count;
	int32  remain_size;
	//int32  total_size;

	int32  i;
	//int32  ret;

	if( p_buf == NULL || buf_size < 0 )
	{
		return -1;
	}

	p_uint8 = (uint8*)p_buf;
	line_count = buf_size / LOG_MAX_LINE_SIZE;
	remain_size = buf_size % LOG_MAX_LINE_SIZE;

	for( i = 0; i < line_count; i++ )
	{
		str_buf_size = sprintf(str_buf, "0x%08X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X\n",
			p_uint8, p_uint8[0], p_uint8[1], p_uint8[2], p_uint8[3], p_uint8[4], p_uint8[5], p_uint8[6], p_uint8[7], p_uint8[8], p_uint8[9], p_uint8[10], p_uint8[11], p_uint8[12], p_uint8[13], p_uint8[14], p_uint8[15]);
		p_uint8 += LOG_MAX_LINE_SIZE;

#if RUN_OS_WINDOWS
		OutputDebugStringA(str_buf);
#else
		printf(str_buf);
#endif
	}

	if( remain_size )
	{
		str_buf_size = sprintf(str_buf, "0x%08X:", p_uint8);

#if RUN_OS_WINDOWS
		OutputDebugStringA(str_buf);
#else
		printf(str_buf);
#endif

		for( i = 0; i < remain_size; i++ )
		{
			if( i == 8 )
			{
				str_buf_size = sprintf(str_buf, "  %02X", p_uint8[i]);

			}else
			{
				str_buf_size = sprintf(str_buf, " %02X", p_uint8[i]);
			}

#if RUN_OS_WINDOWS
			OutputDebugStringA(str_buf);
#else
			printf(str_buf);
#endif
		}

#if RUN_OS_WINDOWS
		OutputDebugStringA("\n");
#else
		printf("\n");
#endif
	}

	return 0;
}

int32 log_to_bufA(int32 level, int8* p_buf, int8 *p_format, ...)
{
	va_list args;

	int32  info_size;

	//int32  ret;

	if( level > log_level )
	{
		return -1;
	}

	va_start(args, p_format);

	info_size = vsprintf(p_buf, p_format, args);

	va_end(args);

	return info_size;
}

int32 log_traceA(int32 level, int8 *p_format, ...)
{
	va_list args;

	int8   info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( level > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

	info_size = vsprintf(info, p_format, args);

	va_end(args);

	ret = log_outputA(info, info_size);

	return 0;
}

int32 log_debugA(int8 *p_format, ...)
{
	va_list args;

	int8   info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_DEBUG_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

	info_size = vsprintf(info, p_format, args);

	va_end(args);

	ret = log_outputA(info, info_size);

	return 0;
}

int32 log_warnA(int8 *p_format, ...)
{
	va_list args;

	int8   info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_WARN_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

	info_size = vsprintf(info, p_format, args);

	va_end(args);

	ret = log_outputA(info, info_size);

	return 0;
}

int32 log_errorA(int8 *p_format, ...)
{
	va_list args;

	int8   info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_ERROR_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

	info_size = vsprintf(info, p_format, args);

	va_end(args);

	ret = log_outputA(info, info_size);

	return 0;
}

int32 log_outputA(int8 *p_str, int32 str_size)
{
	int8   log_time[128];
	int32  log_time_size;

	uint32 write_size;
	int32  ret;

#if RUN_OS_WINDOWS
	SYSTEMTIME sys_time;
#else
	int32 now_time;
	struct tm *p_tm;
#endif

	/** 打印 */
	if( b_output_log )
	{
#if RUN_OS_WINDOWS
		OutputDebugStringA(p_str);
#else
		printf(p_str);
#endif
	}

	/** 转发 */
	if( log_sock > 0 )
	{
		ret = sendto(log_sock, p_str, str_size, 0, (struct sockaddr*)&remote, sizeof(remote));
	}

	/** 写日志文件 */
#if RUN_OS_WINDOWS
	if( log_file != INVALID_HANDLE_VALUE )
	{
		GetLocalTime(&sys_time);
		log_time_size = wsprintfA(log_time, "%04d-%02d-%02d %02d:%02d:%02d ",
			sys_time.wYear, sys_time.wMonth, sys_time.wDay,
			sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
		ret = WriteFile(log_file, log_time, log_time_size, (DWORD*)&write_size, NULL);
		ret = WriteFile(log_file, p_str, str_size, (DWORD*)&write_size, NULL);
	}
#else
	if( log_file > 0 )
	{
		now_time = time(NULL);
		p_tm = localtime(&now_time);
		log_time_size = sprintf(log_time, "%04d-%02d-%02d %02d:%02d:%02d ",
			p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
			p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
		ret = write(log_file, log_time, log_time_size);
		ret = write(log_file, p_str, str_size);
	}
#endif

	return 0;
}

#if RUN_OS_WINDOWS

int32 log_to_bufW(int32 level, wchar_t *p_buf, const wchar_t *p_format, ...)
{
	va_list args;

	int32  info_size;

	//int32  ret;

	if( level > log_level )
	{
		return -1;
	}

	va_start(args, p_format);

	info_size = vswprintf(p_buf, LOG_MAX_STRING_SIZE, p_format, args);

	va_end(args);

	return info_size;
}

int32 log_traceW(int32 level, const wchar_t *p_format, ...)
{
	va_list args;

	uint16 info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;
	
	if( level > log_level )
	{
		return 0;
	}
	
	va_start(args, p_format);

#if _MSC_VER <= 1200
	info_size = vswprintf(info, p_format, args);
#else
	info_size = vswprintf(info, LOG_MAX_STRING_SIZE, p_format, args);
#endif

	va_end(args);

	ret = log_outputW(info, info_size);

	return 0;
}

int32 log_debugW(const wchar_t *p_format, ...)
{
	va_list args;

	uint16 info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_DEBUG_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

#if _MSC_VER <= 1200
	info_size = vswprintf(info, p_format, args);
#else
	info_size = vswprintf(info, LOG_MAX_STRING_SIZE, p_format, args);
#endif

	va_end(args);

	ret = log_outputW(info, info_size);

	return 0;
}

int32 log_warnW(const wchar_t *p_format, ...)
{
	va_list args;

	uint16 info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_WARN_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

#if _MSC_VER <= 1200
	info_size = vswprintf(info, p_format, args);
#else
	info_size = vswprintf(info, LOG_MAX_STRING_SIZE, p_format, args);
#endif

	va_end(args);

	ret = log_outputW(info, info_size);

	return 0;
}

int32 log_errorW(const wchar_t *p_format, ...)
{
	va_list args;

	uint16 info[LOG_MAX_STRING_SIZE];
	int32  info_size;

	int32  ret;

	if( LOG_ERROR_LEVEL > log_level )
	{
		return 0;
	}

	va_start(args, p_format);

#if _MSC_VER <= 1200
	info_size = vswprintf(info, p_format, args);
#else
	info_size = vswprintf(info, LOG_MAX_STRING_SIZE, p_format, args);
#endif

	va_end(args);

	ret = log_outputW(info, info_size);

	return 0;
}

int32 log_outputW(uint16 *p_str, int32 str_size)
{
	int8   log_time[128];
	int32  log_time_size;

	int8   info2[LOG_MAX_STRING_SIZE];
	int32  info2_size;

	uint32 write_size;
	int32  ret;

	SYSTEMTIME sys_time;

	/** 打印 */
	if( b_output_log )
	{
		OutputDebugStringW(p_str);
	}

	/** 转发 */
	if( log_sock > 0 )
	{
		info2_size = WideCharToMultiByte(CP_OEMCP, 0, p_str, str_size, info2, LOG_MAX_STRING_SIZE, NULL, NULL);
		ret = sendto(log_sock, info2, info2_size - 1, 0, (struct sockaddr*)&remote, sizeof(remote));
	}

	/** 写日志文件 */
	if( log_file != INVALID_HANDLE_VALUE )
	{
		GetLocalTime(&sys_time);
		log_time_size = wsprintfA(log_time, "%04d-%02d-%02d %02d:%02d:%02d ",
			sys_time.wYear, sys_time.wMonth, sys_time.wDay,
			sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
		ret = WriteFile(log_file, log_time, log_time_size, (DWORD*)&write_size, NULL);

		info2_size = WideCharToMultiByte(CP_OEMCP, 0, p_str, str_size, info2, LOG_MAX_STRING_SIZE, NULL, NULL);
		if( info2_size > 0 )
		{
			ret = WriteFile(log_file, info2, info2_size, (DWORD*)&write_size, NULL);

		}else
		{
			ret = GetLastError();
		}
	}

	return 0;
}

#endif
