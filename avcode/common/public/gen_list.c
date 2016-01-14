
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>
#include "gen_list.h"

/** begin index */
#define GEN_LIST_INDEX_BEGIN                               (0)
/** end index */
#define GEN_LIST_INDEX_END                                 (p_list->list_size - 1)
/** min block num */
#define GEN_LIST_MIN_BLOCK_NUM                             (1024)


/////////////////////////////////// allocator /////////////////////////////////
#if !RUN_OS_64
static void* gen_list_inner_malloc(uint32 size, void* p_param)
#else
static void* gen_list_inner_malloc(uint64 size, void* p_param)
#endif
{
	return malloc(size);
}

static int32 gen_list_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
GEN_LIST* gen_list_create(GEN_ALLOCATOR* p_allocator)
{
	GEN_LIST *p_list = NULL;

	if( p_allocator )
	{
		p_list = (GEN_LIST *)p_allocator->allocate(sizeof(GEN_LIST), p_allocator->p_param);

	}else
	{
		p_list = (GEN_LIST *)malloc( sizeof(GEN_LIST) );
	}

	if( p_list == NULL )
	{
		return NULL;
	}

	gen_list_setup(p_list, p_allocator);

	return p_list;
}

int32 gen_list_destroy(GEN_LIST *p_list)
{
	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_list->b_init )
	{
		gen_list_deinit(p_list);
	}

	ret = p_list->allocator.deallocate(p_list, p_list->allocator.p_param);

	return GEN_S_OK;
}

int32 gen_list_setup(GEN_LIST *p_list, GEN_ALLOCATOR* p_allocator)
{
	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_list, 0, sizeof(GEN_LIST));

	if( p_allocator )
	{
		memcpy(&p_list->allocator, p_allocator, sizeof(p_list->allocator));

	}else
	{
		p_list->allocator.allocate = &gen_list_inner_malloc;
		p_list->allocator.deallocate = &gen_list_inner_free;
		p_list->allocator.p_param = p_list;
	}

	return GEN_S_OK;
}

int32 gen_list_cleanup(GEN_LIST *p_list)
{
	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_list->b_init )
	{
		gen_list_deinit(p_list);
	}

	return GEN_S_OK;
}

int32 gen_list_init(GEN_LIST *p_list)
{
	//int32   ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_list->b_init )
	{
		return GEN_S_OK;
	}

	memset(&p_list->callback, 0, sizeof(p_list->callback));

	p_list->mem_group = mem_group_create(&p_list->allocator);
	if( p_list->mem_group == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	mem_group_init(p_list->mem_group);
	mem_group_set_block_param(p_list->mem_group, sizeof(LIST_NODE), GEN_LIST_MIN_BLOCK_NUM, INT32_MAX);
	mem_group_alloc_mem(p_list->mem_group);

	p_list->list_size = 0;

	p_list->p_head = &p_list->head;
	p_list->p_rear = &p_list->rear;

	p_list->p_head->p_prev = NULL;
	p_list->p_head->p_next = p_list->p_rear;
	p_list->p_head->p_data = NULL;

	p_list->p_rear->p_prev = p_list->p_head;
	p_list->p_rear->p_next = NULL;
	p_list->p_rear->p_data = NULL;

	p_list->b_init = 1;

	return GEN_S_OK;
}

int32 gen_list_deinit(GEN_LIST *p_list)
{

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_list->list_size > 0 )
	{
		gen_list_clear(p_list);
	}

	if( p_list->p_head )
	{
		p_list->p_head = NULL;
	}

	if( p_list->p_rear )
	{
		p_list->p_rear = NULL;
	}

	if( p_list->mem_group )
	{
		//add by mf 调用释放内存
		mem_group_free_mem(p_list->mem_group);
		//add end

		mem_group_destroy(p_list->mem_group);
		p_list->mem_group = NULL;
	}

	p_list->b_init = 0;

	return GEN_S_OK;
}

int32 gen_list_clear(GEN_LIST *p_list)
{
	LIST_NODE *p_node = NULL;
	LIST_NODE *p_temp_node = NULL;

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_node = p_list->p_head->p_next;
	while( p_node != p_list->p_rear )
	{
		p_temp_node = p_node;
		p_node = p_node->p_next;

		if( p_list->callback.delete_node )
		{
			ret = p_list->callback.delete_node(p_list, p_temp_node, p_list->callback.p_custom_param);
		}

		//ret = p_list->allocator.deallocate(p_temp_node, p_list->allocator.p_param);
		ret = mem_group_release_buf(p_list->mem_group, p_temp_node);
	}

	p_list->p_head->p_next = p_list->p_rear;
	p_list->p_rear->p_prev = p_list->p_head;
	p_list->list_size = 0;

	return GEN_S_OK;
}

int32 gen_list_set_callback(GEN_LIST* p_list, GEN_LIST_CALLBACK* p_callback)
{
	if( p_list == NULL || p_callback == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_list->callback = *p_callback;

	return GEN_S_OK;
}

int32 gen_list_get_size(GEN_LIST *p_list, int32* p_size)
{
	if( p_list == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_size = p_list->list_size;

	return GEN_S_OK;
}

int32 gen_list_get_head_node(GEN_LIST *p_list, LIST_NODE **pp_node)
{
	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*pp_node = p_list->p_head;

	return GEN_S_OK;
}

int32 gen_list_get_rear_node(GEN_LIST *p_list, LIST_NODE **pp_node)
{
	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*pp_node = p_list->p_rear;

	return GEN_S_OK;
}

int32 gen_list_get_front_node(GEN_LIST *p_list, LIST_NODE **pp_node)
{
	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		/** null list */
		*pp_node = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	*pp_node = p_list->p_head->p_next;

	return GEN_S_OK;
}

int32 gen_list_get_front_data(GEN_LIST *p_list, void **pp_data)
{
	if( p_list == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		*pp_data = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	*pp_data = p_list->p_head->p_next->p_data;

	return GEN_S_OK;
}

int32 gen_list_push_front_node(GEN_LIST *p_list, LIST_NODE *p_node)
{

	if( p_list == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_node->p_prev = p_list->p_head;
	p_node->p_next = p_list->p_head->p_next;
	p_list->p_head->p_next->p_prev = p_node;
	p_list->p_head->p_next = p_node;
	p_list->list_size++;

	return GEN_S_OK;
}

int32 gen_list_push_front_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node)
{
	LIST_NODE* p_node = NULL;
	int32   ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	//p_node = (LIST_NODE*)p_list->allocator.allocate(sizeof(LIST_NODE), p_list->allocator.p_param);
	ret = mem_group_get_buf(p_list->mem_group, sizeof(LIST_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_node->p_data = p_data;
	p_node->p_prev = p_list->p_head;
	p_node->p_next = p_list->p_head->p_next;
	p_list->p_head->p_next->p_prev = p_node;
	p_list->p_head->p_next = p_node;
	p_list->list_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

int32 gen_list_pop_front(GEN_LIST* p_list)
{
	LIST_NODE *p_node = NULL;

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		return GEN_S_OK;
	}

	p_node = p_list->p_head->p_next;
	p_list->p_head->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_list->p_head;

	if( p_list->callback.delete_node )
	{
		ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
	}

	//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
	ret = mem_group_release_buf(p_list->mem_group, p_node);

	p_list->list_size--;

	return GEN_S_OK;
}

int32 gen_list_get_back_node(GEN_LIST* p_list, LIST_NODE** pp_node)
{
	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		*pp_node = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	*pp_node = p_list->p_rear->p_prev;

	return GEN_S_OK;
}

int32 gen_list_get_back_data(GEN_LIST* p_list, void** pp_data)
{
	if( p_list == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		*pp_data = NULL;
		return GEN_E_NEED_MORE_DATA;
	}

	*pp_data = p_list->p_rear->p_prev->p_data;

	return GEN_S_OK;
}

int32 gen_list_push_back_node(GEN_LIST* p_list, LIST_NODE* p_node)
{
	if( p_list == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_node->p_prev = p_list->p_rear->p_prev;
	p_node->p_next = p_list->p_rear;
	p_list->p_rear->p_prev->p_next = p_node;
	p_list->p_rear->p_prev = p_node;
	p_list->list_size++;

	return GEN_S_OK;
}

int32 gen_list_push_back_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node)
{
	LIST_NODE *p_node = NULL;
	int32   ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	//p_node = (LIST_NODE*)p_list->allocator.allocate(sizeof(LIST_NODE), p_list->allocator.p_param);
	ret = mem_group_get_buf(p_list->mem_group, sizeof(LIST_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_node->p_data = p_data;
	p_node->p_prev = p_list->p_rear->p_prev;
	p_node->p_next = p_list->p_rear;
	p_list->p_rear->p_prev->p_next = p_node;
	p_list->p_rear->p_prev = p_node;
	p_list->list_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

int32 gen_list_pop_back(GEN_LIST* p_list)
{
	LIST_NODE *p_node = NULL;

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 1 )
	{
		return GEN_S_OK;
	}

	p_node = p_list->p_rear->p_prev;
	p_list->p_rear->p_prev = p_node->p_prev;
	p_node->p_prev->p_next = p_list->p_rear;

	if( p_list->callback.delete_node )
	{
		ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
	}

	//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
	ret = mem_group_release_buf(p_list->mem_group, p_node);

	p_list->list_size--;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_list_get_node(GEN_LIST *p_list, int32 index, LIST_NODE **pp_node)
#else
int32 gen_list_get_node(GEN_LIST *p_list, int64 index, LIST_NODE **pp_node)
#endif
{
	LIST_NODE *p_node = NULL;

#if !RUN_OS_64
	int32  i;
#else
	int64  i;
#endif

	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( index < GEN_LIST_INDEX_BEGIN || index > GEN_LIST_INDEX_END )
	{
		*pp_node = NULL;
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_LIST_INDEX_BEGIN )
	{
		*pp_node = p_list->p_head->p_next;
		return GEN_S_OK;
	}

	if( index == GEN_LIST_INDEX_END )
	{
		*pp_node = p_list->p_rear->p_prev;
		return GEN_S_OK;
	}

	if( index < p_list->list_size/2 )
	{
		/** start from head */
		p_node = p_list->p_head->p_next;
		i = GEN_LIST_INDEX_BEGIN;
		while( i < index )
		{
			p_node = p_node->p_next;
			i++;
		}

	}else
	{
		/** start from rear */
		p_node = p_list->p_rear->p_prev;
		i = GEN_LIST_INDEX_END;
		while( i > index )
		{
			p_node = p_node->p_prev;
			i--;
		}
	}

	*pp_node = p_node;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_list_get_data(GEN_LIST *p_list, int32 index, void **pp_data)
#else
int32 gen_list_get_data(GEN_LIST *p_list, int64 index, void **pp_data)
#endif
{
	LIST_NODE *p_node = NULL;

#if !RUN_OS_64
	int32  i;
#else
	int64  i;
#endif

	if( p_list == NULL || pp_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( index < GEN_LIST_INDEX_BEGIN || index > GEN_LIST_INDEX_END )
	{
		*pp_data = NULL;
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_LIST_INDEX_BEGIN )
	{
		*pp_data = p_list->p_head->p_next->p_data;
		return GEN_S_OK;
	}

	if( index == GEN_LIST_INDEX_END )
	{
		*pp_data = p_list->p_rear->p_prev->p_data;
		return GEN_S_OK;
	}

	if( index < p_list->list_size/2 )
	{
		/** start from head */
		p_node = p_list->p_head->p_next;
		i = GEN_LIST_INDEX_BEGIN;
		while( i < index )
		{
			p_node = p_node->p_next;
			i++;
		}

	}else
	{
		/** start from rear */
		p_node = p_list->p_rear->p_prev;
		i = GEN_LIST_INDEX_END;
		while( i > index )
		{
			p_node = p_node->p_prev;
			i--;
		}
	}

	*pp_data = p_node->p_data;

	return GEN_S_OK;
}

int32 gen_list_get_node_by_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node)
{
	LIST_NODE *p_node = NULL;
	int32 b_find = 0;

	if( p_list == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** find node */
	p_node = p_list->p_head->p_next;
	while( p_node != p_list->p_rear )
	{
		if( p_node->p_data == p_data )
		{
			b_find = 1;
			break;
		}

		p_node = p_node->p_next;
	}

	if( !b_find )
	{
		return GEN_E_NOT_FOUND;
	}

	*pp_node = p_node;

	return GEN_S_OK;
}

int32 gen_list_insert_node_before_node(GEN_LIST *p_list, LIST_NODE *p_node, LIST_NODE *p_before)
{
	if( p_list == NULL || p_before == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_before == p_list->p_head )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_node->p_prev = p_before->p_prev;
	p_node->p_next = p_before;
	p_before->p_prev->p_next = p_node;
	p_before->p_prev = p_node;

	p_list->list_size++;

	return GEN_S_OK;
}

int32 gen_list_insert_node_after_node(GEN_LIST *p_list, LIST_NODE *p_node, LIST_NODE *p_after)
{
	if( p_list == NULL || p_after == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_after == p_list->p_rear )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_node->p_prev = p_after;
	p_node->p_next = p_after->p_next;
	p_after->p_next->p_prev = p_node;
	p_after->p_next = p_node;

	p_list->list_size++;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_list_insert_node_at(GEN_LIST *p_list, int32 index, LIST_NODE *p_node)
#else
int32 gen_list_insert_node_at(GEN_LIST *p_list, int64 index, LIST_NODE *p_node)
#endif
{
	LIST_NODE *p_node2 = NULL;

#if !RUN_OS_64
	int32  i;
#else
	int64  i;
#endif

	if( p_list == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( index < GEN_LIST_INDEX_BEGIN || index > (GEN_LIST_INDEX_END + 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_LIST_INDEX_BEGIN )
	{
		return gen_list_push_front_node(p_list, p_node);
	}

	if( index == (GEN_LIST_INDEX_END + 1) )
	{
		return gen_list_push_back_node(p_list, p_node);
	}

	/** find insert pos */
	if( index < p_list->list_size/2 )
	{
		/** start from head */
		p_node2 = p_list->p_head->p_next;
		i = GEN_LIST_INDEX_BEGIN;
		while( i < index )
		{
			p_node2 = p_node2->p_next;
			i++;
		}

	}else
	{
		/** start from rear */
		p_node2 = p_list->p_rear->p_prev;
		i = GEN_LIST_INDEX_END;
		while( i > index )
		{
			p_node2 = p_node2->p_prev;
			i--;
		}
	}

	/** insert */
	p_node->p_prev = p_node2->p_prev;
	p_node->p_next = p_node2;
	p_node2->p_prev->p_next = p_node;
	p_node2->p_prev = p_node;

	p_list->list_size++;

	return GEN_S_OK;
}

int32 gen_list_insert_data_before_node(GEN_LIST *p_list, void *p_data, LIST_NODE *p_before, LIST_NODE **pp_node)
{
	LIST_NODE *p_node = NULL;
	int32   ret;

	if( p_list == NULL || p_before == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_before == p_list->p_head )
	{
		return GEN_E_INVALID_PARAM;
	}

	//p_node = (LIST_NODE*)p_list->allocator.allocate(sizeof(LIST_NODE), p_list->allocator.p_param);
	ret = mem_group_get_buf(p_list->mem_group, sizeof(LIST_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_node->p_data = p_data;
	p_node->p_prev = p_before->p_prev;
	p_node->p_next = p_before;
	p_before->p_prev->p_next = p_node;
	p_before->p_prev = p_node;

	p_list->list_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

int32 gen_list_insert_data_after_node(GEN_LIST *p_list, void *p_data, LIST_NODE *p_after, LIST_NODE **pp_node)
{
	LIST_NODE *p_node = NULL;
	int32   ret;

	if( p_list == NULL || p_after == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_after == p_list->p_rear )
	{
		return GEN_E_INVALID_PARAM;
	}

	//p_node = (LIST_NODE*)p_list->allocator.allocate(sizeof(LIST_NODE), p_list->allocator.p_param);
	ret = mem_group_get_buf(p_list->mem_group, sizeof(LIST_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_node->p_data = p_data;
	p_node->p_prev = p_after;
	p_node->p_next = p_after->p_next;
	p_after->p_next->p_prev = p_node;
	p_after->p_next = p_node;

	p_list->list_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_list_insert_data_at(GEN_LIST* p_list, int32 index, void* p_data, LIST_NODE** pp_node)
#else
int32 gen_list_insert_data_at(GEN_LIST* p_list, int64 index, void* p_data, LIST_NODE** pp_node)
#endif
{
	LIST_NODE *p_node = NULL;
	LIST_NODE *p_node2 = NULL;

#if !RUN_OS_64
	int32   i;
#else
	int64   i;
#endif

	int32   ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( index < GEN_LIST_INDEX_BEGIN || index > (GEN_LIST_INDEX_END + 1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_LIST_INDEX_BEGIN )
	{
		return gen_list_push_front_data(p_list, p_data, pp_node);
	}

	if( index == (GEN_LIST_INDEX_END + 1) )
	{
		return gen_list_push_back_data(p_list, p_data, pp_node);
	}

	//p_node = (LIST_NODE*)p_list->allocator.allocate(sizeof(LIST_NODE), p_list->allocator.p_param);
	ret = mem_group_get_buf(p_list->mem_group, sizeof(LIST_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	/** find insert pos */
	if( index < p_list->list_size/2 )
	{
		/** start from head */
		p_node2 = p_list->p_head->p_next;
		i = GEN_LIST_INDEX_BEGIN;
		while( i < index )
		{
			p_node2 = p_node2->p_next;
			i++;
		}

	}else
	{
		/** start from rear */
		p_node2 = p_list->p_rear->p_prev;
		i = GEN_LIST_INDEX_END;
		while( i > index )
		{
			p_node2 = p_node2->p_prev;
			i--;
		}
	}

	/** insert */
	p_node->p_data = p_data;
	p_node->p_prev = p_node2->p_prev;
	p_node->p_next = p_node2;
	p_node2->p_prev->p_next = p_node;
	p_node2->p_prev = p_node;

	p_list->list_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

int32 gen_list_delete_node(GEN_LIST *p_list, LIST_NODE *p_node)
{
	int32 ret;

	if( p_list == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_node == p_list->p_head || p_node == p_list->p_rear )
	{
		/** prevent delete head and rear */
		return GEN_E_INVALID_PARAM;
	}

	/** delete node */
	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	if( p_list->callback.delete_node )
	{
		ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
	}

	//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
	ret = mem_group_release_buf(p_list->mem_group, p_node);

	p_list->list_size--;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 gen_list_delete_node_at(GEN_LIST *p_list, int32 index)
#else
int32 gen_list_delete_node_at(GEN_LIST *p_list, int64 index)
#endif
{
	LIST_NODE *p_node = NULL;

#if !RUN_OS_64
	int32  i;
#else
	int64  i;
#endif

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( index < GEN_LIST_INDEX_BEGIN || index > GEN_LIST_INDEX_END )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( index == GEN_LIST_INDEX_BEGIN )
	{
		return gen_list_pop_front(p_list);
	}

	if( index == GEN_LIST_INDEX_END )
	{
		return gen_list_pop_back(p_list);
	}

	/** find node */
	if( index < p_list->list_size/2 )
	{
		/** from head */
		p_node = p_list->p_head->p_next;
		i = GEN_LIST_INDEX_BEGIN;
		while( i < index )
		{
			p_node = p_node->p_next;
			i++;
		}

	}else
	{
		/** from rear */
		p_node = p_list->p_rear->p_prev;
		i = GEN_LIST_INDEX_END;
		while( i > index )
		{
			p_node = p_node->p_prev;
			i--;
		}
	}

	/** delete */
	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	if( p_list->callback.delete_node )
	{
		ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
	}

	//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
	ret = mem_group_release_buf(p_list->mem_group, p_node);

	p_list->list_size--;

	return GEN_S_OK;
}

int32 gen_list_delete_node_by_data(GEN_LIST *p_list, void *p_data)
{
	LIST_NODE *p_node = NULL;
	int32 b_find = 0;

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** find pos */
	p_node = p_list->p_head->p_next;
	while( p_node != p_list->p_rear )
	{
		if( p_node->p_data == p_data )
		{
			b_find = 1;
			break;
		}

		p_node = p_node->p_next;
	}

	if( !b_find )
	{
		return GEN_E_NOT_FOUND;
	}

	/** delete */
	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	if( p_list->callback.delete_node )
	{
		ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
	}

	//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
	ret = mem_group_release_buf(p_list->mem_group, p_node);

	p_list->list_size--;

	return GEN_S_OK;
}

int32 gen_list_delete_all_node_by_data(GEN_LIST *p_list, void *p_data)
{
	LIST_NODE *p_node  = NULL;
	LIST_NODE *p_node2 = NULL;

	int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** find pos */
	p_node = p_list->p_head->p_next;
	while( p_node != p_list->p_rear )
	{
		if( p_node->p_data == p_data )
		{
			p_node2 = p_node;
			p_node = p_node->p_next;

			/** delete */
			p_node2->p_prev->p_next = p_node2->p_next;
			p_node2->p_next->p_prev = p_node2->p_prev;

			if( p_list->callback.delete_node )
			{
				ret = p_list->callback.delete_node(p_list, p_node2, p_list->callback.p_custom_param);
			}

			//ret = p_list->allocator.deallocate(p_node2, p_list->allocator.p_param);
			ret = mem_group_release_buf(p_list->mem_group, p_node2);

			p_list->list_size--;

		}else
		{
			p_node = p_node->p_next;
		}
	}

	return GEN_S_OK;
}

int32 gen_list_delete_range(GEN_LIST *p_list, LIST_NODE *p_begin, LIST_NODE *p_end)
{
	LIST_NODE *p_node  = NULL;
	LIST_NODE *p_node2 = NULL;
	LIST_NODE *p_node3 = NULL;

	int32 ret;

	if( p_list == NULL || p_begin == NULL || p_end == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_begin->p_prev->p_next = p_end->p_next;
	p_end->p_next->p_prev = p_begin->p_prev;

	p_node = p_begin;
	p_node3 = p_end->p_next;
	while( p_node != p_node3 )
	{
		p_node2 = p_node->p_next;

		if( p_list->callback.delete_node )
		{
			ret = p_list->callback.delete_node(p_list, p_node, p_list->callback.p_custom_param);
		}

		//ret = p_list->allocator.deallocate(p_node, p_list->allocator.p_param);
		ret = mem_group_release_buf(p_list->mem_group, p_node);

		p_list->list_size--;

		p_node = p_node2;
	}

	return GEN_S_OK;
}

int32 gen_list_reverse(GEN_LIST *p_list)
{
	LIST_NODE *p_current = NULL;
	LIST_NODE *p_node  = NULL;
	LIST_NODE *p_node2 = NULL;
	LIST_NODE *p_first_node = NULL;

	//int32 i;
	//int32 ret;

	if( p_list == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_list->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_list->list_size < 2 )
	{
		/** not need to adjust */
		return GEN_S_OK;
	}

	p_current = p_list->p_head->p_next;
	p_node = p_current->p_next;
	while( p_node != p_list->p_rear )
	{
		p_node2 = p_node->p_next;

		p_node->p_next = p_current;
		p_current->p_prev = p_node;

		p_current = p_node;
		p_node = p_node2;
	}

	p_first_node = p_list->p_head->p_next;
	p_list->p_head->p_next = p_list->p_rear->p_prev;
	p_list->p_rear->p_prev = p_first_node;

	p_list->p_head->p_next->p_prev = p_list->p_head;
	p_list->p_rear->p_prev->p_next = p_list->p_rear;

	return GEN_S_OK;
}
