/**
 * @file gen_rb_tree.h  Red Black Tree
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.06.09
 *
 *
 */

#ifndef __GEN_RB_TREE_H__
#define __GEN_RB_TREE_H__

#include <public/gen_int.h>
#include <public/gen_key.h>
#include <public/gen_allocator.h>
#include <public/gen_comparer.h>

/** 红 */
#define GEN_RB_TREE_RED                                    (0)
/** 黑 */
#define GEN_RB_TREE_BLACK                                  (1)

/** 红黑树节点 */
typedef struct tag_rb_tree_node
{
	/** 颜色 */
	int32  color;

	/** 节点高度 */
	int32  height;
	/** 左子树高度 */
	int32  left_height;
	/** 右子树高度 */
	int32  right_height;

	/** key pair */
	KEY_PAIR pair;

	/** 父节点 */
	struct tag_rb_tree_node *p_parent;
	/** 左子节点 */
	struct tag_rb_tree_node *p_left;
	/** 右子节点 */
	struct tag_rb_tree_node *p_right;

	/** 前一节点 */
	struct tag_rb_tree_node *p_prev;
	/** 后一节点 */
	struct tag_rb_tree_node *p_next;

} RB_TREE_NODE, *PRB_TREE_NODE;

/** 红黑树 */
typedef struct tag_gen_rb_tree
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;

	/** 是否初始化 */
	int32  b_init;
	/** 节点个数 */
	int32  node_num;

	/** 根节点 */
	RB_TREE_NODE* p_root;

	/** 头节点指针 */
	RB_TREE_NODE* p_head;
	/** 尾节点指针 */
	RB_TREE_NODE* p_rear;
	/** 头节点 */
	RB_TREE_NODE  head;
	/** 尾节点 */
	RB_TREE_NODE  rear;

} GEN_RB_TREE, *PGEN_RB_TREE;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建红黑树
 * @param [in] p_allocator 分配器句柄
 * @param [in] p_comparer 比较器句柄
 * @return
 * 非空值：成功，返回值表示对应的树句柄
 * NULL：失败
 * @remark
 * 1、输入的比较器句柄不能为空!
 */
GEN_RB_TREE* gen_rb_tree_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief 销毁红黑树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_rb_tree_destroy(GEN_RB_TREE* p_tree);

/**  
 * @brief 初始化树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_rb_tree_init(GEN_RB_TREE* p_tree);

/**  
 * @brief 反初始化树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_rb_tree_deinit(GEN_RB_TREE* p_tree);

/**  
 * @brief 清空树
 * @param [in] p_tree 树句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_rb_tree_clear(GEN_RB_TREE* p_tree);

/**  
 * @brief 获取树节点个数
 * @param [in] p_tree 树句柄
 * @return
 * 非负值：成功，返回值表示节点个数
 * -1：失败
 */
int32 gen_rb_tree_get_node_num(GEN_RB_TREE* p_tree);

/**  
 * @brief 获取树高度
 * @param [in] p_tree 树句柄
 * @return
 * 非负值：成功
 * -1：失败
 */
int32 gen_rb_tree_get_height(GEN_RB_TREE* p_tree);

/**  
 * @brief 查找节点
 * @param [in] p_tree 树句柄
 * @param [in] p_key 节点关键字
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_rb_tree_find_node(GEN_RB_TREE* p_tree, void* p_key, RB_TREE_NODE** pp_node);

/**  
 * @brief 插入key pair
 * @param [in] p_tree 树句柄
 * @param [in] p_pair key pair
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 * -3：节点已存在
 * @remark
 * 1、pp_node可以为空，表示不需要获得对应的节点
 */
int32 gen_rb_tree_insert_pair(GEN_RB_TREE* p_tree, KEY_PAIR* p_pair, RB_TREE_NODE** pp_node);

/**  
 * @brief 删除节点
 * @param [in] p_tree 树句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_rb_tree_delete_node(GEN_RB_TREE* p_tree, RB_TREE_NODE* p_node);

/**  
 * @brief 删除对应关键字的节点
 * @param [in] p_tree 树句柄
 * @param [in] p_key 关键字
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_rb_tree_delete_key(GEN_RB_TREE* p_tree, void* p_key);

#ifdef __cplusplus
}
#endif


#endif ///__GEN_RB_TREE_H__
