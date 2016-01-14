/**
 * @file gen_char_public.h  Gen Char Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.08
 *
 *
 */

#ifndef __GEN_CHAR_PUBLIC_H__
#define __GEN_CHAR_PUBLIC_H__

#include <public/gen_platform.h>

#ifdef RUN_OS_WINDOWS

#include <tchar.h>

#else

#define _T(x)                                              x

#endif



#endif ///__GEN_CHAR_PUBLIC_H__
