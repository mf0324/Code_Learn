/**
 * @file mp4_file_stat.h   MP4 File Statistics
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.27
 *
 *
 */


#ifndef __MP4_FILE_STAT_H__
#define __MP4_FILE_STAT_H__

#include <public/gen_int.h>
#include "../iso/iso_media_file.h"
#include "mp4_avc_file.h"

/**  File Stat */
typedef struct tag_mp4_file_stat
{
	/** mvhd box */
	ISO_MOVIE_HEADER_BOX mvhd_box;
	/** duration, unit: ms */
	uint32  duration_ms;
	/** track count */
	uint32  track_count;

} MP4_FILE_STAT, *PMP4_FILE_STAT;

/** Sample */
typedef struct tag_mp4_sample
{
	/** sample time, unit: time scale */
	int64   sample_time;
	/** sample offset */
	int64   sample_offset;
	/** sample size */
	uint32  sample_size;
	/** sample flag */
	uint32  sample_flag;

} MP4_SAMPLE, *PMP4_SAMPLE;

/** File Track */
typedef struct tag_mp4_file_track
{
	/** tkhd box */
	ISO_TRACK_HEADER_BOX tkhd_box;
	/** duration, unit: ms */
	uint32  duration_ms;
	/** mdhd box */
	ISO_MEDIA_HEADER_BOX mdhd_box;
	/** hdlr box */
	ISO_HANDLER_BOX hdlr_box;

	/** desc */
	union
	{
		/** video */
		ISO_VISUAL_SAMPLE_DESC_BOX video;
		/** audio */
		ISO_AUDIO_SAMPLE_DESC_BOX  audio;

	} desc;

	/** entry */
	union
	{
		/** video */
		ISO_VISUAL_SAMPLE_ENTRY video;
		/** audio */
		ISO_AUDIO_SAMPLE_ENTRY  audio;

	} entry;

	AVC_DEC_CONFIG_RECORD avc_record;

	/** stco box */
	ISO_CHUNK_OFFSET_BOX stco_box;
	/** stco entry */
	ISO_CHUNK_OFFSET_ENTRY* p_stco;

	/** stsc box */
	ISO_SAMPLE_TO_CHUNK_BOX stsc_box;
	/** stsc entry */
	ISO_SAMPLE_TO_CHUNK_ENTRY* p_stsc;

	/** stsz box */
	ISO_SAMPLE_SIZE_BOX stsz_box;
	/** stsz entry */
	ISO_SAMPLE_SIZE_ENTRY* p_stsz;

	/** sample */
	MP4_SAMPLE* p_sample;
	/** sample count */
	uint32 sample_count;

} MP4_FILE_TRACK, *PMP4_FILE_TRACK;



/** Sync Sample */
typedef struct tag_mp4_sync_sample
{
	/** sample number */
	uint32  sample_number;

} MP4_SYNC_SAMPLE, *PMP4_SYNC_SAMPLE;

/** Video Track */
typedef struct tag_mp4_video_track
{
	/** track id */
	uint32  track_id;
	/** time scale, unit: HZ */
	uint32  time_scale;
	/** duration, unit: time scale */
	uint32  duration;
	/** duration, unit: ms */
	uint32  duration_ms;
	/** width */
	uint32  width;
	/** height */
	uint32  height;

	/** fourcc */
	uint32  fourcc;
	/** total chunk count */
	uint32  total_chunk_count;
	/** total sample count */
	uint32  total_sample_count;
	/** sample */
	MP4_SAMPLE* p_sample;

	/** total sync sample count */
	uint32  total_sync_sample_count;
	/** sync sample */
	MP4_SYNC_SAMPLE* p_sync_sample;

} MP4_VIDEO_TRACK, *PMP4_VIDEO_TRACK;

/** Audio Track */
typedef struct tag_mp4_audio_track
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

	/** fourcc */
	uint32  fourcc;
	/** total chunk count */
	uint32  total_chunk_count;
	/** total sample count */
	uint32  total_sample_count;
	/** sample */
	MP4_SAMPLE* p_sample;

} MP4_AUDIO_TRACK, *PMP4_AUDIO_TRACK;



#endif ///__MP4_FILE_STAT_H__
