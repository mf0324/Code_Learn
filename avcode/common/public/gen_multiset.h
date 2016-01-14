/**
 * @file gen_multimultiset.h  重复关联表
 *
 * Copyright (C) 2007-2012 北清视通信息技术有限公司北京研发部 
 * 
 *
 * @author 翁彦 <wengyan@bqvision.com>
 * @version 0.1
 * @date 2011.04.20
 *
 *
 */

#ifndef __GEN_MULTISET_H__
#define __GEN_MULTISET_H__

#include "../public/gen_int.h"
#include "../public/gen_allocator.h"
#include "../public/gen_comparer.h"


/** 红 */
#define GEN_MULTISET_COLOR_RED                             (0)
/** 黑 */
#define GEN_MULTISET_COLOR_BLACK                           (1)

/** multiset节点 */
typedef struct tag_multiset_node
{
	/** 颜色 */
	int32  color;

	/** key */
	void*  p_key;

	/** 父节点 */
	struct tag_multiset_node *p_parent;
	/** 左子节点 */
	struct tag_multiset_node *p_left;
	/** 右子节点 */
	struct tag_multiset_node *p_right;

	/** 前一序列节点 */
	struct tag_multiset_node *p_prev_seq;
	/** 后一序列节点 */
	struct tag_multiset_node *p_next_seq;

	/** 孪生节点个数 */
	int32  twin_node_num;
	/** 是否孪生节点 */
	int32  b_twin_node;
	/** 前一孪生节点 */
	struct tag_multiset_node *p_prev_twin;
	/** 后一孪生节点 */
	struct tag_multiset_node *p_next_twin;
	/** 第一个孪生节点 */
	struct tag_multiset_node *p_first_twin;
	/** 最后一个孪生节点 */
	struct tag_multiset_node *p_last_twin;

} MULTISET_NODE, *PMULTISET_NODE;

/** multiset */
typedef struct tag_gen_multiset
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;

	/** 是否初始化 */
	int32  b_init;
	/** multiset长度 */
	int32  multiset_size;
	/** 是否降序排列 */
	int32  b_descend;

	/** 根节点 */
	MULTISET_NODE* p_root;

	/** 序列头节点指针 */
	MULTISET_NODE* p_head;
	/** 序列尾节点指针 */
	MULTISET_NODE* p_rear;
	/** 序列头节点 */
	MULTISET_NODE  head;
	/** 序列尾节点 */
	MULTISET_NODE  rear;

} GEN_MULTISET, *PGEN_MULTISET;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建multiset
 * @param [in] p_allocator 分配器句柄
 * @param [in] p_comparer 比较器句柄
 * @return
 * 非空值：成功，返回值表示对应的multiset句柄
 * NULL：失败
 * @remark
 * 1、输入的比较器句柄不能为空!
 * 2、如果分配器句柄为空，将使用默认分配器(malloc)
 * 3、multiset的实现基于红黑树
 */
GEN_MULTISET* gen_multiset_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief 销毁multiset
 * @param [in] p_multiset multiset句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multiset_destroy(GEN_MULTISET* p_multiset);

/**  
 * @brief 初始化multiset
 * @param [in] p_multiset multiset句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multiset_init(GEN_MULTISET* p_multiset);

/**  
 * @brief 反初始化multiset
 * @param [in] p_multiset multiset句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multiset_deinit(GEN_MULTISET* p_multiset);

/**  
 * @brief 清空multiset
 * @param [in] p_multiset multiset句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multiset_clear(GEN_MULTISET* p_multiset);

/**  
 * @brief 获取multiset长度
 * @param [in] p_multiset multiset句柄
 * @return
 * 非负值：成功，返回值表示multiset长度
 * -1：失败
 */
int32 gen_multiset_get_size(GEN_MULTISET* p_multiset);

/**  
 * @brief 设置multiset排列顺序
 * @param [in] p_multiset multiset句柄
 * @param [in] b_descend 是否降序
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、初始化后默认为升序排列
 */
int32 gen_multiset_set_order(GEN_MULTISET* p_multiset, int32 b_descend);

/**  
 * @brief 查找节点
 * @param [in] p_multiset multiset句柄
 * @param [in] p_key 节点关键字
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_multiset_find_node(GEN_MULTISET* p_multiset, void* p_key, MULTISET_NODE** pp_node);

/**  
 * @brief 插入key
 * @param [in] p_multiset multiset句柄
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
int32 gen_multiset_insert_key(GEN_MULTISET* p_multiset, void* p_key, MULTISET_NODE** pp_node);

/**  
 * @brief 删除节点
 * @param [in] p_multiset multiset句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multiset_delete_node(GEN_MULTISET* p_multiset, MULTISET_NODE* p_node);

/**  
 * @brief 删除关键字
 * @param [in] p_multiset multiset句柄
 * @param [in] p_key 关键字
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_multiset_delete_key(GEN_MULTISET* p_multiset, void* p_key);

#ifdef __cplusplus
}
#endif


#endif ///__GEN_MULTISET_H__
