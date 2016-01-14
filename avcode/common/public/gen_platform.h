/**
 * @file gen_platform.h  Gen Platform
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.07.08
 *
 *
 */

#ifndef __GEN_PLATFORM_H__
#define __GEN_PLATFORM_H__


#ifdef WIN32

#define RUN_OS_WINDOWS                                     (1)

#else

#define RUN_OS_WINDOWS                                     (0)

#endif

#ifdef __GEN_OS_64__

#define RUN_OS_64                                          (1)

#else

#define RUN_OS_64                                          (0)

#endif

#endif //__GEN_PLATFORM_H__
