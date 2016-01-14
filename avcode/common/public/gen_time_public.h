/**
 * @file gen_time_public.h   General Time Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.26
 *
 *
 */

#ifndef __GEN_TIME_PUBLIC_H__
#define __GEN_TIME_PUBLIC_H__

#include <public/gen_int.h>


/** date time */
typedef struct tag_gen_date_time
{
	/** year */
	int32  year;
	/** month */
	int32  month;
	/** day */
	int32  day;
	/** week day */
	int32  week_day;

	/** hour */
	int32  hour;
	/** minute */
	int32  minute;
	/** second */
	int32  second;
	/** millisecond */
	int32  millisecond;

} GEN_DATE_TIME, *PGEN_DATE_TIME;



#endif ///__GEN_TIME_PUBLIC_H__
