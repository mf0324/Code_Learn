
#include <stdlib.h>
#include <string.h>

#include "gen_tree.h"

/** 空树深度 */
#define GEN_TREE_BASE_DEGREE                       (0)
/** 顶层对应编号 */
#define GEN_TREE_TOP_LAYER_NUMBER                  (0)
/** 起始索引 */
#define GEN_TREE_INDEX_BEGIN                       (0)

/**  
 * @brief 删除所有子节点(递归方式)
 * @param [in] p_tree 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_tree_delete_child_node(GEN_TREE *p_tree, TREE_NODE *p_node)
{
	TREE_NODE *p_temp = p_node->p_first_child;
	TREE_NODE *p_temp2;

	int32 ret;

	while( p_temp )
	{
		ret = gen_tree_delete_child_node(p_tree, p_temp);

		p_temp2 = p_temp;
		p_temp = p_temp->p_next;

		if( p_temp2->b_inner )
		{
			free(p_temp2);
		}
	}

	p_node->p_first_child = NULL;
	p_node->p_last_child = NULL;
	p_node->degree = 0;

	return 0;
}

///////////////////////////////////外部接口////////////////////////////////////////
GEN_TREE* gen_tree_create()
{
	GEN_TREE *p_tree = NULL;

	p_tree = (GEN_TREE *)malloc( sizeof(GEN_TREE) );
	if( p_tree == NULL )
	{
		return NULL;
	}

	memset(p_tree, 0, sizeof(GEN_TREE));

	return p_tree;
}


int32 gen_tree_destroy(GEN_TREE *p_tree)
{
	if( p_tree == NULL )
	{
		return -1;
	}

	if( p_tree->b_init )
	{
		gen_tree_deinit(p_tree);
	}

	free(p_tree);

	return 0;
}

int32 gen_tree_init(GEN_TREE *p_tree)
{
	if( p_tree == NULL )
	{
		return -1;
	}

	if( p_tree->b_init )
	{
		return 0;
	}

	/** 分配根节点 */
	p_tree->p_root = (TREE_NODE*)malloc( sizeof(TREE_NODE) );
	if( p_tree->p_root == NULL )
	{
		gen_tree_deinit(p_tree);
		return -2;
	}

	p_tree->p_root->b_inner = 1;
	p_tree->p_root->degree = 0;
	p_tree->p_root->p_parent = NULL;
	p_tree->p_root->p_prev = NULL;
	p_tree->p_root->p_next = NULL;
	p_tree->p_root->p_first_child = NULL;
	p_tree->p_root->p_last_child = NULL;
	p_tree->p_root->p_data = 0;

	p_tree->tree_node_num = 0;
	p_tree->tree_depth = GEN_TREE_BASE_DEGREE;
	p_tree->b_init = 1;

	return 0;
}

int32 gen_tree_deinit(GEN_TREE *p_tree)
{
	int32 ret;

	if( p_tree == NULL )
	{
		return -1;
	}

	/** 删除根节点 */
	if( p_tree->p_root )
	{
		ret = gen_tree_delete_child_node(p_tree, p_tree->p_root);

		if( p_tree->p_root->b_inner )
		{
			free(p_tree->p_root);
		}

		p_tree->p_root = NULL;
	}

	p_tree->b_init = 0;

	return 0;
}

int32 gen_tree_clear(GEN_TREE *p_tree)
{
	int32 ret;

	if( p_tree == NULL )
	{
		return -1;
	}

	if( !p_tree->b_init )
	{
		return -3;
	}

	if( p_tree->p_root )
	{
		ret = gen_tree_delete_child_node(p_tree, p_tree->p_root);

		p_tree->p_root->p_first_child = NULL;
		p_tree->p_root->p_last_child = NULL;
	}

	return 0;
}

int32 gen_tree_get_node_num(GEN_TREE *p_tree)
{
	if( p_tree == NULL )
	{
		return -1;
	}

	return p_tree->tree_node_num;
}

int32 gen_tree_get_depth(GEN_TREE *p_tree)
{
	if( p_tree == NULL )
	{
		return -1;
	}

	return p_tree->tree_depth;
}

int32 gen_tree_add_node(GEN_TREE *p_tree, TREE_NODE *p_parent, TREE_NODE *p_node)
{
	TREE_NODE *p_temp = NULL;

	int32 ret;

	if( p_tree == NULL || p_parent == NULL || p_node == NULL )
	{
		return -1;
	}

	if( p_parent->p_last_child )
	{
		/** 添加到尾节点之后 */
		p_node->b_inner = 0;
		p_node->degree = 0;
		p_node->p_parent = p_parent;
		p_node->p_prev = p_parent->p_last_child;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_parent->p_last_child->p_next = p_node;
		p_parent->p_last_child = p_node;

	}else if( p_parent->p_first_child )
	{
		p_temp = p_parent->p_first_child;
		while( p_temp->p_next )
		{
			p_temp = p_temp->p_next;
		}

		/** 添加节点 */
		p_node->b_inner = 0;
		p_node->degree = 0;
		p_node->p_parent = p_parent;
		p_node->p_prev = p_temp;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_temp->p_next = p_node;
		p_parent->p_last_child = p_node;

	}else
	{
		/** 添加第一个子节点 */
		p_node->b_inner = 0;
		p_node->degree = 0;
		p_node->p_parent = p_parent;
		p_node->p_prev = NULL;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_parent->p_first_child = p_node;
		p_parent->p_last_child = p_node;
	}

	p_parent->degree++;
	p_tree->tree_node_num++;

	return 0;
}

int32 gen_tree_add_data(GEN_TREE *p_tree, TREE_NODE *p_parent, TREE_NODE **pp_node, void *p_data)
{
	TREE_NODE *p_node = NULL;
	TREE_NODE *p_temp = NULL;

	int32 ret;

	if( p_tree == NULL || p_parent == NULL )
	{
		return -1;
	}

	p_node = (TREE_NODE*)malloc( sizeof(TREE_NODE) );
	if( p_node == NULL )
	{
		return -2;
	}

	p_node->b_inner = 1;
	p_node->degree = 0;
	p_node->p_data = p_data;

	if( p_parent->p_last_child )
	{
		/** 添加到尾节点之后 */
		p_node->p_parent = p_parent;
		p_node->p_prev = p_parent->p_last_child;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_parent->p_last_child->p_next = p_node;
		p_parent->p_last_child = p_node;

	}else if( p_parent->p_first_child )
	{
		p_temp = p_parent->p_first_child;
		while( p_temp->p_next )
		{
			p_temp = p_temp->p_next;
		}

		/** 添加节点 */
		p_node->p_parent = p_parent;
		p_node->p_prev = p_temp;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_temp->p_next = p_node;
		p_parent->p_last_child = p_node;

	}else
	{
		/** 添加第一个子节点 */
		p_node->p_parent = p_parent;
		p_node->p_prev = NULL;
		p_node->p_next = NULL;
		p_node->p_first_child = NULL;
		p_node->p_last_child = NULL;

		p_parent->p_first_child = p_node;
		p_parent->p_last_child = p_node;
	}

	p_parent->degree++;
	p_tree->tree_node_num++;

	if( pp_node )
	{
		*pp_node = p_node;
	}

	return 0;
}

int32 gen_tree_delete_node(GEN_TREE *p_tree, TREE_NODE *p_node)
{
	TREE_NODE *p_parent = NULL;

	int32 ret;

	if( p_tree == NULL || p_node == NULL )
	{
		return -1;
	}

	ret = gen_tree_delete_child_node(p_tree, p_node);

	p_parent = p_node->p_parent;
	if( p_parent )
	{
		if( p_node->p_prev )
		{
			p_node->p_prev->p_next = p_node->p_next;
		}

		if( p_node->p_next )
		{
			p_node->p_next->p_prev = p_node->p_prev;
		}

		if( p_node == p_parent->p_first_child )
		{
			p_parent->p_first_child = p_node->p_next;

		}else if( p_node == p_parent->p_last_child )
		{
			p_parent->p_last_child = p_node->p_prev;
		}

		p_parent->degree--;
		if( p_parent->degree < 1 )
		{
			p_parent->p_first_child = NULL;
			p_parent->p_last_child = NULL;
		}
	}

	if( p_node->b_inner )
	{
		free(p_node);
	}

	p_tree->tree_node_num--;

	return 0;
}
