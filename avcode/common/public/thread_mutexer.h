/**
 * @file thread_mutexer.h  Thread Mutexer
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.07.11
 *
 *
 */ 

#ifndef __THREAD_MUTEXER_H__
#define __THREAD_MUTEXER_H__

#include <public/gen_int.h>
#include <public/gen_def.h>
#include <public/gen_platform.h>
#include <public/gen_error.h>

#if RUN_OS_WINDOWS

#include <windows.h>

#else

#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#endif


/** thread mutexer */
typedef struct tag_thread_mutexer
{
	/** init */
	int32  b_init;

#if RUN_OS_WINDOWS

	/** critical section */
	CRITICAL_SECTION  cs;

#else

	/** mutex attr */
	pthread_mutexattr_t mutex_attr;
	/** mutex */
	pthread_mutex_t     mutex;

#endif

} THREAD_MUTEXER, *PTHREAD_MUTEXER;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief setup
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done under exclusive context!
 */
static inline int32 thread_mutexer_setup(THREAD_MUTEXER* p_mutexer);

/**  
 * @brief cleanup
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. it must be done under exclusive context!
 */
static inline int32 thread_mutexer_cleanup(THREAD_MUTEXER* p_mutexer);

/**  
 * @brief init
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 * @remark
 * @remark
 * 1. it must be done under exclusive context!
 * 2. setup must be done at least once before init!
 */
static inline int32 thread_mutexer_init(THREAD_MUTEXER* p_mutexer);

/**  
 * @brief deinit
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 * @remark
 * @remark
 * 1. it must be done under exclusive context!
 * 2. setup must be done at least once before init!
 */
static inline int32 thread_mutexer_deinit(THREAD_MUTEXER* p_mutexer);

/**  
 * @brief lock
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 */
static inline int32 thread_mutexer_lock(THREAD_MUTEXER* p_mutexer);

/**  
 * @brief unlock
 * @param [in] p_mutexer, mutexer
 * @return
 * 0: success
 * other: fail
 */
static inline int32 thread_mutexer_unlock(THREAD_MUTEXER* p_mutexer);


static inline int32 thread_mutexer_setup(THREAD_MUTEXER* p_mutexer)
{
	memset(p_mutexer, 0, sizeof(THREAD_MUTEXER));

	return GEN_S_OK;
}

static inline int32 thread_mutexer_cleanup(THREAD_MUTEXER* p_mutexer)
{
	return thread_mutexer_deinit(p_mutexer);
}

static inline int32 thread_mutexer_init(THREAD_MUTEXER* p_mutexer)
{
	if( p_mutexer->b_init )
	{
		return GEN_S_OK;
	}

#if RUN_OS_WINDOWS

	InitializeCriticalSection(&p_mutexer->cs);

#else

	pthread_mutexattr_init(&p_mutexer->mutex_attr);
	pthread_mutexattr_settype(&p_mutexer->mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&p_mutexer->mutex, &p_mutexer->mutex_attr);

#endif

	p_mutexer->b_init = 1;

	return GEN_S_OK;
}

static inline int32 thread_mutexer_deinit(THREAD_MUTEXER* p_mutexer)
{
	if( p_mutexer->b_init )
	{
#if RUN_OS_WINDOWS

		DeleteCriticalSection(&p_mutexer->cs);

#else

		pthread_mutex_destroy(&p_mutexer->mutex);
		pthread_mutexattr_destroy(&p_mutexer->mutex_attr);

#endif
		p_mutexer->b_init = 0;
	}

	return GEN_S_OK;
}

static inline int32 thread_mutexer_lock(THREAD_MUTEXER* p_mutexer)
{
	if( !p_mutexer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	EnterCriticalSection(&p_mutexer->cs);

#else

	pthread_mutex_lock(&p_mutexer->mutex);

#endif

	return GEN_S_OK;
}

static inline int32 thread_mutexer_unlock(THREAD_MUTEXER* p_mutexer)
{
	if( !p_mutexer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if RUN_OS_WINDOWS

	LeaveCriticalSection(&p_mutexer->cs);

#else

	pthread_mutex_unlock(&p_mutexer->mutex);

#endif

	return GEN_S_OK;
}

#ifdef __cplusplus
}
#endif

#endif ///__THREAD_MUTEXER_H__
