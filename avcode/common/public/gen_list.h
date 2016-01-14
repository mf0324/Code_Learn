/**
 * @file gen_list.h  Dual Link List
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.04.26
 *
 *
 */

#ifndef __GEN_LIST_H__
#define __GEN_LIST_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>
#include <public/mem_group.h>



typedef struct tag_list_node LIST_NODE, *PLIST_NODE;
typedef struct tag_gen_list GEN_LIST, *PGEN_LIST;


/**  
 * @brief delete node
 * @param [in] p_list, list
 * @param [in] p_node, node
 * @param [in] p_custom_param, custom param
 * @return
 * 0: success
 * other: fail
 * @remark
 */
typedef int32 (*gen_list_delete_node_notify)(GEN_LIST* p_list, LIST_NODE* p_node, void* p_custom_param);

/** callback */
typedef struct tag_gen_list_callback
{
	/** delete */
	gen_list_delete_node_notify  delete_node;

	/** custom param */
	void* p_custom_param;

} GEN_LIST_CALLBACK, *PGEN_LIST_CALLBACK;

/** node */
struct tag_list_node
{
	/** prev */
	LIST_NODE* p_prev;
	/** next */
	LIST_NODE* p_next;

	/** data */
	void* p_data;

};

/** list */
struct tag_gen_list
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** callback */
	GEN_LIST_CALLBACK callback;
	/** mem group */
	MEM_GROUP_H  mem_group;

	/** init */
	int32  b_init;

#if !RUN_OS_64
	/** list size */
	int32  list_size;
#else
	/** list size */
	int64  list_size;
#endif

	/** head pointer */
	LIST_NODE* p_head;
	/** rear pointer */
	LIST_NODE* p_rear;
	/** head */
	LIST_NODE  head;
	/** rear */
	LIST_NODE  rear;

};


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @param [in] p_allocator, allocator
 * @return
 * not null: success
 * null: fail
 * @remark
 * 1. if allocator is null, the default allocator(malloc) will be used
 */
GEN_LIST* gen_list_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief destroy
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_destroy(GEN_LIST* p_list);

/**  
 * @brief setup
 * @param [in] p_list, list
 * @param [in] p_allocator, allocator
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_setup(GEN_LIST* p_list, GEN_ALLOCATOR* p_allocator);

/**  
 * @brief cleanup
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_cleanup(GEN_LIST* p_list);

/**  
 * @brief init
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_init(GEN_LIST* p_list);

/**  
 * @brief deinit
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_deinit(GEN_LIST* p_list);

/**  
 * @brief clear
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_clear(GEN_LIST* p_list);

/**  
 * @brief set callback
 * @param [in] p_list, list
 * @param [out] p_callback, list callback
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_set_callback(GEN_LIST* p_list, GEN_LIST_CALLBACK* p_callback);

/**  
 * @brief get size
 * @param [in] p_list, list
 * @param [out] p_size, list size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_size(GEN_LIST* p_list, int32* p_size);

/**  
 * @brief get head node
 * @param [in] p_list, list
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_head_node(GEN_LIST* p_list, LIST_NODE** pp_node);

/**  
 * @brief get rear node
 * @param [in] p_list, list
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_rear_node(GEN_LIST* p_list, LIST_NODE** pp_node);

/**  
 * @brief get front node
 * @param [in] p_list, list
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_front_node(GEN_LIST* p_list, LIST_NODE** pp_node);

/**  
 * @brief get front data
 * @param [in] p_list, list
 * @param [out] pp_data, list data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_front_data(GEN_LIST* p_list, void** pp_data);

/**  
 * @brief push front node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_push_front_node(GEN_LIST* p_list, LIST_NODE* p_node);

/**  
 * @brief push front data
 * @param [in] p_list, list
 * @param [in] p_data, list data
 * @param [out] pp_node, insert node
 * @return
 * 0: success
 * other: fail
 * 1. insert node can be null if do not want it
 */
int32 gen_list_push_front_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node);

/**  
 * @brief pop front
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_pop_front(GEN_LIST* p_list);

/**  
 * @brief get back node
 * @param [in] p_list, list
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_back_node(GEN_LIST* p_list, LIST_NODE** pp_node);

/**  
 * @brief get back data
 * @param [in] p_list, list
 * @param [out] pp_data, list data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_back_data(GEN_LIST* p_list, void** pp_data);

/**  
 * @brief push back node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_push_back_node(GEN_LIST* p_list, LIST_NODE* p_node);

/**  
 * @brief push back data
 * @param [in] p_list, list
 * @param [in] p_data, list data
 * @param [out] pp_node, insert node
 * @return
 * 0: success
 * other: fail
 * 1. insert node can be null if do not want it
 */
int32 gen_list_push_back_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node);

/**  
 * @brief pop back
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_pop_back(GEN_LIST* p_list);

/**  
 * @brief get node by index
 * @param [in] p_list, list
 * @param [in] index, node index
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if not mention, index begin from 0, the same below
 */
#if !RUN_OS_64
int32 gen_list_get_node(GEN_LIST* p_list, int32 index, LIST_NODE** pp_node);
#else
int32 gen_list_get_node(GEN_LIST* p_list, int64 index, LIST_NODE** pp_node);
#endif

/**  
 * @brief get data by index
 * @param [in] p_list, list
 * @param [in] index, node index
 * @param [out] pp_data, node data
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
int32 gen_list_get_data(GEN_LIST* p_list, int32 index, void** pp_data);
#else
int32 gen_list_get_data(GEN_LIST* p_list, int64 index, void** pp_data);
#endif

/**  
 * @brief get node by data
 * @param [in] p_list, list
 * @param [in] p_data, node data
 * @param [out] pp_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_get_node_by_data(GEN_LIST* p_list, void* p_data, LIST_NODE** pp_node);

/**  
 * @brief insert node before node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @param [in] p_before, the node to insert before
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_insert_node_before_node(GEN_LIST* p_list,  LIST_NODE* p_node, LIST_NODE* p_before);

/**  
 * @brief insert node after node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @param [in] p_after, the node to insert after
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_insert_node_after_node(GEN_LIST* p_list, LIST_NODE* p_node, LIST_NODE* p_after);

/**  
 * @brief insert node at pos
 * @param [in] p_list, list
 * @param [in] index, node index
 * @param [in] p_node, list node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. index range from [0, list_size]
 */
#if !RUN_OS_64
int32 gen_list_insert_node_at(GEN_LIST* p_list, int32 index, LIST_NODE* p_node);
#else
int32 gen_list_insert_node_at(GEN_LIST* p_list, int64 index, LIST_NODE* p_node);
#endif

/**  
 * @brief insert data before node
 * @param [in] p_list, list
 * @param [in] p_data. node data
 * @param [in] p_before, the node to insert before
 * @param [out] pp_node, actual node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. pp_node can be null
 */
int32 gen_list_insert_data_before_node(GEN_LIST* p_list, void* p_data, LIST_NODE* p_before, LIST_NODE** pp_node);

/**  
 * @brief insert data after node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @param [in] p_after, the node to insert after
 * @param [out] pp_node, actual node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. pp_node can be null
 */
int32 gen_list_insert_data_after_node(GEN_LIST* p_list, void* p_data, LIST_NODE* p_after, LIST_NODE** pp_node);

/**  
 * @brief insert data at pos
 * @param [in] p_list, list
 * @param [in] index, node index
 * @param [in] p_data, node data
 * @param [out] pp_node, actual node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. index range from [0, list_size]
 * 2. pp_node can be null
 */
#if !RUN_OS_64
int32 gen_list_insert_data_at(GEN_LIST* p_list, int32 index, void* p_data, LIST_NODE** pp_node);
#else
int32 gen_list_insert_data_at(GEN_LIST* p_list, int64 index, void* p_data, LIST_NODE** pp_node);
#endif

/**  
 * @brief delete node
 * @param [in] p_list, list
 * @param [in] p_node, list node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_delete_node(GEN_LIST* p_list, LIST_NODE* p_node);

/**  
 * @brief delete node at pos
 * @param [in] p_list, list
 * @param [in] index, node index
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
int32 gen_list_delete_node_at(GEN_LIST* p_list, int32 index);
#else
int32 gen_list_delete_node_at(GEN_LIST* p_list, int64 index);
#endif

/**  
 * @brief delete node by data
 * @param [in] p_list, list
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if more than one node match, the first match node will be delete
 */
int32 gen_list_delete_node_by_data(GEN_LIST* p_list, void* p_data);

/**  
 * @brief delete all node by data
 * @param [in] p_list, list
 * @param [in] p_data, node data
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_delete_all_node_by_data(GEN_LIST* p_list, void* p_data);

/**  
 * @brief delete range of node
 * @param [in] p_list, list
 * @param [in] p_begin, begin node
 * @param [in] p_end, end node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. begin and end node can not be head node or rear node, the begin node must ahead of end node£¡
 * 2. if begin node equal end node, only one node be delete
 */
int32 gen_list_delete_range(GEN_LIST* p_list, LIST_NODE* p_begin, LIST_NODE* p_end);

/**  
 * @brief reverse
 * @param [in] p_list, list
 * @return
 * 0: success
 * other: fail
 */
int32 gen_list_reverse(GEN_LIST* p_list);

#ifdef __cplusplus
}
#endif


#endif ///__GEN_LIST_H__
