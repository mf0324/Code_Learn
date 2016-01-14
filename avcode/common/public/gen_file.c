

#include <stdlib.h>
#include <string.h>

#ifdef WIN32


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else

//#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#define __USE_LARGEFILE64	1
//#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#endif

#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "gen_file_const.h"
#include "gen_file.h"


/** gen file */
typedef struct tag_gen_file
{
	/** init */
	int32  b_init;

	/** open param */
	GEN_FILE_OPEN_PARAM  open_param;

#if RUN_OS_WINDOWS

	/** handle */
	HANDLE h;

#else

	/** fd */
	int32  fd;

#endif


} GEN_FILE, *PGEN_FILE;


/////////////////////////////// Outter Interface //////////////////////////////
GEN_FILE_H gen_file_malloc()
{
	GEN_FILE* p_file = NULL;

	p_file = (GEN_FILE*)malloc( sizeof(GEN_FILE) );
	if( p_file == NULL )
	{
		return NULL;
	}

	memset(p_file, 0, sizeof(GEN_FILE));

#if RUN_OS_WINDOWS

	p_file->h = INVALID_HANDLE_VALUE;

#else

	p_file->fd = -1;

#endif

	return p_file;
}

int32 gen_file_free(GEN_FILE_H file_h)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_file->b_init )
	{
		gen_file_deinit(p_file);
	}

	free(p_file);

	return GEN_S_OK;
}

int32 gen_file_init(GEN_FILE_H file_h)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_file->b_init )
	{
		return GEN_S_OK;
	}

	memset(&p_file->open_param, 0, sizeof(p_file->open_param));

#if RUN_OS_WINDOWS

	p_file->h = INVALID_HANDLE_VALUE;

#else

	p_file->fd = -1;

#endif

	p_file->b_init = 1;

	return GEN_S_OK;
}

int32 gen_file_deinit(GEN_FILE_H file_h)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	if( p_file->h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(p_file->h);
		p_file->h = INVALID_HANDLE_VALUE;
	}

#else

	if( p_file->fd > 0 )
	{
		close(p_file->fd);
		p_file->fd = -1;
	}

#endif

	p_file->b_init = 0;

	return GEN_S_OK;
}

int32 gen_file_set_param(GEN_FILE_H file_h, uint32 param, uint8* p_value, int32 value_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL || p_value == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	switch( param )
	{
	case GEN_FILE_PARAM_OPEN:
		{
			if( value_size < sizeof(p_file->open_param) )
			{
				return GEN_E_INVALID_PARAM;
			}

			memcpy(&p_file->open_param, p_value, sizeof(p_file->open_param));
		}
		break;

	default:
		{
		}
		break;
	}

	return GEN_S_OK;
}

int32 gen_file_is_open(GEN_FILE_H file_h, int32* p_b_open)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL || p_b_open == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	if( p_file->h != INVALID_HANDLE_VALUE )
	{
		*p_b_open = 1;

	}else
	{
		*p_b_open = 0;
	}

#else

	if( p_file->fd > 0 )
	{
		*p_b_open = 1;

	}else
	{
		*p_b_open = 0;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_openA(GEN_FILE_H file_h, int8* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	uint32 access_mode;
	uint32 share_mode;
	uint32 create_flag;
	uint32 file_flag;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(p_file->h);
		p_file->h = INVALID_HANDLE_VALUE;
	}

	access_mode = 0;
	share_mode = 0;
	create_flag = 0;
	file_flag = 0;

	/** access mode */
	if( p_file->open_param.rw & GEN_FILE_OPEN_READ )
	{
		access_mode |= GENERIC_READ;
	}

	if( p_file->open_param.rw & GEN_FILE_OPEN_WRITE )
	{
		access_mode |= GENERIC_WRITE;
	}

	/** share mode */
	if( p_file->open_param.share & GEN_FILE_SHARE_READ )
	{
		share_mode |= FILE_SHARE_READ;
	}

	if( p_file->open_param.share & GEN_FILE_SHARE_WRITE )
	{
		share_mode |= FILE_SHARE_WRITE;
	}

	if( p_file->open_param.share & GEN_FILE_SHARE_DEL )
	{
		share_mode |= FILE_SHARE_DELETE;
	}

	/** create flag */
	switch( p_file->open_param.create )
	{
	case GEN_FILE_CREATE_ALWAYS:
		{
			create_flag = CREATE_ALWAYS;
		}
		break;

	case GEN_FILE_CREATE_NEW:
		{
			create_flag = CREATE_NEW;
		}
		break;

	case GEN_FILE_CREATE_OPEN:
		{
			create_flag = OPEN_ALWAYS;
		}
		break;

	case GEN_FILE_CREATE_EXIST:
		{
			create_flag = OPEN_EXISTING;
		}
		break;

	case GEN_FILE_CREATE_TRUNC:
		{
			create_flag = TRUNCATE_EXISTING;
		}
		break;

	default:
		{
		}
		break;
	}

	/** file flag */
	file_flag = FILE_ATTRIBUTE_NORMAL;

	p_file->h = CreateFileA(
		p_path,
		access_mode,
		share_mode,
		NULL,
		create_flag,
		file_flag,
		NULL);
	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("file open error = %d\n"), GetLastError());
		return GEN_E_INNER_ERROR;
	}

#else

	if( p_file->fd > 0 )
	{
		close(p_file->fd);
		p_file->fd = -1;
	}

	access_mode = 0;
	share_mode = 0;
	create_flag = 0;
	file_flag = 0;

	/** access mode */
	if( p_file->open_param.rw & GEN_FILE_OPEN_READ )
	{
		if( p_file->open_param.rw & GEN_FILE_OPEN_WRITE )
		{
			access_mode |= O_RDWR;

		}else
		{
			access_mode |= O_RDONLY;
		}

	}else if( p_file->open_param.rw & GEN_FILE_OPEN_WRITE )
	{
		access_mode |= O_WRONLY;
	}

	/** create flag */
	switch( p_file->open_param.create )
	{
	case GEN_FILE_CREATE_ALWAYS:
		{
			access_mode |= O_CREAT;
			access_mode |= O_TRUNC;
		}
		break;

	case GEN_FILE_CREATE_NEW:
		{
			access_mode |= O_CREAT;
		}
		break;

	case GEN_FILE_CREATE_TRUNC:
		{
			access_mode |= O_TRUNC;
		}
		break;

	default:
		{
		}
		break;
	}

	p_file->fd = open(p_path, access_mode, 00777);
	if( p_file->fd < 0 )
	{
		//printf("[gen_file::gen_file_openA] file open error = %d\n", errno);
		return GEN_E_INNER_ERROR;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_open2A(GEN_FILE_H h, GEN_FILE_OPEN_PARAM* p_param, int8* p_path, int32 path_size)
{
	int32  ret;

	ret = gen_file_set_param(h, GEN_FILE_PARAM_OPEN, (uint8*)p_param, sizeof(GEN_FILE_OPEN_PARAM));
	if( ret )
	{
		return ret;
	}

	return gen_file_openA(h, p_path, path_size);
}

int32 gen_file_deleteA(GEN_FILE_H file_h, int8* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	int32  ret;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	ret = DeleteFileA(p_path);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

#else

	ret = unlink(p_path);
	if( !ret )
	{
		return GEN_E_FAIL;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_is_existA(GEN_FILE_H h, int32* p_b_exist, int8* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)h;

#if RUN_OS_WINDOWS

	HANDLE  hFile = INVALID_HANDLE_VALUE;

#else

	int32   fd = -1;

#endif

	uint32 access_mode;
	uint32 share_mode;
	uint32 create_flag;
	uint32 file_flag;

	if( p_b_exist == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	//access_mode = 0;

	/** access mode */
	access_mode = GENERIC_READ;

	/** share mode */
	share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	/** create flag */
	create_flag = OPEN_EXISTING;

	/** file flag */
	file_flag = FILE_ATTRIBUTE_NORMAL;

	hFile = CreateFileA(p_path, access_mode, share_mode, NULL, create_flag, file_flag, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("file open error = %d\n"), GetLastError());
		*p_b_exist = 0;

	}else
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		*p_b_exist = 1;
	}

#else

	/** access mode */
	access_mode = O_RDONLY;
	share_mode = 0;
	create_flag = 0;
	file_flag = 0;

	fd = open(p_path, access_mode, 00777);
	if( fd < 0 )
	{
		*p_b_exist = 0;

	}else
	{
		close(fd);
		fd = -1;

		*p_b_exist = 1;
	}

#endif

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS

int32 gen_file_openW(GEN_FILE_H file_h, wchar_t* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	uint32 access_mode;
	uint32 share_mode;
	uint32 create_flag;
	uint32 file_flag;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_file->h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(p_file->h);
		p_file->h = INVALID_HANDLE_VALUE;
	}

	access_mode = 0;
	share_mode = 0;
	create_flag = 0;
	file_flag = 0;

	/** access mode */
	if( p_file->open_param.rw & GEN_FILE_OPEN_READ )
	{
		access_mode |= GENERIC_READ;
	}

	if( p_file->open_param.rw & GEN_FILE_OPEN_WRITE )
	{
		access_mode |= GENERIC_WRITE;
	}

	/** share mode */
	if( p_file->open_param.share & GEN_FILE_SHARE_READ )
	{
		share_mode |= FILE_SHARE_READ;
	}

	if( p_file->open_param.share & GEN_FILE_SHARE_WRITE )
	{
		share_mode |= FILE_SHARE_WRITE;
	}

	if( p_file->open_param.share & GEN_FILE_SHARE_DEL )
	{
		share_mode |= FILE_SHARE_DELETE;
	}

	/** create flag */
	switch( p_file->open_param.create )
	{
	case GEN_FILE_CREATE_ALWAYS:
		{
			create_flag = CREATE_ALWAYS;
		}
		break;

	case GEN_FILE_CREATE_NEW:
		{
			create_flag = CREATE_NEW;
		}
		break;

	case GEN_FILE_CREATE_OPEN:
		{
			create_flag = OPEN_ALWAYS;
		}
		break;

	case GEN_FILE_CREATE_EXIST:
		{
			create_flag = OPEN_EXISTING;
		}
		break;

	case GEN_FILE_CREATE_TRUNC:
		{
			create_flag = TRUNCATE_EXISTING;
		}
		break;

	default:
		{
		}
		break;
	}

	/** file flag */
	file_flag = FILE_ATTRIBUTE_NORMAL;

	p_file->h = CreateFileW(
		p_path,
		access_mode,
		share_mode,
		NULL,
		create_flag,
		file_flag,
		NULL);
	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("file open error = %d\n"), GetLastError());
		return GEN_E_INNER_ERROR;
	}

	return GEN_S_OK;
}

int32 gen_file_open2W(GEN_FILE_H h, GEN_FILE_OPEN_PARAM* p_param, wchar_t* p_path, int32 path_size)
{
	int32  ret;

	ret = gen_file_set_param(h, GEN_FILE_PARAM_OPEN, (uint8*)p_param, sizeof(GEN_FILE_OPEN_PARAM));
	if( ret )
	{
		return ret;
	}

	return gen_file_openW(h, p_path, path_size);
}

int32 gen_file_deleteW(GEN_FILE_H file_h, wchar_t* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	int32  ret;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = DeleteFileW(p_path);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

	return GEN_S_OK;
}

int32 gen_file_is_existW(GEN_FILE_H h, int32* p_b_exist, wchar_t* p_path, int32 path_size)
{
	GEN_FILE* p_file = (GEN_FILE*)h;

	HANDLE  hFile = INVALID_HANDLE_VALUE;

	uint32 access_mode;
	uint32 share_mode;
	uint32 create_flag;
	uint32 file_flag;

	if( p_b_exist == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	/** access mode */
	access_mode = GENERIC_READ;

	/** share mode */
	share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	/** create flag */
	create_flag = OPEN_EXISTING;

	/** file flag */
	file_flag = FILE_ATTRIBUTE_NORMAL;

	hFile = CreateFileW(p_path, access_mode, share_mode, NULL, create_flag, file_flag, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		*p_b_exist = 0;

	}else
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		*p_b_exist = 1;
	}

	return GEN_S_OK;
}

#endif

int32 gen_file_close(GEN_FILE_H file_h)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h != INVALID_HANDLE_VALUE )
	{
		CloseHandle(p_file->h);
		p_file->h = INVALID_HANDLE_VALUE;
	}

#else

	if( p_file->fd > 0 )
	{
		close(p_file->fd);
		p_file->fd = -1;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_get_size(GEN_FILE_H file_h, int64* p_file_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

#if RUN_OS_WINDOWS
	//LARGE_INTEGER file_size;

#else

	struct stat file_stat;

#endif

	int32  ret;

	if( p_file == NULL || p_file_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	ret = GetFileSizeEx(p_file->h, (LARGE_INTEGER*)p_file_size);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	ret = fstat(p_file->fd, &file_stat);
	if( ret == -1 )
	{
		return GEN_E_INNER_ERROR;
	}

	*p_file_size = file_stat.st_size;

#endif

	return GEN_S_OK;
}

int32 gen_file_seek(GEN_FILE_H file_h, int32 method, int32 pos, uint32* p_current_pos)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	uint32 seek_method;
#if RUN_OS_WINDOWS

	LARGE_INTEGER seek_pos;
	int64  file_pos;

#else

	off_t  offset;

#endif

	int32  ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	switch( method )
	{
	case GEN_FILE_SEEK_BEGIN:
		{
			seek_method = FILE_BEGIN;
		}
		break;

	case GEN_FILE_SEEK_END:
		{
			seek_method = FILE_END;
		}
		break;

	case GEN_FILE_SEEK_CURRENT:
		{
			seek_method = FILE_CURRENT;
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	seek_pos.QuadPart = pos;

	ret = SetFilePointerEx(p_file->h, seek_pos, (LARGE_INTEGER*)&file_pos, seek_method);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_current_pos )
	{
		*p_current_pos = file_pos;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	switch( method )
	{
	case GEN_FILE_SEEK_BEGIN:
		{
			seek_method = SEEK_SET;
		}
		break;

	case GEN_FILE_SEEK_END:
		{
			seek_method = SEEK_END;
		}
		break;

	case GEN_FILE_SEEK_CURRENT:
		{
			seek_method = SEEK_CUR;
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	offset = lseek(p_file->fd, pos, seek_method);
	if( offset == (off_t)-1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_current_pos )
	{
		*p_current_pos = offset;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_seek64(GEN_FILE_H file_h, int32 method, int64 pos, int64* p_current_pos)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	uint32 seek_method;
#if RUN_OS_WINDOWS

	LARGE_INTEGER seek_pos;
	int64  file_pos;

#else

	off64_t offset;

#endif

	int32  ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	switch( method )
	{
	case GEN_FILE_SEEK_BEGIN:
		{
			seek_method = FILE_BEGIN;
		}
		break;

	case GEN_FILE_SEEK_END:
		{
			seek_method = FILE_END;
		}
		break;

	case GEN_FILE_SEEK_CURRENT:
		{
			seek_method = FILE_CURRENT;
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	seek_pos.QuadPart = pos;

	ret = SetFilePointerEx(p_file->h, seek_pos, (LARGE_INTEGER*)&file_pos, seek_method);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_current_pos )
	{
		*p_current_pos = file_pos;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	switch( method )
	{
	case GEN_FILE_SEEK_BEGIN:
		{
			seek_method = SEEK_SET;
		}
		break;

	case GEN_FILE_SEEK_END:
		{
			seek_method = SEEK_END;
		}
		break;

	case GEN_FILE_SEEK_CURRENT:
		{
			seek_method = SEEK_CUR;
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	offset = lseek64(p_file->fd, pos, seek_method);
	if( offset == (off_t)-1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_current_pos )
	{
		*p_current_pos = offset;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_set_eof(GEN_FILE_H file_h)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

#if RUN_OS_WINDOWS

#else

	off_t offset;

#endif

	int32  ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	ret = SetEndOfFile(p_file->h);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	offset = lseek(p_file->fd, 0, SEEK_CUR);
	if( offset == (off_t)-1 )
	{
		return GEN_E_FAIL;
	}

	ret = ftruncate(p_file->fd, offset);
	if( ret )
	{
		return GEN_E_INNER_ERROR;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_read(GEN_FILE_H file_h, uint8* p_buf, uint32 read_size, uint32* p_actual_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	int32  ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	ret = ReadFile(p_file->h, p_buf, read_size, (DWORD*)p_actual_size, NULL);
	if( !ret )
	{
		return GEN_E_INNER_ERROR;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	ret = read(p_file->fd, p_buf, read_size);
	if( ret == -1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_actual_size )
	{
		*p_actual_size = ret;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_read64(GEN_FILE_H h, uint8* p_buf, int64 read_size, int64* p_actual_size)
{
	GEN_FILE* p_file = (GEN_FILE*)h;

	int64   left_size;
	int64   total_read_size;
	uint32  max_read_size;
	uint32  read_data_size;

	uint8*  p_uint8 = NULL;

	int32   b_error;
	int32   error_code;

	int32   ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( read_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = GEN_S_OK;

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	if( sizeof(DWORD) >= sizeof(int64) )
	{
		ret = ReadFile(p_file->h, p_buf, read_size, (DWORD*)p_actual_size, NULL);
		if( !ret )
		{
			return GEN_E_INNER_ERROR;
		}

	}else
	{
		left_size = read_size;
		total_read_size = 0;
		p_uint8 = p_buf;

		while( total_read_size < read_size )
		{
			if( left_size > UINT32_MAX )
			{
				max_read_size = UINT32_MAX;

			}else
			{
				max_read_size = left_size;
			}

			ret = ReadFile(p_file->h, p_uint8, max_read_size, (DWORD*)&read_data_size, NULL);
			if( !ret )
			{
				b_error = 1;
				error_code = GEN_E_INNER_ERROR;
				break;
			}

			left_size -= read_data_size;
			total_read_size += read_data_size;
			p_uint8 += read_data_size;
		}

		if( p_actual_size )
		{
			*p_actual_size = total_read_size;
		}
	}


#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	ret = read(p_file->fd, p_buf, read_size);
	if( ret == -1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_actual_size )
	{
		*p_actual_size = ret;
	}

#endif

	return error_code;
}

int32 gen_file_write(GEN_FILE_H file_h, uint8* p_buf, uint32 write_size, uint32* p_actual_size)
{
	GEN_FILE* p_file = (GEN_FILE*)file_h;

	int32  ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	ret = WriteFile(p_file->h, p_buf, write_size, (DWORD*)p_actual_size, NULL);
	if( !ret )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("file write error = %d\n"), GetLastError());
		return GEN_E_INNER_ERROR;
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	ret = write(p_file->fd, p_buf, write_size);
	if( ret == -1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_actual_size )
	{
		*p_actual_size = ret;
	}

#endif

	return GEN_S_OK;
}

int32 gen_file_write64(GEN_FILE_H h, uint8* p_buf, int64 write_size, int64* p_actual_size)
{
	GEN_FILE* p_file = (GEN_FILE*)h;

	int64   left_size;
	int64   total_write_size;
	uint32  max_write_size;
	uint32  write_data_size;

	uint8*  p_uint8 = NULL;

	int32   b_error;
	int32   error_code;

	int32   ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( write_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = GEN_S_OK;

#if RUN_OS_WINDOWS

	if( p_file->h == INVALID_HANDLE_VALUE )
	{
		return GEN_E_FAIL;
	}

	if( sizeof(DWORD) >= sizeof(int64) )
	{
		ret = WriteFile(p_file->h, p_buf, write_size, (DWORD*)p_actual_size, NULL);
		if( !ret )
		{
			//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("file write error = %d\n"), GetLastError());
			return GEN_E_INNER_ERROR;
		}

	}else
	{
		left_size = write_size;
		total_write_size = 0;
		p_uint8 = p_buf;

		while( total_write_size < write_size )
		{
			if( left_size > UINT32_MAX )
			{
				max_write_size = UINT32_MAX;

			}else
			{
				max_write_size = left_size;
			}

			ret = WriteFile(p_file->h, p_uint8, max_write_size, (DWORD*)&write_data_size, NULL);
			if( !ret )
			{
				b_error = 1;
				error_code = GEN_E_INNER_ERROR;
				break;
			}

			left_size -= write_data_size;
			total_write_size += write_data_size;
			p_uint8 += write_data_size;
		}

		if( p_actual_size )
		{
			*p_actual_size = total_write_size;
		}
	}

#else

	if( p_file->fd < 0 )
	{
		return GEN_E_FAIL;
	}

	ret = write(p_file->fd, p_buf, write_size);
	if( ret == -1 )
	{
		return GEN_E_INNER_ERROR;
	}

	if( p_actual_size )
	{
		*p_actual_size = ret;
	}

#endif

	return error_code;
}
