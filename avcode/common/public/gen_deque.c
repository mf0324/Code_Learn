
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>

#include "gen_deque.h"

/** index begin */
#define GEN_DEQUE_INDEX_BEGIN                              (0)
/** index end */
#define GEN_DEQUE_INDEX_END                                (p_deque->deque_size - 1)
/** array index end */
#define GEN_DEQUE_ARRAY_INDEX_END                          (p_deque->array_valid_size)


/** array min size */
#define GEN_DEQUE_ARRAY_MIN_SIZE                           (1024)
/** array max size */
#define GEN_DEQUE_ARRAY_MAX_SIZE                           (1024)
/** array scale factor */
#define GEN_DEQUE_ARRAY_SCALE_FACTOR                       (2)

/////////////////////////////////// allocator /////////////////////////////////
#if !RUN_OS_64
static void* gen_deque_inner_malloc(uint32 size, void* p_param)
#else
static void* gen_deque_inner_malloc(uint64 size, void* p_param)
#endif
{
	return malloc(size);
}

static int32 gen_deque_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/////////////////////////////// Outter Interface //////////////////////////////
GEN_DEQUE* gen_deque_create(GEN_ALLOCATOR* p_allocator)
{
	GEN_DEQUE *p_deque = NULL;

	if( p_allocator )
	{
		p_deque = (GEN_DEQUE *)p_allocator->allocate(sizeof(GEN_DEQUE), p_allocator->p_param);

	}else
	{
		p_deque = (GEN_DEQUE *)malloc( sizeof(GEN_DEQUE) );
	}

	if( p_deque == NULL )
	{
		return NULL;
	}

	memset(p_deque, 0, sizeof(GEN_DEQUE));

	if( p_allocator )
	{
		memcpy(&p_deque->allocator, p_allocator, sizeof(p_deque->allocator));

	}else
	{
		p_deque->allocator.allocate = &gen_deque_inner_malloc;
		p_deque->allocator.deallocate = &gen_deque_inner_free;
		p_deque->allocator.p_param = p_deque;
	}

	return p_deque;
}


int32 gen_deque_destroy(GEN_DEQUE *p_deque)
{
	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_deque->b_init )
	{
		gen_deque_deinit(p_deque);
	}

	ret = p_deque->allocator.deallocate(p_deque, p_deque->allocator.p_param);

	return GEN_S_OK;
}

int32 gen_deque_setup(GEN_DEQUE *p_deque, GEN_ALLOCATOR* p_allocator)
{
	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_deque, 0, sizeof(GEN_DEQUE));

	if( p_allocator )
	{
		memcpy(&p_deque->allocator, p_allocator, sizeof(p_deque->allocator));

	}else
	{
		p_deque->allocator.allocate = &gen_deque_inner_malloc;
		p_deque->allocator.deallocate = &gen_deque_inner_free;
		p_deque->allocator.p_param = p_deque;
	}

	return GEN_S_OK;
}

int32 gen_deque_cleanup(GEN_DEQUE *p_deque)
{
	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_deque->b_init )
	{
		gen_deque_deinit(p_deque);
	}

	return GEN_S_OK;
}

int32 gen_deque_init(GEN_DEQUE *p_deque)
{
	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_deque->b_init )
	{
		return GEN_S_OK;
	}

	memset(&p_deque->callback, 0, sizeof(p_deque->callback));

	p_deque->b_alloc_mem = 0;
	p_deque->deque_size = 0;
	p_deque->array_min_size = GEN_DEQUE_ARRAY_MIN_SIZE;
	p_deque->array_max_size = GEN_DEQUE_ARRAY_MAX_SIZE;
	p_deque->p_array = NULL;

	p_deque->b_init = 1;

	return GEN_S_OK;
}

int32 gen_deque_deinit(GEN_DEQUE *p_deque)
{
	//int32 ret;

	if( p_deque == NULL )
	{
		return -1;
	}

	if( p_deque->deque_size > 0 )
	{
		gen_deque_clear(p_deque);
	}

	gen_deque_free_mem(p_deque);

	p_deque->b_init = 0;

	return GEN_S_OK;
}

int32 gen_deque_clear(GEN_DEQUE *p_deque)
{
	int32  pos;
	int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	while( p_deque->deque_size > 0 )
	{
		pos = p_deque->head_pos + 1;
		if( pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			pos = 0;
		}

		if( p_deque->callback.delete_node )
		{
			ret = p_deque->callback.delete_node(p_deque, p_deque->p_array + pos, p_deque->callback.p_custom_param);
		}

		p_deque->head_pos++;
		if( p_deque->head_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			p_deque->head_pos = 0;
		}

		p_deque->deque_size--;
	}

	p_deque->deque_size = 0;
	p_deque->head_pos = 0;
	p_deque->rear_pos = 1;

	return GEN_S_OK;
}

int32 gen_deque_set_callback(GEN_DEQUE* p_deque, GEN_DEQUE_CALLBACK* p_callback)
{
	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_deque->callback = *p_callback;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_set_array_param(GEN_DEQUE *p_deque, int32 min_size, int32 max_size)
#else
int32 gen_deque_set_array_param(GEN_DEQUE *p_deque, int64 min_size, int64 max_size)
#endif
{
	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( min_size < 1 || max_size < 1 || max_size < min_size )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	p_deque->array_min_size = min_size;
	p_deque->array_max_size = max_size;

	return GEN_S_OK;
}

int32 gen_deque_get_array_size(GEN_DEQUE *p_deque, int32 *p_size)
{
	if( p_deque == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_deque->b_alloc_mem )
	{
		*p_size = p_deque->array_valid_size;

	}else
	{
		*p_size = 0;
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_resize_array(GEN_DEQUE *p_deque, int32 new_size)
#else
int32 gen_deque_resize_array(GEN_DEQUE *p_deque, int64 new_size)
#endif
{
	DEQUE_NODE *p_temp = NULL;

#if !RUN_OS_64

	int32   use_size;
	uint32  mem_size;
	int32   head_size;
	int32   rear_size;

#else

	int64   use_size;
	uint64  mem_size;
	int64   head_size;
	int64   rear_size;

#endif

	int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( p_deque->deque_size > p_deque->array_min_size )
	{
		use_size = p_deque->deque_size;

	}else
	{
		use_size = p_deque->array_min_size;
	}

	if( new_size < use_size || new_size > p_deque->array_max_size )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( new_size == p_deque->array_valid_size )
	{
		/** do not need adjust */
		return GEN_S_OK;
	}

	mem_size = sizeof(DEQUE_NODE) * (new_size + 1);
	p_temp = (DEQUE_NODE*)p_deque->allocator.allocate(mem_size, p_deque->allocator.p_param);

	if( p_temp == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	if( p_deque->head_pos < p_deque->rear_pos )
	{
		/** direct copy */
		if( p_deque->deque_size > 0 )
		{
			memcpy(p_temp + 1, p_deque->p_array + p_deque->head_pos + 1, sizeof(DEQUE_NODE) * p_deque->deque_size);
		}

	}else
	{
		/** copy first part */
		head_size = GEN_DEQUE_ARRAY_INDEX_END - p_deque->head_pos;
		if( head_size > 0 )
		{
			memcpy(p_temp + 1, p_deque->p_array + p_deque->head_pos + 1, sizeof(DEQUE_NODE) * head_size);
		}

		/** copy last part */
		rear_size = p_deque->rear_pos;
		if( rear_size > 0 )
		{
			memcpy(p_temp + 1 + head_size, p_deque->p_array, sizeof(DEQUE_NODE) * rear_size);
		}
	}

	p_deque->array_total_size = new_size + 1;
	p_deque->array_valid_size = new_size;
	p_deque->head_pos = 0;
	p_deque->rear_pos = p_deque->deque_size + 1;

	if( p_deque->rear_pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		p_deque->rear_pos = 0;
	}

	ret = p_deque->allocator.deallocate(p_deque->p_array, p_deque->allocator.p_param);

	p_deque->p_array = p_temp;

	return GEN_S_OK;
}

int32 gen_deque_alloc_mem(GEN_DEQUE *p_deque)
{
#if !RUN_OS_64
	uint32  mem_size;
#else
	uint64  mem_size;
#endif

	//int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_deque->b_alloc_mem )
	{
		return GEN_S_OK;
	}

	mem_size = sizeof(DEQUE_NODE) * (p_deque->array_min_size + 1);
	p_deque->p_array = (DEQUE_NODE*)p_deque->allocator.allocate(mem_size, p_deque->allocator.p_param);

	if( p_deque->p_array == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_deque->deque_size = 0;
	p_deque->array_total_size = p_deque->array_min_size + 1;
	p_deque->array_valid_size = p_deque->array_min_size;
	p_deque->head_pos = 0;
	p_deque->rear_pos = 1;

	p_deque->b_alloc_mem = 1;

	return GEN_S_OK;
}

int32 gen_deque_free_mem(GEN_DEQUE *p_deque)
{
	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_deque->p_array )
	{
		ret = p_deque->allocator.deallocate(p_deque->p_array, p_deque->allocator.p_param);
		p_deque->p_array = NULL;
	}

	p_deque->deque_size = 0;
	p_deque->b_alloc_mem = 0;

	return GEN_S_OK;
}

int32 gen_deque_get_size(GEN_DEQUE *p_deque, int32 *p_size)
{
	if( p_deque == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*p_size = p_deque->deque_size;

	return GEN_S_OK;
}

int32 gen_deque_get_front_data(GEN_DEQUE *p_deque, void **pp_data)
{
	int32 pos;

	if( p_deque == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		*pp_data = NULL;
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		*pp_data = NULL;
		return GEN_E_INVALID_STATUS;
	}

	if( p_deque->deque_size < 1 )
	{
		*pp_data = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	pos = p_deque->head_pos + 1;
	if( pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		pos = 0;
	}

	*pp_data = p_deque->p_array[pos].p_data;

	return GEN_S_OK;
}

int32 gen_deque_push_front_data(GEN_DEQUE *p_deque, void *p_data)
{
	DEQUE_NODE *p_temp = NULL;

	int32  new_array_size;
	uint32 mem_size;
	int32  head_size;
	int32  rear_size;

	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		ret = gen_deque_alloc_mem(p_deque);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	if( p_deque->deque_size < p_deque->array_valid_size )
	{
		/** deque not full */
		p_deque->p_array[p_deque->head_pos].p_data = p_data;
		p_deque->head_pos--;
		if( p_deque->head_pos < 0 )
		{
			p_deque->head_pos = GEN_DEQUE_ARRAY_INDEX_END;
		}

	}else
	{
		/** deque is full */
		if( p_deque->array_min_size == p_deque->array_max_size )
		{
			/** not need auto increase */
			return GEN_E_INVALID_STATUS;
		}

		new_array_size = p_deque->array_valid_size * GEN_DEQUE_ARRAY_SCALE_FACTOR;
		if( new_array_size > p_deque->array_max_size )
		{
			new_array_size = p_deque->array_max_size;
		}
		//new_array_size++;

		mem_size = sizeof(DEQUE_NODE) * (new_array_size + 1);
		p_temp = (DEQUE_NODE*)p_deque->allocator.allocate(mem_size, p_deque->allocator.p_param);

		if( p_temp == NULL )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}

		/** copy first part */
		head_size = GEN_DEQUE_ARRAY_INDEX_END - p_deque->head_pos;
		if( head_size > 0 )
		{
			memcpy(p_temp + 1, p_deque->p_array + p_deque->head_pos + 1, sizeof(DEQUE_NODE) * head_size);
		}

		/** copy last part */
		rear_size = p_deque->rear_pos;
		if( rear_size > 0 )
		{
			memcpy(p_temp + 1 + head_size, p_deque->p_array, sizeof(DEQUE_NODE) * rear_size);
		}

		p_temp[0].p_data = p_data;
		p_deque->array_total_size = new_array_size + 1;
		p_deque->array_valid_size = new_array_size;
		p_deque->head_pos = GEN_DEQUE_ARRAY_INDEX_END;
		p_deque->rear_pos = p_deque->deque_size + 1;

		ret = p_deque->allocator.deallocate(p_deque->p_array, p_deque->allocator.p_param);

		p_deque->p_array = p_temp;
	}

	p_deque->deque_size++;

	return GEN_S_OK;
}

int32 gen_deque_pop_front(GEN_DEQUE *p_deque)
{
	int32  pos;
	int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( p_deque->deque_size < 1 )
	{
		return GEN_S_OK;
	}

	pos = p_deque->head_pos + 1;
	if( pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		pos = 0;
	}

	if( p_deque->callback.delete_node )
	{
		ret = p_deque->callback.delete_node(p_deque, p_deque->p_array + pos, p_deque->callback.p_custom_param);
	}

	p_deque->head_pos++;
	if( p_deque->head_pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		p_deque->head_pos = 0;
	}

	p_deque->deque_size--;

	return GEN_S_OK;
}

int32 gen_deque_get_back_data(GEN_DEQUE *p_deque, void **pp_data)
{
	int32 pos;

	if( p_deque == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		*pp_data = NULL;
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		*pp_data = NULL;
		return GEN_E_INVALID_STATUS;
	}

	if( p_deque->deque_size < 1 )
	{
		*pp_data = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	pos = p_deque->rear_pos - 1;
	if( pos < 0 )
	{
		pos = GEN_DEQUE_ARRAY_INDEX_END;
	}

	*pp_data = p_deque->p_array[pos].p_data;

	return GEN_S_OK;
}

int32 gen_deque_push_back_data(GEN_DEQUE *p_deque, void *p_data)
{
	DEQUE_NODE *p_temp = NULL;

	int32  new_array_size;
	uint32 mem_size;
	int32  head_size;
	int32  rear_size;

	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		ret = gen_deque_alloc_mem(p_deque);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	if( p_deque->deque_size < p_deque->array_valid_size )
	{
		/** deque not full */
		p_deque->p_array[p_deque->rear_pos].p_data = p_data;
		p_deque->rear_pos++;
		if( p_deque->rear_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			p_deque->rear_pos = 0;
		}

	}else
	{
		/** deque is full */
		if( p_deque->array_min_size == p_deque->array_max_size )
		{
			/** need not auto increase */
			return GEN_E_INVALID_STATUS;
		}

		new_array_size = p_deque->array_valid_size * GEN_DEQUE_ARRAY_SCALE_FACTOR;
		if( new_array_size > p_deque->array_max_size )
		{
			new_array_size = p_deque->array_max_size;
		}

		mem_size = sizeof(DEQUE_NODE) * (new_array_size + 1);
		p_temp = (DEQUE_NODE*)p_deque->allocator.allocate(mem_size, p_deque->allocator.p_param);

		if( p_temp == NULL )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}

		/** copy first part */
		head_size = GEN_DEQUE_ARRAY_INDEX_END - p_deque->head_pos;
		if( head_size > 0 )
		{
			memcpy(p_temp + 1, p_deque->p_array + p_deque->head_pos + 1, sizeof(DEQUE_NODE) * head_size);
		}

		/** copy last part */
		rear_size = p_deque->rear_pos;
		if( rear_size > 0 )
		{
			memcpy(p_temp + 1 + head_size, p_deque->p_array, sizeof(DEQUE_NODE) * rear_size);
		}

		p_temp[p_deque->deque_size + 1].p_data = p_data;
		p_deque->array_total_size = new_array_size + 1;
		p_deque->array_valid_size = new_array_size;
		p_deque->head_pos = 0;
		p_deque->rear_pos = p_deque->deque_size + 2;
		if( p_deque->rear_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			p_deque->rear_pos = 0;
		}

		ret = p_deque->allocator.deallocate(p_deque->p_array, p_deque->allocator.p_param);

		p_deque->p_array = p_temp;
	}

	p_deque->deque_size++;

	return GEN_S_OK;
}

int32 gen_deque_pop_back(GEN_DEQUE *p_deque)
{
	int32  pos;
	int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( p_deque->deque_size < 1 )
	{
		return GEN_S_OK;
	}

	pos = p_deque->rear_pos - 1;
	if( pos < 0 )
	{
		pos = GEN_DEQUE_ARRAY_INDEX_END;
	}

	if( p_deque->callback.delete_node )
	{
		ret = p_deque->callback.delete_node(p_deque, p_deque->p_array + pos, p_deque->callback.p_custom_param);
	}

	p_deque->rear_pos--;
	if( p_deque->rear_pos < 0 )
	{
		p_deque->rear_pos = GEN_DEQUE_ARRAY_INDEX_END;
	}

	p_deque->deque_size--;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_get_data(GEN_DEQUE *p_deque, int32 index, void **pp_data)
#else
int32 gen_deque_get_data(GEN_DEQUE *p_deque, int64 index, void **pp_data)
#endif
{
#if !RUN_OS_64
	int32  pos;
#else
	int64  pos;
#endif

	if( p_deque == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*pp_data = NULL;

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( index < GEN_DEQUE_INDEX_BEGIN || index > GEN_DEQUE_INDEX_END )
	{
		return GEN_E_INVALID_PARAM;
	}

	pos = p_deque->head_pos + 1 + index;
	if( pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		pos -= p_deque->array_total_size;
	}

	*pp_data = p_deque->p_array[pos].p_data;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_set_data(GEN_DEQUE *p_deque, int32 index, void *p_data)
#else
int32 gen_deque_set_data(GEN_DEQUE *p_deque, int64 index, void *p_data)
#endif
{
#if !RUN_OS_64
	int32  pos;
#else
	int64  pos;
#endif

	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		ret = gen_deque_alloc_mem(p_deque);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	if( index < GEN_DEQUE_INDEX_BEGIN || index > GEN_DEQUE_INDEX_END )
	{
		return GEN_E_INVALID_PARAM;
	}

	pos = p_deque->head_pos + 1 + index;
	if( pos > GEN_DEQUE_ARRAY_INDEX_END )
	{
		pos -= p_deque->array_total_size;
	}

	p_deque->p_array[pos].p_data = p_data;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_insert_data_at(GEN_DEQUE *p_deque, int32 index, void *p_data)
#else
int32 gen_deque_insert_data_at(GEN_DEQUE *p_deque, int64 index, void *p_data)
#endif
{
#if !RUN_OS_64

	int32  pos;
	int32  prev_pos;
	int32  next_pos;
	int32  i;

#else

	int64  pos;
	int64  prev_pos;
	int64  next_pos;
	int64  i;

#endif

	int32 ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		ret = gen_deque_alloc_mem(p_deque);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}
	}

	if( index < GEN_DEQUE_INDEX_BEGIN || index > (GEN_DEQUE_INDEX_END + 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_DEQUE_INDEX_BEGIN )
	{
		return gen_deque_push_front_data(p_deque, p_data);
	}

	if( index == (GEN_DEQUE_INDEX_END + 1) )
	{
		return gen_deque_push_back_data(p_deque, p_data);
	}

	if( index < p_deque->deque_size/2 )
	{
		/** move first part */
		i = GEN_DEQUE_INDEX_BEGIN;
		pos = p_deque->head_pos;
		while( i < index )
		{
			pos++;
			if( pos > GEN_DEQUE_ARRAY_INDEX_END )
			{
				pos = 0;
				prev_pos = GEN_DEQUE_ARRAY_INDEX_END;

			}else
			{
				prev_pos = pos - 1;
			}

			p_deque->p_array[prev_pos].p_data = p_deque->p_array[pos].p_data;

			i++;
		}

		p_deque->p_array[pos].p_data = p_data;

		p_deque->head_pos--;
		if( p_deque->head_pos < 0 )
		{
			p_deque->head_pos = GEN_DEQUE_ARRAY_INDEX_END;
		}

	}else
	{
		/** move last part */
		i = GEN_DEQUE_INDEX_END + 1;
		pos = p_deque->rear_pos;
		while( i > index )
		{
			pos--;
			if( pos < 0 )
			{
				pos = GEN_DEQUE_ARRAY_INDEX_END;
				next_pos = 0;

			}else
			{
				next_pos = pos + 1;
			}

			p_deque->p_array[next_pos].p_data = p_deque->p_array[pos].p_data;

			i--;
		}

		p_deque->p_array[pos].p_data = p_data;

		p_deque->rear_pos++;
		if( p_deque->rear_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			p_deque->rear_pos = 0;
		}
	}

	p_deque->deque_size++;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_deque_delete_data_at(GEN_DEQUE *p_deque, int32 index)
#else
int32 gen_deque_delete_data_at(GEN_DEQUE *p_deque, int64 index)
#endif
{
#if !RUN_OS_64

	int32  pos;
	int32  prev_pos;
	int32  next_pos;
	int32  i;

#else

	int64  pos;
	int64  prev_pos;
	int64  next_pos;
	int64  i;

#endif

	int32  ret;

	if( p_deque == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_deque->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_deque->b_alloc_mem )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( index < GEN_DEQUE_INDEX_BEGIN || index > GEN_DEQUE_INDEX_END )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_DEQUE_INDEX_BEGIN )
	{
		return gen_deque_pop_front(p_deque);
	}

	if( index == GEN_DEQUE_INDEX_END )
	{
		return gen_deque_pop_back(p_deque);
	}

	if( index < p_deque->deque_size/2 )
	{
		/** move first part */
		i = GEN_DEQUE_INDEX_BEGIN;
		pos = p_deque->head_pos + index;
		if( pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			pos -= p_deque->array_total_size;
		}

		next_pos = pos + 1;
		if( next_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			next_pos = 0;
		}

		if( p_deque->callback.delete_node )
		{
			ret = p_deque->callback.delete_node(p_deque, p_deque->p_array + next_pos, p_deque->callback.p_custom_param);
		}

		while( i < index )
		{
			if( pos < 0 )
			{
				pos = GEN_DEQUE_ARRAY_INDEX_END;
				next_pos = 0;

			}else
			{
				next_pos = pos + 1;
			}

			p_deque->p_array[next_pos].p_data = p_deque->p_array[pos].p_data;

			i++;
			pos--;
		}

		p_deque->head_pos++;
		if( p_deque->head_pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			p_deque->head_pos = 0;
		}

	}else
	{
		/** move last part */
		i = GEN_DEQUE_INDEX_END;
		pos = p_deque->head_pos + index + 2;
		if( pos > GEN_DEQUE_ARRAY_INDEX_END )
		{
			pos -= p_deque->array_total_size;
		}

		prev_pos = pos - 1;
		if( prev_pos < 0 )
		{
			prev_pos = 0;
		}

		if( p_deque->callback.delete_node )
		{
			ret = p_deque->callback.delete_node(p_deque, p_deque->p_array + prev_pos, p_deque->callback.p_custom_param);
		}

		while( i > index )
		{
			if( pos > GEN_DEQUE_ARRAY_INDEX_END )
			{
				pos = 0;
				prev_pos = GEN_DEQUE_ARRAY_INDEX_END;

			}else
			{
				prev_pos = pos - 1;
			}

			p_deque->p_array[prev_pos].p_data = p_deque->p_array[pos].p_data;

			i--;
			pos++;
		}

		p_deque->rear_pos--;
		if( p_deque->rear_pos < 0 )
		{
			p_deque->rear_pos = GEN_DEQUE_ARRAY_INDEX_END;
		}
	}

	p_deque->deque_size--;

	return GEN_S_OK;
}
