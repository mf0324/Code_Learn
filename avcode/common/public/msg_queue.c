
#include <stdlib.h>
#include <string.h>

#ifdef WIN32

#include <windows.h>

#else

#include <unistd.h>
#include <pthread.h>

#endif


#include <public/thread_mutexer.h>
#include <public/gen_error.h>
#include <util/log_debug.h>
#include "msg_queue.h"

/** max queue size */
#define MSQ_MAX_QUEUE_SIZE                                 (1024)

/** msg queue */
typedef struct tag_msg_queue
{
	/** init */
	int32   b_init;

	/** max queue size */
	int32   max_queue_size;
	/** queue size */
	int32   queue_size;
	/** read pos */
	int32   read_pos;
	/** write pos */
	int32   write_pos;
	/** read flag */
	int32*  p_read_flag;
	/** msg deque */
	MSQ_MSG*  p_msg;

	/** mutex */
	THREAD_MUTEXER  mutex;

} MSG_QUEUE, *PMSG_QUEUE;


/////////////////////////////// Outter Interface //////////////////////////////
MSQ_H msq_create()
{
	MSG_QUEUE *p_msq = NULL;
	p_msq = (MSG_QUEUE*)malloc( sizeof(MSG_QUEUE) );

	if( p_msq == NULL )
	{
		return NULL;
	}

	memset(p_msq, 0, sizeof(MSG_QUEUE));

	return p_msq;
}

int32 msq_destroy(MSQ_H msq_h)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;
	
	if( p_msq == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_msq->b_init )
	{
		msq_deinit(p_msq);
	}

	free(p_msq);

	return GEN_S_OK;
}

int32 msq_init(MSQ_H msq_h)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;
	
	if( p_msq == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}
	
	if( p_msq->b_init )
	{
		return GEN_S_OK;
	}

	p_msq->p_msg = (MSQ_MSG*)malloc( sizeof(MSQ_MSG) * MSQ_MAX_QUEUE_SIZE );
	if( p_msq->p_msg == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_msq->max_queue_size = MSQ_MAX_QUEUE_SIZE;
	p_msq->queue_size = 0;
	p_msq->read_pos = 0;
	p_msq->write_pos = 0;

	thread_mutexer_init(&p_msq->mutex);

	p_msq->b_init = 1;

	return GEN_S_OK;
}

int32 msq_deinit(MSQ_H msq_h)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;
	
	if( p_msq == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_msq->p_msg )
	{
		free(p_msq->p_msg);
		p_msq->p_msg = NULL;
	}

	thread_mutexer_deinit(&p_msq->mutex);

	p_msq->b_init = 0;

	return GEN_S_OK;
}

int32 msq_reset(MSQ_H msq_h)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;

	if( p_msq == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_msq->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_msq->queue_size = 0;
	p_msq->read_pos = 0;
	p_msq->write_pos = 0;

	return GEN_S_OK;
}

int32 msq_set_max_size(MSQ_H msq_h, int32 max_size)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;
	
	if( p_msq == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}
	
	if( !p_msq->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_msq->max_queue_size == max_size )
	{
		p_msq->queue_size = 0;
		p_msq->read_pos = 0;
		p_msq->write_pos = 0;

		return GEN_S_OK;
	}

	if( p_msq->p_msg )
	{
		free(p_msq->p_msg);
		p_msq->p_msg = NULL;
	}

	p_msq->p_msg = (MSQ_MSG*)malloc( sizeof(MSQ_MSG) * max_size );
	if( p_msq->p_msg == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_msq->max_queue_size = max_size;
	p_msq->queue_size = 0;
	p_msq->read_pos = 0;
	p_msq->write_pos = 0;

	return GEN_S_OK;
}

int32 msq_send_msg(MSQ_H msq_h, MSQ_MSG *p_msg, int32 b_wait)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;

#ifdef WIN32

#else

	struct timespec sleep_time;

#endif

	int32 ret = 0;
	
	if( p_msq == NULL || p_msg == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	//log_debug(_T("[msg_queue::msq_send_msg] msg id = %u, param1 = 0x%x, param2 = 0x%x\n"), p_msg->id, p_msg->param1, p_msg->param2);
	
	if( !p_msq->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( b_wait )
	{
		/** block */
#ifdef WIN32

#else

		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 100 * 1000 * 1000;

#endif

		while( 1 )
		{
			thread_mutexer_lock(&p_msq->mutex);

			if( p_msq->queue_size < p_msq->max_queue_size )
			{
				p_msq->p_msg[p_msq->write_pos].id = p_msg->id;
				p_msq->p_msg[p_msq->write_pos].param1 = p_msg->param1;
				p_msq->p_msg[p_msq->write_pos].param2 = p_msg->param2;

				p_msq->queue_size++;
				p_msq->write_pos++;
				if( p_msq->write_pos >= p_msq->max_queue_size )
				{
					p_msq->write_pos = 0;
				}

				thread_mutexer_unlock(&p_msq->mutex);

				break;
			}

#ifdef WIN32

			thread_mutexer_unlock(&p_msq->mutex);
			Sleep(100);

#else

			thread_mutexer_unlock(&p_msq->mutex);
			nanosleep(&sleep_time, NULL);

#endif
		}

	}else
	{
		/** non block */
		thread_mutexer_lock(&p_msq->mutex);

		if( p_msq->queue_size < p_msq->max_queue_size )
		{
			p_msq->p_msg[p_msq->write_pos].id = p_msg->id;
			p_msq->p_msg[p_msq->write_pos].param1 = p_msg->param1;
			p_msq->p_msg[p_msq->write_pos].param2 = p_msg->param2;
			
			p_msq->queue_size++;
			p_msq->write_pos++;
			if( p_msq->write_pos >= p_msq->max_queue_size )
			{
				p_msq->write_pos = 0;
			}

		}else
		{
			ret = GEN_E_FAIL;
		}

		thread_mutexer_unlock(&p_msq->mutex);
	}

	return ret;
}

int32 msq_get_msg(MSQ_H msq_h, MSQ_MSG *p_msg, int32 b_wait)
{
	MSG_QUEUE *p_msq = (MSG_QUEUE *)msq_h;

#ifdef WIN32

#else

	struct timespec sleep_time;

#endif
	
	int32 ret = 0;
	
	if( p_msq == NULL || p_msg == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}
	
	if( !p_msq->b_init )
	{
		return GEN_E_NOT_INIT;
	}
	
	if( b_wait )
	{
		/** block */
#ifdef WIN32

#else

		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 100 * 1000 * 1000;

#endif
		
		while( 1 )
		{
			thread_mutexer_lock(&p_msq->mutex);

			if( p_msq->queue_size > 0 )
			{
				//log_debug(_T("[msg_queue::msq_get_msg] before queue size = %d, read pos = %d, write_pos = %d\n"), p_msq->queue_size, p_msq->read_pos, p_msq->write_pos);
				p_msg->id = p_msq->p_msg[p_msq->read_pos].id;
				p_msg->param1 = p_msq->p_msg[p_msq->read_pos].param1;
				p_msg->param2 = p_msq->p_msg[p_msq->read_pos].param2;

				p_msq->queue_size--;
				p_msq->read_pos++;
				if( p_msq->read_pos >= p_msq->max_queue_size )
				{
					p_msq->read_pos = 0;
				}

				thread_mutexer_unlock(&p_msq->mutex);
				break;
			}

#ifdef WIN32

			thread_mutexer_unlock(&p_msq->mutex);
			Sleep(100);

#else

			thread_mutexer_unlock(&p_msq->mutex);
			nanosleep(&sleep_time, NULL);

#endif
		}
		
	}else
	{
		/** non block */
		thread_mutexer_lock(&p_msq->mutex);

		if( p_msq->queue_size > 0 )
		{
			//log_debug(_T("[msg_queue::msq_get_msg] before nonblock queue size = %d, read pos = %d, write_pos = %d\n"), p_msq->queue_size, p_msq->read_pos, p_msq->write_pos);

			p_msg->id = p_msq->p_msg[p_msq->read_pos].id;
			p_msg->param1 = p_msq->p_msg[p_msq->read_pos].param1;
			p_msg->param2 = p_msq->p_msg[p_msq->read_pos].param2;

			p_msq->queue_size--;
			p_msq->read_pos++;
			if( p_msq->read_pos >= p_msq->max_queue_size )
			{
				p_msq->read_pos = 0;
			}
			
		}else
		{
			ret = GEN_E_FAIL;
		}

		thread_mutexer_unlock(&p_msq->mutex);
	}

	return ret;
}
