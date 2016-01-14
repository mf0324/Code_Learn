/**
 * @file gen_deque.h  General Deque
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.06.28
 *
 *
 */

#ifndef __GEN_DEQUE_H__
#define __GEN_DEQUE_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>

typedef struct tag_deque_node DEQUE_NODE, *PDEQUE_NODE;
typedef struct tag_gen_deque GEN_DEQUE, *PGEN_DEQUE;

/**  
 * @brief delete node
 * @param [in] p_deque, deque
 * @param [in] p_node, node
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 */
typedef int32 (*gen_deque_delete_node_notify)(GEN_DEQUE* p_deque, DEQUE_NODE* p_node, void* p_custom_param);

/** callback */
typedef struct tag_gen_deque_callback
{
	/** delete */
	gen_deque_delete_node_notify  delete_node;

	/** custom param */
	void* p_custom_param;

} GEN_DEQUE_CALLBACK, *PGEN_DEQUE_CALLBACK;

/** Deque Node */
struct tag_deque_node
{
	/** data */
	void*  p_data;

};

/** Deque */
struct tag_gen_deque
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** callback */
	GEN_DEQUE_CALLBACK callback;

	/** init */
	int32  b_init;
	/** alloc memory */
	int32  b_alloc_mem;

#if !RUN_OS_64
	/** deque size */
	int32  deque_size;
#else
	/** deque size */
	int64  deque_size;
#endif

	/** node array */
	DEQUE_NODE* p_array;

#if !RUN_OS_64

	/** array min size, unit: quantity */
	int32  array_min_size;
	/** array max size, unit: quantity */
	int32  array_max_size;
	/** array total size, unit: quantity */
	int32  array_total_size;
	/** array valid size, unit: quantity */
	int32  array_valid_size;
	/** head pos */
	int32  head_pos;
	/** rear pos */
	int32  rear_pos;

#else

	/** array min size, unit: quantity */
	int64  array_min_size;
	/** array max size, unit: quantity */
	int64  array_max_size;
	/** array total size, unit: quantity */
	int64  array_total_size;
	/** array valid size, unit: quantity */
	int64  array_valid_size;
	/** head pos */
	int64  head_pos;
	/** rear pos */
	int64  rear_pos;

#endif
};

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @param [in] p_allocator, allocator
 * @return
 * not NULL: success
 * NULL: fail
 * @remark
 * 1. if allocator is null, the default allocator(malloc) will be used
 */
GEN_DEQUE* gen_deque_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief destroy
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_destroy(GEN_DEQUE *p_deque);

/**  
 * @brief setup
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_setup(GEN_DEQUE *p_deque, GEN_ALLOCATOR* p_allocator);

/**  
 * @brief cleanup
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_cleanup(GEN_DEQUE *p_deque);

/**  
 * @brief init
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_init(GEN_DEQUE *p_deque);

/**  
 * @brief deinit
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_deinit(GEN_DEQUE *p_deque);

/**  
 * @brief clear
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_clear(GEN_DEQUE *p_deque);

/**  
 * @brief set callback
 * @param [in] p_deque, deque
 * @param [out] p_callback, deque callback
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_set_callback(GEN_DEQUE* p_deque, GEN_DEQUE_CALLBACK* p_callback);

/**  
 * @brief set array param
 * @param [in] p_deque, deque
 * @param [in] min_size, array min size, unit: number
 * @param [in] max_size, array max size, unit: number
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. array max size must greater or equal than min size, if equal, deque will not auto increase
 * 2. array param cant be set after alloc memory, default min size is 1024, max size is 1024
 */
#if !RUN_OS_64
int32 gen_deque_set_array_param(GEN_DEQUE *p_deque, int32 min_size, int32 max_size);
#else
int32 gen_deque_set_array_param(GEN_DEQUE *p_deque, int64 min_size, int64 max_size);
#endif

/**  
 * @brief get array size
 * @param [in] p_deque, deque
 * @param [out] p_size, array size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. current array size always greater or equal than current deque size, and equal or less than max array size
 * 2. if memory not alloc, current array size is 0, else is actual alloc size
 */
int32 gen_deque_get_array_size(GEN_DEQUE *p_deque, int32 *p_size);

/**  
 * @brief resize array
 * @param [in] p_deque, deque
 * @param [in] new_size, new array size, unit: number
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. new array size must equal or greater than the bigger of current deque size and min array size, it also must equal or less than max array size
 * 2. the func do not change current deque size, it can release unuse array data
 */
#if !RUN_OS_64
int32 gen_deque_resize_array(GEN_DEQUE *p_deque, int32 new_size);
#else
int32 gen_deque_resize_array(GEN_DEQUE *p_deque, int64 new_size);
#endif

/**  
 * @brief alloc mem
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_alloc_mem(GEN_DEQUE *p_deque);

/**  
 * @brief free mem
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_free_mem(GEN_DEQUE *p_deque);

/**  
 * @brief get deque size
 * @param [in] p_deque, deque
 * @param [out] p_size, deque size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_get_size(GEN_DEQUE *p_deque, int32 *p_size);

/**  
 * @brief get front data
 * @param [in] p_deque, deque
 * @param [out] pp_data, node data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_get_front_data(GEN_DEQUE *p_deque, void **pp_data);

/**  
 * @brief push front data
 * @param [in] p_deque, deque
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_push_front_data(GEN_DEQUE *p_deque, void *p_data);

/**  
 * @brief pop front
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_pop_front(GEN_DEQUE *p_deque);

/**  
 * @brief get back data
 * @param [in] p_deque, deque
 * @param [out] pp_data, node data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_get_back_data(GEN_DEQUE *p_deque, void **pp_data);

/**  
 * @brief push back data
 * @param [in] p_deque, deque
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_push_back_data(GEN_DEQUE *p_deque, void *p_data);

/**  
 * @brief pop back
 * @param [in] p_deque, deque
 * @return
 * 0: success
 * other: fail
 */
int32 gen_deque_pop_back(GEN_DEQUE *p_deque);

/**  
 * @brief get data
 * @param [in] p_deque, deque
 * @param [in] index, node index
 * @param [out] pp_data, node data
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if not mention elsewhere, index begin from 0
 * 2. valid index range is [0, deque_size)
 */
#if !RUN_OS_64
int32 gen_deque_get_data(GEN_DEQUE *p_deque, int32 index, void **pp_data);
#else
int32 gen_deque_get_data(GEN_DEQUE *p_deque, int64 index, void **pp_data);
#endif

/**  
 * @brief set node data
 * @param [in] p_deque, deque
 * @param [in] index, node index
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
int32 gen_deque_set_data(GEN_DEQUE *p_deque, int32 index, void *p_data);
#else
int32 gen_deque_set_data(GEN_DEQUE *p_deque, int64 index, void *p_data);
#endif

/**  
 * @brief insert data at pos
 * @param [in] p_deque, deque
 * @param [in] index, node index
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. valid insert pos range is [0, deque_size]
 */
#if !RUN_OS_64
int32 gen_deque_insert_data_at(GEN_DEQUE *p_deque, int32 index, void *p_data);
#else
int32 gen_deque_insert_data_at(GEN_DEQUE *p_deque, int64 index, void *p_data);
#endif

/**  
 * @brief delete data at pos
 * @param [in] p_deque, deque
 * @param [in] index, node index
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. valid delete pos range is [0, deque_size)
 */
#if !RUN_OS_64
int32 gen_deque_delete_data_at(GEN_DEQUE *p_deque, int32 index);
#else
int32 gen_deque_delete_data_at(GEN_DEQUE *p_deque, int64 index);
#endif


#ifdef __cplusplus
}
#endif


#endif ///__GEN_DEQUE_H__
