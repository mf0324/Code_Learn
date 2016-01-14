
#include <stdlib.h>
#include <string.h>

#include <public/gen_int.h>
#include <public/gen_comparer.h>
#include <public/gen_error.h>
//#include <util/log_debug.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "mem_pool.h"


/** min pool size */
#define MEM_POOL_MIN_POOL_SIZE                             (1024 * 1024)
/** max pool size */
#if !RUN_OS_64
#define MEM_POOL_MAX_POOL_SIZE                             (UINT32_MAX)
#else
#define MEM_POOL_MAX_POOL_SIZE                             (UINT64_MAX)
#endif
/** align size */
#define MEM_POOL_ADDR_ALIGN_SIZE                           (4)
/** max align size */
#define MEM_POOL_MAX_ADDR_ALIGN_SIZE                       (32)

/** red */
#define MEM_POOL_COLOR_RED                                 (0)
/** black */
#define MEM_POOL_COLOR_BLACK                               (1)


typedef struct tag_mem_pool_block MEM_POOL_BLOCK, *PMEM_POOL_BLOCK;
typedef struct tag_mem_pool_unit  MEM_POOL_UNIT, *PMEM_POOL_UNIT;


/** mem pool block */
struct tag_mem_pool_block
{
	/** free */
	int32   b_free;
#if !RUN_OS_64
	/** block size */
	uint32  block_size;
#else
	/** block size */
	uint64  block_size;
#endif

	/** buf */
	uint8*  p_buf;
#if !RUN_OS_64
	/** buf size */
	uint32  buf_size;
#else
	/** buf size */
	uint64  buf_size;
#endif

	/** unit */
	MEM_POOL_UNIT*   p_unit;

	/** prev block */
	MEM_POOL_BLOCK* p_prev_block;
	/** next block */
	MEM_POOL_BLOCK* p_next_block;

	/** for map use */
	/** color */
	int32  color;
	/** parent */
	MEM_POOL_BLOCK* p_parent;
	/** left child */
	MEM_POOL_BLOCK* p_left;
	/** right child */
	MEM_POOL_BLOCK* p_right;
	/** prev seq */
	MEM_POOL_BLOCK* p_prev_seq;
	/** next seq */
	MEM_POOL_BLOCK* p_next_seq;

};

/** mem pool unit */
struct tag_mem_pool_unit
{
#if !RUN_OS_64

	/** unit size */
	uint32  unit_size;
	/** total block num */
	uint32  total_block_num;
	/** use block num */
	uint32  use_block_num;
	/** free block num */
	uint32  free_block_num;

#else

	/** unit size */
	uint64  unit_size;
	/** total block num */
	uint64  total_block_num;
	/** use block num */
	uint64  use_block_num;
	/** free block num */
	uint64  free_block_num;

#endif

	/** head pointer */
	MEM_POOL_BLOCK* p_block_head;
	/** rear pointer */
	MEM_POOL_BLOCK* p_block_rear;
	/** block head */
	MEM_POOL_BLOCK  block_head;
	/** block rear */
	MEM_POOL_BLOCK  block_rear;

	/** prev unit */
	MEM_POOL_UNIT* p_prev_unit;
	/** next unit */
	MEM_POOL_UNIT* p_next_unit;

};

/** block map */
typedef struct tag_mem_pool_block_map
{
#if !RUN_OS_64
	/** block num */
	int32   block_num;
#else
	/** block num */
	int64   block_num;
#endif

	/** root */
	MEM_POOL_BLOCK* p_root;
	/** seq head pointer */
	MEM_POOL_BLOCK* p_seq_head;
	/** seq rear pointer */
	MEM_POOL_BLOCK* p_seq_rear;
	/** seq head */
	MEM_POOL_BLOCK  seq_head;
	/** seq rear */
	MEM_POOL_BLOCK  seq_rear;

} MEM_POOL_BLOCK_MAP, *PMEM_POOL_BLOCK_MAP;

/** mem pool */
typedef struct tag_mem_pool
{
	/** allocator */
	GEN_ALLOCATOR allocator;

	/** init */
	int32   b_init;
	/** alooc mem */
	int32   b_alloc_mem;

	/** align size */
	uint32  align_size;
#if !RUN_OS_64

	/** min pool size */
	uint32  min_pool_size;
	/** max pool size */
	uint32  max_pool_size;
	/** total pool size */
	uint32  total_pool_size;

#else

	/** min pool size */
	uint64  min_pool_size;
	/** max pool size */
	uint64  max_pool_size;
	/** total pool size */
	uint64  total_pool_size;

#endif

#if !RUN_OS_64
	/** total unit num */
	int32   total_unit_num;
#else
	/** total unit num */
	int64   total_unit_num;
#endif
	/** head pointer */
	MEM_POOL_UNIT* p_unit_head;
	/** rear pointer */
	MEM_POOL_UNIT* p_unit_rear;
	/** unit head */
	MEM_POOL_UNIT  unit_head;
	/** unit rear */
	MEM_POOL_UNIT  unit_rear;

	/** use map */
	MEM_POOL_BLOCK_MAP use_map;

	/** free map */
	MEM_POOL_BLOCK_MAP free_map;

} MEM_POOL, *PMEM_POOL;


int32 mem_pool_map_delete_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node);
int32 mem_pool_use_map_insert_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node);
int32 mem_pool_free_map_insert_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node);

/////////////////////////////////// allocator /////////////////////////////////
#if !RUN_OS_64
static void* mem_pool_inner_malloc(uint32 size, void* p_param)
#else
static void* mem_pool_inner_malloc(uint64 size, void* p_param)
#endif
{
	return malloc(size);
}

static int32 mem_pool_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/////////////////////////////////// comparer //////////////////////////////////
/**  
 * @brief compare two buf addr
 * @param [in] data1, buf1 addr
 * @param [in] data2, buf2 addr
 * @param [in] p_param, input context
 * @param [out] p_result, compare result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. result meaning:
 *    -1: data1 < data2
 *     0: data1 = data2
 *     1: data1 > data2
 * 2. compare base on addr(unsigned int)
 */
static int32 mem_pool_use_block_compare(void* data1, void* data2, void* p_param, int32* p_result)
{
#if !RUN_OS_64

	if( (uint32)data1 < (uint32)data2 )
	{
		*p_result = -1;

	}else if( (uint32)data1 == (uint32)data2 )
	{
		*p_result = 0;

	}else
	{
		*p_result = 1;
	}

#else

	if( (uint64)data1 < (uint64)data2 )
	{
		*p_result = -1;

	}else if( (uint64)data1 == (uint64)data2 )
	{
		*p_result = 0;

	}else
	{
		*p_result = 1;
	}

#endif

	return 0;
}

/**  
 * @brief compare free block
 * @param [in] p_block1, block1
 * @param [in] p_block2, block2
 * @param [out] p_result, result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. result meaning:
 *    -1: block1 < block2
 *     0: block1 = block2
 *     1: block1 > block2
 * 2. rule:
 * 2.1 compare block buf size, the longer is bigger
 * 2.2 if the buf size is same, then compare two block buf addr, the bigger is bigger
 */
static int32 mem_pool_free_block_compare(void* data1, void* data2, void* p_param, int32* p_result)
{
	MEM_POOL_BLOCK* p_block1 = data1;
	MEM_POOL_BLOCK* p_block2 = data2;

	if( p_block1->buf_size < p_block2->buf_size )
	{
		*p_result = -1;

	}else if( p_block1->buf_size > p_block2->buf_size )
	{
		*p_result = 1;

	}else
	{
		if( p_block1->p_buf < p_block2->p_buf )
		{
			*p_result = -1;

		}else if( p_block1->p_buf > p_block2->p_buf )
		{
			*p_result = 1;

		}else
		{
			*p_result = 0;
		}
	}

	return 0;
}

/////////////////////////////// unit block list ///////////////////////////////
/**  
 * @brief init unit block list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_init(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	p_unit->total_block_num = 0;
	p_unit->use_block_num = 0;
	p_unit->free_block_num = 0;

	p_unit->p_block_head = &p_unit->block_head;
	p_unit->p_block_rear = &p_unit->block_rear;

	memset(p_unit->p_block_head, 0, sizeof(MEM_POOL_BLOCK));
	p_unit->p_block_head->p_prev_block = NULL;
	p_unit->p_block_head->p_next_block = p_unit->p_block_rear;

	memset(p_unit->p_block_rear, 0, sizeof(MEM_POOL_BLOCK));
	p_unit->p_block_rear->p_prev_block = p_unit->p_block_head;
	p_unit->p_block_rear->p_next_block = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear unit block list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_clear(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	p_unit->p_block_head->p_next_block = p_unit->p_block_rear;
	p_unit->p_block_rear->p_prev_block = p_unit->p_block_head;

	p_unit->total_block_num = 0;
	p_unit->use_block_num = 0;
	p_unit->free_block_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief clear unit block list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. clear with update map
 */
static int32 mem_pool_unit_block_list_clear2(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	MEM_POOL_BLOCK* p_block = NULL;

	int32 ret;

	p_block = p_unit->p_block_head->p_next_block;
	while( p_block != p_unit->p_block_rear )
	{
		if( p_block->b_free )
		{
			/** remove from free map */
			ret = mem_pool_map_delete_node(p_pool, &p_pool->free_map, p_block);

		}else
		{
			/** remove from use map */
			ret = mem_pool_map_delete_node(p_pool, &p_pool->use_map, p_block);
		}

		p_block = p_block->p_next_block;
	}

	p_unit->p_block_head->p_next_block = p_unit->p_block_rear;
	p_unit->p_block_rear->p_prev_block = p_unit->p_block_head;

	p_unit->total_block_num = 0;
	p_unit->use_block_num = 0;
	p_unit->free_block_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit unit block list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_deinit(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	int32 ret;

	if( p_unit->total_block_num > 0 )
	{
		mem_pool_unit_block_list_clear(p_pool, p_unit);
	}

	if( p_unit->p_block_head )
	{
		p_unit->p_block_head = NULL;
	}

	if( p_unit->p_block_rear )
	{
		p_unit->p_block_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief push back into unit block list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_push_back(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit, MEM_POOL_BLOCK* p_block)
{
	p_block->p_prev_block = p_unit->p_block_rear->p_prev_block;
	p_block->p_next_block = p_unit->p_block_rear;
	p_unit->p_block_rear->p_prev_block->p_next_block = p_block;
	p_unit->p_block_rear->p_prev_block = p_block;

#if 1
	if( p_block->b_free )
	{
		p_unit->free_block_num++;

	}else
	{
		p_unit->use_block_num++;
	}
#endif

	p_unit->total_block_num++;

	return GEN_S_OK;
}

/**  
 * @brief insert unit block list before specify block
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @param [in] p_before, specify block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_insert_before(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit, MEM_POOL_BLOCK* p_block, MEM_POOL_BLOCK* p_before)
{
	p_block->p_prev_block = p_before->p_prev_block;
	p_block->p_next_block = p_before;
	p_before->p_prev_block->p_next_block = p_block;
	p_before->p_prev_block = p_block;

#if 1
	if( p_block->b_free )
	{
		p_unit->free_block_num++;

	}else
	{
		p_unit->use_block_num++;
	}
#endif

	p_unit->total_block_num++;

	return GEN_S_OK;
}

/**  
 * @brief insert into unit block list after specify block
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @param [in] p_after, specify block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_insert_after(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit, MEM_POOL_BLOCK* p_block, MEM_POOL_BLOCK* p_after)
{
	p_block->p_prev_block = p_after;
	p_block->p_next_block = p_after->p_next_block;
	p_after->p_next_block->p_prev_block = p_block;
	p_after->p_next_block = p_block;

#if 1
	if( p_block->b_free )
	{
		p_unit->free_block_num++;

	}else
	{
		p_unit->use_block_num++;
	}
#endif

	p_unit->total_block_num++;

	return GEN_S_OK;
}

/**  
 * @brief delete block
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_block_list_delete(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit, MEM_POOL_BLOCK* p_block)
{
	//int32 ret;

	p_block->p_prev_block->p_next_block = p_block->p_next_block;
	p_block->p_next_block->p_prev_block = p_block->p_prev_block;

#if 1
	if( p_block->b_free )
	{
		p_unit->free_block_num--;

	}else
	{
		p_unit->use_block_num--;
	}
#endif

	p_unit->total_block_num--;

	return GEN_S_OK;
}

/////////////////////////////////// unit //////////////////////////////////////
/**  
 * @brief create unit
 * @param [in] p_pool, pool
 * @param [in] unit_size, unit size
 * @param [out] pp_unit, pointer to unit pointer
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. unit size must large than sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK) + p_pool->align_size
 */
#if !RUN_OS_64
static int32 mem_pool_unit_create(MEM_POOL* p_pool, uint32 unit_size, MEM_POOL_UNIT** pp_unit)
#else
static int32 mem_pool_unit_create(MEM_POOL* p_pool, uint64 unit_size, MEM_POOL_UNIT** pp_unit)
#endif
{
	MEM_POOL_UNIT*  p_unit = NULL;
	MEM_POOL_BLOCK*  p_block = NULL;

#if !RUN_OS_64

	uint32  block_size;
	uint32  buf_size;
	uint32 addr_left_size;

#else

	uint64  block_size;
	uint64  buf_size;
	uint64  addr_left_size;

#endif

	uint8*  p_mem_buf = NULL;
	uint8*  p_uint8 = NULL;

	//uint32 padding_size;


	int32  ret;

	p_unit = (MEM_POOL_UNIT*)p_pool->allocator.allocate(unit_size, p_pool->allocator.p_param);
	if( p_unit == NULL )
	{
		*pp_unit = NULL;
		return GEN_E_NOT_ENOUGH_MEM;
	}

	block_size = unit_size - sizeof(MEM_POOL_UNIT);

	//buf_size = unit_size - (sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK) + p_pool->align_size);

	p_mem_buf = (uint8*)p_unit;

	/** unit */
	p_unit->unit_size = unit_size;
	p_unit->total_block_num = 0;
	p_unit->use_block_num = 0;
	p_unit->free_block_num = 0;

	/** first block */
	p_block = (MEM_POOL_BLOCK*)(p_mem_buf + sizeof(MEM_POOL_UNIT));
	p_block->b_free = 1;
	p_block->block_size = block_size;

	p_block->p_buf = p_mem_buf + sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK);

#if !RUN_OS_64
	addr_left_size = (uint32)p_block->p_buf % p_pool->align_size;
#else
	addr_left_size = (uint64)p_block->p_buf % p_pool->align_size;
#endif

	if( addr_left_size != 0 )
	{
		p_block->p_buf += (p_pool->align_size - addr_left_size);
	}
	p_block->buf_size = p_mem_buf + unit_size - p_block->p_buf;

	p_block->p_unit = p_unit;
	p_block->p_prev_block = NULL;
	p_block->p_next_block = NULL;

	/** add to block list */
	mem_pool_unit_block_list_init(p_pool, p_unit);
	ret = mem_pool_unit_block_list_push_back(p_pool, p_unit, p_block);

	//log_debug(_T("[mem_pool::mem_pool_unit_create] p_unit = 0x%x, unit_size = %u, p_block = 0x%x, block_size = %u, p_buf = 0x%x, buf_size = %u\n"),
	//	p_unit, unit_size, p_block, block_size, p_block->p_buf, p_block->buf_size);


	*pp_unit = p_unit;

	return GEN_S_OK;
}

/**  
 * @brief destroy unit
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_destroy(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	int32  ret;

	mem_pool_unit_block_list_deinit(p_pool, p_unit);

	ret = p_pool->allocator.deallocate(p_unit, p_pool->allocator.p_param);

	return GEN_S_OK;
}

/**  
 * @brief reset unit
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_reset(MEM_POOL* p_pool, MEM_POOL_UNIT* p_unit)
{
	MEM_POOL_BLOCK*  p_block = NULL;

#if !RUN_OS_64

	uint32  block_size;
	uint32  buf_size;

#else

	uint64  block_size;
	uint64  buf_size;

#endif

	uint8*  p_mem_buf = NULL;

	int32  ret;

	mem_pool_unit_block_list_clear(p_pool, p_unit);

	block_size = p_unit->unit_size - sizeof(MEM_POOL_UNIT);
	buf_size = p_unit->unit_size - (sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK));
	p_mem_buf = (uint8*)p_unit;

	/** first block */
	p_block = (MEM_POOL_BLOCK*)(p_mem_buf + sizeof(MEM_POOL_UNIT));
	p_block->b_free = 1;
	p_block->block_size = block_size;
	p_block->p_buf = p_mem_buf + sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK);
	p_block->buf_size = buf_size;

	p_block->p_unit = p_unit;
	p_block->p_prev_block = NULL;
	p_block->p_next_block = NULL;

	/** add to block list */
	ret = mem_pool_unit_block_list_push_back(p_pool, p_unit, p_block);

	//p_unit->free_block_num++;

	return GEN_S_OK;
}

/////////////////////////////////// unit list /////////////////////////////////
/**  
 * @brief init unit list
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_init(MEM_POOL* p_pool)
{
	p_pool->total_unit_num = 0;

	p_pool->p_unit_head = &p_pool->unit_head;
	p_pool->p_unit_rear = &p_pool->unit_rear;

	memset(p_pool->p_unit_head, 0, sizeof(MEM_POOL_UNIT));
	p_pool->p_unit_head->p_prev_unit = NULL;
	p_pool->p_unit_head->p_next_unit = p_pool->p_unit_rear;

	memset(p_pool->p_unit_rear, 0, sizeof(MEM_POOL_UNIT));
	p_pool->p_unit_rear->p_prev_unit = p_pool->p_unit_head;
	p_pool->p_unit_rear->p_next_unit = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear unit list
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_clear(MEM_POOL* p_pool)
{
	MEM_POOL_UNIT *p_unit = NULL;
	MEM_POOL_UNIT *p_temp = NULL;

	int32 ret;

	p_unit = p_pool->p_unit_head->p_next_unit;
	while( p_unit != p_pool->p_unit_rear )
	{
		p_temp = p_unit->p_next_unit;
		ret = p_pool->allocator.deallocate(p_unit, p_pool->allocator.p_param);
		p_unit = p_temp;
	}

	p_pool->p_unit_head->p_next_unit = p_pool->p_unit_rear;
	p_pool->p_unit_rear->p_prev_unit = p_pool->p_unit_head;
	p_pool->total_unit_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit unit list
 * @param [in] p_pool, pool
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_deinit(MEM_POOL* p_pool)
{
	int32 ret;

	if( p_pool->total_unit_num > 0 )
	{
		mem_pool_unit_list_clear(p_pool);
	}

	if( p_pool->p_unit_head )
	{
		p_pool->p_unit_head = NULL;
	}

	if( p_pool->p_unit_rear )
	{
		p_pool->p_unit_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief push back unit to unit list
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_push_back(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit)
{
	p_unit->p_prev_unit = p_pool->p_unit_rear->p_prev_unit;
	p_unit->p_next_unit = p_pool->p_unit_rear;
	p_pool->p_unit_rear->p_prev_unit->p_next_unit = p_unit;
	p_pool->p_unit_rear->p_prev_unit = p_unit;

	p_pool->total_unit_num++;

	return GEN_S_OK;
}

/**  
 * @brief delete unit
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_delete(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit)
{
	int32 ret;

	mem_pool_unit_block_list_clear2(p_pool, p_unit);

	p_unit->p_prev_unit->p_next_unit = p_unit->p_next_unit;
	p_unit->p_next_unit->p_prev_unit = p_unit->p_prev_unit;

	ret = p_pool->allocator.deallocate(p_unit, p_pool->allocator.p_param);

	p_pool->total_unit_num--;

	return GEN_S_OK;
}

/**  
 * @brief split block
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @param [in] split_size, split size
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
static int32 mem_pool_unit_list_split_block(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit, MEM_POOL_BLOCK* p_block, uint32 split_size)
#else
static int32 mem_pool_unit_list_split_block(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit, MEM_POOL_BLOCK* p_block, uint64 split_size)
#endif
{
	MEM_POOL_BLOCK* p_origin = p_block;
	MEM_POOL_BLOCK* p_new = NULL;

#if !RUN_OS_64

	uint32  new_block_size;
	uint32  need_size;
	uint32  addr_left_size;

#else

	uint64  new_block_size;
	uint64  need_size;
	uint64  addr_left_size;

#endif

	int32 ret;

	if( p_origin->buf_size < split_size )
	{
		return GEN_E_FAIL;
	}

	/** remove from free map */
	ret = mem_pool_map_delete_node(p_pool, &p_pool->free_map, p_block);

	new_block_size = p_origin->buf_size - split_size;

	/** update */
	p_origin->b_free = 0;
	//p_origin->block_size = sizeof(MEM_POOL_BLOCK) + split_size;
	//p_origin->buf_size = split_size;

	p_unit->use_block_num++;
	p_unit->free_block_num--;

	need_size = sizeof(MEM_POOL_BLOCK);
	addr_left_size = need_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		need_size += (p_pool->align_size - addr_left_size);
	}

	if( new_block_size > need_size )
	{
		p_origin->block_size -= new_block_size;
		p_origin->buf_size = split_size;

		p_new = (MEM_POOL_BLOCK*)(p_origin->p_buf + split_size);

		p_new->b_free = 1;
		p_new->block_size = new_block_size;

		p_new->p_buf = ((int8*)p_new) + need_size;
		p_new->buf_size = new_block_size - need_size;

		p_new->p_unit = p_unit;

		/** insert into block list */
		ret = mem_pool_unit_block_list_insert_after(p_pool, p_unit, p_new, p_origin);

		/** add to free map */
		ret = mem_pool_free_map_insert_node(p_pool, &p_pool->free_map, p_new);
	}

	/** add to use map */
	ret = mem_pool_use_map_insert_node(p_pool, &p_pool->use_map, p_origin);

	return GEN_S_OK;
}

/**  
 * @brief merge block
 * @param [in] p_pool, pool
 * @param [in] p_unit, unit
 * @param [in] p_block, block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_unit_list_merge_block(MEM_POOL *p_pool, MEM_POOL_UNIT *p_unit, MEM_POOL_BLOCK* p_block)
{
	MEM_POOL_BLOCK* p_origin = NULL;
	MEM_POOL_BLOCK* p_origin_prev = NULL;
	MEM_POOL_BLOCK* p_origin_next = NULL;
	MEM_POOL_BLOCK* p_prev = NULL;
	MEM_POOL_BLOCK* p_next = NULL;
	MEM_POOL_BLOCK* p_new = NULL;
	MEM_POOL_BLOCK* p_temp = NULL;

	int32  ret;

	/** remove from use map */
	ret = mem_pool_map_delete_node(p_pool, &p_pool->use_map, p_block);

	p_origin_prev = p_block->p_prev_block;
	p_origin_next = p_block->p_next_block;
	p_new = p_block;

	/** merge prev */
	p_origin = p_block;
	p_prev = p_origin_prev;
	while( p_prev != p_unit->p_block_head )
	{
		p_temp = p_prev->p_prev_block;
		if( !p_prev->b_free )
		{
			break;
		}

		/** remove prev from free map */
		ret = mem_pool_map_delete_node(p_pool, &p_pool->free_map, p_prev);

		/** remove origin from block list */
		ret = mem_pool_unit_block_list_delete(p_pool, p_unit, p_origin);

		p_new = p_prev;
		p_new->block_size += p_origin->block_size;
		p_new->buf_size += p_origin->block_size;

		p_origin = p_prev;
		p_prev = p_temp;
	}

	/** merge next */
	//p_origin = p_new;
	p_next = p_origin_next;
	while( p_next != p_unit->p_block_rear )
	{
		p_temp = p_next->p_next_block;
		if( !p_next->b_free )
		{
			break;
		}

		/** remove next from free map */
		ret = mem_pool_map_delete_node(p_pool, &p_pool->free_map, p_next);

		/** remove next from block list */
		ret = mem_pool_unit_block_list_delete(p_pool, p_unit, p_next);

		p_new->block_size += p_next->block_size;
		p_new->buf_size += p_next->block_size;

		p_next = p_temp;
	}

	/** add to free map */
	ret = mem_pool_free_map_insert_node(p_pool, &p_pool->free_map, p_new);

	return GEN_S_OK;
}

////////////////////////////////// map list ///////////////////////////////////
/**  
 * @brief init map list
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_init(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	p_map->p_seq_head = &p_map->seq_head;
	p_map->p_seq_rear = &p_map->seq_rear;

	memset(p_map->p_seq_head, 0, sizeof(MEM_POOL_BLOCK));
	p_map->p_seq_head->p_prev_seq = NULL;
	p_map->p_seq_head->p_next_seq = p_map->p_seq_rear;

	memset(p_map->p_seq_rear, 0, sizeof(MEM_POOL_BLOCK));
	p_map->p_seq_rear->p_prev_seq = p_map->p_seq_head;
	p_map->p_seq_rear->p_next_seq = NULL;

	return GEN_S_OK;
}

/**  
 * @brief clear map list
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_clear(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	p_map->p_seq_head->p_next_seq = p_map->p_seq_rear;
	p_map->p_seq_rear->p_prev_seq = p_map->p_seq_head;

	return GEN_S_OK;
}

/**  
 * @brief deinit map list
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_deinit(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	int32 ret;

	mem_pool_map_list_clear(p_pool, p_map);

	if( p_map->p_seq_head )
	{
		p_map->p_seq_head = NULL;
	}

	if( p_map->p_seq_rear )
	{
		p_map->p_seq_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief get map list front block
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [out] pp_block, pointer to block pointer
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_get_front(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK** pp_block)
{
	if( p_map->block_num < 1 )
	{
		*pp_block = NULL;
		return -1;
	}

	*pp_block = p_map->p_seq_head->p_next_seq;

	return GEN_S_OK;
}

/**  
 * @brief push to map list front
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_block, block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_push_front(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_block)
{
	p_block->p_prev_seq = p_map->p_seq_head;
	p_block->p_next_seq = p_map->p_seq_head->p_next_seq;
	p_map->p_seq_head->p_next_seq->p_prev_seq = p_block;
	p_map->p_seq_head->p_next_seq = p_block;

	return GEN_S_OK;
}

/**  
 * @brief get map list back block
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [out] pp_block, pointer to block pointer
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_get_back(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK** pp_block)
{
	if( p_map->block_num < 1 )
	{
		*pp_block = NULL;
		return -1;
	}

	*pp_block = p_map->p_seq_rear->p_prev_seq;

	return GEN_S_OK;
}

/**  
 * @brief insert into map list before specify block
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_block, block
 * @param [in] p_before, specify block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_insert_before(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_block, MEM_POOL_BLOCK* p_before)
{
	p_block->p_prev_seq = p_before->p_prev_seq;
	p_block->p_next_seq = p_before;
	p_before->p_prev_seq->p_next_seq = p_block;
	p_before->p_prev_seq = p_block;

	return GEN_S_OK;
}

/**  
 * @brief insert into map list after specify block
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_block, block
 * @param [in] p_after, specify block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_insert_after(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_block, MEM_POOL_BLOCK* p_after)
{
	p_block->p_prev_seq = p_after;
	p_block->p_next_seq = p_after->p_next_seq;
	p_after->p_next_seq->p_prev_seq = p_block;
	p_after->p_next_seq = p_block;

	return GEN_S_OK;
}

/**  
 * @brief delete from map list
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_block, block
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_list_delete(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_block)
{
	p_block->p_prev_seq->p_next_seq = p_block->p_next_seq;
	p_block->p_next_seq->p_prev_seq = p_block->p_prev_seq;

	return GEN_S_OK;
}

///////////////////////////////////// map /////////////////////////////////////
/**  
 * @brief find insert pos in use map
 * @param [in] p_map, map
 * @param [in] p_key, key
 * @param [out] pp_node, pointer to node pointer
 * @param [out] p_left, left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. return value meaning:
 *   0: the node according to the key not exist, *pp_node means parent node of insert pos, if *pp_node is null, the insert pos is root
 *   GEN_E_ALREADY_EXIST: the node according to the key already exist, *pp_node means the node
 */
static int32 mem_pool_use_map_find_insert_pos(MEM_POOL_BLOCK_MAP* p_map, void* p_key, MEM_POOL_BLOCK** pp_node, int32* p_left)
{
	MEM_POOL_BLOCK* p_parent;
	MEM_POOL_BLOCK* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	p_parent = NULL;
	p_node = p_map->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = mem_pool_use_block_compare(p_key, p_node->p_buf, NULL, &result);
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
		*pp_node = p_node;
		return GEN_E_ALREADY_EXIST;

	}else
	{
		*pp_node = p_parent;
		return GEN_S_OK;
	}
}

/**  
 * @brief find insert pos in free map
 * @param [in] p_map, map
 * @param [in] p_key, key
 * @param [out] pp_node, pointer to node pointer
 * @param [out] p_left, left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. return value meaning:
 *   0: the node according to the key not exist, *pp_node means parent node of insert pos, if *pp_node is null, the insert pos is root
 *   GEN_E_ALREADY_EXIST: the node according to the key already exist, *pp_node means the node
 */
static int32 mem_pool_free_map_find_insert_pos(MEM_POOL_BLOCK_MAP* p_map, void* p_key, MEM_POOL_BLOCK** pp_node, int32* p_left)
{
	MEM_POOL_BLOCK* p_parent;
	MEM_POOL_BLOCK* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	p_parent = NULL;
	p_node = p_map->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = mem_pool_free_block_compare(p_key, p_node, NULL, &result);
		if( result == 0 )
		{
			/** 已有对应节点 */
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
		*pp_node = p_node;
		return GEN_E_ALREADY_EXIST;

	}else
	{
		*pp_node = p_parent;
		return GEN_S_OK;
	}
}

/**  
 * @brief find delete pos
 * @param [in] p_map, map
 * @param [in] p_node, node to delete
 * @param [out] pp_node, actual delete pos of the node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. left child of the node to delete must be NOT NULL!
 */
static int32 mem_pool_map_find_delete_pos(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node, MEM_POOL_BLOCK** pp_node)
{
	MEM_POOL_BLOCK* p_current = p_node;
	MEM_POOL_BLOCK* p_temp_node;

	int32 ret;

	/** find the right most node of left child, it is biggest node of left child */
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
 * @param [in] p_map, map
 * @param [in] p_a, ancestor
 * @param [in] p_b, parent
 * @param [in] p_x, child
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before:
 *                              a
 *                             /
 *                            b
 *                           /
 *                          x
 * after:
 *                              b
 *                             / \
 *                            x   a
 */
static int32 mem_pool_map_left_rotate(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_a, MEM_POOL_BLOCK* p_b, MEM_POOL_BLOCK* p_x)
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
		p_map->p_root = p_b;

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
 * @param [in] p_map, map
 * @param [in] p_a, ancestor
 * @param [in] p_b, parent
 * @param [in] p_x, child
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before:
 *                              a
 *                             /
 *                            b
 *                             \
 *                              x
 * after:
 *                              a
 *                             /
 *                            x
 *                           /
 *                          b
 */
static int32 mem_pool_map_left_adjust(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_a, MEM_POOL_BLOCK* p_b, MEM_POOL_BLOCK* p_x)
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
 * @param [in] p_map, map
 * @param [in] p_a, parent
 * @param [in] p_b, current
 * @param [in] p_x, child
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before:
 *                              a
 *                               \
 *                                b
 *                                 \
 *                                  x
 * after:
 *                              b
 *                             / \
 *                            a   x
 */
static int32 mem_pool_map_right_rotate(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_a, MEM_POOL_BLOCK* p_b, MEM_POOL_BLOCK* p_x)
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
		p_map->p_root = p_b;

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
 * @param [in] p_map, map
 * @param [in] p_a, ancestor
 * @param [in] p_b, parent
 * @param [in] p_x, child
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1.
 * before:
 *                              a
 *                               \
 *                                b
 *                               /
 *                              x
 * after:
 *                              a
 *                               \
 *                                x
 *                                 \
 *                                  b
 */
static int32 mem_pool_map_right_adjust(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_a, MEM_POOL_BLOCK* p_b, MEM_POOL_BLOCK* p_x)
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
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. see http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_pool_map_insert_balance(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node)
{
	MEM_POOL_BLOCK* p_ancestor;

	MEM_POOL_BLOCK* p_uncle;
	MEM_POOL_BLOCK* p_current;

	int32 ret;

	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			p_current->color = MEM_POOL_COLOR_BLACK;
			break;
		}

		if( p_current->p_parent->color == MEM_POOL_COLOR_BLACK )
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
		if( p_uncle && p_uncle->color == MEM_POOL_COLOR_RED )
		{
			/** case 3 */
			p_ancestor->color = MEM_POOL_COLOR_RED;
			p_current->p_parent->color = MEM_POOL_COLOR_BLACK;
			p_uncle->color = MEM_POOL_COLOR_BLACK;

			p_current = p_ancestor;
			continue;
		}

		/** from above, uncle is null or always black */
		if( (p_current == p_current->p_parent->p_right) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 4(left) */
			ret = mem_pool_map_left_adjust(p_map, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_left;

		}else if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_right) )
		{
			/** case 4(right) */
			ret = mem_pool_map_right_adjust(p_map, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_right;
		}

		p_current->p_parent->color = MEM_POOL_COLOR_BLACK;
		p_ancestor->color = MEM_POOL_COLOR_RED;

		if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 5(left) */
			ret = mem_pool_map_left_rotate(p_map, p_ancestor, p_current->p_parent, p_current);

		}else
		{
			/** case 5(right) */
			ret = mem_pool_map_right_rotate(p_map, p_ancestor, p_current->p_parent, p_current);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief delete and balance
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. see http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_pool_map_delete_balance(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node)
{
	MEM_POOL_BLOCK* p_sibling;
	MEM_POOL_BLOCK* p_sibling_left;
	MEM_POOL_BLOCK* p_sibling_right;
	MEM_POOL_BLOCK* p_current;

	int32 ret;

	/** from above, p_node is black */
	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			//p_current->color = MEM_POOL_COLOR_BLACK;
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
		if( p_sibling->color == MEM_POOL_COLOR_RED )
		{
			/** case 2 */
			p_current->p_parent->color = MEM_POOL_COLOR_RED;
			p_sibling->color = MEM_POOL_COLOR_BLACK;

			if( p_current == p_current->p_parent->p_left )
			{
				ret = mem_pool_map_right_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_right);
				p_sibling = p_sibling_left;

			}else
			{
				ret = mem_pool_map_left_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_left);
				p_sibling = p_sibling_right;
			}
		}

		/** from above, sibling always black */
		if( (p_current->p_parent->color == MEM_POOL_COLOR_BLACK)
			&& (p_sibling->color == MEM_POOL_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK) )
		{
			/** case 3 */
			p_sibling->color = MEM_POOL_COLOR_RED;
			p_current = p_current->p_parent;
			continue;
		}

		if( (p_current->p_parent->color == MEM_POOL_COLOR_RED)
			&& (p_sibling->color == MEM_POOL_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK) )
		{
			/** case 4 */
			p_sibling->color = MEM_POOL_COLOR_RED;
			p_current->p_parent->color = MEM_POOL_COLOR_BLACK;

			break;
		}

		/** from case 3 & 4, sibling's child can not both be null */
		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		//if( p_sibling->color == MEM_POOL_COLOR_BLACK )
		{
			if( (p_current == p_current->p_parent->p_left)
				&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK)
				&& (p_sibling_left && p_sibling->p_left->color == MEM_POOL_COLOR_RED) )
			{
				/** case 5(left) */
				p_sibling->color = MEM_POOL_COLOR_RED;
				p_sibling->p_left->color = MEM_POOL_COLOR_BLACK;
				ret = mem_pool_map_left_rotate(p_map, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

				p_sibling = p_sibling_left;

			}else if( (p_current == p_current->p_parent->p_right)
				&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
				&& (p_sibling_right && p_sibling->p_right->color == MEM_POOL_COLOR_RED) )
			{
				/** case 5(right) */
				p_sibling->color = MEM_POOL_COLOR_RED;
				p_sibling->p_right->color = MEM_POOL_COLOR_BLACK;
				ret = mem_pool_map_right_rotate(p_map, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

				p_sibling = p_sibling_right;
			}
		}

		p_sibling->color = p_current->p_parent->color;
		p_current->p_parent->color = MEM_POOL_COLOR_BLACK;

		if( p_current == p_current->p_parent->p_left )
		{
			/** case 6(left) */
			if( p_sibling->p_right )
			{
				p_sibling->p_right->color = MEM_POOL_COLOR_BLACK;
			}

			ret = mem_pool_map_right_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_right);

		}else
		{
			/** case 6(right) */
			if( p_sibling->p_left )
			{
				p_sibling->p_left->color = MEM_POOL_COLOR_BLACK;
			}

			ret = mem_pool_map_left_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_left);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief delete and balance(the node is null)
 * @param [in] p_map, map
 * @param [in] p_parent, parent node
 * @param [in] b_left, left child of parent or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. see http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 mem_pool_map_delete_balance2(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_parent, int32 b_left)
{
	MEM_POOL_BLOCK* p_sibling;
	MEM_POOL_BLOCK* p_sibling_left;
	MEM_POOL_BLOCK* p_sibling_right;

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
	if( p_sibling->color == MEM_POOL_COLOR_RED )
	{
		/** case 2 */
		p_parent->color = MEM_POOL_COLOR_RED;
		p_sibling->color = MEM_POOL_COLOR_BLACK;

		if( b_left )
		{
			ret = mem_pool_map_right_rotate(p_map, p_parent, p_sibling, p_sibling->p_right);
			p_sibling = p_sibling_left;

		}else
		{
			ret = mem_pool_map_left_rotate(p_map, p_parent, p_sibling, p_sibling->p_left);
			p_sibling = p_sibling_right;
		}
	}

	/** from above, sibling always black */
	if( (p_parent->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK) )
	{
		/** case 3 */
		p_sibling->color = MEM_POOL_COLOR_RED;

		return mem_pool_map_delete_balance(p_map, p_parent);
	}

	if( (p_parent->color == MEM_POOL_COLOR_RED)
		&& (p_sibling->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK) )
	{
		/** case 4 */
		p_sibling->color = MEM_POOL_COLOR_RED;
		p_parent->color = MEM_POOL_COLOR_BLACK;

		return GEN_S_OK;
	}

	/** from case 3 & 4, sibling's child can not both be null */
	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;

	if( (b_left)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling_left && p_sibling->p_left->color == MEM_POOL_COLOR_RED) )
	{
		/** case 5(left) */
		p_sibling->color = MEM_POOL_COLOR_RED;
		p_sibling->p_left->color = MEM_POOL_COLOR_BLACK;
		ret = mem_pool_map_left_rotate(p_map, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

		p_sibling = p_sibling_left;

	}else if( (!b_left)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == MEM_POOL_COLOR_BLACK)
		&& (p_sibling_right && p_sibling->p_right->color == MEM_POOL_COLOR_RED) )
	{
		/** case 5(right) */
		p_sibling->color = MEM_POOL_COLOR_RED;
		p_sibling->p_right->color = MEM_POOL_COLOR_BLACK;
		ret = mem_pool_map_right_rotate(p_map, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

		p_sibling = p_sibling_right;
	}

	p_sibling->color = p_parent->color;
	p_parent->color = MEM_POOL_COLOR_BLACK;

	if( b_left )
	{
		/** case 6(left) */
		if( p_sibling->p_right )
		{
			p_sibling->p_right->color = MEM_POOL_COLOR_BLACK;
		}

		ret = mem_pool_map_right_rotate(p_map, p_parent, p_sibling, p_sibling->p_right);

	}else
	{
		/** case 6(right) */
		if( p_sibling->p_left )
		{
			p_sibling->p_left->color = MEM_POOL_COLOR_BLACK;
		}

		ret = mem_pool_map_left_rotate(p_map, p_parent, p_sibling, p_sibling->p_left);
	}

	return GEN_S_OK;
}

/**  
 * @brief swap two node in map
 * @param [in] p_map, map
 * @param [in] p_node1, node1
 * @param [in] p_node2, node2
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. swap with:
 * - color
 * - p_parent
 * - p_left
 * - p_right
 * - pointer to this node of parent and child node
 * - pointer in list
 */
static int32 mem_pool_map_swap_node(MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node1, MEM_POOL_BLOCK* p_node2)
{
	MEM_POOL_BLOCK temp;
	int32 b_parent_12;
	int32 b_parent_21;
	int32 b_left1;
	int32 b_left2;

	if( p_node1 == p_node2 )
	{
		/** same node */
		return GEN_S_OK;
	}

	if( p_node1 == p_node2->p_parent )
	{
		b_parent_12 = 1;

	}else
	{
		b_parent_12 = 0;
	}

	if( p_node2 == p_node1->p_parent )
	{
		b_parent_21 = 1;

	}else
	{
		b_parent_21 = 0;
	}

	if( p_node1->p_parent )
	{
		if( p_node1 == p_node1->p_parent->p_left )
		{
			b_left1 = 1;

		}else
		{
			b_left1 = 0;
		}
	}

	if( p_node2->p_parent )
	{
		if( p_node2 == p_node2->p_parent->p_left )
		{
			b_left2 = 1;

		}else
		{
			b_left2 = 0;
		}
	}

	memcpy(&temp, p_node1, sizeof(temp));

	/** update node1 */
	if( p_node1->p_parent )
	{
		if( !b_parent_21 )
		{
			if( b_left1 )
			{
				p_node1->p_parent->p_left = p_node2;

			}else
			{
				p_node1->p_parent->p_right = p_node2;
			}
		}
	}

	if( p_node1->p_left )
	{
		if( !(b_parent_12 && p_node2 == p_node1->p_left) )
		{
			p_node1->p_left->p_parent = p_node2;
		}
	}

	if( p_node1->p_right )
	{
		if( !(b_parent_12 && p_node2 == p_node1->p_right) )
		{
			p_node1->p_right->p_parent = p_node2;
		}
	}

	p_node1->color = p_node2->color;

	if( b_parent_12 )
	{
		p_node1->p_parent = p_node2;

	}else
	{
		p_node1->p_parent = p_node2->p_parent;
	}

	if( b_parent_21 && b_left1 )
	{
		p_node1->p_left = p_node2;

	}else
	{
		p_node1->p_left = p_node2->p_left;
	}

	if( b_parent_21 && !b_left1 )
	{
		p_node1->p_right = p_node2;

	}else
	{
		p_node1->p_right = p_node2->p_right;
	}

	/** update node2 */
	if( p_node2->p_parent )
	{
		if( !b_parent_12 )
		{
			if( b_left2 )
			{
				p_node2->p_parent->p_left = p_node1;

			}else
			{
				p_node2->p_parent->p_right = p_node1;
			}
		}
	}

	if( p_node2->p_left )
	{
		if( !(b_parent_21 && p_node1 == p_node2->p_left) )
		{
			p_node2->p_left->p_parent = p_node1;
		}
	}

	if( p_node2->p_right )
	{
		if( !(b_parent_21 && p_node1 == p_node2->p_right) )
		{
			p_node2->p_right->p_parent = p_node1;
		}
	}

	p_node2->color = temp.color;

	if( b_parent_21 )
	{
		p_node2->p_parent = p_node1;

	}else
	{
		p_node2->p_parent = temp.p_parent;
	}

	if( b_parent_12 && b_left2 )
	{
		p_node2->p_left = p_node1;

	}else
	{
		p_node2->p_left = temp.p_left;
	}

	if( b_parent_12 && !b_left2 )
	{
		p_node2->p_right = p_node1;

	}else
	{
		p_node2->p_right = temp.p_right;
	}

	/** update root */
	if( p_node1 == p_map->p_root )
	{
		p_map->p_root = p_node2;

	}else if( p_node2 == p_map->p_root )
	{
		p_map->p_root = p_node1;
	}

#if 0
	/** update list */
	if( p_node1 == p_node2->p_prev_seq )
	{
		/** node1 -> node2 */
		p_node1->p_prev_seq->p_next_seq = p_node2;
		p_node2->p_next_seq->p_prev_seq = p_node1;

		p_node1->p_prev_seq = p_node2;
		p_node1->p_next_seq = p_node2->p_next_seq;

		p_node2->p_prev_seq = temp.p_prev_seq;
		p_node2->p_next_seq = p_node1;

	}else if( p_node1 == p_node2->p_next_seq )
	{
		/** node2 -> node1 */
		p_node2->p_prev_seq->p_next_seq = p_node1;
		p_node1->p_next_seq->p_prev_seq = p_node2;

		p_node1->p_prev_seq = p_node2->p_prev_seq;
		p_node1->p_next_seq = p_node2;

		p_node2->p_prev_seq = p_node1;
		p_node2->p_next_seq = temp.p_next_seq;

	}else
	{
		p_node1->p_prev_seq->p_next_seq = p_node2;
		p_node1->p_next_seq->p_prev_seq = p_node2;

		p_node2->p_prev_seq->p_next_seq = p_node1;
		p_node2->p_next_seq->p_prev_seq = p_node1;

		p_node1->p_prev_seq = p_node2->p_prev_seq;
		p_node1->p_next_seq = p_node2->p_next_seq;

		p_node2->p_prev_seq = temp.p_prev_seq;
		p_node2->p_next_seq = temp.p_next_seq;
	}
#endif

	return GEN_S_OK;
}

/**  
 * @brief init map
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_init(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	int32 ret;

	p_map->block_num = 0;
	p_map->p_root = NULL;

	ret = mem_pool_map_list_init(p_pool, p_map);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit map
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_deinit(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	mem_pool_map_list_deinit(p_pool, p_map);

	p_map->p_root = NULL;
	p_map->block_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief clear map
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_clear(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map)
{
	mem_pool_map_list_clear(p_pool, p_map);

	p_map->p_root = NULL;
	p_map->block_num = 0;

	return GEN_S_OK;
}

/**  
 * @brief find node
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_key, key of node
 * @param [out] pp_node, pointer to node pointer
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_find_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, void* p_key, MEM_POOL_BLOCK** pp_node)
{
	int32 b_left;
	int32 ret;

	ret = mem_pool_use_map_find_insert_pos(p_map, p_key, pp_node, &b_left);
	if( ret == 0 )
	{
		*pp_node = NULL;
		return GEN_E_NOT_FOUND;
	}

	return GEN_S_OK;
}

/**  
 * @brief insert node
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mem_pool_use_map_insert_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node)
{
	MEM_POOL_BLOCK* p_parent = NULL;
	MEM_POOL_BLOCK* p_temp = NULL;

	int32 b_left;
	int32 ret;

	ret = mem_pool_use_map_find_insert_pos(p_map, p_node->p_buf, &p_parent, &b_left);
	if( ret )
	{
		return GEN_E_ALREADY_EXIST;
	}

	p_node->color = MEM_POOL_COLOR_RED;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	if( p_parent == NULL )
	{
		/** root */
		p_node->color = MEM_POOL_COLOR_BLACK;
		p_map->p_root = p_node;
		ret = mem_pool_map_list_push_front(p_pool, p_map, p_node);

	}else
	{
		/** not root */
		if( b_left )
		{
			/** left child */
			p_parent->p_left = p_node;

			ret = mem_pool_map_list_insert_before(p_pool, p_map, p_node, p_parent);

		}else
		{
			/** right child */
			p_parent->p_right = p_node;

			ret = mem_pool_map_list_insert_after(p_pool, p_map, p_node, p_parent);
		}
	}

	ret = mem_pool_map_insert_balance(p_map, p_node);
	p_map->block_num++;

	return GEN_S_OK;
}

/**  
 * @brief insert node for free map
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: suceess
 * other: fail
 * @remark
 */
static int32 mem_pool_free_map_insert_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node)
{
	MEM_POOL_BLOCK* p_parent = NULL;
	MEM_POOL_BLOCK* p_temp = NULL;

	int32 b_left;
	int32 ret;

	ret = mem_pool_free_map_find_insert_pos(p_map, p_node, &p_parent, &b_left);
	if( ret )
	{
		return GEN_E_ALREADY_EXIST;
	}

	p_node->color = MEM_POOL_COLOR_RED;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	if( p_parent == NULL )
	{
		/** root */
		p_node->color = MEM_POOL_COLOR_BLACK;
		p_map->p_root = p_node;
		ret = mem_pool_map_list_push_front(p_pool, p_map, p_node);

	}else
	{
		/** not root */
		if( b_left )
		{
			/** left child */
			p_parent->p_left = p_node;

			ret = mem_pool_map_list_insert_before(p_pool, p_map, p_node, p_parent);

		}else
		{
			/** right child */
			p_parent->p_right = p_node;

			ret = mem_pool_map_list_insert_after(p_pool, p_map, p_node, p_parent);
		}
	}

	ret = mem_pool_map_insert_balance(p_map, p_node);
	p_map->block_num++;

	return GEN_S_OK;
}

/**  
 * @brief delete node
 * @param [in] p_pool, pool
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 */
static int32 mem_pool_map_delete_node(MEM_POOL* p_pool, MEM_POOL_BLOCK_MAP* p_map, MEM_POOL_BLOCK* p_node)
{
	MEM_POOL_BLOCK *p_node2 = NULL;
	MEM_POOL_BLOCK *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_node->p_left == NULL )
	{
		/** left child is null */
		if( p_node->p_parent == NULL )
		{
			/** root */
			p_map->p_root = p_node->p_right;

		}else
		{
			/** not root */
			if( p_node == p_node->p_parent->p_left )
			{
				p_node->p_parent->p_left = p_node->p_right;
				b_left = 1;

			}else
			{
				p_node->p_parent->p_right = p_node->p_right;
				b_left = 0;
			}
		}

		if( p_node->p_right )
		{
			p_node->p_right->p_parent = p_node->p_parent;
		}

		p_child = p_node->p_right;

	}else
	{
		/** left child not null */
		ret = mem_pool_map_find_delete_pos(p_map, p_node, &p_node2);
		ret = mem_pool_map_swap_node(p_map, p_node, p_node2);

		/** right child of the node must be null */
		if( p_node == p_node->p_parent->p_left )
		{
			p_node->p_parent->p_left = p_node->p_left;
			b_left = 1;

		}else
		{
			p_node->p_parent->p_right = p_node->p_left;
			b_left = 0;
		}

		if( p_node->p_left )
		{
			p_node->p_left->p_parent = p_node->p_parent;
		}

		p_child = p_node->p_left;
	}

	if( p_node->color != MEM_POOL_COLOR_RED )
	{
		/** the node is black */
		if( p_child == NULL )
		{
			/** child node is null */
			ret = mem_pool_map_delete_balance2(p_map, p_node->p_parent, b_left);

		}else
		{
			/** child node not null */
			if( p_child->color == MEM_POOL_COLOR_RED )
			{
				/** child is red */
				p_child->color = MEM_POOL_COLOR_BLACK;

			}else
			{
				/** child is black */
				ret = mem_pool_map_delete_balance(p_map, p_child);
			}
		}
	}

	ret = mem_pool_map_list_delete(p_pool, p_map, p_node);

	p_map->block_num--;

	return GEN_S_OK;
}


/////////////////////////////// Outter Interface //////////////////////////////
MEM_POOL_H mem_pool_create(GEN_ALLOCATOR* p_allocator)
{
	MEM_POOL *p_pool = NULL;

	if( p_allocator )
	{
		p_pool = (MEM_POOL *)p_allocator->allocate(sizeof(MEM_POOL), p_allocator->p_param);

	}else
	{
		p_pool = (MEM_POOL *)malloc( sizeof(MEM_POOL) );
	}

	if( p_pool == NULL )
	{
		return NULL;
	}

	memset(p_pool, 0, sizeof(MEM_POOL));

	if( p_allocator )
	{
		memcpy(&p_pool->allocator, p_allocator, sizeof(p_pool->allocator));

	}else
	{
		p_pool->allocator.allocate = mem_pool_inner_malloc;
		p_pool->allocator.deallocate = mem_pool_inner_free;
		p_pool->allocator.p_param = p_pool;
	}

	return p_pool;
}

int32 mem_pool_destroy(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

	int32 ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_pool->b_init )
	{
		mem_pool_deinit(p_pool);
	}

	ret = p_pool->allocator.deallocate(p_pool, p_pool->allocator.p_param);

	return GEN_S_OK;
}

int32 mem_pool_init(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

	int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_pool->b_init )
	{
		return GEN_S_OK;
	}

	p_pool->b_alloc_mem = 0;

	p_pool->align_size = MEM_POOL_ADDR_ALIGN_SIZE;
	p_pool->min_pool_size = MEM_POOL_MIN_POOL_SIZE;
	p_pool->max_pool_size = MEM_POOL_MAX_POOL_SIZE;
	p_pool->total_pool_size = 0;

	/** unit list */
	ret = mem_pool_unit_list_init(p_pool);
	if( ret )
	{
		mem_pool_deinit(p_pool);
		return GEN_E_FAIL;
	}

	/** use map */
	ret = mem_pool_map_init(p_pool, &p_pool->use_map);
	if( ret )
	{
		mem_pool_deinit(p_pool);
		return GEN_E_FAIL;
	}

	/** free map */
	ret = mem_pool_map_init(p_pool, &p_pool->free_map);
	if( ret )
	{
		mem_pool_deinit(p_pool);
		return GEN_E_FAIL;
	}

	p_pool->b_init = 1;

	return GEN_S_OK;
}

int32 mem_pool_deinit(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

	int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	/** use map */
	mem_pool_map_deinit(p_pool, &p_pool->use_map);

	/** free map */
	mem_pool_map_deinit(p_pool, &p_pool->free_map);

	/** unit list */
	mem_pool_unit_list_deinit(p_pool);

	p_pool->b_init = 0;

	return GEN_S_OK;
}

int32 mem_pool_reset(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

	MEM_POOL_UNIT *p_unit = NULL;

	int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** use map */
	mem_pool_map_clear(p_pool, &p_pool->use_map);

	/** free map */
	mem_pool_map_clear(p_pool, &p_pool->free_map);

	/** unit */
	p_unit = p_pool->p_unit_head->p_next_unit;
	while( p_unit != p_pool->p_unit_rear )
	{
		mem_pool_unit_reset(p_pool, p_unit);
		mem_pool_free_map_insert_node(p_pool, &p_pool->free_map, p_unit->p_block_head->p_next_block);

		p_unit = p_unit->p_next_unit;
	}

	return GEN_S_OK;
}

int32 mem_pool_set_align_size(MEM_POOL_H h, uint32 align_size)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

#if !RUN_OS_64
	uint32  addr_left_size;
#else
	uint64  addr_left_size;
#endif

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_pool->total_unit_num > 0 )
	{
		return GEN_E_FAIL;
	}

	if( align_size > MEM_POOL_MAX_ADDR_ALIGN_SIZE )
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

	p_pool->align_size = align_size;

	/** adjust block size */
	addr_left_size = p_pool->min_pool_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		p_pool->min_pool_size += (p_pool->align_size - addr_left_size);
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 mem_pool_set_pool_param(MEM_POOL_H h, uint32 min_pool_size, uint32 max_pool_size)
#else
int32 mem_pool_set_pool_param(MEM_POOL_H h, uint64 min_pool_size, uint64 max_pool_size)
#endif
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

#if !RUN_OS_64
	uint32 addr_left_size;
#else
	uint64 addr_left_size;
#endif

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_pool->total_unit_num > 0 )
	{
		return GEN_E_FAIL;
	}

	if( min_pool_size <= ( sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK) ) || min_pool_size > max_pool_size )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_pool->min_pool_size = min_pool_size;
	p_pool->max_pool_size = max_pool_size;

	addr_left_size = min_pool_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		p_pool->min_pool_size += (p_pool->align_size - addr_left_size);
	}


	p_pool->total_pool_size = 0;

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 mem_pool_alloc_mem(MEM_POOL_H h, uint32 alloc_size)
#else
int32 mem_pool_alloc_mem(MEM_POOL_H h, uint64 alloc_size)
#endif
{
	MEM_POOL *p_pool = (MEM_POOL*)h;
	MEM_POOL_UNIT *p_unit = NULL;

#if !RUN_OS_64
	uint32  max_alloc_size;
	//uint32 addr_left_size;
#else
	uint64  max_alloc_size;
#endif

	int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_pool->total_pool_size >= p_pool->max_pool_size )
	{
		/** exceed */
		return GEN_E_FAIL;
	}

	max_alloc_size = p_pool->max_pool_size - p_pool->total_pool_size;

#if 0
	addr_left_size = alloc_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		alloc_size += (p_pool->align_size - addr_left_size);
	}
#endif

	if( alloc_size <= ( sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK) ) )
	{
		return GEN_E_INVALID_PARAM;
	}

	alloc_size += p_pool->align_size;

	if( alloc_size > max_alloc_size )
	{
		return GEN_E_FAIL;
	}

	/** create new unit */
	ret = mem_pool_unit_create(p_pool, alloc_size, &p_unit);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** update list */
	mem_pool_unit_list_push_back(p_pool, p_unit);
	ret = mem_pool_free_map_insert_node(p_pool, &p_pool->free_map, p_unit->p_block_head->p_next_block);

	p_pool->total_pool_size += alloc_size;
	p_pool->b_alloc_mem = 1;

	return GEN_S_OK;
}

int32 mem_pool_free_mem(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;

	//int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** use map */
	mem_pool_map_clear(p_pool, &p_pool->use_map);

	/** free map */
	mem_pool_map_clear(p_pool, &p_pool->free_map);

	/** unit */
	mem_pool_unit_list_clear(p_pool);

	p_pool->total_pool_size = 0;
	p_pool->b_alloc_mem = 0;

	return GEN_S_OK;
}

int32 mem_pool_free_unuse_mem(MEM_POOL_H h)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;
	MEM_POOL_UNIT *p_unit = NULL;
	MEM_POOL_UNIT *p_temp = NULL;

	int32  ret;

	if( p_pool == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_unit = p_pool->p_unit_head->p_next_unit;
	while( p_unit != p_pool->p_unit_rear )
	{
		p_temp = p_unit->p_next_unit;

		if( p_unit->use_block_num < 1 )
		{
			p_pool->total_pool_size -= p_unit->unit_size;
			ret = mem_pool_unit_list_delete(p_pool, p_unit);
		}

		p_unit = p_temp;
	}

	return GEN_S_OK;
}

#if !RUN_OS_64
int32 mem_pool_get_buf(MEM_POOL_H h, uint32 buf_size, void** pp_buf)
#else
int32 mem_pool_get_buf(MEM_POOL_H h, uint64 buf_size, void** pp_buf)
#endif
{
	MEM_POOL *p_pool = (MEM_POOL*)h;
	MEM_POOL_BLOCK *p_block = NULL;

#if !RUN_OS_64
	uint32  alloc_size;
	uint32  addr_left_size;
#else
	uint64  alloc_size;
	uint64  addr_left_size;
#endif

	int32  ret;

	if( p_pool == NULL || pp_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*pp_buf = NULL;

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	addr_left_size = buf_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		buf_size += (p_pool->align_size - addr_left_size);
	}

	/** get max block */
	ret = mem_pool_map_list_get_back(p_pool, &p_pool->free_map, &p_block);
	if( ret == 0 )
	{
		/** split */
		ret = mem_pool_unit_list_split_block(p_pool, (MEM_POOL_UNIT*)p_block->p_unit, p_block, buf_size);
		if( ret == 0 )
		{
			*pp_buf = p_block->p_buf;

			return GEN_S_OK;
		}
	}

	alloc_size = buf_size + sizeof(MEM_POOL_UNIT) + sizeof(MEM_POOL_BLOCK);

#if 0
	addr_left_size = alloc_size % p_pool->align_size;
	if( addr_left_size != 0 )
	{
		alloc_size += (p_pool->align_size - addr_left_size);
	}
#endif

	/** alloc mem */
	if( alloc_size < p_pool->min_pool_size )
	{
		alloc_size = p_pool->min_pool_size;
	}

	ret = mem_pool_alloc_mem(h, alloc_size);
	if( ret )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	/** get max block */
	ret = mem_pool_map_list_get_back(p_pool, &p_pool->free_map, &p_block);
	if( ret == 0 )
	{
		/** split */
		ret = mem_pool_unit_list_split_block(p_pool, (MEM_POOL_UNIT*)p_block->p_unit, p_block, buf_size);
		if( ret == 0 )
		{
			*pp_buf = p_block->p_buf;

			return GEN_S_OK;
		}
	}

	return GEN_E_NOT_ENOUGH_MEM;
}

int32 mem_pool_release_buf(MEM_POOL_H h, void* p_buf)
{
	MEM_POOL *p_pool = (MEM_POOL*)h;
	MEM_POOL_BLOCK *p_block = NULL;

	int32  ret;

	if( p_pool == NULL || p_buf == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_pool->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	/** find block */
	ret = mem_pool_map_find_node(p_pool, &p_pool->use_map, p_buf, &p_block);
	if( ret )
	{
		return GEN_E_NOT_FOUND;
	}

	/** merge */
	ret = mem_pool_unit_list_merge_block(p_pool, (MEM_POOL_UNIT*)p_block->p_unit, p_block);

	return GEN_S_OK;
}
