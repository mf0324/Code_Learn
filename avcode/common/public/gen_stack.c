
#include <stdlib.h>
#include <string.h>

#include "gen_stack.h"


/** 默认数组最小长度 */
#define GEN_STACK_ARRAY_MIN_SIZE                           (1024)
/** 默认数组最大长度 */
#define GEN_STACK_ARRAY_MAX_SIZE                           (1024)
/** 默认数组扩充系数 */
#define GEN_STACK_ARRAY_SCALE_FACTOR                       (2)


///////////////////////////////////外部接口////////////////////////////////////
GEN_STACK* gen_stack_create(GEN_ALLOCATOR* p_allocator)
{
	GEN_STACK *p_stack = NULL;

	if( p_allocator )
	{
		p_stack = (GEN_STACK *)p_allocator->allocate(sizeof(GEN_STACK), p_allocator->p_param);

	}else
	{
		p_stack = (GEN_STACK *)malloc( sizeof(GEN_STACK) );
	}

	if( p_stack == NULL )
	{
		return NULL;
	}

	memset(p_stack, 0, sizeof(GEN_STACK));
	p_stack->p_allocator = p_allocator;

	return p_stack;
}


int32 gen_stack_destroy(GEN_STACK *p_stack)
{
	int32 ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( p_stack->b_init )
	{
		gen_stack_deinit(p_stack);
	}

	if( p_stack->p_allocator )
	{
		ret = p_stack->p_allocator->deallocate(p_stack, p_stack->p_allocator->p_param);

	}else
	{
		free(p_stack);
	}

	return 0;
}

int32 gen_stack_init(GEN_STACK *p_stack)
{
	if( p_stack == NULL )
	{
		return -1;
	}

	if( p_stack->b_init )
	{
		return 0;
	}

	p_stack->stack_size = 0;
	p_stack->array_min_size = GEN_STACK_ARRAY_MIN_SIZE;
	p_stack->array_max_size = GEN_STACK_ARRAY_MAX_SIZE;
	p_stack->p_array = NULL;
	p_stack->b_alloc_mem = 0;

	p_stack->b_init = 1;

	return 0;
}

int32 gen_stack_deinit(GEN_STACK *p_stack)
{
	int32 ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( p_stack->stack_size > 0 )
	{
		gen_stack_clear(p_stack);
	}

	gen_stack_free_mem(p_stack);

	p_stack->b_init = 0;

	return 0;
}

int32 gen_stack_clear(GEN_STACK *p_stack)
{
	if( p_stack == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	p_stack->stack_size = 0;
	p_stack->top_pos = 0;
	//p_stack->bottom_pos = 0;

	return 0;
}

int32 gen_stack_set_array_param(GEN_STACK *p_stack, int32 min_size, int32 max_size)
{
	if( p_stack == NULL )
	{
		return -1;
	}

	if( min_size < 1 || max_size < min_size )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	if( p_stack->b_alloc_mem )
	{
		return -3;
	}

	p_stack->array_min_size = min_size;
	p_stack->array_max_size = max_size;

	return 0;
}

int32 gen_stack_alloc_mem(GEN_STACK *p_stack)
{
	uint32 mem_size;
	int32  ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	if( p_stack->b_alloc_mem )
	{
		return 0;
	}

	mem_size = sizeof(STACK_NODE) * (p_stack->array_min_size + 1);
	if( p_stack->p_allocator )
	{
		p_stack->p_array = (STACK_NODE*)p_stack->p_allocator->allocate(mem_size, p_stack->p_allocator->p_param);

	}else
	{
		p_stack->p_array = (STACK_NODE*)malloc( mem_size );
	}

	if( p_stack->p_array == NULL )
	{
		return -2;
	}

	p_stack->stack_size = 0;
	p_stack->array_total_size = p_stack->array_min_size + 1;
	p_stack->array_valid_size = p_stack->array_min_size;
	p_stack->top_pos = 0;
	p_stack->bottom_pos = 0;

	p_stack->b_alloc_mem = 1;

	return 0;
}

int32 gen_stack_free_mem(GEN_STACK *p_stack)
{
	int32 ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	if( p_stack->p_array )
	{
		if( p_stack->p_allocator )
		{
			ret = p_stack->p_allocator->deallocate(p_stack->p_array, p_stack->p_allocator->p_param);

		}else
		{
			free(p_stack->p_array);
		}

		p_stack->p_array = NULL;
	}

	p_stack->stack_size = 0;
	p_stack->b_alloc_mem = 0;

	return 0;
}

int32 gen_stack_get_size(GEN_STACK *p_stack, int32 *p_size)
{
	if( p_stack == NULL || p_size == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	*p_size = p_stack->stack_size;

	return 0;
}

int32 gen_stack_get_top(GEN_STACK *p_stack, void **pp_data)
{
	if( p_stack == NULL || pp_data == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		*pp_data = NULL;
		return -3;
	}

	if( !p_stack->b_alloc_mem )
	{
		*pp_data = NULL;
		return -4;
	}

	if( p_stack->stack_size < 1 )
	{
		/** 栈为空 */
		*pp_data = NULL;
		return -2;
	}

	*pp_data = p_stack->p_array[p_stack->top_pos - 1].p_data;

	return 0;
}

int32 gen_stack_get_bottom(GEN_STACK *p_stack, void **pp_data)
{
	if( p_stack == NULL || pp_data == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		*pp_data = NULL;
		return -3;
	}

	if( !p_stack->b_alloc_mem )
	{
		*pp_data = NULL;
		return -4;
	}

	if( p_stack->stack_size < 1 )
	{
		/** 栈为空 */
		*pp_data = NULL;
		return -2;
	}

	*pp_data = p_stack->p_array[p_stack->bottom_pos].p_data;

	return 0;
}

int32 gen_stack_push(GEN_STACK *p_stack, void *p_data)
{
	STACK_NODE *p_temp = NULL;

	int32  new_array_size;
	uint32 mem_size;

	int32 ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	if( !p_stack->b_alloc_mem )
	{
		ret = gen_stack_alloc_mem(p_stack);
		if( ret )
		{
			return -2;
		}
	}

	if( p_stack->stack_size < p_stack->array_valid_size )
	{
		/** 数组未满 */
		p_stack->p_array[p_stack->top_pos].p_data = p_data;
		p_stack->top_pos++;

	}else
	{
		/** 数组已满 */
		if( p_stack->array_min_size == p_stack->array_max_size )
		{
			/** 不自动增长 */
			return -4;
		}

		new_array_size = p_stack->array_valid_size * GEN_STACK_ARRAY_SCALE_FACTOR;
		if( new_array_size > p_stack->array_max_size )
		{
			new_array_size = p_stack->array_max_size;
		}

		mem_size = sizeof(STACK_NODE) * (new_array_size + 1);
		if( p_stack->p_allocator )
		{
			p_temp = (STACK_NODE*)p_stack->p_allocator->allocate(mem_size, p_stack->p_allocator->p_param);

		}else
		{
			p_temp = (STACK_NODE*)malloc( mem_size );
		}

		if( p_temp == NULL )
		{
			return -2;
		}

		/** 复制原数据 */
		memcpy(p_temp, p_stack->p_array, sizeof(STACK_NODE) * p_stack->stack_size);

		p_temp[p_stack->top_pos].p_data = p_data;
		p_stack->array_total_size = new_array_size + 1;
		p_stack->array_valid_size = new_array_size;
		p_stack->top_pos++;

		if( p_stack->p_allocator )
		{
			ret = p_stack->p_allocator->deallocate(p_stack->p_array, p_stack->p_allocator->p_param);

		}else
		{
			free(p_stack->p_array);
		}

		p_stack->p_array = p_temp;
	}

	p_stack->stack_size++;

	return 0;
}

int32 gen_stack_pop(GEN_STACK *p_stack)
{
	int32 ret;

	if( p_stack == NULL )
	{
		return -1;
	}

	if( !p_stack->b_init )
	{
		return -3;
	}

	if( !p_stack->b_alloc_mem )
	{
		return -3;
	}

	if( p_stack->stack_size < 1 )
	{
		return 0;
	}

	p_stack->top_pos--;
	p_stack->stack_size--;

	return 0;
}
