/**
 * @file gen_file_public.h  Gen File Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.08
 *
 *
 */

#ifndef __GEN_FILE_PUBLIC_H__
#define __GEN_FILE_PUBLIC_H__

#include <public/gen_int.h>



/** open param */
typedef struct tag_gen_file_open_param
{
	/** rw */
	uint32 rw;
	/** share */
	uint32 share;
	/** create */
	uint32 create;
	/** open flag */
	uint32 open_flag;

} GEN_FILE_OPEN_PARAM, *PGEN_FILE_OPEN_PARAM;


#endif ///__GEN_FILE_PUBLIC_H__
