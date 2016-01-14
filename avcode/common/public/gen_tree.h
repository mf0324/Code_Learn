/**
 * @file gen_tree.h  Tree
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.04.27
 *
 *
 */

#ifndef __GEN_TREE_H__
#define __GEN_TREE_H__

#include <public/gen_int.h>

/** 树节点 */
typedef struct tag_tree_node
{
	/** 是否内部节点 */
	int32  b_inner;
	/** 节点的度(子节点数) */
	int32  degree;

	/** 父节点 */
	struct tag_tree_node *p_parent;
	/** 前一个兄弟节点 */
	struct tag_tree_node *p_prev;
	/** 下一个兄弟节点 */
	struct tag_tree_node *p_next;
	/** 第一个子节点 */
	struct tag_tree_node *p_first_child;
	/** 最后一个子节点 */
	struct tag_tree_node *p_last_child;

	/** 节点数据 */
	void*  p_data;

} TREE_NODE, *PTREE_NODE;

/** 树 */
typedef struct tag_gen_tree
{
	/** 是否初始化 */
	int32     b_init;
	/** 树节点个数 */
	int32     tree_node_num;
	/** 树深度 */
	int32     tree_depth;

	/** 根节点 */
	TREE_NODE *p_root;

} GEN_TREE, *PGEN_TREE;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建树
 * @return
 * 非空值：成功，返回值表示树句柄
 * NULL：失败
 */
GEN_TREE* gen_tree_create();

/**  
 * @brief 销毁树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、树中内部分配的节点会被内部释放，外部分配的节点需要外部自行处理，切记!
 */
int32 gen_tree_destroy(GEN_TREE *p_tree);

/**  
 * @brief 初始化树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_tree_init(GEN_TREE *p_tree);

/**  
 * @brief 反初始化树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_tree_deinit(GEN_TREE *p_tree);

/**  
 * @brief 清空树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、树中内部分配的节点会被内部释放，外部分配的节点需要外部自行处理，切记!
 */
int32 gen_tree_clear(GEN_TREE *p_tree);

/**  
 * @brief 获取树节点个数
 * @param [in] p_tree 树句柄
 * @return
 * 非负值：成功
 * -1：失败
 */
int32 gen_tree_get_node_num(GEN_TREE *p_tree);

/**  
 * @brief 获取树深度
 * @param [in] p_tree 树句柄
 * @return
 * 非负值：成功
 * -1：失败
 */
int32 gen_tree_get_depth(GEN_TREE *p_tree);

/**  
 * @brief 添加子节点
 * @param [in] p_tree 树句柄
 * @param [in] p_parent 父节点句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、添加的节点属于外部节点
 */
int32 gen_tree_add_node(GEN_TREE *p_tree, TREE_NODE *p_parent, TREE_NODE *p_node);

/**  
 * @brief 添加节点数据
 * @param [in] p_tree 树句柄
 * @param [in] p_parent 父节点句柄
 * @param [out] pp_node 节点地址
 * @param [in] p_data 节点数据
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_tree_add_data(GEN_TREE *p_tree, TREE_NODE *p_parent, TREE_NODE **pp_node, void *p_data);

/**  
 * @brief 删除节点
 * @param [in] p_tree 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_tree_delete_node(GEN_TREE *p_tree, TREE_NODE *p_node);


#ifdef __cplusplus
}
#endif


#endif ///__GEN_TREE_H__
