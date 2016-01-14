
#include "mpeg2_public.h"
#include "mpeg2_util.h"

int32 mpeg2_sys2stream_time(int64 *p_frame_time)
{
	*p_frame_time = (*p_frame_time) / (MPEG2_SYS_TICK_PER_SECOND/1000);

	return 0;
}

int32 mpeg2_stream2sys_time(int64 *p_frame_time)
{
	*p_frame_time = (*p_frame_time) * (MPEG2_SYS_TICK_PER_SECOND/1000);

	return 0;
}
