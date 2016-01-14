
#include <string.h>
#include <public/gen_error.h>

#include "gen_thread.h"


/////////////////////////////// Outter Interface //////////////////////////////
int32 gen_thread_setup(GEN_THREAD* p_thread)
{
	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_thread, 0, sizeof(*p_thread));

	return GEN_S_OK;
}

int32 gen_thread_cleanup(GEN_THREAD* p_thread)
{
	int32   ret;

	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_thread->b_init )
	{
		ret = gen_thread_deinit(p_thread);
	}

	return GEN_S_OK;
}

int32 gen_thread_init(GEN_THREAD* p_thread)
{
	//int32   ret;

	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_thread->b_init )
	{
		return GEN_S_OK;
	}

#if RUN_OS_WINDOWS

	p_thread->handle = NULL;

#else

	p_thread->pid = 0x0;

#endif

	p_thread->b_open = 0;
	p_thread->b_init = 1;

	return GEN_S_OK;
}

int32 gen_thread_deinit(GEN_THREAD* p_thread)
{
	//int32   ret;

	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_thread->b_open )
	{
		gen_thread_close(p_thread);
	}

	p_thread->b_init = 0;

	return GEN_S_OK;
}

int32 gen_thread_open(GEN_THREAD* p_thread, gen_thread_proc p_proc, void* p_param)
{
	//int32   ret;

	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_thread->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_thread->b_open )
	{
		return GEN_S_OK;
	}

	p_thread->b_exit = 0;

#if RUN_OS_WINDOWS

	p_thread->handle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)p_proc,
		p_param,
		0,
		&p_thread->id);
	if( p_thread->handle == NULL )
	{
		//log_error(_T("[solid_player::solid_player_init] thread create error = %d\n"), GetLastError());

		return GEN_E_FAIL;
	}

#else

	ret = pthread_create(&p_thread->pid, NULL, p_proc, p_param);
	if( ret )
	{
		return GEN_E_FAIL;
	}

#endif

	p_thread->b_open = 1;

	return GEN_S_OK;
}

int32 gen_thread_close(GEN_THREAD* p_thread)
{
	//int32   ret;

	if( p_thread == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_thread->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_thread->b_open )
	{
		p_thread->b_exit = 1;

#if RUN_OS_WINDOWS

		WaitForSingleObject(p_thread->handle, INFINITE);
		CloseHandle(p_thread->handle);
		p_thread->handle = NULL;

#else

		ret = pthread_join(p_thread->pid, NULL);

#endif

		p_thread->b_open = 0;
	}

	return GEN_S_OK;
}
