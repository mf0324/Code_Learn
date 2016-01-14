/**
 * @file gen_def.h  General Define
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.08.26
 *
 *
 */

#ifndef __GEN_DEF_H__
#define __GEN_DEF_H__

#include <public/gen_platform.h>

#if RUN_OS_WINDOWS

#ifndef inline
#define inline     __inline
#endif

#endif

/** bit per byte */
#define GEN_BYTE_BIT                                       (8)
/** byte size */
#define GEN_BYTE_SIZE                                      (8)

#endif //__GEN_DEF_H__
