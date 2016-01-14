/**
 * @file gen_thread.h   General Thread
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.04.18
 *
 *
 */

#ifndef __GEN_THREAD_H__
#define __GEN_THREAD_H__

#include <public/gen_int.h>
#include "gen_thread_public.h"

#if RUN_OS_WINDOWS

/** thread proc */
typedef DWORD (WINAPI *gen_thread_proc)(void* p_param);

#else

/** thread proc */
typedef void* (*gen_thread_proc)(void* p_param);

#endif


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief setup
 * @param [in] p_thread, thread
 * @return
 * 0: success
 * other: fail
 */
int32 gen_thread_setup(GEN_THREAD* p_thread);

/**  
 * @brief cleanup
 * @param [in] p_thread, thread
 * @return
 * 0: success
 * other: fail
 */
int32 gen_thread_cleanup(GEN_THREAD* p_thread);

/**  
 * @brief init
 * @param [in] p_thread, thread
 * @return
 * 0: success
 * other: fail
 */
int32 gen_thread_init(GEN_THREAD* p_thread);

/**  
 * @brief deinit
 * @param [in] p_thread, thread
 * @return
 * 0: success
 * other: fail
 */
int32 gen_thread_deinit(GEN_THREAD* p_thread);

/**  
 * @brief open
 * @param [in] p_thread, thread
 * @param [in] p_proc, thread proc
 * @param [in] p_param, user custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. thread proc must be exit under condition of p_thread->b_exit = 1, else it would infinite deadloop!!!
 * 2. p_thread->p_user_data can be set to custom use
 */
int32 gen_thread_open(GEN_THREAD* p_thread, gen_thread_proc p_proc, void* p_param);

/**  
 * @brief close
 * @param [in] p_thread, thread
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. thread proc must be exit under condition of p_thread->b_exit = 1, else it would infinite deadloop!!!
 */
int32 gen_thread_close(GEN_THREAD* p_thread);


#ifdef __cplusplus
}
#endif

#endif ///__GEN_THREAD_H__
