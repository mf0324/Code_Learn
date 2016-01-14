/**
 * @file multimedia_def.h  Multimedia Define
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.01.15
 *
 *
 */

#ifndef __MULTIMEDIA_DEF_H__
#define __MULTIMEDIA_DEF_H__

#include <public/gen_int.h>

/** 结构体按1字节对齐 */
//#pragma pack(push)
//#pragma pack(1)

/** GUID */
typedef struct tag_multimedia_guid
{
	uint32 data1;
	uint16 data2;
	uint16 data3;
	uint8  data4[8];

} MULTIMEDIA_GUID, *PMULTIMEDIA_GUID;

//#pragma pack(pop)

#endif ///__MULTIMEDIA_DEF_H__
