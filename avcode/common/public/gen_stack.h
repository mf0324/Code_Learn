/**
 * @file gen_stack.h  Stack
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.11.05
 *
 *
 */

#ifndef __GEN_STACK_H__
#define __GEN_STACK_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>

/** 栈节点 */
typedef struct tag_stack_node
{
	/** 节点数据 */
	void*  p_data;

} STACK_NODE, *PSTACK_NODE;

/** 栈 */
typedef struct tag_gen_stack
{
	/** allocator */
	GEN_ALLOCATOR* p_allocator;

	/** 是否初始化 */
	int32  b_init;
	/** 是否已分配内存 */
	int32  b_alloc_mem;
	/** 栈当前长度 */
	int32  stack_size;

	/** 节点数组 */
	STACK_NODE *p_array;
	/** 数组最小长度，单位：个 */
	int32  array_min_size;
	/** 数组最大长度，单位：个 */
	int32  array_max_size;
	/** 数组总长度，单位：个 */
	int32  array_total_size;
	/** 数组有效长度，单位：个 */
	int32  array_valid_size;
	/** 栈顶位置 */
	int32  top_pos;
	/** 栈底位置 */
	int32  bottom_pos;

} GEN_STACK, *PGEN_STACK;

#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 创建栈
 * @param [in] p_allocator 分配器句柄
 * @return
 * 非空值：成功，返回值表示栈句柄
 * NULL：失败
 * @remark
 * 1、如果分配器为空，那么将使用默认分配方法(malloc)
 */
GEN_STACK* gen_stack_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief 销毁栈
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_destroy(GEN_STACK *p_stack);

/**  
 * @brief 初始化栈
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_init(GEN_STACK *p_stack);

/**  
 * @brief 反初始化栈
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_deinit(GEN_STACK *p_stack);

/**  
 * @brief 清空栈
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_clear(GEN_STACK *p_stack);

/**  
 * @brief 设置数组参数
 * @param [in] p_stack 队列句柄
 * @param [in] min_size 最小长度，单位：个
 * @param [in] max_size 最大长度，单位：个
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、数组参数只能在未分配内存时设置，默认最小长度为1024，最大长度为1024
 * 2、最大长度必须大于等于最小长度，如果最大长度等于最小长度，栈不会自动增长
 */
int32 gen_stack_set_array_param(GEN_STACK *p_stack, int32 min_size, int32 max_size);

/**  
 * @brief 预分配内存
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_alloc_mem(GEN_STACK *p_stack);

/**  
 * @brief 释放预分配内存
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_free_mem(GEN_STACK *p_stack);

/**  
 * @brief 获取当前栈长度
 * @param [in] p_stack 栈句柄
 * @param [out] p_size 栈长度
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_get_size(GEN_STACK *p_stack, int32 *p_size);

/**  
 * @brief 获取栈顶
 * @param [in] p_stack 栈句柄
 * @param [out] pp_data 节点值句柄
 * @return
 * 0：成功
 * -1：失败
 * -2：空栈
 */
int32 gen_stack_get_top(GEN_STACK *p_stack, void **pp_data);

/**  
 * @brief 获取栈底
 * @param [in] p_stack 栈句柄
 * @param [out] pp_data 节点数据地址
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_get_bottom(GEN_STACK *p_stack, void **pp_data);

/**  
 * @brief 入栈
 * @param [in] p_stack 栈句柄
 * @param [in] p_data 节点数据
 * @return
 * 0：成功
 * -1：失败
 * -2：内存不足
 * -4：栈已满
 */
int32 gen_stack_push(GEN_STACK *p_stack, void *p_data);

/**  
 * @brief 出栈
 * @param [in] p_stack 栈句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 gen_stack_pop(GEN_STACK *p_stack);



#ifdef __cplusplus
}
#endif


#endif ///__GEN_STACK_H__
