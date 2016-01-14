
#include <stdlib.h>
#include <string.h>

#include "mp4_file_tree.h"




/////////////////////////////////// allocator /////////////////////////////////
static void* mp4_file_tree_inner_malloc(uint32 size, void* p_param)
{
	return malloc(size);
}

static int32 mp4_file_tree_inner_free(void *p_buf, void* p_param)
{
	free(p_buf);

	return 0;
}

/////////////////////////////// Inner Interface ///////////////////////////////
/**  
 * @brief init root
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_init_root(MP4_FILE_TREE* p_tree)
{
	p_tree->p_root = &p_tree->root;

	memset(p_tree->p_root, 0, sizeof(p_tree->root));

	p_tree->p_root->p_parent = NULL;
	p_tree->p_root->p_prev_sibling = NULL;
	p_tree->p_root->p_next_sibling = NULL;
	p_tree->p_root->p_first_child = NULL;
	p_tree->p_root->p_last_child = NULL;
	p_tree->p_root->p_prev = NULL;
	p_tree->p_root->p_next = NULL;

	return 0;
}

/**  
 * @brief deinit root
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_deinit_root(MP4_FILE_TREE* p_tree)
{
	if( p_tree->p_root )
	{
		p_tree->p_root = NULL;
	}

	return 0;
}

/**  
 * @brief reset root
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * other: fail
 */
static int32 mp4_file_tree_reset_root(MP4_FILE_TREE* p_tree)
{
	p_tree->p_root->p_parent = NULL;
	p_tree->p_root->p_prev_sibling = NULL;
	p_tree->p_root->p_next_sibling = NULL;
	p_tree->p_root->p_first_child = NULL;
	p_tree->p_root->p_last_child = NULL;
	p_tree->p_root->p_prev = NULL;
	p_tree->p_root->p_next = NULL;

	return 0;
}

/**  
 * @brief init list
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_init_list(MP4_FILE_TREE* p_tree)
{
	p_tree->p_head = &p_tree->head;
	p_tree->p_rear = &p_tree->rear;

	memset(p_tree->p_head, 0, sizeof(p_tree->head));
	p_tree->p_head->p_prev = NULL;
	p_tree->p_head->p_next = p_tree->p_rear;

	memset(p_tree->p_rear, 0, sizeof(p_tree->rear));
	p_tree->p_rear->p_prev = p_tree->p_head;
	p_tree->p_rear->p_next = NULL;

	return 0;
}

/**  
 * @brief deinit list
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_deinit_list(MP4_FILE_TREE* p_tree)
{
	//int32 ret;

	if( p_tree->p_head )
	{
		p_tree->p_head = NULL;
	}

	if( p_tree->p_rear )
	{
		p_tree->p_rear = NULL;
	}

	return 0;
}

/**  
 * @brief clear list
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_clear_list(MP4_FILE_TREE* p_tree)
{
	MP4_FILE_BOX_NODE* p_node = NULL;
	MP4_FILE_BOX_NODE* p_temp_node = NULL;

	int32 ret;

	p_node = p_tree->p_head->p_next;
	while( p_node != p_tree->p_rear )
	{
		p_temp_node = p_node;
		p_node = p_node->p_next;

		ret = p_tree->allocator.deallocate(p_temp_node, p_tree->allocator.p_param);
	}

	p_tree->p_head->p_next = p_tree->p_rear;
	p_tree->p_rear->p_prev = p_tree->p_head;

	return 0;
}

/**  
 * @brief push back
 * @param [in] p_tree, tree
 * @param [in] p_node, tree node
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_push_back_node(MP4_FILE_TREE* p_tree, MP4_FILE_BOX_NODE* p_node)
{
	p_node->p_prev = p_tree->p_rear->p_prev;
	p_node->p_next = p_tree->p_rear;
	p_tree->p_rear->p_prev->p_next = p_node;
	p_tree->p_rear->p_prev = p_node;

	return 0;
}

/**  
 * @brief delete list node
 * @param [in] p_tree, tree
 * @param [in] p_node, tree node
 * @return
 * 0: success
 * -1: fail
 */
static int32 mp4_file_tree_delete_list_node(MP4_FILE_TREE* p_tree, MP4_FILE_BOX_NODE* p_node)
{

	if( p_node == p_tree->p_head || p_node == p_tree->p_rear )
	{
		/** prevent to delete head or rear */
		return -1;
	}

	p_node->p_prev->p_next = p_node->p_next;
	p_node->p_next->p_prev = p_node->p_prev;

	return 0;
}

/**  
 * @brief find box by type
 * @param [in] p_tree, tree
 * @param [in] box type, box type
 * @param [in] p_parent, parent node
 * @param [out] pp_node, found file box node
 * @return
 * 0: success
 * -1: fail
 * -2: not find
 * @remark
 * 1. on success return, pp_node contain the first node that match box type
 * 2. if p_parent is null, search from root node
 */
static int32 mp4_file_tree_find_box_by_type2(MP4_FILE_TREE* p_tree, uint32 box_type, MP4_FILE_BOX_NODE* p_parent, MP4_FILE_BOX_NODE** pp_node)
{
	MP4_FILE_BOX_NODE* p_temp_node = NULL;

	int32  b_find;

	int32  ret;

	b_find = 0;

	p_temp_node = p_parent->p_first_child;
	while( p_temp_node )
	{
		if( p_temp_node->box.payload.box.type == box_type )
		{
			b_find = 1;
			*pp_node = p_temp_node;
			break;
		}

		ret = mp4_file_tree_find_box_by_type2(p_tree, box_type, p_temp_node, pp_node);
		if( ret == 0 )
		{
			b_find = 1;
			break;
		}

		p_temp_node = p_temp_node->p_next_sibling;
	}

	if( b_find )
	{
		return 0;

	}else
	{
		return -2;
	}
}

/////////////////////////////// Outter Interface //////////////////////////////
MP4_FILE_TREE* mp4_file_tree_create(GEN_ALLOCATOR* p_allocator)
{
	MP4_FILE_TREE* p_tree = NULL;

	if( p_allocator )
	{
		p_tree = (MP4_FILE_TREE*)p_allocator->allocate(sizeof(MP4_FILE_TREE), p_allocator->p_param);

	}else
	{
		p_tree = (MP4_FILE_TREE*)malloc( sizeof(MP4_FILE_TREE) );
	}

	if( p_tree == NULL )
	{
		return NULL;
	}

	memset(p_tree, 0, sizeof(MP4_FILE_TREE));

	if( p_allocator )
	{
		memcpy(&p_tree->allocator, p_allocator, sizeof(p_tree->allocator));

	}else
	{
		p_tree->allocator.allocate = &mp4_file_tree_inner_malloc;
		p_tree->allocator.deallocate = &mp4_file_tree_inner_free;
		p_tree->allocator.p_param = p_tree;
	}

	return p_tree;
}

int32 mp4_file_tree_destroy(MP4_FILE_TREE* p_tree)
{
	int32 ret;

	if( p_tree == NULL )
	{
		return -1;
	}

	if( p_tree->b_init )
	{
		mp4_file_tree_deinit(p_tree);
	}

	ret = p_tree->allocator.deallocate(p_tree, p_tree->allocator.p_param);

	return 0;
}

int32 mp4_file_tree_init(MP4_FILE_TREE* p_tree)
{
	if( p_tree == NULL )
	{
		return -1;
	}

	if( p_tree->b_init )
	{
		return 0;
	}

	p_tree->tree_size = 0;

	mp4_file_tree_init_root(p_tree);

	mp4_file_tree_init_list(p_tree);

	p_tree->b_init = 1;

	return 0;
}

int32 mp4_file_tree_deinit(MP4_FILE_TREE* p_tree)
{
	//int32 ret;

	if( p_tree == NULL )
	{
		return -1;
	}

	if( p_tree->tree_size > 0 )
	{
		mp4_file_tree_clear(p_tree);
	}

	mp4_file_tree_deinit_list(p_tree);

	mp4_file_tree_deinit_root(p_tree);

	p_tree->b_init = 0;

	return 0;
}

int32 mp4_file_tree_clear(MP4_FILE_TREE* p_tree)
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

	ret = mp4_file_tree_clear_list(p_tree);
	ret = mp4_file_tree_reset_root(p_tree);

	p_tree->tree_size = 0;

	return 0;
}

int32 mp4_file_tree_get_size(MP4_FILE_TREE* p_tree, int32* p_size)
{
	if( p_tree == NULL || p_size == NULL )
	{
		return -1;
	}

	*p_size = p_tree->tree_size;

	return 0;
}

int32 mp4_file_tree_insert_box(MP4_FILE_TREE* p_tree, MP4_FILE_BOX* p_box, MP4_FILE_BOX_NODE* p_parent, MP4_FILE_BOX_NODE** pp_node)
{
	MP4_FILE_BOX_NODE* p_new_node = NULL;

	int32  ret;

	if( p_tree == NULL || p_box == NULL || p_parent == NULL )
	{
		return -1;
	}

	p_new_node = (MP4_FILE_BOX_NODE*)p_tree->allocator.allocate(sizeof(MP4_FILE_BOX_NODE), p_tree->allocator.p_param);
	if( p_new_node == NULL )
	{
		return -2;
	}

	p_new_node->box = *p_box;
	p_new_node->p_parent = p_parent;
	p_new_node->p_prev_sibling = NULL;
	p_new_node->p_next_sibling = NULL;
	p_new_node->p_first_child = NULL;
	p_new_node->p_last_child = NULL;
	p_new_node->p_prev = NULL;
	p_new_node->p_next = NULL;

	if( p_parent->p_first_child == NULL )
	{
		p_parent->p_first_child = p_new_node;
	}

	if( p_parent->p_last_child )
	{
		/** add to last child */
		p_parent->p_last_child->p_next_sibling = p_new_node;
		p_new_node->p_prev_sibling = p_parent->p_last_child;
	}

	p_parent->p_last_child = p_new_node;

	ret = mp4_file_tree_push_back_node(p_tree, p_new_node);

	p_tree->tree_size++;

	if( pp_node )
	{
		*pp_node = p_new_node;
	}

	return 0;
}

int32 mp4_file_tree_delete_node(MP4_FILE_TREE* p_tree, MP4_FILE_BOX_NODE* p_node)
{
	if( p_tree == NULL || p_node == NULL )
	{
		return -1;
	}

	if( !p_tree->b_init )
	{
		return -3;
	}

	return -1;
}

int32 mp4_file_tree_find_box_by_type(MP4_FILE_TREE* p_tree, uint32 box_type, MP4_FILE_BOX_NODE* p_parent, MP4_FILE_BOX_NODE** pp_node)
{
	MP4_FILE_BOX_NODE* p_from = p_parent;

	int32  ret;

	if( p_tree == NULL || pp_node == NULL )
	{
		return -1;
	}

	if( !p_tree->b_init )
	{
		return -3;
	}

	if( p_from == NULL )
	{
		p_from = p_tree->p_root;
	}

	ret = mp4_file_tree_find_box_by_type2(p_tree, box_type, p_from, pp_node);

	return ret;
}

int32 mp4_file_tree_find_next_sibling_by_type(MP4_FILE_TREE* p_tree, uint32 box_type, MP4_FILE_BOX_NODE* p_from, MP4_FILE_BOX_NODE** pp_node)
{
	MP4_FILE_BOX_NODE* p_next = NULL;

	int32  b_find;
	//int32  ret;

	if( p_tree == NULL || pp_node == NULL )
	{
		return -1;
	}

	if( !p_tree->b_init )
	{
		return -3;
	}

	if( p_from == NULL )
	{
		p_from = p_tree->p_root;
	}

	b_find = 0;
	*pp_node = NULL;

	p_next = p_from->p_next_sibling;
	while( p_next )
	{
		if( p_next->box.payload.box.type == box_type )
		{
			b_find = 1;
			*pp_node = p_next;
			break;
		}

		p_next = p_next->p_next_sibling;
	}

	if( b_find )
	{
		return 0;

	}else
	{
		return -2;
	}
}
