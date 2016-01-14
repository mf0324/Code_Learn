
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>
#include "gen_map.h"

/** min block num */
#define GEN_MAP_MIN_BLOCK_NUM                              (1024)
/** null tree height */
#define GEN_MAP_NULL_TREE_HEIGHT                           (0)

/////////////////////////////////// allocator /////////////////////////////////
#if !RUN_OS_64
static void* gen_map_inner_malloc(uint32 size, void* p_param)
#else
static void* gen_map_inner_malloc(uint64 size, void* p_param)
#endif
{
	return malloc(size);
}

static int32 gen_map_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}


/**  
 * @brief find insert pos
 * @param [in] p_map, map
 * @param [in] p_key, insert key
 * @param [out] pp_node, node
 * @param [out] p_left, insert node is left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. on return value 0 means the node equal insert key is not exist, *pp_node means parent node, if *pp_node is null, root is insert
 * 2. if return value is GEN_E_ALREADY_EXIST, *pp_node means the node that equal insert key
 */
static int32 gen_map_find_insert_pos(GEN_MAP* p_map, void* p_key, MAP_NODE** pp_node, int32* p_left)
{
	MAP_NODE* p_parent = NULL;
	MAP_NODE* p_node = NULL;

	int32 b_find;
	int32 result;

	int32 ret;

	//*p_left = 1;

	p_parent = NULL;
	p_node = p_map->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = p_map->comparer.compare(p_key, p_node->pair.p_key, p_map->comparer.p_param, &result);
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
 * @brief find delete pos
 * @param [in] p_map, map
 * @param [in] p_node, to delete node
 * @param [out] pp_node, actual delete node that match delete pos
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. left child of the node to delete must be not null!
 */
static int32 gen_map_find_delete_pos(GEN_MAP* p_map, MAP_NODE* p_node, MAP_NODE** pp_node)
{
	MAP_NODE* p_current = p_node;
	MAP_NODE* p_temp_node;

	KEY_PAIR temp_pair;

	//int32 ret;

	/** find right most node of left child, it is the biggest node of child */
	p_temp_node = p_current->p_left;
	while( p_temp_node->p_right )
	{
		p_temp_node = p_temp_node->p_right;
	}

	/** swap node */
	temp_pair = p_temp_node->pair;
	p_temp_node->pair = p_current->pair;
	p_current->pair = temp_pair;

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
 * 0: child
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
static int32 gen_map_left_rotate(GEN_MAP* p_map, MAP_NODE* p_a, MAP_NODE* p_b, MAP_NODE* p_x)
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
static int32 gen_map_left_adjust(GEN_MAP* p_map, MAP_NODE* p_a, MAP_NODE* p_b, MAP_NODE* p_x)
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
 * @param [in] p_a, ancestor
 * @param [in] p_b, current node
 * @param [in] p_x, child
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
static int32 gen_map_right_rotate(GEN_MAP* p_map, MAP_NODE* p_a, MAP_NODE* p_b, MAP_NODE* p_x)
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
static int32 gen_map_right_adjust(GEN_MAP* p_map, MAP_NODE* p_a, MAP_NODE* p_b, MAP_NODE* p_x)
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
 * @brief adjust insert balance
 * @param [in] p_map, map
 * @param [in] p_node, insert node
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref to http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_map_insert_balance(GEN_MAP* p_map, MAP_NODE* p_node)
{
	MAP_NODE* p_ancestor;

	MAP_NODE* p_uncle;
	MAP_NODE* p_current;

	int32 ret;

	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			p_current->color = GEN_MAP_COLOR_BLACK;
			break;
		}

		if( p_current->p_parent->color == GEN_MAP_COLOR_BLACK )
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
		if( p_uncle && p_uncle->color == GEN_MAP_COLOR_RED )
		{
			/** case 3 */
			p_ancestor->color = GEN_MAP_COLOR_RED;
			p_current->p_parent->color = GEN_MAP_COLOR_BLACK;
			p_uncle->color = GEN_MAP_COLOR_BLACK;

			p_current = p_ancestor;
			continue;
		}

		/** from above, uncle is null or always black */
		if( (p_current == p_current->p_parent->p_right) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 4(left) */
			ret = gen_map_left_adjust(p_map, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_left;

		}else if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_right) )
		{
			/** case 4(right) */
			ret = gen_map_right_adjust(p_map, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_right;
		}

		p_current->p_parent->color = GEN_MAP_COLOR_BLACK;
		p_ancestor->color = GEN_MAP_COLOR_RED;

		if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 5(left) */
			ret = gen_map_left_rotate(p_map, p_ancestor, p_current->p_parent, p_current);

		}else
		{
			/** case 5(right) */
			ret = gen_map_right_rotate(p_map, p_ancestor, p_current->p_parent, p_current);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief adjust delete balance
 * @param [in] p_map, map
 * @param [in] p_node, the node start from
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref to http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_map_delete_balance(GEN_MAP* p_map, MAP_NODE* p_node)
{
	//MAP_NODE* p_parent;
	MAP_NODE* p_sibling;
	MAP_NODE* p_sibling_left;
	MAP_NODE* p_sibling_right;
	MAP_NODE* p_current;

	int32 ret;

	/** from above, p_node is black */
	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			//p_current->color = GEN_MAP_COLOR_BLACK;
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
		if( p_sibling->color == GEN_MAP_COLOR_RED )
		{
			/** case 2 */
			p_current->p_parent->color = GEN_MAP_COLOR_RED;
			p_sibling->color = GEN_MAP_COLOR_BLACK;

			if( p_current == p_current->p_parent->p_left )
			{
				ret = gen_map_right_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_right);
				p_sibling = p_sibling_left;

			}else
			{
				ret = gen_map_left_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_left);
				p_sibling = p_sibling_right;
			}
		}

		/** from above, sibling always black */
		if( (p_current->p_parent->color == GEN_MAP_COLOR_BLACK)
			&& (p_sibling->color == GEN_MAP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK) )
		{
			/** case 3 */
			p_sibling->color = GEN_MAP_COLOR_RED;
			p_current = p_current->p_parent;
			continue;
		}

		if( (p_current->p_parent->color == GEN_MAP_COLOR_RED)
			&& (p_sibling->color == GEN_MAP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK) )
		{
			/** case 4 */
			p_sibling->color = GEN_MAP_COLOR_RED;
			p_current->p_parent->color = GEN_MAP_COLOR_BLACK;

			break;
		}

		/** from case 3 & 4, sibling's child can not both be null */
		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		//if( p_sibling->color == GEN_MAP_COLOR_BLACK )
		{
			if( (p_current == p_current->p_parent->p_left)
				&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK)
				&& (p_sibling_left && p_sibling->p_left->color == GEN_MAP_COLOR_RED) )
			{
				/** case 5(left) */
				p_sibling->color = GEN_MAP_COLOR_RED;
				p_sibling->p_left->color = GEN_MAP_COLOR_BLACK;
				ret = gen_map_left_rotate(p_map, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

				p_sibling = p_sibling_left;

			}else if( (p_current == p_current->p_parent->p_right)
				&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
				&& (p_sibling_right && p_sibling->p_right->color == GEN_MAP_COLOR_RED) )
			{
				/** case 5(right) */
				p_sibling->color = GEN_MAP_COLOR_RED;
				p_sibling->p_right->color = GEN_MAP_COLOR_BLACK;
				ret = gen_map_right_rotate(p_map, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

				p_sibling = p_sibling_right;
			}
		}

		p_sibling->color = p_current->p_parent->color;
		p_current->p_parent->color = GEN_MAP_COLOR_BLACK;

		if( p_current == p_current->p_parent->p_left )
		{
			/** case 6(left) */
			if( p_sibling->p_right )
			{
				p_sibling->p_right->color = GEN_MAP_COLOR_BLACK;
			}

			ret = gen_map_right_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_right);

		}else
		{
			/** case 6(right) */
			if( p_sibling->p_left )
			{
				p_sibling->p_left->color = GEN_MAP_COLOR_BLACK;
			}

			ret = gen_map_left_rotate(p_map, p_current->p_parent, p_sibling, p_sibling->p_left);
		}

		break;
	}

	return GEN_S_OK;
}

/**  
 * @brief adjust delete balance(null node)
 * @param [in] p_map, map
 * @param [in] p_parent, parent
 * @param [in] b_left, the node is parent's left child or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. ref to http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_map_delete_balance2(GEN_MAP* p_map, MAP_NODE* p_parent, int32 b_left)
{
	MAP_NODE* p_sibling;
	MAP_NODE* p_sibling_left;
	MAP_NODE* p_sibling_right;

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
	if( p_sibling->color == GEN_MAP_COLOR_RED )
	{
		/** case 2 */
		p_parent->color = GEN_MAP_COLOR_RED;
		p_sibling->color = GEN_MAP_COLOR_BLACK;

		if( b_left )
		{
			ret = gen_map_right_rotate(p_map, p_parent, p_sibling, p_sibling->p_right);
			p_sibling = p_sibling_left;

		}else
		{
			ret = gen_map_left_rotate(p_map, p_parent, p_sibling, p_sibling->p_left);
			p_sibling = p_sibling_right;
		}
	}

	/** from above, sibling always black */
	if( (p_parent->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK) )
	{
		/** case 3 */
		p_sibling->color = GEN_MAP_COLOR_RED;

		return gen_map_delete_balance(p_map, p_parent);
	}

	if( (p_parent->color == GEN_MAP_COLOR_RED)
		&& (p_sibling->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK) )
	{
		/** case 4 */
		p_sibling->color = GEN_MAP_COLOR_RED;
		p_parent->color = GEN_MAP_COLOR_BLACK;

		return GEN_S_OK;
	}

	/** from case 3 & 4, sibling's child can not both be null */
	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;

	if( (b_left)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling_left && p_sibling->p_left->color == GEN_MAP_COLOR_RED) )
	{
		/** case 5(left) */
		p_sibling->color = GEN_MAP_COLOR_RED;
		p_sibling->p_left->color = GEN_MAP_COLOR_BLACK;
		ret = gen_map_left_rotate(p_map, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

		p_sibling = p_sibling_left;

	}else if( (!b_left)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MAP_COLOR_BLACK)
		&& (p_sibling_right && p_sibling->p_right->color == GEN_MAP_COLOR_RED) )
	{
		/** case 5(right) */
		p_sibling->color = GEN_MAP_COLOR_RED;
		p_sibling->p_right->color = GEN_MAP_COLOR_BLACK;
		ret = gen_map_right_rotate(p_map, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

		p_sibling = p_sibling_right;
	}

	p_sibling->color = p_parent->color;
	p_parent->color = GEN_MAP_COLOR_BLACK;

	if( b_left )
	{
		/** case 6(left) */
		if( p_sibling->p_right )
		{
			p_sibling->p_right->color = GEN_MAP_COLOR_BLACK;
		}

		ret = gen_map_right_rotate(p_map, p_parent, p_sibling, p_sibling->p_right);

	}else
	{
		/** case 6(right) */
		if( p_sibling->p_left )
		{
			p_sibling->p_left->color = GEN_MAP_COLOR_BLACK;
		}

		ret = gen_map_left_rotate(p_map, p_parent, p_sibling, p_sibling->p_left);
	}

	return GEN_S_OK;
}

/**  
 * @brief delete all child(recursive)
 * @param [in] p_map, map
 * @param [in] p_node, to delete node
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_delete_child_node(GEN_MAP* p_map, MAP_NODE* p_node)
{
	int32 ret;

	/** delete left child */
	if( p_node->p_left )
	{
		ret = gen_map_delete_child_node(p_map, p_node->p_left);

		//ret = p_map->allocator.deallocate(p_node->p_left, p_map->allocator.p_param);
		ret = mem_group_release_buf(p_map->mem_group, p_node->p_left);

		p_node->p_left = NULL;
	}

	/** delete right child */
	if( p_node->p_right )
	{
		ret = gen_map_delete_child_node(p_map, p_node->p_right);

		//ret = p_map->allocator.deallocate(p_node->p_right, p_map->allocator.p_param);
		ret = mem_group_release_buf(p_map->mem_group, p_node->p_right);

		p_node->p_right = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_init(GEN_MAP* p_map)
{
	p_map->p_head = &p_map->head;
	p_map->p_rear = &p_map->rear;

	memset(p_map->p_head, 0, sizeof(MAP_NODE));
	p_map->p_head->p_prev = NULL;
	p_map->p_head->p_next = p_map->p_rear;

	memset(p_map->p_rear, 0, sizeof(MAP_NODE));
	p_map->p_rear->p_prev = p_map->p_head;
	p_map->p_rear->p_next = NULL;

	return GEN_S_OK;
}

/**  
 * @brief deinit
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_deinit(GEN_MAP* p_map)
{
	//int32 ret;

	if( p_map->p_head )
	{
		p_map->p_head = NULL;
	}

	if( p_map->p_rear )
	{
		p_map->p_rear = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief clear
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_clear(GEN_MAP* p_map)
{
	p_map->p_head->p_next = p_map->p_rear;
	p_map->p_rear->p_prev = p_map->p_head;

	return GEN_S_OK;
}

/**  
 * @brief push front
 * @param [in] p_map, map
 * @param [in] p_node, node
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_push_front_node(GEN_MAP *p_map, MAP_NODE *p_node)
{
	p_node->p_prev = p_map->p_head;
	p_node->p_next = p_map->p_head->p_next;
	p_map->p_head->p_next->p_prev = p_node;
	p_map->p_head->p_next = p_node;

	return GEN_S_OK;
}

/**  
 * @brief insert before node
 * @param [in] p_map, map
 * @param [in] p_node, insert node
 * @param [in] p_before, the node to insert before
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_insert_before_node(GEN_MAP *p_map, MAP_NODE *p_node, MAP_NODE *p_before)
{
	p_node->p_prev = p_before->p_prev;
	p_node->p_next = p_before;
	p_before->p_prev->p_next = p_node;
	p_before->p_prev = p_node;

	return GEN_S_OK;
}

/**  
 * @brief insert after node
 * @param [in] p_map, map
 * @param [in] p_node, insert node
 * @param [in] p_after, the node to insert after
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_insert_after_node(GEN_MAP *p_map, MAP_NODE *p_node, MAP_NODE *p_after)
{
	p_node->p_prev = p_after;
	p_node->p_next = p_after->p_next;
	p_after->p_next->p_prev = p_node;
	p_after->p_next = p_node;

	return GEN_S_OK;
}

/**  
 * @brief delete
 * @param [in] p_map, map
 * @param [in] p_node, delete node
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_delete_node(GEN_MAP *p_map, MAP_NODE *p_node)
{
	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	return GEN_S_OK;
}

/**  
 * @brief reverse
 * @param [in] p_map, map
 * @return
 * 0: success
 * other: fail
 */
static int32 gen_map_list_reverse(GEN_MAP *p_map)
{
	MAP_NODE *p_current = NULL;
	MAP_NODE *p_node  = NULL;
	MAP_NODE *p_node2 = NULL;
	MAP_NODE *p_first_node = NULL;

	//int32 ret;

	if( p_map->map_size < 2 )
	{
		/** only one node */
		return GEN_S_OK;
	}

	p_current = p_map->p_head->p_next;
	p_node = p_current->p_next;
	while( p_node != p_map->p_rear )
	{
		p_node2 = p_node->p_next;

		p_node->p_next = p_current;
		p_current->p_prev = p_node;

		p_current = p_node;
		p_node = p_node2;
	}

	p_first_node = p_map->p_head->p_next;
	p_map->p_head->p_next = p_map->p_rear->p_prev;
	p_map->p_rear->p_prev = p_first_node;

	p_map->p_head->p_next->p_prev = p_map->p_head;
	p_map->p_rear->p_prev->p_next = p_map->p_rear;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
GEN_MAP* gen_map_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer)
{
	GEN_MAP *p_map = NULL;

	if( p_comparer == NULL )
	{
		return NULL;
	}

	if( p_allocator )
	{
		p_map = (GEN_MAP *)p_allocator->allocate(sizeof(GEN_MAP), p_allocator->p_param);

	}else
	{
		p_map = (GEN_MAP *)malloc( sizeof(GEN_MAP) );
	}

	if( p_map == NULL )
	{
		return NULL;
	}

	gen_map_setup(p_map, p_allocator, p_comparer);

	return p_map;
}


int32 gen_map_destroy(GEN_MAP *p_map)
{
	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_map->b_init )
	{
		gen_map_deinit(p_map);
	}

	ret = p_map->allocator.deallocate(p_map, p_map->allocator.p_param);

	return GEN_S_OK;
}

int32 gen_map_setup(GEN_MAP* p_map, GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer)
{
	if( p_map == NULL || p_comparer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	memset(p_map, 0, sizeof(GEN_MAP));

	if( p_allocator )
	{
		memcpy(&p_map->allocator, p_allocator, sizeof(p_map->allocator));

	}else
	{
		p_map->allocator.allocate = &gen_map_inner_malloc;
		p_map->allocator.deallocate = &gen_map_inner_free;
		p_map->allocator.p_param = p_map;
	}

	p_map->comparer = *p_comparer;
	
	return GEN_S_OK;
}

int32 gen_map_cleanup(GEN_MAP* p_map)
{
	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_map->b_init )
	{
		gen_map_deinit(p_map);
	}

	return GEN_S_OK;
}

int32 gen_map_init(GEN_MAP *p_map)
{
	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_map->b_init )
	{
		return GEN_S_OK;
	}

	p_map->mem_group = mem_group_create(&p_map->allocator);
	if( p_map->mem_group == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	mem_group_init(p_map->mem_group);
	mem_group_set_block_param(p_map->mem_group, sizeof(MAP_NODE), GEN_MAP_MIN_BLOCK_NUM, INT32_MAX);
	mem_group_alloc_mem(p_map->mem_group);

	p_map->map_size = 0;
	p_map->b_descend = 0;
	p_map->p_root = NULL;

	ret = gen_map_list_init(p_map);
	if( ret )
	{
		gen_map_deinit(p_map);
		return GEN_E_FAIL;
	}

	p_map->b_init = 1;

	return GEN_S_OK;
}

int32 gen_map_deinit(GEN_MAP *p_map)
{
	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	/** delete root */
	if( p_map->p_root )
	{
		ret = gen_map_delete_child_node(p_map, p_map->p_root);

		//ret = p_map->allocator.deallocate(p_map->p_root, p_map->allocator.p_param);
		ret = mem_group_release_buf(p_map->mem_group, p_map->p_root);

		p_map->p_root = NULL;
	}

	gen_map_list_deinit(p_map);
	
	if( p_map->mem_group )
	{
		mem_group_destroy(p_map->mem_group);
		p_map->mem_group = NULL;
	}

	p_map->map_size = 0;
	p_map->b_init = 0;

	return GEN_S_OK;
}

int32 gen_map_clear(GEN_MAP *p_map)
{
	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_map->p_root )
	{
		ret = gen_map_delete_child_node(p_map, p_map->p_root);

		//ret = p_map->allocator.deallocate(p_map->p_root, p_map->allocator.p_param);
		ret = mem_group_release_buf(p_map->mem_group, p_map->p_root);

		p_map->p_root = NULL;
	}

	gen_map_list_clear(p_map);

	p_map->map_size = 0;

	return GEN_S_OK;
}

int32 gen_map_get_size(GEN_MAP* p_map, int32* p_size)
{
	if( p_map == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*p_size = p_map->map_size;

	return GEN_S_OK;
}

int32 gen_map_get_size64(GEN_MAP* p_map, int64* p_size)
{
	if( p_map == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*p_size = p_map->map_size;

	return GEN_S_OK;
}

int32 gen_map_set_order(GEN_MAP* p_map, int32 b_descend)
{
	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_map->b_descend )
	{
		/** descend */
		if( !b_descend )
		{
			ret = gen_map_list_reverse(p_map);
		}

	}else
	{
		/** ascend */
		if( b_descend )
		{
			ret = gen_map_list_reverse(p_map);
		}
	}

	p_map->b_descend = b_descend;

	return GEN_S_OK;
}

int32 gen_map_find_node(GEN_MAP* p_map, void* p_key, MAP_NODE** pp_node)
{
	int32 b_left;
	int32 ret;

	if( p_map == NULL || pp_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = gen_map_find_insert_pos(p_map, p_key, pp_node, &b_left);
	if( ret == 0 )
	{
		*pp_node = NULL;
		return GEN_E_NOT_FOUND;
	}

	return GEN_S_OK;
}

int32 gen_map_insert_pair(GEN_MAP* p_map, KEY_PAIR* p_pair, MAP_NODE** pp_node)
{
	MAP_NODE* p_parent = NULL;
	MAP_NODE* p_node = NULL;
	MAP_NODE* p_temp = NULL;

	int32 b_left;
	int32 ret;

	if( p_map == NULL || p_pair == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = gen_map_find_insert_pos(p_map, p_pair->p_key, &p_parent, &b_left);
	if( ret )
	{
		return GEN_E_ALREADY_EXIST;
	}

	//p_node = (MAP_NODE*)p_map->allocator.allocate(sizeof(MAP_NODE), p_map->allocator.p_param);
	ret = mem_group_get_buf(p_map->mem_group, sizeof(MAP_NODE), (void**)&p_node);
	if( p_node == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_node->color = GEN_MAP_COLOR_RED;

	p_node->pair.p_key = p_pair->p_key;
	p_node->pair.p_value = p_pair->p_value;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	if( p_parent == NULL )
	{
		/** root */
		p_node->color = GEN_MAP_COLOR_BLACK;
		p_map->p_root = p_node;
		ret = gen_map_list_push_front_node(p_map, p_node);

	}else
	{
		/** not root */
		if( b_left )
		{
			/** left child */
			p_parent->p_left = p_node;

			if( p_map->b_descend )
			{
				ret = gen_map_list_insert_after_node(p_map, p_node, p_parent);

			}else
			{
				ret = gen_map_list_insert_before_node(p_map, p_node, p_parent);
			}

		}else
		{
			/** right child */
			p_parent->p_right = p_node;

			if( p_map->b_descend )
			{
				ret = gen_map_list_insert_before_node(p_map, p_node, p_parent);

			}else
			{
				ret = gen_map_list_insert_after_node(p_map, p_node, p_parent);
			}
		}
	}

	ret = gen_map_insert_balance(p_map, p_node);
	p_map->map_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return GEN_S_OK;
}

int32 gen_map_delete_node(GEN_MAP* p_map, MAP_NODE* p_node)
{
	MAP_NODE *p_node2 = NULL;
	MAP_NODE *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_map == NULL || p_node == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

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
		ret = gen_map_find_delete_pos(p_map, p_node, &p_node2);
		p_node = p_node2;

		/** right child of node should be null */
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

	if( p_node->color != GEN_MAP_COLOR_RED )
	{
		/** node is black */
		if( p_child == NULL )
		{
			/** child is null */
			ret = gen_map_delete_balance2(p_map, p_node->p_parent, b_left);

		}else
		{
			/** child not null */
			if( p_child->color == GEN_MAP_COLOR_RED )
			{
				/** child is red */
				p_child->color = GEN_MAP_COLOR_BLACK;

			}else
			{
				/** child is black */
				ret = gen_map_delete_balance(p_map, p_child);
			}
		}
	}

	ret = gen_map_list_delete_node(p_map, p_node);

	//ret = p_map->allocator.deallocate(p_node, p_map->allocator.p_param);
	ret = mem_group_release_buf(p_map->mem_group, p_node);

	p_map->map_size--;

	return GEN_S_OK;
}

int32 gen_map_delete_key(GEN_MAP* p_map, void* p_key)
{
	MAP_NODE *p_node = NULL;
	MAP_NODE *p_node2 = NULL;
	MAP_NODE *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_map == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_map->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = gen_map_find_insert_pos(p_map, p_key, &p_node, &b_left);
	if( ret == 0 )
	{
		/** not found */
		return GEN_E_NOT_FOUND;
	}

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
		ret = gen_map_find_delete_pos(p_map, p_node, &p_node2);
		p_node = p_node2;

		/** right child of node should be null */
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

	if( p_node->color != GEN_MAP_COLOR_RED )
	{
		/** black node */
		if( p_child == NULL )
		{
			/** child is null */
			ret = gen_map_delete_balance2(p_map, p_node->p_parent, b_left);

		}else
		{
			/** child is not null */
			if( p_child->color == GEN_MAP_COLOR_RED )
			{
				/** child is red */
				p_child->color = GEN_MAP_COLOR_BLACK;

			}else
			{
				/** child is black */
				ret = gen_map_delete_balance(p_map, p_child);
			}
		}
	}

	ret = gen_map_list_delete_node(p_map, p_node);

	//ret = p_map->allocator.deallocate(p_node, p_map->allocator.p_param);
	ret = mem_group_release_buf(p_map->mem_group, p_node);

	p_map->map_size--;

	return GEN_S_OK;
}
