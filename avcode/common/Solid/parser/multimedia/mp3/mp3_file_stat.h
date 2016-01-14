/**
 * @file mp3_file_stat.h   MP3 File Statistics
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.09.12
 *
 *
 */

#ifndef __MP3_FILE_STAT_H__
#define __MP3_FILE_STAT_H__

#include <public/gen_int.h>

/**  File Stat */
typedef struct tag_mp3_file_stat
{
	/** duration, unit: ms */
	uint32  duration_ms;
	/** track count */
	uint32  track_count;

} MP3_FILE_STAT, *PMP3_FILE_STAT;

/** Sample */
typedef struct tag_mp3_sample
{
	/** sample time, unit: time scale */
	int64   sample_time;
	/** sample offset */
	int64   sample_offset;
	/** sample size */
	uint32  sample_size;
	/** sample flag */
	uint32  sample_flag;

} MP3_SAMPLE, *PMP3_SAMPLE;

/** Audio Track */
typedef struct tag_mp3_audio_track
{
	/** track id */
	uint32  track_id;
	/** time scale, unit: HZ */
	uint32  time_scale;
	/** duration, unit: time scale */
	uint32  duration;
	/** duration, unit: ms */
	uint32  duration_ms;
	/** sample rate */
	uint32  sample_rate;
	/** sample size */
	uint32  sample_size;
	/** chn count */
	uint32  chn_count;

	/** total sample count */
	uint32  total_sample_count;
	/** sample */
	MP3_SAMPLE* p_sample;

} MP3_AUDIO_TRACK, *PMP3_AUDIO_TRACK;



#endif ///__MP3_FILE_STAT_H__
