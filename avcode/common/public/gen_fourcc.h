/**
 * @file gen_fourcc.h  General FourCC
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2015.01.17
 *
 *
 */

#ifndef __GEN_FOURCC_H__
#define __GEN_FOURCC_H__

#include <public/gen_int.h>


#ifndef GEN_FOURCC
#define GEN_FOURCC(ch0, ch1, ch2, ch3)                     ( (uint32)(uint8)(ch0) \
															| ((uint32)(uint8)(ch1) << 8) \
															| ((uint32)(uint8)(ch2) << 16) \
															| ((uint32)(uint8)(ch3) << 24 ) )
#endif

#endif //__GEN_FOURCC_H__
