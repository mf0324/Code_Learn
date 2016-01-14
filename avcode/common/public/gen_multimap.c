
#include <stdlib.h>
#include <string.h>

#include "gen_multimap.h"

/////////////////////////////////// allocator /////////////////////////////////
static void* gen_multimap_inner_malloc(uint32 size, void* p_param)
{
	return malloc(size);
}

static int32 gen_multimap_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/**  
 * @brief 寻找节点位置
 * @param [in] p_multimap multimap句柄
 * @param [in] p_key 关键字
 * @param [out] pp_node 节点句柄
 * @param [out] p_left 是否左子树
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 * @remark
 */
static int32 gen_multimap_find_node_pos(GEN_MULTIMAP* p_multimap, void* p_key, MULTIMAP_NODE** pp_node, int32* p_left)
{
	MULTIMAP_NODE* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	*pp_node = NULL;
	p_node = p_multimap->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = p_multimap->comparer.compare(p_key, p_node->pair.p_key, p_multimap->comparer.p_param, &result);
		if( result == 0 )
		{
			/** 找到对应节点 */
			b_find = 1;
			break;
		}

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

	if( !b_find )
	{
		return -4;
	}
	
	*pp_node = p_node;
	return 0;
}

/**  
 * @brief 寻找插入位置
 * @param [in] p_multimap multimap句柄
 * @param [in] p_key 关键字
 * @param [out] pp_node 节点句柄
 * @param [out] p_left 是否左子树
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、*pp_node表示插入位置的父节点句柄，如果*pp_node为空，表示插入位置为根节点
 */
static int32 gen_multimap_find_insert_pos(GEN_MULTIMAP* p_multimap, void* p_key, MULTIMAP_NODE** pp_node, int32* p_left)
{
	MULTIMAP_NODE* p_parent;
	MULTIMAP_NODE* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	p_parent = NULL;
	p_node = p_multimap->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = p_multimap->comparer.compare(p_key, p_node->pair.p_key, p_multimap->comparer.p_param, &result);

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

	*pp_node = p_parent;
	return 0;
}

/**  
 * @brief 寻找删除位置
 * @param [in] p_multimap 树句柄
 * @param [in] p_node 要删除的节点句柄
 * @param [out] pp_node 实际删除位置对应的节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、要删除的节点左子树必须非空!
 */
static int32 gen_multimap_find_delete_pos(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node, MULTIMAP_NODE** pp_node)
{
	MULTIMAP_NODE* p_current = p_node;
	MULTIMAP_NODE* p_temp_node;

	/** 寻找左子树的最右节点，该节点是左子树中最大节点 */
	p_temp_node = p_current->p_left;
	while( p_temp_node->p_right )
	{
		p_temp_node = p_temp_node->p_right;
	}

	*pp_node = p_temp_node;

	return 0;
}

/**  
 * @brief 左旋转
 * @param [in] p_multimap 树句柄
 * @param [in] p_a 祖先节点句柄
 * @param [in] p_b 父节点句柄
 * @param [in] p_x 子节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、
 * 调整前的节点位置关系：
 *                              a
 *                             /
 *                            b
 *                           /
 *                          x
 * 调整后的节点位置关系：
 *                              b
 *                             / \
 *                            x   a
 */
static int32 gen_multimap_left_rotate(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_a, MULTIMAP_NODE* p_b, MULTIMAP_NODE* p_x)
{
	if( p_b->p_right )
	{
		p_b->p_right->p_parent = p_a;
	}
	p_a->p_left = p_b->p_right;
	p_b->p_right = p_a;

	if( p_a->p_parent == NULL )
	{
		/** 根节点 */
		p_multimap->p_root = p_b;

	}else
	{
		/** 非根节点 */
		if( p_a == p_a->p_parent->p_left )
		{
			/** 左子树 */
			p_a->p_parent->p_left = p_b;

		}else
		{
			/** 右子树 */
			p_a->p_parent->p_right = p_b;
		}
	}

	p_b->p_parent = p_a->p_parent;
	p_a->p_parent = p_b;

	return 0;
}

/**  
 * @brief 左调整
 * @param [in] p_multimap 树句柄
 * @param [in] p_a 祖先节点句柄
 * @param [in] p_b 父节点句柄
 * @param [in] p_x 子节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、
 * 调整前的节点位置关系：
 *                              a
 *                             /
 *                            b
 *                             \
 *                              x
 * 调整后的节点位置关系：
 *                              a
 *                             /
 *                            x
 *                           /
 *                          b
 */
static int32 gen_multimap_left_adjust(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_a, MULTIMAP_NODE* p_b, MULTIMAP_NODE* p_x)
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

	return 0;
}

/**  
 * @brief 右旋转
 * @param [in] p_multimap 树句柄
 * @param [in] p_a 父节点句柄
 * @param [in] p_b 当前节点句柄
 * @param [in] p_x 子节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、
 * 调整前的节点位置关系：
 *                              a
 *                               \
 *                                b
 *                                 \
 *                                  x
 * 调整后的节点位置关系：
 *                              b
 *                             / \
 *                            a   x
 */
static int32 gen_multimap_right_rotate(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_a, MULTIMAP_NODE* p_b, MULTIMAP_NODE* p_x)
{
	if( p_b->p_left )
	{
		p_b->p_left->p_parent = p_a;
	}
	p_a->p_right = p_b->p_left;
	p_b->p_left = p_a;

	if( p_a->p_parent == NULL )
	{
		/** 根节点 */
		p_multimap->p_root = p_b;

	}else
	{
		/** 非根节点 */
		if( p_a == p_a->p_parent->p_left )
		{
			/** 左子树 */
			p_a->p_parent->p_left = p_b;

		}else
		{
			/** 右子树 */
			p_a->p_parent->p_right = p_b;
		}
	}

	p_b->p_parent = p_a->p_parent;
	p_a->p_parent = p_b;

	return 0;
}

/**  
 * @brief 右调整
 * @param [in] p_multimap 树句柄
 * @param [in] p_a 祖先节点句柄
 * @param [in] p_b 父节点句柄
 * @param [in] p_x 子节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、
 * 调整前的节点位置关系：
 *                              a
 *                               \
 *                                b
 *                               /
 *                              x
 * 调整后的节点位置关系：
 *                              a
 *                               \
 *                                x
 *                                 \
 *                                  b
 */
static int32 gen_multimap_right_adjust(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_a, MULTIMAP_NODE* p_b, MULTIMAP_NODE* p_x)
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

	return 0;
}

/**  
 * @brief 调整插入平衡
 * @param [in] p_multimap 树句柄
 * @param [in] p_node 插入节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_multimap_insert_balance(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node)
{
	MULTIMAP_NODE* p_ancestor;

	MULTIMAP_NODE* p_uncle;
	MULTIMAP_NODE* p_current;

	int32 ret;

	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			p_current->color = GEN_MULTIMAP_COLOR_BLACK;
			break;
		}

		if( p_current->p_parent->color == GEN_MULTIMAP_COLOR_BLACK )
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
		if( p_uncle && p_uncle->color == GEN_MULTIMAP_COLOR_RED )
		{
			/** case 3 */
			p_ancestor->color = GEN_MULTIMAP_COLOR_RED;
			p_current->p_parent->color = GEN_MULTIMAP_COLOR_BLACK;
			p_uncle->color = GEN_MULTIMAP_COLOR_BLACK;

			p_current = p_ancestor;
			continue;
		}

		/** from above, uncle is null or always black */
		if( (p_current == p_current->p_parent->p_right) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 4(left) */
			ret = gen_multimap_left_adjust(p_multimap, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_left;

		}else if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_right) )
		{
			/** case 4(right) */
			ret = gen_multimap_right_adjust(p_multimap, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_right;
		}

		p_current->p_parent->color = GEN_MULTIMAP_COLOR_BLACK;
		p_ancestor->color = GEN_MULTIMAP_COLOR_RED;

		if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 5(left) */
			ret = gen_multimap_left_rotate(p_multimap, p_ancestor, p_current->p_parent, p_current);

		}else
		{
			/** case 5(right) */
			ret = gen_multimap_right_rotate(p_multimap, p_ancestor, p_current->p_parent, p_current);
		}

		break;
	}

	return 0;
}

/**  
 * @brief 调整删除平衡
 * @param [in] p_multimap 树句柄
 * @param [in] p_node 起始节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_multimap_delete_balance(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node)
{
	//MULTIMAP_NODE* p_parent;
	MULTIMAP_NODE* p_sibling;
	MULTIMAP_NODE* p_sibling_left;
	MULTIMAP_NODE* p_sibling_right;
	MULTIMAP_NODE* p_current;

	int32 ret;

	/** from above, p_node is black */
	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			//p_current->color = GEN_MULTIMAP_COLOR_BLACK;
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
		if( p_sibling->color == GEN_MULTIMAP_COLOR_RED )
		{
			/** case 2 */
			p_current->p_parent->color = GEN_MULTIMAP_COLOR_RED;
			p_sibling->color = GEN_MULTIMAP_COLOR_BLACK;

			if( p_current == p_current->p_parent->p_left )
			{
				ret = gen_multimap_right_rotate(p_multimap, p_current->p_parent, p_sibling, p_sibling->p_right);
				p_sibling = p_sibling_left;

			}else
			{
				ret = gen_multimap_left_rotate(p_multimap, p_current->p_parent, p_sibling, p_sibling->p_left);
				p_sibling = p_sibling_right;
			}
		}

		/** from above, sibling always black */
		if( (p_current->p_parent->color == GEN_MULTIMAP_COLOR_BLACK)
			&& (p_sibling->color == GEN_MULTIMAP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK) )
		{
			/** case 3 */
			p_sibling->color = GEN_MULTIMAP_COLOR_RED;
			p_current = p_current->p_parent;
			continue;
		}

		if( (p_current->p_parent->color == GEN_MULTIMAP_COLOR_RED)
			&& (p_sibling->color == GEN_MULTIMAP_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK) )
		{
			/** case 4 */
			p_sibling->color = GEN_MULTIMAP_COLOR_RED;
			p_current->p_parent->color = GEN_MULTIMAP_COLOR_BLACK;

			break;
		}

		/** from case 3 & 4, sibling's child can not both be null */
		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		//if( p_sibling->color == GEN_MULTIMAP_COLOR_BLACK )
		{
			if( (p_current == p_current->p_parent->p_left)
				&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK)
				&& (p_sibling_left && p_sibling->p_left->color == GEN_MULTIMAP_COLOR_RED) )
			{
				/** case 5(left) */
				p_sibling->color = GEN_MULTIMAP_COLOR_RED;
				p_sibling->p_left->color = GEN_MULTIMAP_COLOR_BLACK;
				ret = gen_multimap_left_rotate(p_multimap, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

				p_sibling = p_sibling_left;

			}else if( (p_current == p_current->p_parent->p_right)
				&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
				&& (p_sibling_right && p_sibling->p_right->color == GEN_MULTIMAP_COLOR_RED) )
			{
				/** case 5(right) */
				p_sibling->color = GEN_MULTIMAP_COLOR_RED;
				p_sibling->p_right->color = GEN_MULTIMAP_COLOR_BLACK;
				ret = gen_multimap_right_rotate(p_multimap, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

				p_sibling = p_sibling_right;
			}
		}

		p_sibling->color = p_current->p_parent->color;
		p_current->p_parent->color = GEN_MULTIMAP_COLOR_BLACK;

		if( p_current == p_current->p_parent->p_left )
		{
			/** case 6(left) */
			if( p_sibling->p_right )
			{
				p_sibling->p_right->color = GEN_MULTIMAP_COLOR_BLACK;
			}

			ret = gen_multimap_right_rotate(p_multimap, p_current->p_parent, p_sibling, p_sibling->p_right);

		}else
		{
			/** case 6(right) */
			if( p_sibling->p_left )
			{
				p_sibling->p_left->color = GEN_MULTIMAP_COLOR_BLACK;
			}

			ret = gen_multimap_left_rotate(p_multimap, p_current->p_parent, p_sibling, p_sibling->p_left);
		}

		break;
	}

	return 0;
}

/**  
 * @brief 调整红黑树删除平衡2(对应节点为空)
 * @param [in] p_multimap 树句柄
 * @param [in] p_parent 父节点句柄
 * @param [in] b_left 是否为父节点左子树
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
static int32 gen_multimap_delete_balance2(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_parent, int32 b_left)
{
	MULTIMAP_NODE* p_sibling;
	MULTIMAP_NODE* p_sibling_left;
	MULTIMAP_NODE* p_sibling_right;

	int32 ret;

	if( p_parent == NULL )
	{
		/** case 1 */
		return 0;
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
		return 0;
	}

	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;
	if( p_sibling->color == GEN_MULTIMAP_COLOR_RED )
	{
		/** case 2 */
		p_parent->color = GEN_MULTIMAP_COLOR_RED;
		p_sibling->color = GEN_MULTIMAP_COLOR_BLACK;

		if( b_left )
		{
			ret = gen_multimap_right_rotate(p_multimap, p_parent, p_sibling, p_sibling->p_right);
			p_sibling = p_sibling_left;

		}else
		{
			ret = gen_multimap_left_rotate(p_multimap, p_parent, p_sibling, p_sibling->p_left);
			p_sibling = p_sibling_right;
		}
	}

	/** from above, sibling always black */
	if( (p_parent->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK) )
	{
		/** case 3 */
		p_sibling->color = GEN_MULTIMAP_COLOR_RED;

		return gen_multimap_delete_balance(p_multimap, p_parent);
	}

	if( (p_parent->color == GEN_MULTIMAP_COLOR_RED)
		&& (p_sibling->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK) )
	{
		/** case 4 */
		p_sibling->color = GEN_MULTIMAP_COLOR_RED;
		p_parent->color = GEN_MULTIMAP_COLOR_BLACK;

		return 0;
	}

	/** from case 3 & 4, sibling's child can not both be null */
	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;

	if( (b_left)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling_left && p_sibling->p_left->color == GEN_MULTIMAP_COLOR_RED) )
	{
		/** case 5(left) */
		p_sibling->color = GEN_MULTIMAP_COLOR_RED;
		p_sibling->p_left->color = GEN_MULTIMAP_COLOR_BLACK;
		ret = gen_multimap_left_rotate(p_multimap, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

		p_sibling = p_sibling_left;

	}else if( (!b_left)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_MULTIMAP_COLOR_BLACK)
		&& (p_sibling_right && p_sibling->p_right->color == GEN_MULTIMAP_COLOR_RED) )
	{
		/** case 5(right) */
		p_sibling->color = GEN_MULTIMAP_COLOR_RED;
		p_sibling->p_right->color = GEN_MULTIMAP_COLOR_BLACK;
		ret = gen_multimap_right_rotate(p_multimap, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

		p_sibling = p_sibling_right;
	}

	p_sibling->color = p_parent->color;
	p_parent->color = GEN_MULTIMAP_COLOR_BLACK;

	if( b_left )
	{
		/** case 6(left) */
		if( p_sibling->p_right )
		{
			p_sibling->p_right->color = GEN_MULTIMAP_COLOR_BLACK;
		}

		ret = gen_multimap_right_rotate(p_multimap, p_parent, p_sibling, p_sibling->p_right);

	}else
	{
		/** case 6(right) */
		if( p_sibling->p_left )
		{
			p_sibling->p_left->color = GEN_MULTIMAP_COLOR_BLACK;
		}

		ret = gen_multimap_left_rotate(p_multimap, p_parent, p_sibling, p_sibling->p_left);
	}

	return 0;
}

/**  
 * @brief 删除所有子节点(递归方式)
 * @param [in] p_multimap 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_delete_child_node(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node)
{
	int32 ret;

	/** 删除左子树 */
	if( p_node->p_left )
	{
		ret = gen_multimap_delete_child_node(p_multimap, p_node->p_left);

		ret = p_multimap->allocator.deallocate(p_node->p_left, p_multimap->allocator.p_param);

		p_node->p_left = NULL;
	}

	/** 删除右子树 */
	if( p_node->p_right )
	{
		ret = gen_multimap_delete_child_node(p_multimap, p_node->p_right);

		ret = p_multimap->allocator.deallocate(p_node->p_right, p_multimap->allocator.p_param);

		p_node->p_right = NULL;
	}

	return 0;
}

/**  
 * @brief 交换multimap中节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node1 节点1句柄
 * @param [in] p_node2 节点2句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、交换的内容包括下面这些：
 * - color
 * - p_parent
 * - p_left
 * - p_right
 * - 父节点和子节点与该节点相关指针
 * - 序列前后节点相关指针
 */
static int32 gen_multimap_swap_node(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node1, MULTIMAP_NODE* p_node2)
{
	MULTIMAP_NODE temp;
	int32 b_parent_12;
	int32 b_parent_21;
	int32 b_left1;
	int32 b_left2;

	if( p_node1 == p_node2 )
	{
		/** same node */
		return 0;
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
	if( p_node1 == p_multimap->p_root )
	{
		p_multimap->p_root = p_node2;

	}else if( p_node2 == p_multimap->p_root )
	{
		p_multimap->p_root = p_node1;
	}

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


	return 0;
}

////////////////////////////////// list ///////////////////////////////////////
/**  
 * @brief 初始化树链表
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 */
static int32 gen_multimap_list_init(GEN_MULTIMAP* p_multimap)
{
	p_multimap->p_head = &p_multimap->head;
	p_multimap->p_rear = &p_multimap->rear;

	memset(p_multimap->p_head, 0, sizeof(MULTIMAP_NODE));
	p_multimap->p_head->p_prev_seq = NULL;
	p_multimap->p_head->p_next_seq = p_multimap->p_rear;

	memset(p_multimap->p_rear, 0, sizeof(MULTIMAP_NODE));
	p_multimap->p_rear->p_prev_seq = p_multimap->p_head;
	p_multimap->p_rear->p_next_seq = NULL;

	return 0;
}

/**  
 * @brief 反初始化树链表
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_deinit(GEN_MULTIMAP* p_multimap)
{
	int32 ret;

	if( p_multimap->p_head )
	{
		p_multimap->p_head = NULL;
	}

	if( p_multimap->p_rear )
	{
		p_multimap->p_rear = NULL;
	}

	return 0;
}

/**  
 * @brief 清空树链表
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_clear(GEN_MULTIMAP* p_multimap)
{
	p_multimap->p_head->p_next_seq = p_multimap->p_rear;
	p_multimap->p_rear->p_prev_seq = p_multimap->p_head;

	return 0;
}

/**  
 * @brief 向树链表首部插入节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_push_front_node(GEN_MULTIMAP *p_multimap, MULTIMAP_NODE *p_node)
{
	p_node->p_prev_seq = p_multimap->p_head;
	p_node->p_next_seq = p_multimap->p_head->p_next_seq;
	p_multimap->p_head->p_next_seq->p_prev_seq = p_node;
	p_multimap->p_head->p_next_seq = p_node;

	return 0;
}

/**  
 * @brief 向树链表中指定节点之前插入节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_before 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_insert_before_node(GEN_MULTIMAP *p_multimap, MULTIMAP_NODE *p_node, MULTIMAP_NODE *p_before)
{
	p_node->p_prev_seq = p_before->p_prev_seq;
	p_node->p_next_seq = p_before;
	p_before->p_prev_seq->p_next_seq = p_node;
	p_before->p_prev_seq = p_node;

	return 0;
}

/**  
 * @brief 向树链表中指定节点之后插入节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_after 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_insert_after_node(GEN_MULTIMAP *p_multimap, MULTIMAP_NODE *p_node, MULTIMAP_NODE *p_after)
{
	p_node->p_prev_seq = p_after;
	p_node->p_next_seq = p_after->p_next_seq;
	p_after->p_next_seq->p_prev_seq = p_node;
	p_after->p_next_seq = p_node;

	return 0;
}

/**  
 * @brief 删除树链表中指定节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_after 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_delete_node(GEN_MULTIMAP *p_multimap, MULTIMAP_NODE *p_node)
{
	p_node->p_prev_seq->p_next_seq = p_node->p_next_seq;
	p_node->p_next_seq->p_prev_seq = p_node->p_prev_seq;

	return 0;
}

/**  
 * @brief 树链表逆序
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_multimap_list_reverse(GEN_MULTIMAP *p_multimap)
{
	MULTIMAP_NODE *p_current = NULL;
	MULTIMAP_NODE *p_node  = NULL;
	MULTIMAP_NODE *p_node2 = NULL;
	MULTIMAP_NODE *p_first_node = NULL;

	int32 ret;

	if( p_multimap->multimap_size < 2 )
	{
		/** 无需调整 */
		return 0;
	}

	p_current = p_multimap->p_head->p_next_seq;
	p_node = p_current->p_next_seq;
	while( p_node != p_multimap->p_rear )
	{
		p_node2 = p_node->p_next_seq;

		p_node->p_next_seq = p_current;
		p_current->p_prev_seq = p_node;

		p_current = p_node;
		p_node = p_node2;
	}

	p_first_node = p_multimap->p_head->p_next_seq;
	p_multimap->p_head->p_next_seq = p_multimap->p_rear->p_prev_seq;
	p_multimap->p_rear->p_prev_seq = p_first_node;

	p_multimap->p_head->p_next_seq->p_prev_seq = p_multimap->p_head;
	p_multimap->p_rear->p_prev_seq->p_next_seq = p_multimap->p_rear;

	return 0;
}

///////////////////////////////////外部接口////////////////////////////////////////
GEN_MULTIMAP* gen_multimap_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer)
{
	GEN_MULTIMAP *p_multimap = NULL;

	if( p_comparer == NULL )
	{
		return NULL;
	}

	if( p_allocator )
	{
		p_multimap = (GEN_MULTIMAP *)p_allocator->allocate(sizeof(GEN_MULTIMAP), p_allocator->p_param);

	}else
	{
		p_multimap = (GEN_MULTIMAP *)malloc( sizeof(GEN_MULTIMAP) );
	}

	if( p_multimap == NULL )
	{
		return NULL;
	}

	memset(p_multimap, 0, sizeof(GEN_MULTIMAP));

	if( p_allocator )
	{
		memcpy(&p_multimap->allocator, p_allocator, sizeof(p_multimap->allocator));

	}else
	{
		p_multimap->allocator.allocate = &gen_multimap_inner_malloc;
		p_multimap->allocator.deallocate = &gen_multimap_inner_free;
		p_multimap->allocator.p_param = p_multimap;
	}

	p_multimap->comparer = *p_comparer;

	return p_multimap;
}


int32 gen_multimap_destroy(GEN_MULTIMAP *p_multimap)
{
	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( p_multimap->b_init )
	{
		gen_multimap_deinit(p_multimap);
	}

	ret = p_multimap->allocator.deallocate(p_multimap, p_multimap->allocator.p_param);

	return 0;
}

int32 gen_multimap_init(GEN_MULTIMAP *p_multimap)
{
	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( p_multimap->b_init )
	{
		return 0;
	}

	p_multimap->multimap_size = 0;
	p_multimap->b_descend = 0;
	p_multimap->p_root = NULL;

	ret = gen_multimap_list_init(p_multimap);
	if( ret )
	{
		gen_multimap_deinit(p_multimap);
		return -3;
	}

	p_multimap->b_init = 1;

	return 0;
}

int32 gen_multimap_deinit(GEN_MULTIMAP *p_multimap)
{
	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	/** 删除根节点 */
	if( p_multimap->p_root )
	{
		ret = gen_multimap_delete_child_node(p_multimap, p_multimap->p_root);

		ret = p_multimap->allocator.deallocate(p_multimap->p_root, p_multimap->allocator.p_param);

		p_multimap->p_root = NULL;
	}

	gen_multimap_list_deinit(p_multimap);

	p_multimap->multimap_size = 0;
	p_multimap->b_init = 0;

	return 0;
}

int32 gen_multimap_clear(GEN_MULTIMAP *p_multimap)
{
	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -3;
	}

	if( p_multimap->p_root )
	{
		ret = gen_multimap_delete_child_node(p_multimap, p_multimap->p_root);

		ret = p_multimap->allocator.deallocate(p_multimap->p_root, p_multimap->allocator.p_param);

		p_multimap->p_root = NULL;
	}

	gen_multimap_list_clear(p_multimap);

	p_multimap->multimap_size = 0;

	return 0;
}

int32 gen_multimap_get_size(GEN_MULTIMAP *p_multimap)
{
	if( p_multimap == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -1;
	}

	return p_multimap->multimap_size;
}

int32 gen_multimap_set_order(GEN_MULTIMAP* p_multimap, int32 b_descend)
{
	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -1;
	}

	if( p_multimap->b_descend )
	{
		/** 降序 */
		if( !b_descend )
		{
			ret = gen_multimap_list_reverse(p_multimap);
		}

	}else
	{
		/** 升序 */
		if( b_descend )
		{
			ret = gen_multimap_list_reverse(p_multimap);
		}
	}

	p_multimap->b_descend = b_descend;

	return 0;
}

int32 gen_multimap_find_node(GEN_MULTIMAP* p_multimap, void* p_key, MULTIMAP_NODE** pp_node)
{
	int32 b_left;
	int32 ret;

	if( p_multimap == NULL || pp_node == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -1;
	}

	ret = gen_multimap_find_node_pos(p_multimap, p_key, pp_node, &b_left);
	return ret;
}

int32 gen_multimap_insert_pair(GEN_MULTIMAP* p_multimap, KEY_PAIR* p_pair, MULTIMAP_NODE** pp_node)
{
	MULTIMAP_NODE* p_parent = NULL;
	MULTIMAP_NODE* p_node = NULL;

	int32 b_left;

	int32 ret;

	if( p_multimap == NULL || p_pair == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -1;
	}

	ret = gen_multimap_find_insert_pos(p_multimap, p_pair->p_key, &p_parent, &b_left);
	if( ret )
	{
		return -3;
	}

	p_node = (MULTIMAP_NODE*)p_multimap->allocator.allocate(sizeof(MULTIMAP_NODE), p_multimap->allocator.p_param);

	if( p_node == NULL )
	{
		return -2;
	}

	p_node->color = GEN_MULTIMAP_COLOR_RED;

	p_node->pair.p_key = p_pair->p_key;
	p_node->pair.p_value = p_pair->p_value;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	p_node->p_prev_seq = NULL;
	p_node->p_next_seq = NULL;

	if( p_parent == NULL )
	{
		/** 插入根节点 */
		p_node->color = GEN_MULTIMAP_COLOR_BLACK;
		p_multimap->p_root = p_node;
		ret = gen_multimap_list_push_front_node(p_multimap, p_node);

	}else
	{
		/** 插入非根节点 */
		if( b_left )
		{
			/** 左子树 */
			p_parent->p_left = p_node;

			if( p_multimap->b_descend )
			{
				ret = gen_multimap_list_insert_after_node(p_multimap, p_node, p_parent);

			}else
			{
				ret = gen_multimap_list_insert_before_node(p_multimap, p_node, p_parent);
			}

		}else
		{
			/** 右子树 */
			p_parent->p_right = p_node;

			if( p_multimap->b_descend )
			{
				ret = gen_multimap_list_insert_before_node(p_multimap, p_node, p_parent);

			}else
			{
				ret = gen_multimap_list_insert_after_node(p_multimap, p_node, p_parent);
			}
		}
	}

	ret = gen_multimap_insert_balance(p_multimap, p_node);

	p_multimap->multimap_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return 0;
}

int32 gen_multimap_delete_node(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node)
{
	MULTIMAP_NODE *p_node2 = NULL;
	MULTIMAP_NODE *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_multimap == NULL || p_node == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -3;
	}

	if( p_node->p_left == NULL )
	{
		/** 左子树为空 */
		if( p_node->p_parent == NULL )
		{
			/** 根节点 */
			p_multimap->p_root = p_node->p_right;

		}else
		{
			/** 非根节点 */
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
		/** 左子树非空 */
		ret = gen_multimap_find_delete_pos(p_multimap, p_node, &p_node2);
		ret = gen_multimap_swap_node(p_multimap, p_node, p_node2);

		/** 该节点右子树必然为空 */
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

	if( p_node->color != GEN_MULTIMAP_COLOR_RED )
	{
		/** 要删除的节点是黑色 */
		if( p_child == NULL )
		{
			/** 子节点为空 */
			ret = gen_multimap_delete_balance2(p_multimap, p_node->p_parent, b_left);

		}else
		{
			/** 子节点非空 */
			if( p_child->color == GEN_MULTIMAP_COLOR_RED )
			{
				/** 子节点是红色 */
				p_child->color = GEN_MULTIMAP_COLOR_BLACK;

			}else
			{
				/** 子节点是黑色 */
				ret = gen_multimap_delete_balance(p_multimap, p_child);
			}
		}
	}

	ret = gen_multimap_list_delete_node(p_multimap, p_node);

	ret = p_multimap->allocator.deallocate(p_node, p_multimap->allocator.p_param);

	p_multimap->multimap_size--;

	return 0;
}

int32 gen_multimap_delete_key(GEN_MULTIMAP* p_multimap, void* p_key)
{
	MULTIMAP_NODE *p_node = NULL;
	MULTIMAP_NODE *p_node2 = NULL;
	MULTIMAP_NODE *p_child = NULL;
	MULTIMAP_NODE *p_temp = NULL;

	int32 b_left;

	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -3;
	}

	ret = gen_multimap_find_node_pos(p_multimap, p_key, &p_node, &b_left);
	if( ret )
	{
		/** 未找到对应节点 */
		return -4;
	}

	if( p_node->p_left == NULL )
	{
		/** 左子树为空 */
		if( p_node->p_parent == NULL )
		{
			/** 根节点 */
			p_multimap->p_root = p_node->p_right;

		}else
		{
			/** 非根节点 */
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
		/** 左子树非空 */
		ret = gen_multimap_find_delete_pos(p_multimap, p_node, &p_node2);
		ret = gen_multimap_swap_node(p_multimap, p_node, p_node2);

		/** 该节点右子树必然为空 */
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

	if( p_node->color != GEN_MULTIMAP_COLOR_RED )
	{
		/** 要删除的节点是黑色 */
		if( p_child == NULL )
		{
			/** 子节点为空 */
			ret = gen_multimap_delete_balance2(p_multimap, p_node->p_parent, b_left);

		}else
		{
			/** 子节点非空 */
			if( p_child->color == GEN_MULTIMAP_COLOR_RED )
			{
				/** 子节点是红色 */
				p_child->color = GEN_MULTIMAP_COLOR_BLACK;

			}else
			{
				/** 子节点是黑色 */
				ret = gen_multimap_delete_balance(p_multimap, p_child);
			}
		}
	}

	ret = gen_multimap_list_delete_node(p_multimap, p_node);

	ret = p_multimap->allocator.deallocate(p_node, p_multimap->allocator.p_param);

	p_multimap->multimap_size--;

	return 0;
}

int32 gen_multimap_delete_all_key(GEN_MULTIMAP* p_multimap, void* p_key, int32* p_count)
{
	MULTIMAP_NODE *p_node = NULL;

	int32 b_left;

	int32 i;

	int32 ret;

	if( p_multimap == NULL )
	{
		return -1;
	}

	if( !p_multimap->b_init )
	{
		return -3;
	}

	i = 0;
	while( 1 )
	{
		ret = gen_multimap_find_node_pos(p_multimap, p_key, &p_node, &b_left);
		if( ret )
		{
			/** 未找到对应节点 */
			break;
		}

		ret = gen_multimap_delete_node(p_multimap, p_node);

		i++;
	}

	if( p_count )
	{
		*p_count = i;
	}

	return 0;
}
