/**
 * @file gen_allocator.h  Allocator Interface
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.04.26
 *
 *
 */

#ifndef __GEN_ALLOCATOR_H__
#define __GEN_ALLOCATOR_H__

#include <public/gen_int.h>


typedef struct tag_gen_allocator GEN_ALLOCATOR, *PGEN_ALLOCATOR;


/**  
 * @brief allocate
 * @param [in] size, size to allocate
 * @param [in] p_param, user data
 * @return
 * not NULL: success
 * NULL: fail
 */
typedef void* (*gen_allocate)(uint32 size, void* p_param);

/**  
 * @brief allocate(64 bit)
 * @param [in] size, size to allocate
 * @param [in] p_param, user data
 * @return
 * not NULL: success
 * NULL: fail
 */
typedef void* (*gen_allocate64)(uint64 size, void* p_param);

/**  
 * @brief deallocate
 * @param [in] p_buf, mem buf addr
 * @param [in] p_param, user data
 * @return
 * 0: success
 * other: fail
 */
typedef int32 (*gen_deallocate)(void *p_buf, void* p_param);

/** allocator */
struct tag_gen_allocator
{
	/** allocate */
#if !RUN_OS_64
	gen_allocate     allocate;
#else
	gen_allocate64   allocate;
#endif
	/** deallocate */
	gen_deallocate   deallocate;

	/** param */
	void* p_param;

};

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif


#endif ///__GEN_ALLOCATOR_H__
