/**
 * @file mem_group.h  Memory Group
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.08.12
 *
 *
 */

#ifndef __MEM_GROUP_H__
#define __MEM_GROUP_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>


/** Memory Group Handle */
typedef void*    MEM_GROUP_H;


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
MEM_GROUP_H mem_group_create(GEN_ALLOCATOR* p_allocator);

/**  
 * @brief destroy
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_destroy(MEM_GROUP_H group_h);

/**  
 * @brief init
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_init(MEM_GROUP_H group_h);

/**  
 * @brief deinit
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_deinit(MEM_GROUP_H group_h);

/**  
 * @brief reset
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. reset make all block free for use, but not release it to allocator
 */
int32 mem_group_reset(MEM_GROUP_H group_h);

/**  
 * @brief set align
 * @param [in] group_h, group
 * @param [in] align_size, align size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. set align size only can be done before alloc mem, valid value include 4. 8. 16. 32, default is 4
 */
int32 mem_group_set_align_size(MEM_GROUP_H group_h, uint32 align_size);

/**  
 * @brief set block param
 * @param [in] group_h, group
 * @param [in] block_size, block size
 * @param [in] min_block_num, min block num
 * @param [in] max_block_num, max block num
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. block param can only be set before alloc mem, default block size is 128, min block num is 1024, max block num is 1024
 * 2. block size must >=0 , actual size would align on preset align size
 * 3. min block num must >=0 , and <= max block num, if equal each, the block number would not increase autolly when need more block
 */
#if !RUN_OS_64
int32 mem_group_set_block_param(MEM_GROUP_H group_h, uint32 block_size, int32 min_block_num, int32 max_block_num);
#else
int32 mem_group_set_block_param(MEM_GROUP_H group_h, uint64 block_size, int64 min_block_num, int64 max_block_num);
#endif

/**  
 * @brief alloc mem
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. alloc mem block quantity is min_block_num for every time, total quantity would not exceed max_block_num
 */
int32 mem_group_alloc_mem(MEM_GROUP_H group_h);

/**  
 * @brief free alloc mem
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_free_mem(MEM_GROUP_H group_h);

/**  
 * @brief free unuse mem
 * @param [in] group_h, group
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_free_unuse_mem(MEM_GROUP_H group_h);

/**  
 * @brief get buf
 * @param [in] group_h, group
 * @param [in] buf_size, buf size
 * @param [out] pp_buf, buf addr
 * @return
 * 0: success
 * other: fail
 */
#if !RUN_OS_64
int32 mem_group_get_buf(MEM_GROUP_H group_h, uint32 buf_size, void** pp_buf);
#else
int32 mem_group_get_buf(MEM_GROUP_H group_h, uint64 buf_size, void** pp_buf);
#endif

/**  
 * @brief release buf
 * @param [in] group_h, group
 * @param [in] p_buf, buf addr
 * @return
 * 0: success
 * other: fail
 */
int32 mem_group_release_buf(MEM_GROUP_H group_h, void* p_buf);


#ifdef __cplusplus
}
#endif


#endif ///__MEM_GROUP_H__
