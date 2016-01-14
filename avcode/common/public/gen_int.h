/**
 * @file gen_int.h  General Integer Type
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.03.03
 *
 *
 */

#ifndef __GEN_INT_H__
#define __GEN_INT_H__

#include <public/gen_platform.h>

typedef char                 int8;
typedef unsigned char        uint8;
typedef short                int16;
typedef unsigned short       uint16;
typedef int                  int32;
typedef unsigned int         uint32;

#if RUN_OS_WINDOWS

typedef __int64              int64;
typedef unsigned __int64     uint64;

#else

#include <stdint.h>

//typedef long long            int64;
//typedef unsigned long long   uint64;
typedef int64_t              int64;
typedef uint64_t             uint64;

#endif

#ifndef INT8_MIN
#define INT8_MIN                                           ((int8)0x80)
#endif

#ifndef INT8_MAX
#define INT8_MAX                                           ((int8)0x7F)
#endif

#ifndef UINT8_MIN
#define UINT8_MIN                                          (0x00)
#endif

#ifndef UINT8_MAX
#define UINT8_MAX                                          (0xFF)
#endif

#ifndef INT16_MIN
#define INT16_MIN                                          ((int16)0x8000)
#endif

#ifndef INT16_MAX
#define INT16_MAX                                          ((int16)0x7FFF)
#endif

#ifndef UINT16_MIN
#define UINT16_MIN                                         (0x0000)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX                                         (0xFFFF)
#endif

#ifndef INT32_MIN
#define INT32_MIN                                          ((int32)0x80000000)
#endif

#ifndef INT32_MAX
#define INT32_MAX                                          ((int32)0x7FFFFFFF)
#endif

#ifndef UINT32_MIN
#define UINT32_MIN                                         (0x00000000)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX                                         (0xFFFFFFFF)
#endif

#ifndef INT64_MIN
#define INT64_MIN                                          ((int64)0x8000000000000000)
#endif

#ifndef INT64_MAX
#define INT64_MAX                                          ((int64)0x7FFFFFFFFFFFFFFF)
#endif

#ifndef UINT64_MIN
#define UINT64_MIN                                         (0x0000000000000000)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX                                         (0xFFFFFFFFFFFFFFFF)
#endif

#ifndef UINT8_OVERFLOW
#define UINT8_OVERFLOW                                     (0x100)
#endif

#ifndef UINT16_OVERFLOW
#define UINT16_OVERFLOW                                    (0x10000)
#endif

#ifndef UINT32_OVERFLOW
#define UINT32_OVERFLOW                                    (0x100000000LL)
#endif

#ifndef NULL

#ifdef  __cplusplus

#define NULL                                               0

#else

#define NULL                                               ((void *)0)

#endif

#endif//NULL


#endif //__GEN_INT_H__
