/**
 * @file gen_set.h  Set
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.06.22
 *
 *
 */

#ifndef __GEN_SET_H__
#define __GEN_SET_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>
#include <public/gen_comparer.h>


/** 红 */
#define GEN_SET_COLOR_RED                                  (0)
/** 黑 */
#define GEN_SET_COLOR_BLACK                                (1)

/** set节点 */
typedef struct tag_set_node
{
	/** 颜色 */
	int32  color;

	/** 节点高度 */
	int32  height;
	/** 左子树高度 */
	int32  left_height;
	/** 右子树高度 */
	int32  right_height;

	/** key */
	void*  p_key;

	/** 父节点 */
	struct tag_set_node *p_parent;
	/** 左子节点 */
	struct tag_set_node *p_left;
	/** 右子节点 */
	struct tag_set_node *p_right;

	/** 前一节点 */
	struct tag_set_node *p_prev;
	/** 后一节点 */
	struct tag_set_node *p_next;

} SET_NODE, *PSET_NODE;

/** set */
typedef struct tag_gen_set
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;

	/** 是否初始化 */
	int32  b_init;
	/** set长度 */
	int32  set_size;
	/** 是否降序排列 */
	int32  b_descend;

	/** 根节点 */
	SET_NODE* p_root;

	/** 头节点指针 */
	SET_NODE* p_head;
	/** 尾节点指针 */
	SET_NODE* p_rear;
	/** 头节点 */
	SET_NODE  head;
	/** 尾节点 */
	SET_NODE  rear;

} GEN_SET, *PGEN_SET;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建set
 * @param [in] p_allocator 分配器句柄
 * @param [in] p_comparer 比较器句柄
 * @return
 * 非空值：成功，返回值表示对应的set句柄
 * NULL：失败
 * @remark
 * 1、输入的比较器句柄不能为空!
 * 2、set的实现基于红黑树
 */
GEN_SET* gen_set_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief 销毁set
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_destroy(GEN_SET* p_set);

/**  
 * @brief 初始化set
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_init(GEN_SET* p_set);

/**  
 * @brief 反初始化set
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_deinit(GEN_SET* p_set);

/**  
 * @brief 清空set
 * @param [in] p_set set句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_clear(GEN_SET* p_set);

/**  
 * @brief 获取set长度
 * @param [in] p_set set句柄
 * @return
 * 非负值：成功，返回值表示set长度
 * -1：失败
 */
int32 gen_set_get_size(GEN_SET* p_set);

/**  
 * @brief 设置set排列顺序
 * @param [in] p_set set句柄
 * @param [in] b_descend 是否降序
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、初始化后默认为升序排列
 */
int32 gen_set_set_order(GEN_SET* p_set, int32 b_descend);

/**  
 * @brief 查找节点
 * @param [in] p_set set句柄
 * @param [in] p_key 节点关键字
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_set_find_node(GEN_SET* p_set, void* p_key, SET_NODE** pp_node);

/**  
 * @brief 插入key
 * @param [in] p_set set句柄
 * @param [in] p_key key
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 * -3：节点已存在
 * @remark
 * 1、pp_node可以为空，表示不需要获得对应的节点
 */
int32 gen_set_insert_key(GEN_SET* p_set, void* p_key, SET_NODE** pp_node);

/**  
 * @brief 删除节点
 * @param [in] p_set set句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_set_delete_node(GEN_SET* p_set, SET_NODE* p_node);

/**  
 * @brief 删除关键字
 * @param [in] p_set set句柄
 * @param [in] p_key 关键字
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_set_delete_key(GEN_SET* p_set, void* p_key);

#ifdef __cplusplus
}
#endif


#endif ///__GEN_SET_H__
