
#include <stdlib.h>
#include <string.h>

#include <public/gen_comparer.h>
#include <public/gen_error.h>

#include <public/gen_platform.h>
//#if RUN_OS_WINDOWS
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#endif
#include <util/log_agent.h>

#include "mem_group.h"



/** block size */
#define MEM_GROUP_BLOCK_SIZE                               (128)
/** min block num */
#define MEM_GROUP_MIN_BLOCK_NUM                            (1024)
/** max block num */
#define MEM_GROUP_MAX_BLOCK_NUM                            (1024)
/** buf min align size */
#define MEM_GROUP_BUF_MIN_ALIGN_SIZE                       (4)
/** buf max align size */
#define MEM_GROUP_BUF_MAX_ALIGN_SIZE                       (32)

/** red */
#define MEM_GROUP_COLOR_RED                                (0)
/** black */
#define MEM_GROUP_COLOR_BLACK                              (1)

/** print mem */
#define MEM_GROUP_PRINT_MEM_ARRAY                          (0)


/** mem block */
typedef struct tag_mem_block
{
	/** in use */
	int32   b_in_use;

	/** buf */
	uint8*  p_buf;

} MEM_BLOCK, *PMEM_BLOCK;

typedef struct tag_mem_array MEM_ARRAY, *PMEM_ARRAY;

/** mem array */
struct tag_mem_array
{
	/** free */
	int32   b_free;

#if !RUN_OS_64

	/** block size */
	uint32  block_size;
	/** total block num */
	uint32  total_block_num;
	/** use block num */
	uint32  use_block_num;
	/** total block buf size */
	uint32  total_block_buf_size;
	/** block array buf size */
	uint32  block_array_buf_size;

#else

	/** block size */
	uint64  block_size;
	/** total block num */
	uint64  total_block_num;
	/** use block num */
	uint64  use_block_num;
	/** total block buf size */
	uint64  total_block_buf_size;
	/** block array buf size */
	uint64  block_array_buf_size;

#endif

	/** block array */
	MEM_BLOCK*  p_block;
	/** buf */
	uint8*  p_buf;
	/** first block */
	uint8*  p_first_block;
	/** last block */
	uint8*  p_last_block;


	/** free block deque */
	MEM_BLOCK** p_free_deque;
#if !RUN_OS_64

	/** free deque buf size */
	uint32  free_deque_buf_size;
	/** free deque total size, unit: number */
	int32   free_deque_total_size;
	/** free deque valid size, unit: number */
	int32   free_deque_valid_size;
	/** free deque size, unit: number */
	int32   free_deque_size;
	/** free deque head pos */
	int32   free_deque_head_pos;
	/** free deque rear pos */
	int32   free_deque_rear_pos;

#else

	/** free deque buf size */
	uint64  free_deque_buf_size;
	/** free deque total size, unit: number */
	int64   free_deque_total_size;
	/** free deque valid size, unit: number */
	int64   free_deque_valid_size;
	/** free deque size, unit: number */
	int64   free_deque_size;
	/** free deque head pos */
	int64   free_deque_head_pos;
	/** free deque rear pos */
	int64   free_deque_rear_pos;

#endif

	/** list */
	/** prev array */
	MEM_ARRAY* p_prev_array;
	/** next array */
	MEM_ARRAY* p_next_array;

	/** prev free array */
	MEM_ARRAY* p_prev_free;
	/** next free array */
	MEM_ARRAY* p_next_free;

	/** map */
	/** color */
	int32  color;
	/** parent */
	MEM_ARRAY* p_parent;
	/** left child */
	MEM_ARRAY* p_left;
	/** right child */
	MEM_ARRAY* p_right;
	/** prev seq */
	MEM_ARRAY* p_prev_seq;
	/** next seq */
	MEM_ARRAY* p_next_seq;

};

/** mem group */
typedef struct tag_mem_group
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;

	/** init */
	int32   b_init;
	/** alloc mem */
	int32   b_alloc_mem;

#if !RUN_OS_64

	/** buf align size */
	uint32  align_size;
	/** block size */
	uint32  block_size;
	/** adjust block size */
	uint32  adjust_block_size;
	/** min block num */
	int32   min_block_num;
	/** max block num */
	int32   max_block_num;
	/** total block num */
	int32   total_block_num;

#else

	/** buf align size */
	uint64  align_size;
	/** block size */
	uint64  block_size;
	/** adjust block size */
	uint64  adjust_block_size;
	/** min block num */
	int64   min_block_num;
	/** max block num */
	int64   max_block_num;
	/** total block num */
	int64   total_block_num;

#endif

#if !RUN_OS_64
	/** total array num */
	int32   total_array_num;
#else
	/** total array num */
	int64   total_array_num;
#endif
	/** array head pointer */
	MEM_ARRAY* p_array_head;
	/** array rear pointer */
	MEM_ARRAY* p_array_rear;
	/** array head */
	MEM_ARRAY  array_head;
	/** array rear */
	MEM_ARRAY  array_rear;

#if !RUN_OS_64
	/** free array num */
	int32   free_array_num;
#else
	/** free array num */
	int64   free_array_num;
#endif
	/** free head pointer */
	MEM_ARRAY* p_free_head;
	/** free rear pointer */
	MEM_ARRAY* p_free_rear;
	/** free head */
	MEM_ARRAY  free_head;
	/** free rear */
	MEM_ARRAY  free_rear;

	/** map root */
	MEM_ARRAY* p_root;
	/** map seq head pointer */
	MEM_ARRAY* p_seq_head;
	/** map seq rear pointer */
	MEM_ARRAY* p_seq_rear;
	/** map seq head */
	MEM_ARRAY  seq_head;
	/** map seq rear */
	MEM_ARRAY  seq_rear;

} MEM_GROUP, *PMEM_GROUP;

/////////////////////////////////// allocator /////////////////////////////////
#if !RUN_OS_64
static void* mem_group_inner_malloc(uint32 size, void* p_param)
#else
static void* mem_group_inner_malloc(uint64 size, void* p_param)
#endif
{
	return malloc(size);
}

static int32 mem_group_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/////////////////////////////////// comparer //////////////////////////////////
static int32 mem_group_inner_compare(void* data1, void* data2, void* p_param, int32* p_result)
{
	if( data1 < data2 )
	{
		*p_result = -1;

	}else if( data1 == data2 )
	{
		*p_result = 0;

	}else
	{
		*p_result = 1;
	}

	return 0;
}

/////////////////////////////////// array /////////////////////////////////////
/**  
 * @brief create
 * @param [in] p_group, group
 * @param [in] block_size, block size
 * @param [in] block_num, block num
 * @param [out] pp_array, new array
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
static int32 mem_group_array_create(MEM_GROUP* p_group, uint32 block_size, uint32 block_num, MEM_ARRAY** pp_array)
#else
static int32 mem_group_array_create(MEM_GROUP* p_group, uint64 block_size, uint64 block_num, MEM_ARRAY** pp_array)
#endif
{
	MEM_ARRAY*  p_array = NULL;
	MEM_BLOCK** pp_block = NULL;
	MEM_BLOCK*  p_block = NULL;
	uint8*  p_mem_buf = NULL;
	uint8*  p_uint8 = NULL;
	//uint8** pp_uint8 = NULL;

#if !RUN_OS_64

	uint32  total_block_buf_size;
	uint32  block_array_buf_size;
	uint32  free_deque_buf_size;
	uint32  total_buf_size;
	uint32  addr_left_size;
	int32   i;

#else

	uint64  total_block_buf_size;
	uint64  block_array_buf_size;
	uint64  free_deque_buf_size;
	uint64  total_buf_size;
	uint64  addr_left_size;
	int64   i;

#endif

	//int32  ret;

	total_block_buf_size = block_size * block_num;
	block_array_buf_size = sizeof(MEM_BLOCK) * block_num;
	free_deque_buf_size = sizeof(MEM_BLOCK*) * (block_num + 1);
	//free_deque_buf_size = sizeof(uint8*) * (block_num + 1);
	total_buf_size = sizeof(MEM_ARRAY) + free_deque_buf_size + block_array_buf_size + total_block_buf_size + MEM_GROUP_BUF_MAX_ALIGN_SIZE;

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("block_size = %u, block_num = %u, mem_array_size = %d, free_deque_buf_size = %u, block_array_buf_size = %u, total_block_buf_size = %u, total_buf_size = %u\n"),
	//	block_size, block_num, sizeof(MEM_ARRAY), free_deque_buf_size, block_array_buf_size, total_block_buf_size, total_buf_size);

	p_array = (MEM_ARRAY*)p_group->allocator.allocate(total_buf_size, p_group->allocator.p_param);
	if( p_array == NULL )
	{
		*pp_array = NULL;
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_mem_buf = (uint8*)p_array;

	/** array */
	p_array->b_free = 1;
	p_array->block_size = block_size;
	p_array->total_block_num = block_num;
	p_array->use_block_num = 0;
	p_array->total_block_buf_size = total_block_buf_size;
	p_array->p_buf = p_mem_buf + sizeof(MEM_ARRAY) + free_deque_buf_size + block_array_buf_size;
#if !RUN_OS_64
	addr_left_size = (uint32)p_array->p_buf % p_group->align_size;
#else
	addr_left_size = (uint64)p_array->p_buf % p_group->align_size;
#endif
	if( addr_left_size != 0 )
	{
		p_array->p_buf += (p_group->align_size - addr_left_size);
	}

	/** free deque */
	p_array->p_free_deque = (MEM_BLOCK**)(p_mem_buf + sizeof(MEM_ARRAY));
	p_array->free_deque_buf_size = free_deque_buf_size;
	p_array->free_deque_total_size = block_num + 1;
	p_array->free_deque_valid_size = block_num;
	p_array->free_deque_size = 0;
	p_array->free_deque_head_pos = 0;
	p_array->free_deque_rear_pos = 1;

	/** mem block */
	p_array->p_block = (MEM_BLOCK*)(((uint8*)p_array->p_free_deque) + p_array->free_deque_buf_size);
	p_array->block_array_buf_size = block_array_buf_size;
	p_array->p_first_block = p_array->p_buf;
	p_array->p_last_block = p_array->p_first_block + total_block_buf_size - p_array->block_size;

	/** add to free deque */
	pp_block = p_array->p_free_deque + p_array->free_deque_rear_pos;
	p_block = p_array->p_block;
	p_uint8 = p_array->p_first_block;

	for( i = 0; i < p_array->total_block_num; i++ )
	{
		p_block->b_in_use = 0;
		p_block->p_buf = p_uint8;

		*pp_block = p_block;

		pp_block++;
		p_block++;
		p_uint8 += p_array->block_size;
	}

	p_array->free_deque_size = p_array->total_block_num;
	p_array->free_deque_rear_pos = 0;

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("p_array = 0x%x, p_free_deque = 0x%x, p_buf = 0x%x, p_first_block = 0x%x, p_last_block = 0x%x\n"),
	//	p_array, p_array->p_free_deque, p_array->p_buf, p_array->p_first_block, p_array->p_last_block);

	*pp_array = p_array;

	return GEN_S_OK;
}

/**  
 * @brief destroy
 * @param [in] p_group, group
 * @param [in] p_array, array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_array_destroy(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	int32  ret;

	ret = p_group->allocator.deallocate(p_array, p_group->allocator.p_param);

	return GEN_S_OK;
}

/**  
 * @brief reset
 * @param [in] p_group, group
 * @param [in] p_array, array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_array_reset(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	MEM_BLOCK **pp_block = NULL;
	MEM_BLOCK *p_block = NULL;
	uint8*  p_uint8 = NULL;

#if !RUN_OS_64
	int32  i;
#else
	int64  i;
#endif
	//int32  ret;

	/** array */
	p_array->b_free = 1;
	p_array->use_block_num = 0;

	/** free deque */
	p_array->free_deque_size = 0;
	p_array->free_deque_head_pos = 0;
	p_array->free_deque_rear_pos = 1;

	/** reset free deque */
	pp_block = p_array->p_free_deque + p_array->free_deque_rear_pos;
	p_block = p_array->p_block;

	for( i = 0; i < p_array->total_block_num; i++ )
	{
		p_block->b_in_use = 0;

		*pp_block = p_block;

		pp_block++;
		p_block++;
	}

	p_array->free_deque_size = p_array->total_block_num;
	p_array->free_deque_rear_pos = 0;

	return GEN_S_OK;
}

/////////////////////////////////// full list /////////////////////////////////
/**  
 * @brief init
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_full_list_init(MEM_GROUP* p_group)
{
	p_group->total_array_num = 0;

	p_group->p_array_head = &p_group->array_head;
	p_group->p_array_rear = &p_group->array_rear;

	memset(p_group->p_array_head, 0, sizeof(MEM_ARRAY));
	p_group->p_array_head->p_prev_array = NULL;
	p_group->p_array_head->p_next_array = p_group->p_array_rear;

	memset(p_group->p_array_rear, 0, sizeof(MEM_ARRAY));
	p_group->p_array_rear->p_prev_array = p_group->p_array_head;
	p_group->p_array_rear->p_next_array = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_full_list_clear(MEM_GROUP* p_group)
{
	MEM_ARRAY* p_array = NULL;
	MEM_ARRAY* p_temp = NULL;

	int32   ret;

	p_array = p_group->p_array_head->p_next_array;
	while( p_array != p_group->p_array_rear )
	{
		p_temp = p_array->p_next_array;
		ret = p_group->allocator.deallocate(p_array, p_group->allocator.p_param);
		p_array = p_temp;
	}

	p_group->p_array_head->p_next_array = p_group->p_array_rear;
	p_group->p_array_rear->p_prev_array = p_group->p_array_head;
	p_group->total_array_num = 0;
	p_group->total_block_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_full_list_deinit(MEM_GROUP* p_group)
{

	if( p_group->total_array_num > 0 )
	{
		mem_group_full_list_clear(p_group);
	}

	if( p_group->p_array_head )
	{
		p_group->p_array_head = NULL;
	}

	if( p_group->p_array_rear )
	{
		p_group->p_array_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief push back
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_full_list_push_back(MEM_GROUP *p_group, MEM_ARRAY *p_array)
{
	p_array->p_prev_array = p_group->p_array_rear->p_prev_array;
	p_array->p_next_array = p_group->p_array_rear;
	p_group->p_array_rear->p_prev_array->p_next_array = p_array;
	p_group->p_array_rear->p_prev_array = p_array;
	p_group->total_array_num++;

	return GEN_S_OK;
}

/**  
 * @brief delete
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_full_list_delete(MEM_GROUP *p_group, MEM_ARRAY *p_array)
{
	int32 ret;

	p_group->total_block_num -= p_array->total_block_num;

	p_array->p_prev_array->p_next_array = p_array->p_next_array;
	p_array->p_next_array->p_prev_array = p_array->p_prev_array;

	ret = p_group->allocator.deallocate(p_array, p_group->allocator.p_param);

	p_group->total_array_num--;

	return GEN_S_OK;
}

/////////////////////////////////// free list /////////////////////////////////
/**  
 * @brief init
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_init(MEM_GROUP* p_group)
{
	p_group->free_array_num = 0;

	p_group->p_free_head = &p_group->free_head;
	p_group->p_free_rear = &p_group->free_rear;

	memset(p_group->p_free_head, 0, sizeof(MEM_ARRAY));
	p_group->p_free_head->p_prev_free = NULL;
	p_group->p_free_head->p_next_free = p_group->p_free_rear;

	memset(p_group->p_free_rear, 0, sizeof(MEM_ARRAY));
	p_group->p_free_rear->p_prev_free = p_group->p_free_head;
	p_group->p_free_rear->p_next_free = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_clear(MEM_GROUP* p_group)
{
	p_group->p_free_head->p_next_free = p_group->p_free_rear;
	p_group->p_free_rear->p_prev_free = p_group->p_free_head;
	p_group->free_array_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_deinit(MEM_GROUP* p_group)
{

	if( p_group->free_array_num > 0 )
	{
		mem_group_free_list_clear(p_group);
	}

	if( p_group->p_free_head )
	{
		p_group->p_free_head = NULL;
	}

	if( p_group->p_free_rear )
	{
		p_group->p_free_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief get front
 * @param [in] p_group, group
 * @param [out] pp_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_get_front(MEM_GROUP *p_group, MEM_ARRAY **pp_array)
{
	*pp_array = p_group->p_free_head->p_next_free;

	return GEN_S_OK;
}

/**  
 * @brief pop front
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_pop_front(MEM_GROUP *p_group)
{
	MEM_ARRAY *p_array = NULL;

	if( p_group->free_array_num < 1 )
	{
		return GEN_S_OK;
	}

	p_array = p_group->p_free_head->p_next_free;
	p_group->p_free_head->p_next_free = p_array->p_next_free;
	p_array->p_next_free->p_prev_free = p_group->p_free_head;

	p_group->free_array_num--;

	return GEN_S_OK;
}

/**  
 * @brief push back
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_push_back(MEM_GROUP *p_group, MEM_ARRAY *p_array)
{
	p_array->p_prev_free = p_group->p_free_rear->p_prev_free;
	p_array->p_next_free = p_group->p_free_rear;
	p_group->p_free_rear->p_prev_free->p_next_free = p_array;
	p_group->p_free_rear->p_prev_free = p_array;
	p_group->free_array_num++;

	return GEN_S_OK;
}

/**  
 * @brief delete
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_free_list_delete(MEM_GROUP *p_group, MEM_ARRAY *p_array)
{
	p_array->p_prev_free->p_next_free = p_array->p_next_free;
	p_array->p_next_free->p_prev_free = p_array->p_prev_free;

	p_group->free_array_num--;

	return GEN_S_OK;
}

/////////////////////////////// map list //////////////////////////////////////
/**  
 * @brief init
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_init(MEM_GROUP* p_group)
{
	p_group->p_seq_head = &p_group->seq_head;
	p_group->p_seq_rear = &p_group->seq_rear;

	memset(p_group->p_seq_head, 0, sizeof(MEM_ARRAY));
	p_group->p_seq_head->p_prev_seq = NULL;
	p_group->p_seq_head->p_next_seq = p_group->p_seq_rear;

	memset(p_group->p_seq_rear, 0, sizeof(MEM_ARRAY));
	p_group->p_seq_rear->p_prev_seq = p_group->p_seq_head;
	p_group->p_seq_rear->p_next_seq = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_clear(MEM_GROUP* p_group)
{
	p_group->p_seq_head->p_next_seq = p_group->p_seq_rear;
	p_group->p_seq_rear->p_prev_seq = p_group->p_seq_head;

	return GEN_S_OK;
}

/**  
 * @brief deinit
 * @param [in] p_group, group
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_deinit(MEM_GROUP* p_group)
{

	mem_group_map_list_clear(p_group);

	if( p_group->p_seq_head )
	{
		p_group->p_seq_head = NULL;
	}

	if( p_group->p_seq_rear )
	{
		p_group->p_seq_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief push front
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_push_front(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	p_array->p_prev_seq = p_group->p_seq_head;
	p_array->p_next_seq = p_group->p_seq_head->p_next_seq;
	p_group->p_seq_head->p_next_seq->p_prev_seq = p_array;
	p_group->p_seq_head->p_next_seq = p_array;

	return GEN_S_OK;
}

/**  
 * @brief insert array before another
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @param [in] p_before, the array to be insert before
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_insert_before(MEM_GROUP* p_group, MEM_ARRAY* p_array, MEM_ARRAY* p_before)
{
	p_array->p_prev_seq = p_before->p_prev_seq;
	p_array->p_next_seq = p_before;
	p_before->p_prev_seq->p_next_seq = p_array;
	p_before->p_prev_seq = p_array;

	return GEN_S_OK;
}

/**  
 * @brief insert array after another
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @param [in] p_after, the array to be insert after
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_insert_after(MEM_GROUP* p_group, MEM_ARRAY* p_array, MEM_ARRAY* p_after)
{
	p_array->p_prev_seq = p_after;
	p_array->p_next_seq = p_after->p_next_seq;
	p_after->p_next_seq->p_prev_seq = p_array;
	p_after->p_next_seq = p_array;

	return GEN_S_OK;
}

/**  
 * @brief delete
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_list_delete(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	p_array->p_prev_seq->p_next_seq = p_array->p_next_seq;
	p_array->p_next_seq->p_prev_seq = p_array->p_prev_seq;

	return GEN_S_OK;
}

///////////////////////////////  map  /////////////////////////////////////////
/**  
 * @brief find insert pos
 * @param [in] p_group, group
 * @param [in] p_array, mem array
 * @param [out] pp_array, parent of insert mem array
 * @param [out] p_left, insert pos is left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. return 0 means insert success, *pp_array means parent array, if *pp_array is null, the insert pos is root
 * 2. if return value is GEN_E_ALREADY_EXIST, *pp_array means the exist array
 */
static int32 mem_group_map_find_insert_pos(MEM_GROUP* p_group, MEM_ARRAY* p_array, MEM_ARRAY** pp_array, int32* p_left)
{
	MEM_ARRAY* p_parent;
	MEM_ARRAY* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	p_parent = NULL;
	p_node = p_group->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = p_group->comparer.compare(p_array, p_node, p_group->comparer.p_param, &result);
		if( result == 0 )
		{
			/** find */
			b_find = 1;
			break;
		}

		p_parent = p_node;
		if( result < 0 )
		{
			p_node = p_node->p_left;
			*p_left = 1;

		}else
		{
			p_node = p_node->p_right;
			*p_left = 0;
		}
	}

	if( b_find )
	{
		*pp_array = p_node;
		return GEN_E_ALREADY_EXIST;

	}else
	{
		*pp_array = p_parent;
		return GEN_S_OK;
	}
}

/**  
 * @brief find delete pos
 * @param [in] p_group, group
 * @param [in] p_node, the node to delete
 * @param [out] pp_node, actual delete node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. left child of the delete node must be null!
 */
static int32 mem_group_map_find_delete_pos(MEM_GROUP* p_group, MEM_ARRAY* p_node, MEM_ARRAY** pp_node)
{
	MEM_ARRAY* p_current = p_node;
	MEM_ARRAY* p_temp_node;

	//int32 ret;

	/** find the right most of left child, it is biggest node in left child */
	p_temp_node = p_current->p_left;
	while( p_temp_node->p_right )
	{
		p_temp_node = p_temp_node->p_right;
	}

	*pp_node = p_temp_node;

	return GEN_S_OK;
}

/**  
 * @brief left rotate
 * @param [in] p_group, group
 * @param [in] p_a, ancestor node
 * @param [in] p_b, parent node
 * @param [in] p_x, child node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before rotate:
 *                              a
 *                             /
 *                            b
 *                           /
 *                          x
 * after rotate:
 *                              b
 *                             / \
 *                            x   a
 */
static int32 mem_group_map_left_rotate(MEM_GROUP* p_group, MEM_ARRAY* p_a, MEM_ARRAY* p_b, MEM_ARRAY* p_x)
{
	if( p_b->p_right )
	{
		p_b->p_right->p_parent = p_a;
	}
	p_a->p_left = p_b->p_right;
	p_b->p_right = p_a;

	if( p_a->p_parent == NULL )
	{
		/** root */
		p_group->p_root = p_b;

	}else
	{
		/** not root */
		if( p_a == p_a->p_parent->p_left )
		{
			/** left child */
			p_a->p_parent->p_left = p_b;

		}else
		{
			/** right child */
			p_a->p_parent->p_right = p_b;
		}
	}

	p_b->p_parent = p_a->p_parent;
	p_a->p_parent = p_b;

	return GEN_S_OK;
}

/**  
 * @brief left adjust
 * @param [in] p_group, group
 * @param [in] p_a, ancestor node
 * @param [in] p_b, parent node
 * @param [in] p_x, child node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before adjust:
 *                              a
 *                             /
 *                            b
 *                             \
 *                              x
 * after adjust:
 *                              a
 *                             /
 *                            x
 *                           /
 *                          b
 */
static int32 mem_group_map_left_adjust(MEM_GROUP* p_group, MEM_ARRAY* p_a, MEM_ARRAY* p_b, MEM_ARRAY* p_x)
{
	p_a->p_left = p_x;

	if( p_x->p_left )
	{
		p_x->p_left->p_parent = p_b;
	}
	p_b->p_right = p_x->p_left;
	p_x->p_left = p_b;

	p_b->p_parent = p_x;
	p_x->p_parent = p_a;

	return GEN_S_OK;
}

/**  
 * @brief right rotate
 * @param [in] p_group, group
 * @param [in] p_a, parent node
 * @param [in] p_b, current node
 * @param [in] p_x, child node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before rotate:
 *                              a
 *                               \
 *                                b
 *                                 \
 *                                  x
 * after rotate:
 *                              b
 *                             / \
 *                            a   x
 */
static int32 mem_group_map_right_rotate(MEM_GROUP* p_group, MEM_ARRAY* p_a, MEM_ARRAY* p_b, MEM_ARRAY* p_x)
{
	if( p_b->p_left )
	{
		p_b->p_left->p_parent = p_a;
	}
	p_a->p_right = p_b->p_left;
	p_b->p_left = p_a;

	if( p_a->p_parent == NULL )
	{
		/** root */
		p_group->p_root = p_b;

	}else
	{
		/** not root */
		if( p_a == p_a->p_parent->p_left )
		{
			/** left child */
			p_a->p_parent->p_left = p_b;

		}else
		{
			/** right child */
			p_a->p_parent->p_right = p_b;
		}
	}

	p_b->p_parent = p_a->p_parent;
	p_a->p_parent = p_b;

	return GEN_S_OK;
}

/**  
 * @brief right adjust
 * @param [in] p_group, group
 * @param [in] p_a, ancestor node
 * @param [in] p_b, parent node
 * @param [in] p_x, child node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before adjust:
 *                              a
 *                               \
 *                                b
 *                               /
 *                              x
 * after adjust:
 *                              a
 *                               \
 *                                x
 *                                 \
 *                                  b
 */
static int32 mem_group_map_right_adjust(MEM_GROUP* p_group, MEM_ARRAY* p_a, MEM_ARRAY* p_b, MEM_ARRAY* p_x)
{
	p_a->p_right = p_x;

	if( p_x->p_right )
	{
		p_x->p_right->p_parent = p_b;
	}
	p_b->p_left = p_x->p_right;
	p_x->p_right = p_b;

	p_b->p_parent = p_x;
	p_x->p_parent = p_a;

	return GEN_S_OK;
}

/**  
 * @brief insert balance
 * @param [in] p_group, group
 * @param [in] p_node, insert node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_group_map_insert_balance(MEM_GROUP* p_group, MEM_ARRAY* p_node)
{
	MEM_ARRAY* p_ancestor;

	MEM_ARRAY* p_uncle;
	MEM_ARRAY* p_current;

	int32 ret;

	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			p_current->color = MEM_GROUP_COLOR_BLACK;
			break;
		}

		if( p_current->p_parent->color == MEM_GROUP_COLOR_BLACK )
		{
			/** case 2 */
			break;
		}

		//p_parent = p_current->p_parent;
		p_ancestor = p_current->p_parent->p_parent;
		if( p_ancestor == NULL )
		{
 			break;
		}

		if( p_current->p_parent == p_ancestor->p_left )
		{
			p_uncle = p_ancestor->p_right;

		}else
		{
			p_uncle = p_ancestor->p_left;
		}

		/** from above, parent always red, ancestor always black */
		if( p_uncle && p_uncle->color == MEM_GROUP_COLOR_RED )
		{
			/** case 3 */
			p_ancestor->color = MEM_GROUP_COLOR_RED;
			p_current->p_parent->color = MEM_GROUP_COLOR_BLACK;
			p_uncle->color = MEM_GROUP_COLOR_BLACK;

			p_current = p_ancestor;
			continue;
		}

		/** from above, uncle is null or always black */
		if( (p_current == p_current->p_parent->p_right) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 4(left) */
			ret = mem_group_map_left_adjust(p_group, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_left;

		}else if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_right) )
		{
			/** case 4(right) */
			ret = mem_group_map_right_adjust(p_group, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_right;
		}

		p_current->p_parent->color = MEM_GROUP_COLOR_BLACK;
		p_ancestor->color = MEM_GROUP_COLOR_RED;

		if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 5(left) */
			ret = mem_group_map_left_rotate(p_group, p_ancestor, p_current->p_parent, p_current);

		}else
		{
			/** case 5(right) */
			ret = mem_group_map_right_rotate(p_group, p_ancestor, p_current->p_parent, p_current);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief delete balance
 * @param [in] p_group, group
 * @param [in] p_node, from which node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_group_map_delete_balance(MEM_GROUP* p_group, MEM_ARRAY* p_node)
{
	//MEM_ARRAY* p_parent;
	MEM_ARRAY* p_sibling;
	MEM_ARRAY* p_sibling_left;
	MEM_ARRAY* p_sibling_right;
	MEM_ARRAY* p_current;

	int32 ret;

	/** from above, p_node is black */
	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			//p_current->color = MEM_GROUP_COLOR_BLACK;
			break;
		}

		//p_parent = p_current->p_parent;
		if( p_current == p_current->p_parent->p_left )
		{
			p_sibling = p_current->p_parent->p_right;

		}else
		{
			p_sibling = p_current->p_parent->p_left;
		}

		if( p_sibling == NULL )
		{
			break;
		}

		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		if( p_sibling->color == MEM_GROUP_COLOR_RED )
		{
			/** case 2 */
			p_current->p_parent->color = MEM_GROUP_COLOR_RED;
			p_sibling->color = MEM_GROUP_COLOR_BLACK;

			if( p_current == p_current->p_parent->p_left )
			{
				ret = mem_group_map_right_rotate(p_group, p_current->p_parent, p_sibling, p_sibling->p_right);
				p_sibling = p_sibling_left;

			}else
			{
				ret = mem_group_map_left_rotate(p_group, p_current->p_parent, p_sibling, p_sibling->p_left);
				p_sibling = p_sibling_right;
			}
		}

		/** from above, sibling always black */
		if( (p_current->p_parent->color == MEM_GROUP_COLOR_BLACK)
			&& (p_sibling->color == MEM_GROUP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK) )
		{
			/** case 3 */
			p_sibling->color = MEM_GROUP_COLOR_RED;
			p_current = p_current->p_parent;
			continue;
		}

		if( (p_current->p_parent->color == MEM_GROUP_COLOR_RED)
			&& (p_sibling->color == MEM_GROUP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK) )
		{
			/** case 4 */
			p_sibling->color = MEM_GROUP_COLOR_RED;
			p_current->p_parent->color = MEM_GROUP_COLOR_BLACK;

			break;
		}

		/** from case 3 & 4, sibling's child can not both be null */
		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		//if( p_sibling->color == MEM_GROUP_COLOR_BLACK )
		{
			if( (p_current == p_current->p_parent->p_left)
				&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK)
				&& (p_sibling_left && p_sibling->p_left->color == MEM_GROUP_COLOR_RED) )
			{
				/** case 5(left) */
				p_sibling->color = MEM_GROUP_COLOR_RED;
				p_sibling->p_left->color = MEM_GROUP_COLOR_BLACK;
				ret = mem_group_map_left_rotate(p_group, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

				p_sibling = p_sibling_left;

			}else if( (p_current == p_current->p_parent->p_right)
				&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
				&& (p_sibling_right && p_sibling->p_right->color == MEM_GROUP_COLOR_RED) )
			{
				/** case 5(right) */
				p_sibling->color = MEM_GROUP_COLOR_RED;
				p_sibling->p_right->color = MEM_GROUP_COLOR_BLACK;
				ret = mem_group_map_right_rotate(p_group, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

				p_sibling = p_sibling_right;
			}
		}

		p_sibling->color = p_current->p_parent->color;
		p_current->p_parent->color = MEM_GROUP_COLOR_BLACK;

		if( p_current == p_current->p_parent->p_left )
		{
			/** case 6(left) */
			if( p_sibling->p_right )
			{
				p_sibling->p_right->color = MEM_GROUP_COLOR_BLACK;
			}

			ret = mem_group_map_right_rotate(p_group, p_current->p_parent, p_sibling, p_sibling->p_right);

		}else
		{
			/** case 6(right) */
			if( p_sibling->p_left )
			{
				p_sibling->p_left->color = MEM_GROUP_COLOR_BLACK;
			}

			ret = mem_group_map_left_rotate(p_group, p_current->p_parent, p_sibling, p_sibling->p_left);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief delete balance(delete null node)
 * @param [in] p_group, group
 * @param [in] p_parent, parent node
 * @param [in] b_left, the node is parent node's left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_group_map_delete_balance2(MEM_GROUP* p_group, MEM_ARRAY* p_parent, int32 b_left)
{
	MEM_ARRAY* p_sibling;
	MEM_ARRAY* p_sibling_left;
	MEM_ARRAY* p_sibling_right;

	int32 ret;

	if( p_parent == NULL )
	{
		/** case 1 */
		return GEN_S_OK;
	}

	if( b_left )
	{
		p_sibling = p_parent->p_right;

	}else
	{
		p_sibling = p_parent->p_left;
	}

	if( p_sibling == NULL )
	{
		return GEN_S_OK;
	}

	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;
	if( p_sibling->color == MEM_GROUP_COLOR_RED )
	{
		/** case 2 */
		p_parent->color = MEM_GROUP_COLOR_RED;
		p_sibling->color = MEM_GROUP_COLOR_BLACK;

		if( b_left )
		{
			ret = mem_group_map_right_rotate(p_group, p_parent, p_sibling, p_sibling->p_right);
			p_sibling = p_sibling_left;

		}else
		{
			ret = mem_group_map_left_rotate(p_group, p_parent, p_sibling, p_sibling->p_left);
			p_sibling = p_sibling_right;
		}
	}

	/** from above, sibling always black */
	if( (p_parent->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK) )
	{
		/** case 3 */
		p_sibling->color = MEM_GROUP_COLOR_RED;

		return mem_group_map_delete_balance(p_group, p_parent);
	}

	if( (p_parent->color == MEM_GROUP_COLOR_RED)
		&& (p_sibling->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK) )
	{
		/** case 4 */
		p_sibling->color = MEM_GROUP_COLOR_RED;
		p_parent->color = MEM_GROUP_COLOR_BLACK;

		return 0;
	}

	/** from case 3 & 4, sibling's child can not both be null */
	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;

	if( (b_left)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling_left && p_sibling->p_left->color == MEM_GROUP_COLOR_RED) )
	{
		/** case 5(left) */
		p_sibling->color = MEM_GROUP_COLOR_RED;
		p_sibling->p_left->color = MEM_GROUP_COLOR_BLACK;
		ret = mem_group_map_left_rotate(p_group, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

		p_sibling = p_sibling_left;

	}else if( (!b_left)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_GROUP_COLOR_BLACK)
		&& (p_sibling_right && p_sibling->p_right->color == MEM_GROUP_COLOR_RED) )
	{
		/** case 5(right) */
		p_sibling->color = MEM_GROUP_COLOR_RED;
		p_sibling->p_right->color = MEM_GROUP_COLOR_BLACK;
		ret = mem_group_map_right_rotate(p_group, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

		p_sibling = p_sibling_right;
	}

	p_sibling->color = p_parent->color;
	p_parent->color = MEM_GROUP_COLOR_BLACK;

	if( b_left )
	{
		/** case 6(left) */
		if( p_sibling->p_right )
		{
			p_sibling->p_right->color = MEM_GROUP_COLOR_BLACK;
		}

		ret = mem_group_map_right_rotate(p_group, p_parent, p_sibling, p_sibling->p_right);

	}else
	{
		/** case 6(right) */
		if( p_sibling->p_left )
		{
			p_sibling->p_left->color = MEM_GROUP_COLOR_BLACK;
		}

		ret = mem_group_map_left_rotate(p_group, p_parent, p_sibling, p_sibling->p_left);
	}

	return GEN_S_OK;
}


/**  
 * @brief insert array
 * @param [in] p_group, group
 * @param [in] p_array, the insert array
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_map_insert_array(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	MEM_ARRAY* p_parent = NULL;
	MEM_ARRAY* p_node = p_array;
	MEM_ARRAY* p_temp = NULL;

	int32 b_left;
	int32 ret;

	ret = mem_group_map_find_insert_pos(p_group, p_array, &p_parent, &b_left);
	if( ret )
	{
		return ret;
	}

	p_node->color = MEM_GROUP_COLOR_RED;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	if( p_parent == NULL )
	{
		/** insert root */
		p_node->color = MEM_GROUP_COLOR_BLACK;
		p_group->p_root = p_node;
		ret = mem_group_map_list_push_front(p_group, p_node);

	}else
	{
		/** insert node is not root */
		if( b_left )
		{
			/** left child */
			p_parent->p_left = p_node;

			ret = mem_group_map_list_insert_before(p_group, p_node, p_parent);

		}else
		{
			/** right child */
			p_parent->p_right = p_node;

			ret = mem_group_map_list_insert_after(p_group, p_node, p_parent);
		}
	}

	ret = mem_group_map_insert_balance(p_group, p_node);

	return GEN_S_OK;
}

/**  
 * @brief swap array
 * @param [in] p_group, group
 * @param [in] p_array1, array1
 * @param [in] p_array2, array2
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. swap info include:
 * - color
 * - p_parent
 * - p_left
 * - p_right
 * - parent and child relate to p_array
 */
int32 mem_group_map_swap_array(MEM_GROUP* p_group, MEM_ARRAY* p_array1, MEM_ARRAY* p_array2)
{
	MEM_ARRAY temp_array;
	int32 b_parent_12;
	int32 b_parent_21;
	int32 b_left1;
	int32 b_left2;

	if( p_array1 == p_array2 )
	{
		/** same node */
		return GEN_S_OK;
	}

	if( p_array1 == p_array2->p_parent )
	{
		b_parent_12 = 1;

	}else
	{
		b_parent_12 = 0;
	}

	if( p_array2 == p_array1->p_parent )
	{
		b_parent_21 = 1;

	}else
	{
		b_parent_21 = 0;
	}

	if( p_array1->p_parent )
	{
		if( p_array1 == p_array1->p_parent->p_left )
		{
			b_left1 = 1;

		}else
		{
			b_left1 = 0;
		}
	}

	if( p_array2->p_parent )
	{
		if( p_array2 == p_array2->p_parent->p_left )
		{
			b_left2 = 1;

		}else
		{
			b_left2 = 0;
		}
	}

	memcpy(&temp_array, p_array1, sizeof(temp_array));

	/** update array1 */
	if( p_array1->p_parent )
	{
		if( !b_parent_21 )
		{
			if( b_left1 )
			{
				p_array1->p_parent->p_left = p_array2;

			}else
			{
				p_array1->p_parent->p_right = p_array2;
			}
		}
	}

	if( p_array1->p_left )
	{
		if( !(b_parent_12 && p_array2 == p_array1->p_left) )
		{
			p_array1->p_left->p_parent = p_array2;
		}
	}

	if( p_array1->p_right )
	{
		if( !(b_parent_12 && p_array2 == p_array1->p_right) )
		{
			p_array1->p_right->p_parent = p_array2;
		}
	}

	p_array1->color = p_array2->color;

	if( b_parent_12 )
	{
		p_array1->p_parent = p_array2;

	}else
	{
		p_array1->p_parent = p_array2->p_parent;
	}

	if( b_parent_21 && b_left1 )
	{
		p_array1->p_left = p_array2;

	}else
	{
		p_array1->p_left = p_array2->p_left;
	}

	if( b_parent_21 && !b_left1 )
	{
		p_array1->p_right = p_array2;

	}else
	{
		p_array1->p_right = p_array2->p_right;
	}

	/** update array2 */
	if( p_array2->p_parent )
	{
		if( !b_parent_12 )
		{
			if( b_left2 )
			{
				p_array2->p_parent->p_left = p_array1;

			}else
			{
				p_array2->p_parent->p_right = p_array1;
			}
		}
	}

	if( p_array2->p_left )
	{
		if( !(b_parent_21 && p_array1 == p_array2->p_left) )
		{
			p_array2->p_left->p_parent = p_array1;
		}
	}

	if( p_array2->p_right )
	{
		if( !(b_parent_21 && p_array1 == p_array2->p_right) )
		{
			p_array2->p_right->p_parent = p_array1;
		}
	}

	p_array2->color = temp_array.color;

	if( b_parent_21 )
	{
		p_array2->p_parent = p_array1;

	}else
	{
		p_array2->p_parent = temp_array.p_parent;
	}

	if( b_parent_12 && b_left2 )
	{
		p_array2->p_left = p_array1;

	}else
	{
		p_array2->p_left = temp_array.p_left;
	}

	if( b_parent_12 && !b_left2 )
	{
		p_array2->p_right = p_array1;

	}else
	{
		p_array2->p_right = temp_array.p_right;
	}

	/** update root */
	if( p_array1 == p_group->p_root )
	{
		p_group->p_root = p_array2;

	}else if( p_array2 == p_group->p_root )
	{
		p_group->p_root = p_array1;
	}

	return GEN_S_OK;
}


/**  
 * @brief delete
 * @param [in] p_group, group
 * @param [in] p_array, array
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_map_delete_array(MEM_GROUP* p_group, MEM_ARRAY* p_array)
{
	MEM_ARRAY *p_array2 = NULL;
	MEM_ARRAY *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_array->p_left == NULL )
	{
		/** left child is null */
		if( p_array->p_parent == NULL )
		{
			/** root */
			p_group->p_root = p_array->p_right;

		}else
		{
			/** not root */
			if( p_array == p_array->p_parent->p_left )
			{
				p_array->p_parent->p_left = p_array->p_right;
				b_left = 1;

			}else
			{
				p_array->p_parent->p_right = p_array->p_right;
				b_left = 0;
			}
		}

		if( p_array->p_right )
		{
			p_array->p_right->p_parent = p_array->p_parent;
		}

		p_child = p_array->p_right;

	}else
	{
		/** left child is not null */
		ret = mem_group_map_find_delete_pos(p_group, p_array, &p_array2);
		ret = mem_group_map_swap_array(p_group, p_array, p_array2);

		/** right child of the node should be null */
		if( p_array == p_array->p_parent->p_left )
		{
			p_array->p_parent->p_left = p_array->p_left;
			b_left = 1;

		}else
		{
			p_array->p_parent->p_right = p_array->p_left;
			b_left = 0;
		}

		if( p_array->p_left )
		{
			p_array->p_left->p_parent = p_array->p_parent;
		}

		p_child = p_array->p_left;
	}

	if( p_array->color != MEM_GROUP_COLOR_RED )
	{
		/** black node */
		if( p_child == NULL )
		{
			/** child is null */
			ret = mem_group_map_delete_balance2(p_group, p_array->p_parent, b_left);

		}else
		{
			/** child not null */
			if( p_child->color == MEM_GROUP_COLOR_RED )
			{
				/** red */
				p_child->color = MEM_GROUP_COLOR_BLACK;

			}else
			{
				/** black */
				ret = mem_group_map_delete_balance(p_group, p_child);
			}
		}
	}

	ret = mem_group_map_list_delete(p_group, p_array);

	return GEN_S_OK;
}

/**  
 * @brief get array by buf
 * @param [in] p_group, group
 * @param [in] p_buf, buf
 * @param [out] pp_array, array involve buf
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mem_group_get_array_by_buf(MEM_GROUP* p_group, uint8* p_buf, MEM_ARRAY** pp_array)
{
	MEM_ARRAY* p_array;

	int32 b_find;

	//int32 ret;

	p_array = p_group->p_root;
	b_find = 0;

	while( p_array )
	{
		if( p_buf >= p_array->p_first_block )
		{
			if( p_buf <= p_array->p_last_block )
			{
				/** find */
				b_find = 1;
				break;
			}

			/** search right child */
			p_array = p_array->p_right;

		}else
		{
			/** search left child  */
			p_array = p_array->p_left;
		}
	}

	if( b_find )
	{
		*pp_array = p_array;
		return GEN_S_OK;

	}else
	{
		*pp_array = NULL;
		return GEN_E_NOT_FOUND;
	}
}

/**  
 * @brief get block by buf
 * @param [in] p_group, group
 * @param [in] p_buf, buf
 * @param [in] p_array, mem array
 * @param [out] pp_block, mem block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_group_get_block_by_buf(MEM_GROUP* p_group, uint8* p_buf, MEM_ARRAY* p_array, MEM_BLOCK** pp_block)
{
#if !RUN_OS_64
	uint32  diff_addr;
	int32   index;
#else
	uint64  diff_addr;
	int64   index;
#endif

	//int32  ret;

	*pp_block = NULL;

#if 0
	if( p_buf < p_array->p_first_block || p_buf > p_array->p_last_block )
	{
		/** 非法地址 */
		return -2;
	}
#endif

	diff_addr = p_buf - p_array->p_first_block;
	if( diff_addr % p_group->adjust_block_size )
	{
		/** illegal buf */
		return GEN_E_INVALID_PARAM;
	}

	index = diff_addr / p_array->block_size;
	*pp_block = p_array->p_block + index;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
MEM_GROUP_H mem_group_create(GEN_ALLOCATOR* p_allocator)
{
	MEM_GROUP *p_group = NULL;

	if( p_allocator )
	{
		p_group = (MEM_GROUP *)p_allocator->allocate(sizeof(MEM_GROUP), p_allocator->p_param);

	}else
	{
		p_group = (MEM_GROUP *)malloc( sizeof(MEM_GROUP) );
	}

	if( p_group == NULL )
	{
		return NULL;
	}

	memset(p_group, 0, sizeof(MEM_GROUP));

	if( p_allocator )
	{
		memcpy(&p_group->allocator, p_allocator, sizeof(p_group->allocator));

	}else
	{
		p_group->allocator.allocate = mem_group_inner_malloc;
		p_group->allocator.deallocate = mem_group_inner_free;
		p_group->allocator.p_param = p_group;
	}

	p_group->comparer.compare = mem_group_inner_compare;
	p_group->comparer.p_param = p_group;

	return p_group;
}

int32 mem_group_destroy(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

	int32 ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_group->b_init )
	{
		mem_group_deinit(p_group);
	}

	ret = p_group->allocator.deallocate(p_group, p_group->allocator.p_param);

	return GEN_S_OK;
}

int32 mem_group_init(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

	int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_group->b_init )
	{
		return GEN_S_OK;
	}

	p_group->align_size = MEM_GROUP_BUF_MIN_ALIGN_SIZE;
	p_group->block_size = MEM_GROUP_BLOCK_SIZE;
	p_group->adjust_block_size = MEM_GROUP_BLOCK_SIZE;
	p_group->min_block_num = MEM_GROUP_MIN_BLOCK_NUM;
	p_group->max_block_num = MEM_GROUP_MAX_BLOCK_NUM;
	p_group->total_block_num = 0;

	ret = mem_group_full_list_init(p_group);
	if( ret )
	{
		mem_group_deinit(p_group);
		return GEN_E_FAIL;
	}

	ret = mem_group_free_list_init(p_group);
	if( ret )
	{
		mem_group_deinit(p_group);
		return GEN_E_FAIL;
	}

	p_group->p_root = NULL;
	ret = mem_group_map_list_init(p_group);
	if( ret )
	{
		mem_group_deinit(p_group);
		return GEN_E_FAIL;
	}

	p_group->b_alloc_mem = 0;
	p_group->b_init = 1;

	return GEN_S_OK;
}

int32 mem_group_deinit(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

	//int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_group->p_root = NULL;
	mem_group_map_list_deinit(p_group);

	mem_group_free_list_deinit(p_group);

	mem_group_full_list_deinit(p_group);

	p_group->b_init = 0;

	return GEN_S_OK;
}

int32 mem_group_reset(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

	MEM_ARRAY *p_array = NULL;

	//int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** clear free list */
	mem_group_free_list_clear(p_group);

	/** set up free list */
	p_array = p_group->p_array_head->p_next_array;
	while( p_array != p_group->p_array_rear )
	{
		mem_group_array_reset(p_group, p_array);
		mem_group_free_list_push_back(p_group, p_array);

		p_array = p_array->p_next_array;
	}

	return GEN_S_OK;
}

int32 mem_group_set_align_size(MEM_GROUP_H group_h, uint32 align_size)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

#if !RUN_OS_64
	uint32  left_size;
#else
	uint64  left_size;
#endif
	//int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_group->total_array_num > 0 )
	{
		return GEN_E_FAIL;
	}

	if( align_size > MEM_GROUP_BUF_MAX_ALIGN_SIZE )
	{
		return GEN_E_INVALID_PARAM;
	}

	switch( align_size )
	{
	case 4:
	case 8:
	case 16:
	case 32:
		{
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	p_group->align_size = align_size;

	/** adjust block size */
	left_size = p_group->adjust_block_size % p_group->align_size;
	if( left_size != 0 )
	{
		p_group->adjust_block_size += (p_group->align_size - left_size);
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 mem_group_set_block_param(MEM_GROUP_H group_h, uint32 block_size, int32 min_block_num, int32 max_block_num)
#else
int32 mem_group_set_block_param(MEM_GROUP_H group_h, uint64 block_size, int64 min_block_num, int64 max_block_num)
#endif
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

#if !RUN_OS_64
	uint32  left_size;
#else
	uint64  left_size;
#endif

	//int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_group->total_array_num > 0 )
	{
		return GEN_E_FAIL;
	}

	if( block_size < 1 || min_block_num < 1 || min_block_num > max_block_num )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_group->block_size = block_size;
	p_group->adjust_block_size = block_size;
	left_size = block_size % p_group->align_size;
	if( left_size != 0 )
	{
		p_group->adjust_block_size += (p_group->align_size - left_size);
	}

	p_group->min_block_num = min_block_num;
	p_group->max_block_num = max_block_num;
	p_group->total_block_num = 0;

	return GEN_S_OK;
}

int32 mem_group_alloc_mem(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;
	MEM_ARRAY *p_array = NULL;

#if !RUN_OS_64
	int32  block_num;
#else
	int64  block_num;
#endif

	int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_group->total_block_num >= p_group->max_block_num )
	{
		/** exceed */
		return GEN_E_NOT_SUPPORT;
	}

	/** create new array */
	block_num = p_group->max_block_num - p_group->total_block_num;
	if( block_num > p_group->min_block_num )
	{
		block_num = p_group->min_block_num;
	}

	ret = mem_group_array_create(p_group, p_group->adjust_block_size, block_num, &p_array);
	if( ret )
	{
		return ret;
	}

	/** update list */
	mem_group_full_list_push_back(p_group, p_array);
	mem_group_free_list_push_back(p_group, p_array);
	ret = mem_group_map_insert_array(p_group, p_array);

	p_group->total_block_num += block_num;
	p_group->b_alloc_mem = 1;

	return GEN_S_OK;
}

int32 mem_group_free_mem(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;

	//int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_group->p_root = NULL;
	//add by mf 释放保存的一些临时内存
	mem_group_free_unuse_mem(group_h);
	//add end

	mem_group_map_list_clear(p_group);
	mem_group_free_list_clear(p_group);
	mem_group_full_list_clear(p_group);

	p_group->b_alloc_mem = 0;

	return GEN_S_OK;
}

int32 mem_group_free_unuse_mem(MEM_GROUP_H group_h)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;
	MEM_ARRAY *p_array = NULL;
	MEM_ARRAY *p_temp = NULL;

	int32  ret;

	if( p_group == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_array = p_group->p_free_head->p_next_free;
	while( p_array != p_group->p_free_rear )
	{
		p_temp = p_array->p_next_free;
		if( p_array->use_block_num < 1 )
		{
			ret = mem_group_map_delete_array(p_group, p_array);
			mem_group_free_list_delete(p_group, p_array);
			mem_group_full_list_delete(p_group, p_array);
		}

		p_array = p_temp;
	}

	if( p_group->total_array_num < 1 )
	{
		p_group->p_root = NULL;
		p_group->b_alloc_mem = 0;
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 mem_group_get_buf(MEM_GROUP_H group_h, uint32 buf_size, void** pp_buf)
#else
int32 mem_group_get_buf(MEM_GROUP_H group_h, uint64 buf_size, void** pp_buf)
#endif
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;
	MEM_ARRAY *p_array = NULL;
	MEM_BLOCK *p_block = NULL;

#if !RUN_OS_64
	int32  pos;
	int32  block_num;
#else
	int64  pos;
	int64  block_num;
#endif

	int32  ret;

	if( p_group == NULL || pp_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*pp_buf = NULL;

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( buf_size > p_group->block_size )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_group->free_array_num > 0 )
	{
		/** get array from free list */
		p_array = p_group->p_free_head->p_next_free;

	}else
	{
		/** create new array */
		if( p_group->total_block_num >= p_group->max_block_num )
		{
			/** exceed */
			return GEN_E_NOT_ENOUGH_MEM;
		}

		block_num = p_group->max_block_num - p_group->total_block_num;
		if( block_num > p_group->min_block_num )
		{
			block_num = p_group->min_block_num;
		}

		ret = mem_group_array_create(p_group, p_group->adjust_block_size, block_num, &p_array);
		if( ret )
		{
			return GEN_E_NOT_ENOUGH_MEM;
		}

		/** update list */
		mem_group_full_list_push_back(p_group, p_array);
		mem_group_free_list_push_back(p_group, p_array);

		ret = mem_group_map_insert_array(p_group, p_array);

		p_group->total_block_num += block_num;
		p_group->b_alloc_mem = 1;
	}

	if( p_array->free_deque_size < 1 )
	{
		/** no free block */
		return GEN_E_NOT_ENOUGH_MEM;
	}

	/** get block from array free deque */
	pos = p_array->free_deque_head_pos + 1;
	if( pos > p_array->free_deque_valid_size )
	{
		pos = 0;
	}

	p_block = p_array->p_free_deque[pos];
	p_block->b_in_use = 1;
	p_array->use_block_num++;

	/** pop from array free deque */
	p_array->free_deque_head_pos++;
	if( p_array->free_deque_head_pos > p_array->free_deque_valid_size )
	{
		p_array->free_deque_head_pos = 0;
	}
	p_array->free_deque_size--;

	if( p_array->free_deque_size < 1 )
	{
		p_array->b_free = 0;

		/** remove from free list */
		mem_group_free_list_delete(p_group, p_array);
	}

	*pp_buf = p_block->p_buf;

	return GEN_S_OK;
}

int32 mem_group_release_buf(MEM_GROUP_H group_h, void* p_buf)
{
	MEM_GROUP *p_group = (MEM_GROUP*)group_h;
	MEM_ARRAY *p_array = NULL;
	MEM_BLOCK *p_block = NULL;

	int32  ret;

	if( p_group == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_group->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = mem_group_get_array_by_buf(p_group, p_buf, &p_array);
	if( ret )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("p_buf = 0x%x find array fail!\n"), p_buf);
		return GEN_E_FAIL;
	}

	ret = mem_group_get_block_by_buf(p_group, p_buf, p_array, &p_block);
	if( ret )
	{
		log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("p_buf = 0x%x find block fail!\n"), p_buf);
		return GEN_E_FAIL;
	}

	if( !p_block->b_in_use )
	{
		return GEN_S_OK;
	}
	p_block->b_in_use = 0;
	p_array->use_block_num--;

	/** update free deque */
	p_array->p_free_deque[p_array->free_deque_rear_pos] = p_block;
	p_array->free_deque_rear_pos++;
	if( p_array->free_deque_rear_pos > p_array->free_deque_valid_size )
	{
		p_array->free_deque_rear_pos = 0;
	}
	p_array->free_deque_size++;

	if( !p_array->b_free )
	{
		p_array->b_free = 1;

		/** update free list */
		ret = mem_group_free_list_push_back(p_group, p_array);
	}

	return GEN_S_OK;
}
