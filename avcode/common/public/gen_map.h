/**
 * @file gen_map.h  Map
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.06.22
 *
 *
 */

#ifndef __GEN_MAP_H__
#define __GEN_MAP_H__

#include <public/gen_int.h>
#include <public/gen_key.h>
#include <public/gen_allocator.h>
#include <public/gen_comparer.h>
#include <public/mem_group.h>

/** red */
#define GEN_MAP_COLOR_RED                                  (0)
/** black */
#define GEN_MAP_COLOR_BLACK                                (1)

typedef struct tag_map_node MAP_NODE, *PMAP_NODE;
typedef struct tag_gen_map  GEN_MAP, *PGEN_MAP;

/** map node */
struct tag_map_node
{
	/** color */
	int32  color;

	/** key pair */
	KEY_PAIR pair;

	/** parent */
	MAP_NODE* p_parent;
	/** left child */
	MAP_NODE* p_left;
	/** right child */
	MAP_NODE* p_right;

	/** fore */
	MAP_NODE* p_prev;
	/** next */
	MAP_NODE* p_next;

};

/** map */
struct tag_gen_map
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;
	/** mem group */
	MEM_GROUP_H   mem_group;

	/** init */
	int32  b_init;

#if !RUN_OS_64
	/** map size */
	int32  map_size;
#else
	/** map size */
	int64  map_size;
#endif

	/** descend */
	int32  b_descend;

	/** root */
	MAP_NODE* p_root;

	/** head pointer */
	MAP_NODE* p_head;
	/** rear pointer */
	MAP_NODE* p_rear;
	/** head */
	MAP_NODE  head;
	/** rear */
	MAP_NODE  rear;

};

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @param [in] p_allocator, allocator
 * @param [in] p_comparer, comparer
 * @return
 * not NULL: success
 * NULL: fail
 * @remark
 * 1. comparer can't be null!
 * 2. if allocator is null, default allocator would be used(malloc)
 * 3. implement base on red-black-tree
 */
GEN_MAP* gen_map_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief destroy
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_destroy(GEN_MAP* p_map);

/**  
 * @brief setup
 * @param [in] p_map, map
 * @param [in] p_allocator, allocator
 * @param [in] p_comparer, comparer
 * @return
 * 0: success
 * other: fail
 * 1. comparer can't be null!
 * 2. if allocator is null, default allocator would be used(malloc)
 */
int32 gen_map_setup(GEN_MAP* p_map, GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief cleanup
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_cleanup(GEN_MAP* p_map);

/**  
 * @brief init
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_init(GEN_MAP* p_map);

/**  
 * @brief deinit
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_deinit(GEN_MAP* p_map);

/**  
 * @brief clear
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_clear(GEN_MAP* p_map);

/**  
 * @brief get map size
 * @param [in] p_map, map
 * @param [out] p_size, map size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_get_size(GEN_MAP* p_map, int32* p_size);

/**  
 * @brief get map size(64 bit)
 * @param [in] p_map, map
 * @param [out] p_size, map size
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_get_size64(GEN_MAP* p_map, int64* p_size);

/**  
 * @brief set sort order
 * @param [in] p_map, map
 * @param [in] b_descend, sort descend or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. after init, default order is ascend
 */
int32 gen_map_set_order(GEN_MAP* p_map, int32 b_descend);

/**  
 * @brief find
 * @param [in] p_map, map
 * @param [in] p_key, key
 * @param [out] pp_node, node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_find_node(GEN_MAP* p_map, void* p_key, MAP_NODE** pp_node);

/**  
 * @brief insert key pair
 * @param [in] p_map, map
 * @param [in] p_pair, key pair
 * @param [out] pp_node, node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. pp_node can be null if u do not want the node
 */
int32 gen_map_insert_pair(GEN_MAP* p_map, KEY_PAIR* p_pair, MAP_NODE** pp_node);

/**  
 * @brief delete node
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_delete_node(GEN_MAP* p_map, MAP_NODE* p_node);

/**  
 * @brief delete key
 * @param [in] p_map, map
 * @param [in] p_key, key
 * @return
 * 0: success
 * other: fail
 */
int32 gen_map_delete_key(GEN_MAP* p_map, void* p_key);

#ifdef __cplusplus
}
#endif


#endif ///__GEN_MAP_H__
