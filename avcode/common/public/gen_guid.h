/**
 * @file gen_guid.h  General GUID
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.05.04
 *
 *
 */

#ifndef __GEN_GUID_H__
#define __GEN_GUID_H__

#include <public/gen_int.h>


/** guid size */
#define GEN_GUID_SIZE                                      (16)


/** guid */
typedef struct tag_gen_guid
{
	/** id */
	uint8   id[GEN_GUID_SIZE];


} GEN_GUID, *PGEN_GUID;



#endif ///__GEN_GUID_H__
