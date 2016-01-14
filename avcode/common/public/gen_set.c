
#include <stdlib.h>
#include <string.h>

#include "gen_set.h"

/** 空树高度 */
#define GEN_SET_NULL_HEIGHT                                (0)

/////////////////////////////////// allocator /////////////////////////////////
static void* gen_set_inner_malloc(uint32 size, void* p_param)
{
	return malloc(size);
}

static int32 gen_set_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/**  
 * @brief 计算节点高度
 * @param [in] p_set 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_calc_height(GEN_SET* p_set, SET_NODE* p_node)
{
	/** 计算左子树高度 */
	if( p_node->p_left )
	{
		p_node->left_height = p_node->p_left->height;

	}else
	{
		p_node->left_height = 0;
	}

	/** 计算右子树高度 */
	if( p_node->p_right )
	{
		p_node->right_height = p_node->p_right->height;

	}else
	{
		p_node->right_height = 0;
	}

	/** 更新树高度 */
	if( p_node->left_height > p_node->right_height )
	{
		p_node->height = p_node->left_height + 1;

	}else
	{
		p_node->height = p_node->right_height + 1;
	}

	return 0;
}

/**  
 * @brief 寻找插入位置
 * @param [in] p_set 树句柄
 * @param [in] p_key 关键字
 * @param [out] pp_node 节点句柄
 * @param [out] p_left 是否左子树
 * @return
 * 0：成功
 * -1：失败
 * -3：节点已存在
 * @remark
 * 1、返回值为0表示要查找的关键字对应的节点不存在，此时pp_node表示插入位置的父节点句柄；
 * 返回值为-3表示要查找的关键字对应的节点已存在，此时pp_node表示对应节点句柄
 * 2、返回值为0时，如果*pp_node为空，表示插入位置为根节点
 */
int32 gen_set_find_insert_pos(GEN_SET* p_set, void* p_key, SET_NODE** pp_node, int32* p_left)
{
	SET_NODE* p_parent;
	SET_NODE* p_node;

	int32 b_find;
	int32 result;

	int32 ret;

	//*p_left = 1;

	p_parent = NULL;
	p_node = p_set->p_root;
	b_find = 0;

	while( p_node )
	{
		ret = p_set->comparer.compare(p_key, p_node->p_key, p_set->comparer.p_param, &result);
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
		return -3;

	}else
	{
		*pp_node = p_parent;
		return 0;
	}
}

/**  
 * @brief 寻找删除位置
 * @param [in] p_set 树句柄
 * @param [in] p_node 要删除的节点句柄
 * @param [out] pp_node 实际删除位置对应的节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、要删除的节点左子树必须非空!
 */
int32 gen_set_find_delete_pos(GEN_SET* p_set, SET_NODE* p_node, SET_NODE** pp_node)
{
	SET_NODE* p_current = p_node;
	SET_NODE* p_temp_node;

	void* p_temp_key;

	int32 ret;

	/** 寻找左子树的最右节点，该节点是左子树中最大节点 */
	p_temp_node = p_current->p_left;
	while( p_temp_node->p_right )
	{
		p_temp_node = p_temp_node->p_right;
	}

	/** 交换当前节点与最大节点 */
	p_temp_key = p_temp_node->p_key;
	p_temp_node->p_key = p_current->p_key;
	p_current->p_key = p_temp_key;

	*pp_node = p_temp_node;

	return 0;
}

/**  
 * @brief 左旋转
 * @param [in] p_set 树句柄
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
int32 gen_set_left_rotate(GEN_SET* p_set, SET_NODE* p_a, SET_NODE* p_b, SET_NODE* p_x)
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
		p_set->p_root = p_b;

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
 * @param [in] p_set 树句柄
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
int32 gen_set_left_adjust(GEN_SET* p_set, SET_NODE* p_a, SET_NODE* p_b, SET_NODE* p_x)
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
 * @param [in] p_set 树句柄
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
int32 gen_set_right_rotate(GEN_SET* p_set, SET_NODE* p_a, SET_NODE* p_b, SET_NODE* p_x)
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
		p_set->p_root = p_b;

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
 * @param [in] p_set 树句柄
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
int32 gen_set_right_adjust(GEN_SET* p_set, SET_NODE* p_a, SET_NODE* p_b, SET_NODE* p_x)
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
 * @param [in] p_set 树句柄
 * @param [in] p_node 插入节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
int32 gen_set_insert_balance(GEN_SET* p_set, SET_NODE* p_node)
{
	SET_NODE* p_ancestor;

	SET_NODE* p_uncle;
	SET_NODE* p_current;

	int32 ret;

	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			p_current->color = GEN_SET_COLOR_BLACK;
			break;
		}

		if( p_current->p_parent->color == GEN_SET_COLOR_BLACK )
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
		if( p_uncle && p_uncle->color == GEN_SET_COLOR_RED )
		{
			/** case 3 */
			p_ancestor->color = GEN_SET_COLOR_RED;
			p_current->p_parent->color = GEN_SET_COLOR_BLACK;
			p_uncle->color = GEN_SET_COLOR_BLACK;
			p_current = p_ancestor;

			continue;
		}

		/** from above, uncle is null or always black */
		if( (p_current == p_current->p_parent->p_right) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 4(left) */
			ret = gen_set_left_adjust(p_set, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_left;

		}else if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_right) )
		{
			/** case 4(right) */
			ret = gen_set_right_adjust(p_set, p_ancestor, p_current->p_parent, p_current);
			p_current = p_current->p_right;
		}

		p_current->p_parent->color = GEN_SET_COLOR_BLACK;
		p_ancestor->color = GEN_SET_COLOR_RED;

		if( (p_current == p_current->p_parent->p_left) && (p_current->p_parent == p_ancestor->p_left) )
		{
			/** case 5(left) */
			ret = gen_set_left_rotate(p_set, p_ancestor, p_current->p_parent, p_current);

		}else
		{
			/** case 5(right) */
			ret = gen_set_right_rotate(p_set, p_ancestor, p_current->p_parent, p_current);
		}

		break;
	}

	return 0;
}

/**  
 * @brief 调整删除平衡
 * @param [in] p_set 树句柄
 * @param [in] p_node 起始节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
int32 gen_set_delete_balance(GEN_SET* p_set, SET_NODE* p_node)
{
	SET_NODE* p_sibling;
	SET_NODE* p_sibling_left;
	SET_NODE* p_sibling_right;
	SET_NODE* p_current;

	int32 ret;

	/** from above, p_node is black */
	p_current = p_node;

	while( 1 )
	{
		if( p_current->p_parent == NULL )
		{
			/** case 1 */
			//p_current->color = GEN_SET_COLOR_BLACK;
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
		if( p_sibling->color == GEN_SET_COLOR_RED )
		{
			/** case 2 */
			p_current->p_parent->color = GEN_SET_COLOR_RED;
			p_sibling->color = GEN_SET_COLOR_BLACK;

			if( p_current == p_current->p_parent->p_left )
			{
				ret = gen_set_right_rotate(p_set, p_current->p_parent, p_sibling, p_sibling->p_right);
				p_sibling = p_sibling_left;

			}else
			{
				ret = gen_set_left_rotate(p_set, p_current->p_parent, p_sibling, p_sibling->p_left);
				p_sibling = p_sibling_right;
			}
		}

		/** from above, sibling always black */
		if( (p_current->p_parent->color == GEN_SET_COLOR_BLACK)
			&& (p_sibling->color == GEN_SET_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK) )
		{
			/** case 3 */
			p_sibling->color = GEN_SET_COLOR_RED;

			//ret = gen_set_calc_height(p_set, p_current->p_parent);

			p_current = p_current->p_parent;
			continue;
		}

		if( (p_current->p_parent->color == GEN_SET_COLOR_RED)
			&& (p_sibling->color == GEN_SET_COLOR_BLACK)
			&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
			&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK) )
		{
			/** case 4 */
			p_sibling->color = GEN_SET_COLOR_RED;
			p_current->p_parent->color = GEN_SET_COLOR_BLACK;

			break;
		}

		/** from case 3 & 4, sibling's child can not both be null */
		p_sibling_left = p_sibling->p_left;
		p_sibling_right = p_sibling->p_right;
		//if( p_sibling->color == GEN_SET_COLOR_BLACK )
		{
			if( (p_current == p_current->p_parent->p_left)
				&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK)
				&& (p_sibling_left && p_sibling->p_left->color == GEN_SET_COLOR_RED) )
			{
				/** case 5(left) */
				p_sibling->color = GEN_SET_COLOR_RED;
				p_sibling->p_left->color = GEN_SET_COLOR_BLACK;
				ret = gen_set_left_rotate(p_set, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

				p_sibling = p_sibling_left;

			}else if( (p_current == p_current->p_parent->p_right)
				&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
				&& (p_sibling_right && p_sibling->p_right->color == GEN_SET_COLOR_RED) )
			{
				/** case 5(right) */
				p_sibling->color = GEN_SET_COLOR_RED;
				p_sibling->p_right->color = GEN_SET_COLOR_BLACK;
				ret = gen_set_right_rotate(p_set, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

				p_sibling = p_sibling_right;
			}
		}

		p_sibling->color = p_current->p_parent->color;
		p_current->p_parent->color = GEN_SET_COLOR_BLACK;

		if( p_current == p_current->p_parent->p_left )
		{
			/** case 6(left) */
			if( p_sibling->p_right )
			{
				p_sibling->p_right->color = GEN_SET_COLOR_BLACK;
			}

			ret = gen_set_right_rotate(p_set, p_current->p_parent, p_sibling, p_sibling->p_right);

		}else
		{
			/** case 6(right) */
			if( p_sibling->p_left )
			{
				p_sibling->p_left->color = GEN_SET_COLOR_BLACK;
			}

			ret = gen_set_left_rotate(p_set, p_current->p_parent, p_sibling, p_sibling->p_left);
		}

		break;
	}

	return 0;
}

/**  
 * @brief 调整删除平衡2(对应节点为空)
 * @param [in] p_set 树句柄
 * @param [in] p_parent 父节点句柄
 * @param [in] b_left 是否为父节点左子树
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、实现参考http://en.wikipedia.org/wiki/Red-black_tree
 */
int32 gen_set_delete_balance2(GEN_SET* p_set, SET_NODE* p_parent, int32 b_left)
{
	SET_NODE* p_sibling;
	SET_NODE* p_sibling_left;
	SET_NODE* p_sibling_right;

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
	if( p_sibling->color == GEN_SET_COLOR_RED )
	{
		/** case 2 */
		p_parent->color = GEN_SET_COLOR_RED;
		p_sibling->color = GEN_SET_COLOR_BLACK;

		if( b_left )
		{
			ret = gen_set_right_rotate(p_set, p_parent, p_sibling, p_sibling->p_right);
			p_sibling = p_sibling_left;

		}else
		{
			ret = gen_set_left_rotate(p_set, p_parent, p_sibling, p_sibling->p_left);
			p_sibling = p_sibling_right;
		}
	}

	/** from above, sibling always black */
	if( (p_parent->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK) )
	{
		/** case 3 */
		p_sibling->color = GEN_SET_COLOR_RED;
		return gen_set_delete_balance(p_set, p_parent);
	}

	if( (p_parent->color == GEN_SET_COLOR_RED)
		&& (p_sibling->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK) )
	{
		/** case 4 */
		p_sibling->color = GEN_SET_COLOR_RED;
		p_parent->color = GEN_SET_COLOR_BLACK;

		return 0;
	}

	/** from case 3 & 4, sibling's child can not both be null */
	p_sibling_left = p_sibling->p_left;
	p_sibling_right = p_sibling->p_right;

	if( (b_left)
		&& (p_sibling->p_right == NULL || p_sibling->p_right->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling_left && p_sibling->p_left->color == GEN_SET_COLOR_RED) )
	{
		/** case 5(left) */
		p_sibling->color = GEN_SET_COLOR_RED;
		p_sibling->p_left->color = GEN_SET_COLOR_BLACK;
		ret = gen_set_left_rotate(p_set, p_sibling, p_sibling->p_left, p_sibling->p_left->p_left);

		p_sibling = p_sibling_left;

	}else if( (!b_left)
		&& (p_sibling->p_left == NULL || p_sibling->p_left->color == GEN_SET_COLOR_BLACK)
		&& (p_sibling_right && p_sibling->p_right->color == GEN_SET_COLOR_RED) )
	{
		/** case 5(right) */
		p_sibling->color = GEN_SET_COLOR_RED;
		p_sibling->p_right->color = GEN_SET_COLOR_BLACK;
		ret = gen_set_right_rotate(p_set, p_sibling, p_sibling->p_right, p_sibling->p_right->p_right);

		p_sibling = p_sibling_right;
	}

	p_sibling->color = p_parent->color;
	p_parent->color = GEN_SET_COLOR_BLACK;

	if( b_left )
	{
		/** case 6(left) */
		if( p_sibling->p_right )
		{
			p_sibling->p_right->color = GEN_SET_COLOR_BLACK;
		}

		ret = gen_set_right_rotate(p_set, p_parent, p_sibling, p_sibling->p_right);

	}else
	{
		/** case 6(right) */
		if( p_sibling->p_left )
		{
			p_sibling->p_left->color = GEN_SET_COLOR_BLACK;
		}

		ret = gen_set_left_rotate(p_set, p_parent, p_sibling, p_sibling->p_left);
	}

	return 0;
}

/**  
 * @brief 删除所有子节点(递归方式)
 * @param [in] p_set 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_delete_child_node(GEN_SET* p_set, SET_NODE* p_node)
{
	int32 ret;

	/** 删除左子树 */
	if( p_node->p_left )
	{
		ret = gen_set_delete_child_node(p_set, p_node->p_left);

		ret = p_set->allocator.deallocate(p_node->p_left, p_set->allocator.p_param);

		p_node->p_left = NULL;
		p_node->left_height = 0;
	}

	/** 删除右子树 */
	if( p_node->p_right )
	{
		ret = gen_set_delete_child_node(p_set, p_node->p_right);

		ret = p_set->allocator.deallocate(p_node->p_right, p_set->allocator.p_param);

		p_node->p_right = NULL;
		p_node->right_height = 0;
	}

	p_node->height = 1;

	return 0;
}

/**  
 * @brief 初始化树链表
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 */
static int32 gen_set_list_init(GEN_SET* p_set)
{
	p_set->p_head = &p_set->head;
	p_set->p_rear = &p_set->rear;

	memset(p_set->p_head, 0, sizeof(SET_NODE));
	p_set->p_head->p_prev = NULL;
	p_set->p_head->p_next = p_set->p_rear;

	memset(p_set->p_rear, 0, sizeof(SET_NODE));
	p_set->p_rear->p_prev = p_set->p_head;
	p_set->p_rear->p_next = NULL;

	return 0;
}

/**  
 * @brief 反初始化树链表
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_deinit(GEN_SET* p_set)
{
	int32 ret;

	if( p_set->p_head )
	{
		p_set->p_head = NULL;
	}

	if( p_set->p_rear )
	{
		p_set->p_rear = NULL;
	}

	return 0;
}

/**  
 * @brief 清空树链表
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_clear(GEN_SET* p_set)
{
	p_set->p_head->p_next = p_set->p_rear;
	p_set->p_rear->p_prev = p_set->p_head;

	return 0;
}

/**  
 * @brief 向树链表首部插入节点
 * @param [in] p_set set句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_push_front_node(GEN_SET *p_set, SET_NODE *p_node)
{
	p_node->p_prev = p_set->p_head;
	p_node->p_next = p_set->p_head->p_next;
	p_set->p_head->p_next->p_prev = p_node;
	p_set->p_head->p_next = p_node;

	return 0;
}

/**  
 * @brief 向树链表中指定节点之前插入节点
 * @param [in] p_set set句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_before 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_insert_before_node(GEN_SET *p_set, SET_NODE *p_node, SET_NODE *p_before)
{
	p_node->p_prev = p_before->p_prev;
	p_node->p_next = p_before;
	p_before->p_prev->p_next = p_node;
	p_before->p_prev = p_node;

	return 0;
}

/**  
 * @brief 向树链表中指定节点之后插入节点
 * @param [in] p_set set句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_after 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_insert_after_node(GEN_SET *p_set, SET_NODE *p_node, SET_NODE *p_after)
{
	p_node->p_prev = p_after;
	p_node->p_next = p_after->p_next;
	p_after->p_next->p_prev = p_node;
	p_after->p_next = p_node;

	return 0;
}

/**  
 * @brief 删除树链表中指定节点
 * @param [in] p_set set句柄
 * @param [in] p_node 节点句柄
 * @param [in] p_after 指定节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_delete_node(GEN_SET *p_set, SET_NODE *p_node)
{
	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	return 0;
}

/**  
 * @brief 树链表逆序
 * @param [in] p_set map句柄
 * @return
 * 0：成功
 * -1：失败
 */
static int32 gen_set_list_reverse(GEN_SET *p_set)
{
	SET_NODE *p_current = NULL;
	SET_NODE *p_node  = NULL;
	SET_NODE *p_node2 = NULL;
	SET_NODE *p_first_node = NULL;

	int32 ret;

	if( p_set->set_size < 2 )
	{
		/** 无需调整 */
		return 0;
	}

	p_current = p_set->p_head->p_next;
	p_node = p_current->p_next;
	while( p_node != p_set->p_rear )
	{
		p_node2 = p_node->p_next;

		p_node->p_next = p_current;
		p_current->p_prev = p_node;

		p_current = p_node;
		p_node = p_node2;
	}

	p_first_node = p_set->p_head->p_next;
	p_set->p_head->p_next = p_set->p_rear->p_prev;
	p_set->p_rear->p_prev = p_first_node;

	p_set->p_head->p_next->p_prev = p_set->p_head;
	p_set->p_rear->p_prev->p_next = p_set->p_rear;

	return 0;
}

///////////////////////////////////外部接口////////////////////////////////////////
GEN_SET* gen_set_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer)
{
	GEN_SET *p_set = NULL;

	if( p_comparer == NULL )
	{
		return NULL;
	}

	if( p_allocator )
	{
		p_set = (GEN_SET *)p_allocator->allocate(sizeof(GEN_SET), p_allocator->p_param);

	}else
	{
		p_set = (GEN_SET *)malloc( sizeof(GEN_SET) );
	}

	if( p_set == NULL )
	{
		return NULL;
	}

	memset(p_set, 0, sizeof(GEN_SET));

	if( p_allocator )
	{
		memcpy(&p_set->allocator, p_allocator, sizeof(p_set->allocator));

	}else
	{
		p_set->allocator.allocate = &gen_set_inner_malloc;
		p_set->allocator.deallocate = &gen_set_inner_free;
		p_set->allocator.p_param = p_set;
	}

	p_set->comparer = *p_comparer;

	return p_set;
}


int32 gen_set_destroy(GEN_SET *p_set)
{
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( p_set->b_init )
	{
		gen_set_deinit(p_set);
	}

	ret = p_set->allocator.deallocate(p_set, p_set->allocator.p_param);

	return 0;
}

int32 gen_set_init(GEN_SET *p_set)
{
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( p_set->b_init )
	{
		return 0;
	}

	p_set->set_size = 0;
	p_set->b_descend = 0;
	p_set->p_root = NULL;

	ret = gen_set_list_init(p_set);
	if( ret )
	{
		gen_set_deinit(p_set);
		return -3;
	}

	p_set->b_init = 1;

	return 0;
}

int32 gen_set_deinit(GEN_SET *p_set)
{
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	/** 删除根节点 */
	if( p_set->p_root )
	{
		ret = gen_set_delete_child_node(p_set, p_set->p_root);

		ret = p_set->allocator.deallocate(p_set->p_root, p_set->allocator.p_param);

		p_set->p_root = NULL;
	}

	gen_set_list_deinit(p_set);

	p_set->set_size = 0;
	p_set->b_init = 0;

	return 0;
}

int32 gen_set_clear(GEN_SET *p_set)
{
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -3;
	}

	if( p_set->p_root )
	{
		ret = gen_set_delete_child_node(p_set, p_set->p_root);

		ret = p_set->allocator.deallocate(p_set->p_root, p_set->allocator.p_param);

		p_set->p_root = NULL;
	}

	gen_set_list_clear(p_set);

	p_set->set_size = 0;

	return 0;
}

int32 gen_set_get_size(GEN_SET *p_set)
{
	if( p_set == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	return p_set->set_size;
}

int32 gen_set_set_order(GEN_SET* p_set, int32 b_descend)
{
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	if( p_set->b_descend )
	{
		/** 降序 */
		if( !b_descend )
		{
			ret = gen_set_list_reverse(p_set);
		}

	}else
	{
		/** 升序 */
		if( b_descend )
		{
			ret = gen_set_list_reverse(p_set);
		}
	}

	p_set->b_descend = b_descend;

	return 0;
}

int32 gen_set_find_node(GEN_SET* p_set, void* p_key, SET_NODE** pp_node)
{
	int32 b_left;
	int32 ret;

	if( p_set == NULL || pp_node == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	ret = gen_set_find_insert_pos(p_set, p_key, pp_node, &b_left);
	if( ret == 0 )
	{
		*pp_node = NULL;
		return -4;

	}else
	{
		return 0;
	}
}

int32 gen_set_insert_key(GEN_SET* p_set, void* p_key, SET_NODE** pp_node)
{
	SET_NODE* p_parent = NULL;
	SET_NODE* p_node = NULL;
	SET_NODE* p_temp = NULL;

	int32 b_left;
	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	ret = gen_set_find_insert_pos(p_set, p_key, &p_parent, &b_left);
	if( ret )
	{
		return -3;
	}

	p_node = (SET_NODE*)p_set->allocator.allocate(sizeof(SET_NODE), p_set->allocator.p_param);

	if( p_node == NULL )
	{
		return -2;
	}

	p_node->color = GEN_SET_COLOR_RED;

	p_node->height = 1;
	p_node->left_height = 0;
	p_node->right_height = 0;

	p_node->p_key = p_key;

	p_node->p_parent = p_parent;
	p_node->p_left = NULL;
	p_node->p_right = NULL;

	if( p_parent == NULL )
	{
		/** 插入根节点 */
		p_set->p_root = p_node;
		ret = gen_set_list_push_front_node(p_set, p_node);

	}else
	{
		/** 插入非根节点 */
		if( b_left )
		{
			p_parent->p_left = p_node;

			if( p_set->b_descend )
			{
				/** 降序 */
				ret = gen_set_list_insert_after_node(p_set, p_node, p_parent);

			}else
			{
				/** 升序 */
				ret = gen_set_list_insert_before_node(p_set, p_node, p_parent);
			}

		}else
		{
			p_parent->p_right = p_node;

			if( p_set->b_descend )
			{
				ret = gen_set_list_insert_before_node(p_set, p_node, p_parent);

			}else
			{
				ret = gen_set_list_insert_after_node(p_set, p_node, p_parent);
			}
		}
	}

	ret = gen_set_insert_balance(p_set, p_node);
	p_set->set_size++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return 0;
}

int32 gen_set_delete_node(GEN_SET* p_set, SET_NODE* p_node)
{
	SET_NODE *p_node2 = NULL;
	SET_NODE *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_set == NULL || p_node == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	if( p_node->p_left == NULL )
	{
		/** 左子树为空 */
		if( p_node->p_parent == NULL )
		{
			/** 根节点 */
			p_set->p_root = p_node->p_right;

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
		ret = gen_set_find_delete_pos(p_set, p_node, &p_node2);
		p_node = p_node2;

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

	if( p_node->color != GEN_SET_COLOR_RED )
	{
		/** 要删除的节点是黑色 */
		if( p_child == NULL )
		{
			/** 子节点为空 */
			ret = gen_set_delete_balance2(p_set, p_node->p_parent, b_left);

		}else
		{
			/** 子节点非空 */
			if( p_child->color == GEN_SET_COLOR_RED )
			{
				/** 子节点是红色 */
				p_child->color = GEN_SET_COLOR_BLACK;

			}else
			{
				/** 子节点是黑色 */
				ret = gen_set_delete_balance(p_set, p_child);
			}
		}
	}

	ret = gen_set_list_delete_node(p_set, p_node);

	ret = p_set->allocator.deallocate(p_node, p_set->allocator.p_param);

	p_set->set_size--;

	return 0;
}

int32 gen_set_delete_key(GEN_SET* p_set, void* p_key)
{
	SET_NODE *p_node = NULL;
	SET_NODE *p_node2 = NULL;
	SET_NODE *p_child = NULL;

	int32 b_left = 0;

	int32 ret;

	if( p_set == NULL )
	{
		return -1;
	}

	if( !p_set->b_init )
	{
		return -1;
	}

	ret = gen_set_find_insert_pos(p_set, p_key, &p_node, &b_left);
	if( ret == 0 )
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
			p_set->p_root = p_node->p_right;

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
		ret = gen_set_find_delete_pos(p_set, p_node, &p_node2);
		p_node = p_node2;

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

	if( p_node->color != GEN_SET_COLOR_RED )
	{
		/** 要删除的节点是黑色 */
		if( p_child == NULL )
		{
			/** 子节点为空 */
			ret = gen_set_delete_balance2(p_set, p_node->p_parent, b_left);

		}else
		{
			/** 子节点非空 */
			if( p_child->color == GEN_SET_COLOR_RED )
			{
				/** 子节点是红色 */
				p_child->color = GEN_SET_COLOR_BLACK;

			}else
			{
				/** 子节点是黑色 */
				ret = gen_set_delete_balance(p_set, p_child);
			}
		}
	}

	ret = gen_set_list_delete_node(p_set, p_node);

	ret = p_set->allocator.deallocate(p_node, p_set->allocator.p_param);

	p_set->set_size--;

	return 0;
}
