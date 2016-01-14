/**
 * @file mem_pool.h  Memory Pool
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.06.16
 *
 *
 */

#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>

/** mem pool handle */
typedef void* MEM_POOL_H;


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
 */
MEM_POOL_H mem_pool_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief destroy
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 */
int32 mem_pool_destroy(MEM_POOL_H h);

/**  
 * @brief init
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 */
int32 mem_pool_init(MEM_POOL_H h);

/**  
 * @brief deinit
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 */
int32 mem_pool_deinit(MEM_POOL_H h);

/**  
 * @brief reset
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. reset make all block free for use, but not release it to allocator
 */
int32 mem_pool_reset(MEM_POOL_H h);

/**  
 * @brief set align size
 * @param [in] h, pool
 * @param [in] align_size, align size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. set align size only can be done before alloc mem, valid value include 4. 8. 16. 32, default is 4
 */
int32 mem_pool_set_align_size(MEM_POOL_H h, uint32 align_size);

/**  
 * @brief set pool param
 * @param [in] h, pool
 * @param [in] min_pool_size, min pool size
 * @param [in] max_pool_size, max pool size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. pool param can only be set before alloc mem, pool size align on align boundary, default min pool size is 1024 * 1024, max pool size is unlimited
 * 2. min pool size must > 0 , and <= max pool size, if equal each, the pool size would not increase autolly when need more memory
 */
#if !RUN_OS_64
int32 mem_pool_set_pool_param(MEM_POOL_H h, uint32 min_pool_size, uint32 max_pool_size);
#else
int32 mem_pool_set_pool_param(MEM_POOL_H h, uint64 min_pool_size, uint64 max_pool_size);
#endif

/**  
 * @brief pre alloc mem
 * @param [in] h, pool
 * @param [in] alloc_size, alloc mem size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. alloc size would align on 4 byte, and no more than max_pool_size
 */
#if !RUN_OS_64
int32 mem_pool_alloc_mem(MEM_POOL_H h, uint32 alloc_size);
#else
int32 mem_pool_alloc_mem(MEM_POOL_H h, uint64 alloc_size);
#endif

/**  
 * @brief free mem that pre alloc
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. this func would release all mem that pre alloc
 */
int32 mem_pool_free_mem(MEM_POOL_H h);

/**  
 * @brief free unuse mem
 * @param [in] h, pool
 * @return
 * 0: success
 * other: fail
 */
int32 mem_pool_free_unuse_mem(MEM_POOL_H h);

/**  
 * @brief get buf
 * @param [in] h, pool
 * @param [in] buf_size, buf size
 * @param [out] pp_buf, buf addr
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. buf size would align on 4 byte boundary, no more than min_pool_size
 */
#if !RUN_OS_64
int32 mem_pool_get_buf(MEM_POOL_H h, uint32 buf_size, void** pp_buf);
#else
int32 mem_pool_get_buf(MEM_POOL_H h, uint64 buf_size, void** pp_buf);
#endif

/**  
 * @brief release buf
 * @param [in] h, pool
 * @param [in] p_buf, buf addr
 * @return
 * 0: success
 * other: fail
 */
int32 mem_pool_release_buf(MEM_POOL_H h, void* p_buf);


#ifdef __cplusplus
}
#endif


#endif ///__MEM_POOL_H__
