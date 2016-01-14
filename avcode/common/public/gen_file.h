/**
 * @file gen_file.h   Gen File
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.08
 *
 *
 */

#ifndef __GEN_FILE_H__
#define __GEN_FILE_H__


#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <public/gen_char_public.h>

#include "gen_file_public.h"

/** gen file handle */
typedef void*  GEN_FILE_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief malloc
 * @return
 * not null: success
 * NULL: fail
 */
GEN_FILE_H gen_file_malloc();

/**  
 * @brief free
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_file_free(GEN_FILE_H h);

/**  
 * @brief init
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_file_init(GEN_FILE_H h);

/**  
 * @brief deinit
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 */
int32 gen_file_deinit(GEN_FILE_H h);

/**  
 * @brief set param
 * @param [in] h, file handle
 * @param [in] param, param
 * @param [in] p_value, value buf
 * @param [in] value_size, value buf size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_set_param(GEN_FILE_H h, uint32 param, uint8* p_value, int32 value_size);

/**  
 * @brief is open
 * @param [in] h, file handle
 * @param [out] p_b_open, open or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on success return, *p_b_open means open or not
 */
int32 gen_file_is_open(GEN_FILE_H h, int32* p_b_open);

/**  
 * @brief open
 * @param [in] h, file handle
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_openA(GEN_FILE_H h, int8* p_path, int32 path_size);

/**  
 * @brief open(with param)
 * @param [in] h, file handle
 * @param [in] p_param, open param
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_open2A(GEN_FILE_H h, GEN_FILE_OPEN_PARAM* p_param, int8* p_path, int32 path_size);

/**  
 * @brief delete
 * @param [in] h, file handle
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_deleteA(GEN_FILE_H h, int8* p_path, int32 path_size);

/**  
 * @brief check exist
 * @param [in] h, file handle
 * @param [out] p_b_exist, file exist or not
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_is_existA(GEN_FILE_H h, int32* p_b_exist, int8* p_path, int32 path_size);

#if RUN_OS_WINDOWS
/**  
 * @brief open(unicode)
 * @param [in] h, file handle
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_openW(GEN_FILE_H h, wchar_t* p_path, int32 path_size);

/**  
 * @brief open(with param)
 * @param [in] h, file handle
 * @param [in] p_param, open param
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_open2W(GEN_FILE_H h, GEN_FILE_OPEN_PARAM* p_param, wchar_t* p_path, int32 path_size);

/**  
 * @brief delete(unicode)
 * @param [in] h, file handle
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_deleteW(GEN_FILE_H h, wchar_t* p_path, int32 path_size);

/**  
 * @brief check exist(unicode)
 * @param [in] h, file handle
 * @param [out] p_b_exist, file exist or not
 * @param [in] p_path, file path
 * @param [in] path_size, file path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_is_existW(GEN_FILE_H h, int32* p_b_exist, wchar_t* p_path, int32 path_size);

#endif

/**  
 * @brief close
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_close(GEN_FILE_H h);

/**  
 * @brief get file size
 * @param [in] h, file handle
 * @param [out] p_file_size, file size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. file must be open first!
 */
int32 gen_file_get_size(GEN_FILE_H h, int64* p_file_size);

/**  
 * @brief seek
 * @param [in] h, file handle
 * @param [in] method, seek method
 * @param [in] pos, seek pos
 * @param [out] p_current_pos, current pos
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_seek(GEN_FILE_H h, int32 method, int32 pos, uint32* p_current_pos);

/**  
 * @brief seek64
 * @param [in] h, file handle
 * @param [in] method, seek method
 * @param [in] pos, seek pos
 * @param [out] p_current_pos, current pos
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_seek64(GEN_FILE_H h, int32 method, int64 pos, int64* p_current_pos);

/**  
 * @brief set end of file
 * @param [in] h, file handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_set_eof(GEN_FILE_H h);

/**  
 * @brief read
 * @param [in] h, file handle
 * @param [out] p_buf, buf
 * @param [in] read_size, read_size
 * @param [out] p_actual_size, actual read size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_read(GEN_FILE_H h, uint8* p_buf, uint32 read_size, uint32* p_actual_size);

/**  
 * @brief read(64 bit)
 * @param [in] h, file handle
 * @param [out] p_buf, buf
 * @param [in] read_size, read_size
 * @param [out] p_actual_size, actual read size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_read64(GEN_FILE_H h, uint8* p_buf, int64 read_size, int64* p_actual_size);

/**  
 * @brief write
 * @param [in] h, file handle
 * @param [in] p_buf, buf
 * @param [in] write_size, write_size
 * @param [out] p_actual_size, actual read size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_write(GEN_FILE_H h, uint8* p_buf, uint32 write_size, uint32* p_actual_size);

/**  
 * @brief write(64 bit)
 * @param [in] h, file handle
 * @param [in] p_buf, buf
 * @param [in] write_size, write_size
 * @param [out] p_actual_size, actual read size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_file_write64(GEN_FILE_H h, uint8* p_buf, int64 write_size, int64* p_actual_size);


#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define gen_file_open                                      gen_file_openW
#define gen_file_open2                                     gen_file_open2W
#define gen_file_delete                                    gen_file_deleteW
#define gen_file_is_exist                                  gen_file_is_existW

#else

#define gen_file_open                                      gen_file_openA
#define gen_file_open2                                     gen_file_open2A
#define gen_file_delete                                    gen_file_deleteA
#define gen_file_is_exist                                  gen_file_is_existA

#endif//unicode

#else

/** linux */
#define gen_file_open                                      gen_file_openA
#define gen_file_open2                                     gen_file_open2A
#define gen_file_delete                                    gen_file_deleteA
#define gen_file_is_exist                                  gen_file_is_existA

#endif//if RUN_OS_WINDOWS


#ifdef __cplusplus
}
#endif

#endif ///__GEN_FILE_H__
