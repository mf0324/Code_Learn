
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <public/gen_platform.h>


#ifdef RUN_OS_WINDOWS


//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#else

//#define _LARGEFILE64_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include <errno.h>

#endif

#include <public/gen_list.h>
#include <public/gen_file_const.h>
#include <public/gen_file.h>
#include <public/gen_text_public.h>
#include <public/gen_text.h>
#include <public/gen_error.h>
//#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "gen_ini_file.h"

/** max file path size */
#define GEN_INI_FILE_MAX_PATH_SIZE                         (1024)
/** max param size */
#define GEN_INI_FILE_MAX_PARAM_SIZE                        (512)
/** max value size */
#define GEN_INI_FILE_MAX_VALUE_SIZE                        (1024)

/** io file */
typedef struct tag_ini_io_file
{
	/** file handle */
	GEN_FILE_H  h;

	/** file path */
	int8   file_path[GEN_INI_FILE_MAX_PATH_SIZE + 4];
	/** file path size */
	int32  file_path_size;

} INI_IO_FILE, *PINI_IO_FILE;

/** param */
typedef struct tag_ini_param
{
	/** param name */
	int8   name[GEN_INI_FILE_MAX_PARAM_SIZE + 4];
	/** param name size */
	int32  name_size;

	/** param value */
	int8   value[GEN_INI_FILE_MAX_VALUE_SIZE + 4];
	/** param value size */
	int32  value_size;

} INI_PARAM, *PINI_PARAM;

/** gen ini file */
typedef struct tag_gen_ini_file
{
	/** init */
	int32  b_init;

	/** param list */
	GEN_LIST     param_list;

	/** input file */
	INI_IO_FILE  input;
	/** output file */
	INI_IO_FILE  output;

} GEN_INI_FILE, *PGEN_INI_FILE;


/////////////////////////////// Inner  Interface //////////////////////////////
/**  
 * @brief init param list
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_init_param_list(GEN_INI_FILE* p_file)
{
	GEN_LIST*  p_list = &p_file->param_list;

	gen_list_setup(p_list, NULL);
	gen_list_init(p_list);

	return GEN_S_OK;
}

/**  
 * @brief deinit param list
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_deinit_param_list(GEN_INI_FILE* p_file)
{
	GEN_LIST*  p_list = &p_file->param_list;

	INI_PARAM*  p_param = NULL;

	int32   ret;

	while( 1 )
	{
		ret = gen_list_get_front_data(p_list, (void**)&p_param);
		if( ret )
		{
			break;
		}

		gen_list_pop_front(p_list);

		free(p_param);
		p_param = NULL;
	}

	gen_list_cleanup(p_list);

	return GEN_S_OK;
}

/**  
 * @brief init input file
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_init_input(GEN_INI_FILE* p_file)
{
	INI_IO_FILE*  p_io_file = &p_file->input;

	p_io_file->h = NULL;
	p_io_file->h = gen_file_malloc();
	if( p_io_file->h )
	{
		gen_file_init(p_io_file->h);
	}

	p_io_file->file_path[0] = 0x00;
	p_io_file->file_path_size = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit input file
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_deinit_input(GEN_INI_FILE* p_file)
{
	INI_IO_FILE*  p_io_file = &p_file->input;

	gen_file_free(p_io_file->h);
	p_io_file->h = NULL;

	return GEN_S_OK;
}

/**  
 * @brief init output file
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_init_output(GEN_INI_FILE* p_file)
{
	INI_IO_FILE*  p_io_file = &p_file->output;

	p_io_file->h = NULL;
	p_io_file->h = gen_file_malloc();
	if( p_io_file->h )
	{
		gen_file_init(p_io_file->h);
	}

	p_io_file->file_path[0] = 0x00;
	p_io_file->file_path_size = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit output file
 * @param [in] p_file, file
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_ini_file_deinit_output(GEN_INI_FILE* p_file)
{
	INI_IO_FILE*  p_io_file = &p_file->output;

	gen_file_free(p_io_file->h);
	p_io_file->h = NULL;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
GEN_INI_FILE_H gen_ini_file_create()
{
	GEN_INI_FILE* p_file = NULL;

	p_file = (GEN_INI_FILE*)malloc( sizeof(GEN_INI_FILE) );
	if( p_file == NULL )
	{
		return NULL;
	}

	memset(p_file, 0, sizeof(*p_file));

	return p_file;
}

int32 gen_ini_file_destroy(GEN_INI_FILE_H h)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_file->b_init )
	{
		gen_ini_file_deinit(p_file);
	}

	free(p_file);

	return GEN_S_OK;
}

int32 gen_ini_file_init(GEN_INI_FILE_H h)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	int32   ret;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_file->b_init )
	{
		return GEN_S_OK;
	}

	ret = gen_ini_file_init_param_list(p_file);
	ret = gen_ini_file_init_input(p_file);
	ret = gen_ini_file_init_output(p_file);

	p_file->b_init = 1;

	return GEN_S_OK;
}

int32 gen_ini_file_deinit(GEN_INI_FILE_H h)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	gen_ini_file_deinit_param_list(p_file);
	gen_ini_file_deinit_input(p_file);
	gen_ini_file_deinit_output(p_file);

	p_file->b_init = 0;

	return GEN_S_OK;
}

int32 gen_ini_file_set_input_fileA(GEN_INI_FILE_H h, int8* p_path, int32 path_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 0 || path_size > GEN_INI_FILE_MAX_PATH_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( path_size > 0 )
	{
		memcpy(p_file->input.file_path, p_path, path_size);
	}

	p_file->input.file_path[path_size] = 0x00;
	p_file->input.file_path_size = path_size;

	return GEN_S_OK;
}

int32 gen_ini_file_set_output_fileA(GEN_INI_FILE_H h, int8* p_path, int32 path_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 0 || path_size > GEN_INI_FILE_MAX_PATH_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( path_size > 0 )
	{
		memcpy(p_file->output.file_path, p_path, path_size);
	}

	p_file->output.file_path[path_size] = 0x00;
	p_file->output.file_path_size = path_size;

	return GEN_S_OK;
}

#if RUN_OS_WINDOWS

int32 gen_ini_file_set_input_fileW(GEN_INI_FILE_H h, wchar_t* p_path, int32 path_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 0 || path_size > GEN_INI_FILE_MAX_PATH_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( path_size > 0 )
	{
		memcpy(p_file->input.file_path, p_path, path_size);
	}

	p_file->input.file_path[path_size] = 0x00;
	p_file->input.file_path[path_size + 1] = 0x00;
	p_file->input.file_path_size = path_size;

	return GEN_S_OK;
}

int32 gen_ini_file_set_output_fileW(GEN_INI_FILE_H h, wchar_t* p_path, int32 path_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	if( p_file == NULL || p_path == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( path_size < 0 || path_size > GEN_INI_FILE_MAX_PATH_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( path_size > 0 )
	{
		memcpy(p_file->output.file_path, p_path, path_size);
	}

	p_file->output.file_path[path_size] = 0x00;
	p_file->output.file_path[path_size + 1] = 0x00;
	p_file->output.file_path_size = path_size;

	return GEN_S_OK;
}

#endif

int32 gen_ini_file_get_param(GEN_INI_FILE_H h, int8* p_param, int32 param_size, int8* p_value, int32* p_value_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	LIST_NODE*  p_node = NULL;
	INI_PARAM*  p_param1 = NULL;

	int32   value_buf_size;

	int32   b_find;
	int32   ret;

	if( p_file == NULL || p_param == NULL || p_value == NULL || p_value_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( param_size < 1 || (*p_value_size < 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	value_buf_size = *p_value_size;

	p_value[0] = 0x00;
	*p_value_size = 0;

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	b_find = 0;
	p_node = p_file->param_list.p_head->p_next;
	while( p_node != p_file->param_list.p_rear )
	{
		p_param1 = (INI_PARAM*)p_node->p_data;
		if( strncmp(p_param1->name, p_param, param_size) == 0 )
		{
			b_find = 1;
			break;
		}

		p_node = p_node->p_next;
	}

	if( !b_find )
	{
		return GEN_E_NOT_FOUND;
	}

	if( value_buf_size <= p_param1->value_size )
	{
		*p_value_size = p_param1->value_size + 4;
		return GEN_E_NEED_MORE_BUF;
	}

	if( p_param1->value_size > 0 )
	{
		memcpy(p_value, p_param1->value, p_param1->value_size);
	}
	p_value[p_param1->value_size] = 0x00;
	*p_value_size = p_param1->value_size;

	return GEN_S_OK;
}

int32 gen_ini_file_get_param_pair(GEN_INI_FILE_H h, int32 param_index, int8* p_name, int32* p_name_size, int8* p_value, int32* p_value_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	LIST_NODE*  p_node = NULL;
	INI_PARAM*  p_param1 = NULL;

	int32   name_size;
	int32   value_size;

	int32   ret;

	if( p_file == NULL || p_name == NULL || p_name_size == NULL || p_value == NULL || p_value_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( param_index < 0 || (*p_name_size < 1) || (*p_value_size < 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_name[0] = 0x00;
	name_size = *p_name_size;
	*p_name_size = 0;

	p_value[0] = 0x00;
	value_size = *p_value_size;
	*p_value_size = 0;

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = gen_list_get_node(&p_file->param_list, param_index, &p_node);
	if( ret )
	{
		return GEN_E_NOT_FOUND;
	}

	p_param1 = (INI_PARAM*)p_node->p_data;
	if( (name_size <= p_param1->name_size) || (value_size <= p_param1->value_size) )
	{
		*p_name_size = p_param1->name_size + 4;
		*p_value_size = p_param1->value_size + 4;
		return GEN_E_NEED_MORE_BUF;
	}

	if( p_param1->name_size > 0 )
	{
		memcpy(p_name, p_param1->name, p_param1->name_size);
	}
	p_name[p_param1->name_size] = 0x00;
	*p_name_size = p_param1->name_size;

	if( p_param1->value_size > 0 )
	{
		memcpy(p_value, p_param1->value, p_param1->value_size);
	}
	p_value[p_param1->value_size] = 0x00;
	*p_value_size = p_param1->value_size;

	return GEN_S_OK;
}

int32 gen_ini_file_set_param(GEN_INI_FILE_H h, int8* p_param, int32 param_size, int8* p_value, int32 value_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	LIST_NODE*  p_node = NULL;
	INI_PARAM*  p_param1 = NULL;

	int32   b_find;
	int32   ret;

	if( p_file == NULL || p_param == NULL || p_value == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( param_size < 1 || param_size > GEN_INI_FILE_MAX_PARAM_SIZE || value_size < 0 || value_size > GEN_INI_FILE_MAX_VALUE_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	b_find = 0;
	p_node = p_file->param_list.p_head->p_next;
	while( p_node != p_file->param_list.p_rear )
	{
		p_param1 = (INI_PARAM*)p_node->p_data;
		if( strncmp(p_param1->name, p_param, param_size) == 0 )
		{
			b_find = 1;
			break;
		}

		p_node = p_node->p_next;
	}

	if( b_find )
	{
		/** replace with new value */
		if( value_size > 0 )
		{
			memcpy(p_param1->value, p_value, value_size);
		}

		p_param1->value[value_size] = 0x00;
		p_param1->value_size = value_size;

	}else
	{
		/** insert new param */
		p_param1 = (INI_PARAM*)malloc( sizeof(INI_PARAM) );
		if( p_param1 == NULL )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}

		memcpy(p_param1->name, p_param, param_size);
		p_param1->name[param_size] = 0x00;
		p_param1->name_size = param_size;

		if( value_size > 0 )
		{
			memcpy(p_param1->value, p_value, value_size);
		}
		p_param1->value[value_size] = 0x00;
		p_param1->value_size = value_size;

		ret = gen_list_push_back_data(&p_file->param_list, p_param1, NULL);
		if( ret )
		{
			free(p_param1);
			return GEN_E_FAIL;
		}
	}

	return GEN_S_OK;
}

int32 gen_ini_file_parse(GEN_INI_FILE_H h)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	//LIST_NODE*  p_node = NULL;
	INI_PARAM*  p_param1 = NULL;

	GEN_TEXT    text;
	GEN_FILE_OPEN_PARAM  open_param;

	int64   file_size;
	uint32  file_size32;
 	int64   read_size;
	uint8*  p_file_buf = NULL;

	int32   parse_size;
	int32   total_parse_size;
	int32   left_size;
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

	if( p_file->input.file_path_size == 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = 0;

	gen_text_setup(&text);
	gen_text_init(&text);

	memset(&open_param, 0, sizeof(open_param));
	open_param.rw = GEN_FILE_OPEN_READ;
	open_param.share = GEN_FILE_SHARE_READ | GEN_FILE_SHARE_WRITE;
	open_param.create = GEN_FILE_CREATE_EXIST;
	open_param.open_flag = 0;

#if RUN_OS_WINDOWS

#ifdef UNICODE
	ret = gen_file_open2W(p_file->input.h, &open_param, (wchar_t*)p_file->input.file_path, p_file->input.file_path_size);
#else
	ret = gen_file_open2A(p_file->input.h, &open_param, p_file->input.file_path, p_file->input.file_path_size);
#endif

#else
	ret = gen_file_open2(p_file->input.h, &open_param, p_file->input.file_path, p_file->input.file_path_size);
#endif

	if( ret )
	{
		return GEN_E_NOT_FOUND;
	}

	ret = gen_file_get_size(p_file->input.h, &file_size);
	if( ret )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("get file = %s size error = 0x%x\n"), p_file->input.file_path, ret);
		b_error = 1;
		error_code = GEN_E_FAIL;
		goto FIN;
	}

	p_file_buf = (uint8*)malloc( file_size + 4 );
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	ret = gen_file_read64(p_file->input.h, p_file_buf, file_size, &read_size);
	if( ret )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("read file = %s error = 0x%x\n"), p_file->input.file_path, ret);
		b_error = 1;
		error_code = GEN_E_FAIL;
		goto FIN;
	}

	gen_file_close(p_file->input.h);

#if 1

	file_size32 = read_size;
	p_file_buf[file_size32] = 0x00;

	total_parse_size = 0;
	left_size = file_size;
	p_uint8 = p_file_buf;

	while( total_parse_size < file_size )
	{
		p_param1 = (INI_PARAM*)malloc( sizeof(INI_PARAM) );
		if( p_param1 == NULL )
		{
			b_error = 1;
			error_code = GEN_E_NOT_ENOUGH_MEM;
			break;
		}

		p_param1->name_size = GEN_INI_FILE_MAX_PARAM_SIZE;
		p_param1->value_size = GEN_INI_FILE_MAX_VALUE_SIZE;
		ret = gen_text_get_param_pair(&text, p_uint8, left_size, &parse_size, (uint8*)p_param1->name, &p_param1->name_size, (uint8*)p_param1->value, &p_param1->value_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			break;
		}

		ret = gen_list_push_back_data(&p_file->param_list, p_param1, NULL);
		if( ret )
		{
			free(p_param1);
			p_param1 = NULL;

			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		p_param1 = NULL;

		total_parse_size += parse_size;
		left_size -= parse_size;
		p_uint8 += parse_size;
	}

#endif

FIN:

	gen_file_close(p_file->input.h);

	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	if( p_param1 )
	{
		free(p_param1);
		p_param1 = NULL;
	}

	gen_text_deinit(&text);

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 gen_ini_file_parse2(GEN_INI_FILE_H h, int8* p_data, int32 data_size)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	INI_PARAM*  p_param1 = NULL;

	GEN_TEXT    text;

	int32   parse_size;
	int32   total_parse_size;
	int32   left_size;
	uint8*  p_uint8 = NULL;

	int32   b_error;
	int32   error_code;

	int32   ret;

	if( p_file == NULL || p_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( data_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_file->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	b_error = 0;
	error_code = 0;

	gen_text_setup(&text);
	gen_text_init(&text);

	total_parse_size = 0;
	left_size = data_size;
	p_uint8 = p_data;

	while( total_parse_size < data_size )
	{
		p_param1 = (INI_PARAM*)malloc( sizeof(INI_PARAM) );
		if( p_param1 == NULL )
		{
			b_error = 1;
			error_code = GEN_E_NOT_ENOUGH_MEM;
			break;
		}

		p_param1->name_size = GEN_INI_FILE_MAX_PARAM_SIZE;
		p_param1->value_size = GEN_INI_FILE_MAX_VALUE_SIZE;
		ret = gen_text_get_param_pair(&text, p_uint8, left_size, &parse_size, (uint8*)p_param1->name, &p_param1->name_size, (uint8*)p_param1->value, &p_param1->value_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			break;
		}

		ret = gen_list_push_back_data(&p_file->param_list, p_param1, NULL);
		if( ret )
		{
			free(p_param1);
			p_param1 = NULL;

			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		p_param1 = NULL;

		total_parse_size += parse_size;
		left_size -= parse_size;
		p_uint8 += parse_size;
	}

//FIN:

	if( p_param1 )
	{
		free(p_param1);
		p_param1 = NULL;
	}

	gen_text_deinit(&text);

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 gen_ini_file_save(GEN_INI_FILE_H h)
{
	GEN_INI_FILE* p_file = (GEN_INI_FILE*)h;

	LIST_NODE*  p_node = NULL;
	INI_PARAM*  p_param1 = NULL;

	GEN_FILE_OPEN_PARAM  open_param;

	int64   write_size;
	int8    str_buf[GEN_INI_FILE_MAX_VALUE_SIZE];
	int32   str_buf_size;

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

	if( p_file->output.file_path_size == 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	b_error = 0;
	error_code = 0;

	str_buf_size = GEN_INI_FILE_MAX_VALUE_SIZE;

#if RUN_OS_WINDOWS
	ret = _snprintf(str_buf, str_buf_size, " = ");
#else
	ret = snprintf(str_buf, str_buf_size, " = ");
#endif
	str_buf_size = ret;

	memset(&open_param, 0, sizeof(open_param));
	open_param.rw = GEN_FILE_OPEN_WRITE;
	open_param.share = GEN_FILE_SHARE_READ | GEN_FILE_SHARE_WRITE;
	open_param.create = GEN_FILE_CREATE_ALWAYS;
	open_param.open_flag = 0;

#if RUN_OS_WINDOWS

#ifdef UNICODE
	ret = gen_file_open2W(p_file->output.h, &open_param, (wchar_t*)p_file->output.file_path, p_file->output.file_path_size);
#else
	ret = gen_file_open2A(p_file->output.h, &open_param, p_file->output.file_path, p_file->output.file_path_size);
#endif

#else
	ret = gen_file_open2(p_file->output.h, &open_param, p_file->output.file_path, p_file->output.file_path_size);
#endif

	if( ret )
	{
		return GEN_E_FAIL;
	}

	p_node = p_file->param_list.p_head->p_next;
	while( p_node != p_file->param_list.p_rear )
	{
		p_param1 = (INI_PARAM*)p_node->p_data;

		/** name */
		ret = gen_file_write64(p_file->output.h, p_param1->name, p_param1->name_size, &write_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		/** "=" */
		ret = gen_file_write64(p_file->output.h, str_buf, str_buf_size, &write_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		/** value */
		ret = gen_file_write64(p_file->output.h, p_param1->value, p_param1->value_size, &write_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		/** "\n" */
		ret = gen_file_write64(p_file->output.h, "\n", strlen("\n"), &write_size);
		if( ret )
		{
			b_error = 1;
			error_code = GEN_E_FAIL;
			break;
		}

		p_node = p_node->p_next;
	}

	gen_file_close(p_file->output.h);

//FIN:

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}
