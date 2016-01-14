/**
 * @file mp4_file_tree.h  MP4 File Tree
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.03.28
 *
 *
 */

#ifndef __MP4_FILE_TREE_H__
#define __MP4_FILE_TREE_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>
#include "mp4_file_public.h"


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
 * 1. if allocator is null, default allocator(malloc) will be used
 */
MP4_FILE_TREE* mp4_file_tree_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief destroy
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_destroy(MP4_FILE_TREE* p_tree);

/**  
 * @brief init
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_init(MP4_FILE_TREE* p_tree);

/**  
 * @brief deinit
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_deinit(MP4_FILE_TREE* p_tree);

/**  
 * @brief clear
 * @param [in] p_tree, tree
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_clear(MP4_FILE_TREE* p_tree);

/**  
 * @brief get tree size
 * @param [in] p_tree, tree
 * @param [out] p_size, tree size
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_get_size(MP4_FILE_TREE* p_tree, int32* p_size);

/**  
 * @brief insert box
 * @param [in] p_tree, tree
 * @param [in] p_box, file box
 * @param [in] p_parent, parent node
 * @param [out] pp_node, actually insert file box node
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_insert_box(MP4_FILE_TREE* p_tree, MP4_FILE_BOX* p_box, MP4_FILE_BOX_NODE* p_parent, MP4_FILE_BOX_NODE** pp_node);

/**  
 * @brief delete node
 * @param [in] p_tree, tree
 * @param [in] p_node, the node to delete
 * @return
 * 0: success
 * -1: fail
 */
int32 mp4_file_tree_delete_node(MP4_FILE_TREE* p_tree, MP4_FILE_BOX_NODE* p_node);

/**  
 * @brief find box by type
 * @param [in] p_tree, tree
 * @param [in] box type, box type
 * @param [in] p_parent, parent node
 * @param [out] pp_node, found file box node
 * @return
 * 0: success
 * -1: fail
 * @remark
 * 1. on success return, pp_node contain the first node that match box type
 * 2. if p_parent is null, search from root node
 */
int32 mp4_file_tree_find_box_by_type(MP4_FILE_TREE* p_tree, uint32 box_type, MP4_FILE_BOX_NODE* p_parent, MP4_FILE_BOX_NODE** pp_node);

/**  
 * @brief find next sibling by type
 * @param [in] p_tree, tree
 * @param [in] box type, box type
 * @param [in] p_from, current node
 * @param [out] pp_node, found file box node
 * @return
 * 0: success
 * -1: fail
 * @remark
 * 1. on success return, pp_node contain the first node that match box type
 * 2. if p_from is null, search from root node
 */
int32 mp4_file_tree_find_next_sibling_by_type(MP4_FILE_TREE* p_tree, uint32 box_type, MP4_FILE_BOX_NODE* p_from, MP4_FILE_BOX_NODE** pp_node);

#ifdef __cplusplus
}
#endif


#endif ///__MP4_FILE_TREE_H__
