/**
 * @file gen_multimultimap.h  重复关联表
 *
 * Copyright (C) 2007-2012 北清视通信息技术有限公司北京研发部 
 * 
 *
 * @author 翁彦 <wengyan@bqvision.com>
 * @version 0.1
 * @date 2011.04.19
 *
 *
 */

#ifndef __GEN_MULTIMAP_H__
#define __GEN_MULTIMAP_H__

#include "../public/gen_int.h"
#include "../public/gen_key.h"
#include "../public/gen_allocator.h"
#include "../public/gen_comparer.h"


/** 红 */
#define GEN_MULTIMAP_COLOR_RED                             (0)
/** 黑 */
#define GEN_MULTIMAP_COLOR_BLACK                           (1)

/** multimap节点 */
typedef struct tag_multimap_node
{
	/** 颜色 */
	int32  color;

	/** key pair */
	KEY_PAIR pair;

	/** 父节点 */
	struct tag_multimap_node *p_parent;
	/** 左子节点 */
	struct tag_multimap_node *p_left;
	/** 右子节点 */
	struct tag_multimap_node *p_right;

	/** 前一序列节点 */
	struct tag_multimap_node *p_prev_seq;
	/** 后一序列节点 */
	struct tag_multimap_node *p_next_seq;

} MULTIMAP_NODE, *PMULTIMAP_NODE;

/** multimap */
typedef struct tag_gen_multimap
{
	/** allocator */
	GEN_ALLOCATOR allocator;
	/** comparer */
	GEN_COMPARER  comparer;

	/** 是否初始化 */
	int32  b_init;
	/** multimap长度 */
	int32  multimap_size;
	/** 是否降序排列 */
	int32  b_descend;

	/** 根节点 */
	MULTIMAP_NODE* p_root;

	/** 序列头节点指针 */
	MULTIMAP_NODE* p_head;
	/** 序列尾节点指针 */
	MULTIMAP_NODE* p_rear;
	/** 序列头节点 */
	MULTIMAP_NODE  head;
	/** 序列尾节点 */
	MULTIMAP_NODE  rear;

} GEN_MULTIMAP, *PGEN_MULTIMAP;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建multimap
 * @param [in] p_allocator 分配器句柄
 * @param [in] p_comparer 比较器句柄
 * @return
 * 非空值：成功，返回值表示对应的multimap句柄
 * NULL：失败
 * @remark
 * 1、输入的比较器句柄不能为空!
 * 2、如果分配器句柄为空，将使用默认分配器(malloc)
 * 3、multimap的实现基于红黑树
 */
GEN_MULTIMAP* gen_multimap_create(GEN_ALLOCATOR* p_allocator, GEN_COMPARER* p_comparer);

/**  
 * @brief 销毁multimap
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multimap_destroy(GEN_MULTIMAP* p_multimap);

/**  
 * @brief 初始化multimap
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multimap_init(GEN_MULTIMAP* p_multimap);

/**  
 * @brief 反初始化multimap
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multimap_deinit(GEN_MULTIMAP* p_multimap);

/**  
 * @brief 清空multimap
 * @param [in] p_multimap multimap句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multimap_clear(GEN_MULTIMAP* p_multimap);

/**  
 * @brief 获取multimap长度
 * @param [in] p_multimap multimap句柄
 * @return
 * 非负值：成功，返回值表示multimap长度
 * -1：失败
 */
int32 gen_multimap_get_size(GEN_MULTIMAP* p_multimap);

/**  
 * @brief 设置multimap排列顺序
 * @param [in] p_multimap multimap句柄
 * @param [in] b_descend 是否降序
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、初始化后默认为升序排列
 */
int32 gen_multimap_set_order(GEN_MULTIMAP* p_multimap, int32 b_descend);

/**  
 * @brief 查找节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_key 节点关键字
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 * @remark
 * 1、该接口返回找到的第一个匹配节点
 */
int32 gen_multimap_find_node(GEN_MULTIMAP* p_multimap, void* p_key, MULTIMAP_NODE** pp_node);

/**  
 * @brief 插入key pair
 * @param [in] p_multimap multimap句柄
 * @param [in] p_pair key pair
 * @param [out] pp_node 节点地址
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 * @remark
 * 1、pp_node可以为空，表示不需要获得对应的节点
 */
int32 gen_multimap_insert_pair(GEN_MULTIMAP* p_multimap, KEY_PAIR* p_pair, MULTIMAP_NODE** pp_node);

/**  
 * @brief 删除节点
 * @param [in] p_multimap multimap句柄
 * @param [in] p_node 节点句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_multimap_delete_node(GEN_MULTIMAP* p_multimap, MULTIMAP_NODE* p_node);

/**  
 * @brief 删除指定关键字
 * @param [in] p_multimap multimap句柄
 * @param [in] p_key 关键字
 * @return
 * 0：成功
 * -1：失败
 * -4：节点不存在
 */
int32 gen_multimap_delete_key(GEN_MULTIMAP* p_multimap, void* p_key);

/**  
 * @brief 删除所有符合条件指定关键字
 * @param [in] p_multimap multimap句柄
 * @param [in] p_key 关键字
 * @param [out] p_count 实际删除个数
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、p_count可以为空
 */
int32 gen_multimap_delete_all_key(GEN_MULTIMAP* p_multimap, void* p_key, int32* p_count);


#ifdef __cplusplus
}
#endif


#endif ///__GEN_MULTIMAP_H__
