/**
 * @file gen_comparer.h  Comparer Interface
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.04.26
 *
 *
 */

#ifndef __GEN_COMPARER_H__
#define __GEN_COMPARER_H__

#include <public/gen_int.h>


typedef struct tag_gen_comparer GEN_COMPARER, *PGEN_COMPARER;

/**  
 * @brief compare two data
 * @param [in] data1, data1
 * @param [in] data2, data2
 * @param [in] p_param, user param
 * @param [out] p_result, result
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. result meaning
 * < 0: data1 < data2
 * = 0: data1 = data2
 * > 0: data1 > data2
 */
typedef int32 (*gen_compare)(void* data1, void* data2, void* p_param, int32* p_result);

/** comparer */
struct tag_gen_comparer
{
	/** compare */
	gen_compare  compare;

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


#endif ///__GEN_COMPARER_H__
