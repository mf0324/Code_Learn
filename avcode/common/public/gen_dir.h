/**
 * @file gen_dir.h   General Directory
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.10
 *
 *
 */

#ifndef __GEN_DIR_H__
#define __GEN_DIR_H__


#include <public/gen_int.h>
#include <public/gen_platform.h>
#include <public/gen_char_public.h>



#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create dir
 * @param [in] p_path, path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_dir_createA(int8* p_path, int32 path_size);

/**  
 * @brief delete dir
 * @param [in] p_path, path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 gen_dir_deleteA(int8* p_path, int32 path_size);

/**  
 * @brief remove all files under dir
 * @param [in] p_path, dir path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. remove all normal files under specify dir 
 */
int32 get_dir_remove_all_fileA(int8* p_path, int32 path_size);


#if RUN_OS_WINDOWS
/**  
 * @brief create dir(unicode)
 * @param [in] p_path, path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. NOTICE: the path size count in bytes, not in wchar_t!!!
 * 2. dir should be absolute path
 */
int32 gen_dir_createW(wchar_t* p_path, int32 path_size);

/**  
 * @brief delete dir(unicode)
 * @param [in] p_path, path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. NOTICE: the path size count in bytes, not in wchar_t!!!
 * 2. dir should be absolute path
 */
int32 gen_dir_deleteW(wchar_t* p_path, int32 path_size);

/**  
 * @brief remove all files under dir
 * @param [in] p_path, dir path
 * @param [in] path_size, path size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. remove all normal files under specify dir 
 * 2. path_size cout in byte!
 */
int32 get_dir_remove_all_fileW(wchar_t* p_path, int32 path_size);

#endif


#if RUN_OS_WINDOWS

/** windows */
#ifdef UNICODE

#define gen_dir_create                                     gen_dir_createW
#define gen_dir_delete                                     gen_dir_deleteW
#define get_dir_remove_all_file                            get_dir_remove_all_fileW

#else

#define gen_dir_create                                     gen_dir_createA
#define gen_dir_delete                                     gen_dir_deleteA
#define get_dir_remove_all_file                            get_dir_remove_all_fileA

#endif//unicode

#else

/** linux */
#define gen_dir_create                                     gen_dir_createA
#define gen_dir_delete                                     gen_dir_deleteA
#define get_dir_remove_all_file                            get_dir_remove_all_fileA

#endif//if RUN_OS_WINDOWS


#ifdef __cplusplus
}
#endif

#endif ///__GEN_DIR_H__
