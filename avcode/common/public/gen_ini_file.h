/**
 * @file gen_ini_file.h   General Ini File
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2014.07.23
 *
 *
 */

#ifndef __GEN_INI_FILE_H__
#define __GEN_INI_FILE_H__


#include <public/gen_int.h>
#include <public/gen_platform.h>

/** gen ini file handle */
typedef void*  GEN_INI_FILE_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not null: success
 * NULL: fail
 */
GEN_INI_FILE_H gen_ini_file_create();

/**  
 * @brief destroy
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_ini_file_destroy(GEN_INI_FILE_H h);

/**  
 * @brief init
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_ini_file_init(GEN_INI_FILE_H h);

/**  
 * @brief deinit
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_ini_file_deinit(GEN_INI_FILE_H h);

/**  
 * @brief set input file
 * @param [in] h, file handle
 * @param [in] p_path, input file path
 * @param [in] path_size, input file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_set_input_fileA(GEN_INI_FILE_H h, int8* p_path, int32 path_size);

/**  
 * @brief set output file
 * @param [in] h, file handle
 * @param [in] p_path, output file path
 * @param [in] path_size, output file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_set_output_fileA(GEN_INI_FILE_H h, int8* p_path, int32 path_size);


#if RUN_OS_WINDOWS
/**  
 * @brief set input file(unicode)
 * @param [in] h, file handle
 * @param [in] p_path, input file path
 * @param [in] path_size, input file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. path size count in bytes!
 */
int32 gen_ini_file_set_input_fileW(GEN_INI_FILE_H h, wchar_t* p_path, int32 path_size);

/**  
 * @brief set output file(unicode)
 * @param [in] h, file handle
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. path size count in bytes!
 */
int32 gen_ini_file_set_output_fileW(GEN_INI_FILE_H h, wchar_t* p_path, int32 path_size);

#endif

/**  
 * @brief get param
 * @param [in] h, file handle
 * @param [in] p_param, param name
 * @param [in] param_size, param name size
 * @param [in] p_value, output value buf
 * @param [in/out] p_value_size, output value buf size/actual used buf size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_get_param(GEN_INI_FILE_H h, int8* p_param, int32 param_size, int8* p_value, int32* p_value_size);

/**  
 * @brief get param pair
 * @param [in] h, file handle
 * @param [in] param_index, param index
 * @param [in] p_name, param name buf
 * @param [in/out] p_name_size, param name buf size/actual param name size
 * @param [in] p_value, value buf
 * @param [in/out] p_value_size, value buf size/actual value size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_get_param_pair(GEN_INI_FILE_H h, int32 param_index, int8* p_name, int32* p_name_size, int8* p_value, int32* p_value_size);

/**  
 * @brief set param
 * @param [in] h, file handle
 * @param [in] p_param, param name
 * @param [in] param_size, param name size
 * @param [in] p_value, value
 * @param [in] value_size, value size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_set_param(GEN_INI_FILE_H h, int8* p_param, int32 param_size, int8* p_value, int32 value_size);

/**  
 * @brief parse input file
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_parse(GEN_INI_FILE_H h);

/**  
 * @brief parse input data
 * @param [in] h, file handle
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_parse2(GEN_INI_FILE_H h, int8* p_data, int32 data_size);

/**  
 * @brief save to output file
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_ini_file_save(GEN_INI_FILE_H h);



#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define gen_ini_file_set_input_file                        gen_ini_file_set_input_fileW
#define gen_ini_file_set_output_file                       gen_ini_file_set_output_fileW

#else

#define gen_ini_file_set_input_file                        gen_ini_file_set_input_fileA
#define gen_ini_file_set_output_file                       gen_ini_file_set_output_fileA

#endif//unicode

#else

/** linux */
#define gen_ini_file_set_input_file                        gen_ini_file_set_input_fileA
#define gen_ini_file_set_output_file                       gen_ini_file_set_output_fileA

#endif//if RUN_OS_WINDOWS


#ifdef __cplusplus
}
#endif

#endif ///__GEN_INI_FILE_H__
