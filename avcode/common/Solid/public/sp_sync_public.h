/**
 * @file sp_sync_public.h  SP Sync Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.09.26
 *
 *
 */

#ifndef __SP_SYNC_PUBLIC_H__
#define __SP_SYNC_PUBLIC_H__

#include <public/gen_int.h>

/** sync param */
typedef struct tag_sp_sync_param
{
	/** sys timescale */
	uint32  sys_timescale;
	/** sys ts, unit: timescale */
	int64   sys_ts;

	/** frame timescale */
	uint32  frame_timescale;
	/** frame ts, unit: timescale */
	int64   frame_ts;

} SP_SYNC_PARAM, *PSP_SYNC_PARAM;


#endif ///__SP_SYNC_PUBLIC_H__
