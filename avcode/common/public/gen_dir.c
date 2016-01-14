


#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#else

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#endif

#include <public/gen_error.h>
#include "gen_dir.h"

/** max dir size */
#define GEN_DIR_MAX_PATH_SIZE                              (1024)


/////////////////////////////// Outter Interface //////////////////////////////
int32 gen_dir_createA(int8* p_path, int32 path_size)
{
	int8   str_path[GEN_DIR_MAX_PATH_SIZE + 4];
	//int32  str_size;

	int32  i;
	int32  ret;

	if( p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 1 || path_size > GEN_DIR_MAX_PATH_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	for( i = 0; i < path_size; i++ )
	{
		if( p_path[i] == '\\' )
		{
			str_path[i] = '\0';

			ret = CreateDirectoryA(str_path, NULL);
		}

		str_path[i] = p_path[i];
	}

	if( str_path[path_size - 1] != '\\' )
	{
		str_path[path_size] = '\0';

		ret = CreateDirectoryA(str_path, NULL);
	}

#else

	str_path[0] = p_path[0];

	for( i = 1; i < path_size; i++ )
	{
		if( p_path[i] == '/' )
		{
			str_path[i] = '\0';

			ret = mkdir(str_path, 0777);
			//printf("i = %d, path = %s, ret = %d\n", i, str_path, ret);
		}

		str_path[i] = p_path[i];
	}

	if( str_path[path_size - 1] != '/' )
	{
		str_path[path_size] = '\0';

		ret = mkdir(str_path, 0777);
	}

#endif

	return GEN_S_OK;
}

int32 gen_dir_deleteA(int8* p_path, int32 path_size)
{
#if RUN_OS_WINDOWS

	int8*   p_str = NULL;
	int32  str_size;

	SHFILEOPSTRUCTA shell_file;

#else

	DIR*  p_top_dir = NULL;
	struct dirent* p_entry = NULL;
	struct dirent  entry;
	struct stat entry_stat;

	int8   str_path[GEN_DIR_MAX_PATH_SIZE + 4];
	int32  str_path_size;
	int32  left_buf_size;

	int32  b_error;
	int32  error_code;

#endif

	//int32  i;
	int32  ret;

	if( p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

#if RUN_OS_WINDOWS

	str_size = path_size + 4;
	p_str = (int8*)malloc( str_size );
	if( p_str == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	memcpy(p_str, p_path, path_size);
	p_str[path_size] = 0x00;
	p_str[path_size + 1] = 0x00;

	memset(&shell_file, 0, sizeof(shell_file));

	shell_file.hwnd = NULL;
	shell_file.wFunc = FO_DELETE;
	shell_file.pFrom = p_str;
	shell_file.pTo = NULL;
	shell_file.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	ret = SHFileOperationA(&shell_file);

	if( p_str )
	{
		free(p_str);
		p_str = NULL;
	}

#else

	b_error = 0;
	error_code = 0;

	p_top_dir = opendir(p_path);
	if( p_top_dir == NULL )
	{
		return GEN_E_FAIL;
	}

	while( 1 )
	{
		ret = readdir_r(p_top_dir, &entry, &p_entry);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		if( p_entry == NULL )
		{
			/** end of dir */
			break;
		}

		if( (strncmp(p_entry->d_name, ".", 1) == 0)
			|| (strncmp(p_entry->d_name, "..", 2) == 0) )
		{
			/** "." or ".." */
			continue;
		}

		left_buf_size = GEN_DIR_MAX_PATH_SIZE + 4;
		str_path_size = snprintf(str_path, left_buf_size, "%s", p_path);
		if( str_path[str_path_size - 1] != '/' )
		{
			str_path[str_path_size] = '/';
			str_path[str_path_size + 1] = 0x00;
			str_path_size++;
		}

		left_buf_size -= str_path_size;
		ret = snprintf(str_path + str_path_size, left_buf_size, "%s", p_entry->d_name);
		str_path_size += ret;

		ret = lstat(str_path, &entry_stat);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			//DEBUG(0, ("[%s][%d] lstat %s error = %d\n", __FUNCTION__, __LINE__, str_path, errno));

			break;
		}

		if( S_ISDIR(entry_stat.st_mode) )
		{
			/** recursive delete */
			ret = gen_dir_deleteA(str_path, str_path_size);
			if( ret )
			{
				b_error = 1;
				error_code = ret;
				//DEBUG(0, ("[%s][%d] remove dir %s error = 0x%x\n", __FUNCTION__, __LINE__, str_path, ret));

				break;
			}

		}else
		{
			ret = remove(str_path);
			if( ret )
			{
				//DEBUG(0, ("[%s][%d] remove file %s error = %d\n", __FUNCTION__, __LINE__, str_path, errno));
			}
		}
	}

	closedir(p_top_dir);

	if( b_error )
	{
		return error_code;
	}

	ret = remove(p_path);

#endif

	return GEN_S_OK;
}

int32 get_dir_remove_all_fileA(int8* p_path, int32 path_size)
{
#if RUN_OS_WINDOWS

	HANDLE  hDir = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA  find_data;

	int8    str_path[GEN_DIR_MAX_PATH_SIZE + 4];
	int32   str_path_size;

	int32   ret;

	if( p_path == NULL || path_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	str_path_size = GEN_DIR_MAX_PATH_SIZE;
	ret = sprintf_s(str_path, str_path_size, "%s\\*", p_path);
	if( ret < 0 )
	{
		return GEN_E_FAIL;
	}

	hDir = FindFirstFileA(str_path, &find_data);
	if( hDir == INVALID_HANDLE_VALUE )
	{
		return GEN_E_INNER_ERROR;
	}

	if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		&& (_strnicmp(find_data.cFileName, ".", 1) != 0) 
		&& (_strnicmp(find_data.cFileName, "..", 2) != 0) )
	{
		str_path_size = GEN_DIR_MAX_PATH_SIZE;
		ret = sprintf_s(str_path, str_path_size, "%s\\%s", p_path, find_data.cFileName);
		if( ret < 0 )
		{
			return GEN_E_FAIL;
		}

		DeleteFileA(str_path);
	}

	while( 1 )
	{
		ret = FindNextFileA(hDir, &find_data);
		if( !ret )
		{
			break;
		}

		if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
			&& (_strnicmp(find_data.cFileName, ".", 1) != 0) 
			&& (_strnicmp(find_data.cFileName, "..", 2) != 0) )
		{
			str_path_size = GEN_DIR_MAX_PATH_SIZE;
			ret = sprintf_s(str_path, str_path_size, "%s\\%s", p_path, find_data.cFileName);
			if( ret < 0 )
			{
				break;
			}

			DeleteFileA(str_path);
		}
	}

	FindClose(hDir);
	hDir = INVALID_HANDLE_VALUE;

#else

	DIR*  p_top_dir = NULL;
	struct dirent* p_entry = NULL;
	struct dirent  entry;

	int32  b_error;
	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_top_dir = opendir(p_path);
	if( p_top_dir == NULL )
	{
		return GEN_E_FAIL;
	}

	while( 1 )
	{
		ret = readdir_r(p_top_dir, &entry, &p_entry);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		if( p_entry == NULL )
		{
			/** end of dir */
			break;
		}

		if( p_entry->d_type == DT_REG || p_entry->d_type == DT_LNK )
		{
			remove(p_entry->d_name);
		}
	}

	closedir(p_top_dir);

	if( b_error )
	{
		return error_code;
	}

#endif

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS
int32 gen_dir_createW(wchar_t* p_path, int32 path_size)
{
	uint16  str_path[GEN_DIR_MAX_PATH_SIZE + 4];
	int32  char_path_size;

	int32  i;
	int32  ret;

	if( p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 1 || path_size > (GEN_DIR_MAX_PATH_SIZE * sizeof(wchar_t)) )
	{
		return GEN_E_INVALID_PARAM;
	}

	char_path_size = path_size / sizeof(wchar_t);
	for( i = 0; i < char_path_size; i++ )
	{
		if( p_path[i] == L'\\' )
		{
			str_path[i] = 0x0;

			ret = CreateDirectoryW(str_path, NULL);
		}

		str_path[i] = p_path[i];
	}

	if( str_path[char_path_size - 1] != L'\\' )
	{
		str_path[char_path_size] = 0x0;

		ret = CreateDirectoryW(str_path, NULL);
	}

	return GEN_S_OK;
}

int32 gen_dir_deleteW(wchar_t* p_path, int32 path_size)
{
	uint16* p_str = NULL;
	int32   str_size;

	SHFILEOPSTRUCTW shell_file;

	int32  ret;

	if( p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	str_size = path_size + 4;
	p_str = (uint16*)malloc( str_size );
	if( p_str == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	memcpy(p_str, p_path, path_size);
	p_str[path_size] = 0x00;
	p_str[path_size + 1] = 0x00;

	memset(&shell_file, 0, sizeof(shell_file));

	shell_file.hwnd = NULL;
	shell_file.wFunc = FO_DELETE;
	shell_file.pFrom = p_str;
	shell_file.pTo = NULL;
	shell_file.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	ret = SHFileOperationW(&shell_file);

	if( p_str )
	{
		free(p_str);
		p_str = NULL;
	}

	//ret = RemoveDirectoryW(p_path);

	return GEN_S_OK;
}

int32 get_dir_remove_all_fileW(wchar_t* p_path, int32 path_size)
{
	HANDLE  hDir = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW  find_data;

	uint16  str_path[GEN_DIR_MAX_PATH_SIZE + 4];
	int32   str_path_size;

	int32   ret;

	if( p_path == NULL || path_size < 1 )
	{
		return GEN_E_INVALID_PARAM;
	}

	str_path_size = GEN_DIR_MAX_PATH_SIZE;
	ret = swprintf_s(str_path, str_path_size, L"%s\\*", p_path);
	if( ret < 0 )
	{
		return GEN_E_FAIL;
	}

	hDir = FindFirstFileW(str_path, &find_data);
	if( hDir == INVALID_HANDLE_VALUE )
	{
		return GEN_E_INNER_ERROR;
	}

	if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
		&& (_wcsnicmp(find_data.cFileName, L".", 1) != 0) 
		&& (_wcsnicmp(find_data.cFileName, L"..", 2) != 0) )
	{
		str_path_size = GEN_DIR_MAX_PATH_SIZE;
		ret = swprintf_s(str_path, str_path_size, L"%s\\%s", p_path, find_data.cFileName);
		if( ret < 0 )
		{
			return GEN_E_FAIL;
		}

		DeleteFileW(str_path);
	}

	while( 1 )
	{
		ret = FindNextFileW(hDir, &find_data);
		if( !ret )
		{
			break;
		}

		if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
			&& (_wcsnicmp(find_data.cFileName, L".", 1) != 0) 
			&& (_wcsnicmp(find_data.cFileName, L"..", 2) != 0) )
		{
			str_path_size = GEN_DIR_MAX_PATH_SIZE;
			ret = swprintf_s(str_path, str_path_size, L"%s\\%s", p_path, find_data.cFileName);
			if( ret < 0 )
			{
				break;
			}

			DeleteFileW(str_path);
		}
	}

	FindClose(hDir);
	hDir = INVALID_HANDLE_VALUE;

	return GEN_S_OK;
}

#endif
