
#include <time.h>


#include <windows.h>

#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#include <public/gen_error.h>
//#include <public/thread_mutexer2.h>

#include "gen_sync_src.h"


/** sync src */
typedef struct tag_gen_sync_src
{
	/** init */
	int32  b_init;

	/** last sys tick */
	int64   last_sys_tick;
	/** sys tick total overflow */
	int64   sys_tick_total_overflow;

} GEN_SYNC_SRC, *PGEN_SYNC_SRC;


/////////////////////////////// Outter Interface //////////////////////////////
GEN_SYNC_SRC_H gen_sync_src_create()
{
	GEN_SYNC_SRC* p_src = NULL;

	p_src = (GEN_SYNC_SRC*)malloc( sizeof(GEN_SYNC_SRC) );

	if( p_src == NULL )
	{
		return NULL;
	}

	memset(p_src, 0, sizeof(GEN_SYNC_SRC));

	return p_src;
}

int32 gen_sync_src_destroy(GEN_SYNC_SRC_H src_h)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	if( p_src == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_src->b_init )
	{
		gen_sync_src_deinit(src_h);
	}

	free(p_src);

	return GEN_SUCCESS;
}

int32 gen_sync_src_init(GEN_SYNC_SRC_H src_h)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	if( p_src == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_src->b_init )
	{
		return GEN_SUCCESS;
	}

	p_src->last_sys_tick = timeGetTime();
	p_src->sys_tick_total_overflow = 0;

	p_src->b_init = 1;

	return GEN_SUCCESS;
}

int32 gen_sync_src_deinit(GEN_SYNC_SRC_H src_h)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	if( p_src == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_src->b_init = 0;

	return GEN_SUCCESS;
}

int32 gen_sync_src_get_sys_tick(GEN_SYNC_SRC_H src_h, int64* p_tick)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	int64  sys_tick;

	if( p_src == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if 0

	sys_tick = timeGetTime();
	if( sys_tick < p_src->last_sys_tick )
	{
		//p_src->sys_tick_overflow_count++;
		p_src->sys_tick_total_overflow += UINT32_OVERFLOW;
		//log_debug(_T("[media_engine_agent::media_engine_do_mixer_task] sys time overflow!\n"));
	}
	p_src->last_sys_tick = sys_tick;
	*p_tick = sys_tick + p_src->sys_tick_total_overflow;

#else

	sys_tick = timeGetTime();
	while( sys_tick < (*p_tick) )
	{
		sys_tick += UINT32_OVERFLOW;
		//log_debug(_T("[gen_sync_src::gen_sync_src_get_sys_tick] sys time overflow!\n"));
	}
	*p_tick = sys_tick;

#endif

	return GEN_SUCCESS;
}

int32 gen_sync_src_get_utc_time(GEN_SYNC_SRC_H src_h, int64* p_second)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	if( p_src == NULL || p_second == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	*p_second = time(NULL);

	return GEN_SUCCESS;
}

int32 gen_sync_src_get_utc_time_ms(GEN_SYNC_SRC_H src_h, int64* p_ms)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	int64  utc_time;

	SYSTEMTIME sys_time;

	if( p_src == NULL || p_ms == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	utc_time = time(NULL);

	GetSystemTime(&sys_time);

	*p_ms = utc_time * 1000 + sys_time.wMilliseconds;

	return GEN_SUCCESS;
}

int32 gen_sync_src_get_utc_date_time(GEN_SYNC_SRC_H src_h, GEN_DATE_TIME* p_date_time)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	SYSTEMTIME sys_time;

	if( p_src == NULL || p_date_time == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	GetSystemTime(&sys_time);

	p_date_time->year = sys_time.wYear;
	p_date_time->month = sys_time.wMonth;
	p_date_time->day = sys_time.wDay;
	p_date_time->week_day = sys_time.wDayOfWeek;
	p_date_time->hour = sys_time.wHour;
	p_date_time->minute = sys_time.wMinute;
	p_date_time->second = sys_time.wSecond;
	p_date_time->millisecond = sys_time.wMilliseconds;

	return GEN_SUCCESS;
}

int32 gen_sync_src_convert_utc_time_to_date(GEN_SYNC_SRC_H src_h, int64* p_second, GEN_DATE_TIME* p_date_time)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	struct tm date;

	int32  ret;

	if( p_src == NULL || p_second == NULL || p_date_time == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = gmtime_s(&date, p_second);
	if( ret )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_date_time->year = date.tm_year + 1900;
	p_date_time->month = date.tm_mon + 1;
	p_date_time->day = date.tm_mday;
	p_date_time->week_day = date.tm_wday;
	p_date_time->hour = date.tm_hour;
	p_date_time->minute = date.tm_min;
	p_date_time->second = date.tm_sec;
	p_date_time->millisecond = 0;

	return GEN_SUCCESS;
}

int32 gen_sync_src_convert_utc_date_to_time(GEN_SYNC_SRC_H src_h, GEN_DATE_TIME* p_date_time, int64* p_second)
{
	GEN_SYNC_SRC* p_src = (GEN_SYNC_SRC*)src_h;

	struct tm date;

	//int32  ret;

	if( p_src == NULL || p_second == NULL || p_date_time == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_src->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_date_time->year < 1970 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_date_time->month < 1 || p_date_time->month > 12 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_date_time->day < 1 || p_date_time->day > 31 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_date_time->hour < 0 || p_date_time->hour > 23 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_date_time->minute < 0 || p_date_time->minute > 59 )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_date_time->second < 0 || p_date_time->second > 59 )
	{
		return GEN_E_INVALID_PARAM;
	}

	date.tm_year = p_date_time->year - 1900;
	date.tm_mon = p_date_time->month - 1;
	date.tm_mday = p_date_time->day;
	date.tm_wday = p_date_time->week_day;
	date.tm_hour = p_date_time->hour;
	date.tm_min = p_date_time->minute;
	date.tm_sec = p_date_time->second;
	date.tm_isdst = 0;

	*p_second = _mkgmtime(&date);
	if( *p_second == (-1) )
	{
		return GEN_E_INVALID_PARAM;
	}

	return GEN_SUCCESS;
}
