/**
 * @file gen_thread_public.h  Gen Thread Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.04.18
 *
 *
 */

#ifndef __GEN_THREAD_PUBLIC_H__
#define __GEN_THREAD_PUBLIC_H__

#include <public/gen_int.h>
#include <public/gen_platform.h>

#if RUN_OS_WINDOWS

#include <windows.h>

#else

#include <pthread.h>

#endif

/** thread */
typedef struct tag_gen_thread
{
	/** init */
	int32   b_init;
	/** open */
	int32   b_open;
	/** exit */
	int32   b_exit;

#if RUN_OS_WINDOWS

	/** thread handle */
	HANDLE  handle;
	/** thread id */
	DWORD   id;

#else

	/** thread id */
	pthread_t pid;

#endif

	/** user data */
	void*   p_user_data;

} GEN_THREAD, *PGEN_THREAD;

#endif ///__GEN_THREAD_PUBLIC_H__
