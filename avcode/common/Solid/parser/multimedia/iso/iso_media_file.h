/**
 * @file iso_media_file.h   ISO Base Media File Format
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.04.27
 *
 *
 */


#ifndef __ISO_MEDIA_FILE_H__
#define __ISO_MEDIA_FILE_H__

#include <public/gen_int.h>

/** UUID长度 */
#define ISO_MEDIA_FILE_UUID_SIZE                           (16)
/** 名称长度 */
#define ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE                (32)


/** Box */
typedef struct tag_iso_box
{
	/** size */
	uint32 size;
	/** type */
	uint32 type;

} ISO_BOX, *PISO_BOX;

/** Box64 */
typedef struct tag_iso_box64
{
	/** size */
	uint32 size;
	/** type */
	uint32 type;
	/** large size */
	uint64 large_size;

} ISO_BOX64, *PISO_BOX64;

/** User Box */
typedef struct tag_iso_user_box
{
	/** box */
	ISO_BOX box;

	/** uuid */
	uint8  uuid[ISO_MEDIA_FILE_UUID_SIZE];

} ISO_USER_BOX, *PISO_USER_BOX;

/** User Box64 */
typedef struct tag_iso_user_box64
{
	/** box64 */
	ISO_BOX64 box;

	/** uuid */
	uint8  uuid[ISO_MEDIA_FILE_UUID_SIZE];

} ISO_USER_BOX64, *PISO_USER_BOX64;

/** Full Box */
typedef struct tag_iso_full_box
{
	/** box */
	ISO_BOX box;

	/** version & flag */
	union
	{
		uint32 value;

		struct
		{
			uint8 flag[3];
			uint8 version;
		} s1;

	} version_flag;

} ISO_FULL_BOX, *PISO_FULL_BOX;

/** Full Box64 */
typedef struct tag_iso_full_box64
{
	/** box64 */
	ISO_BOX64 box;

	/** version & flag */
	union
	{
		uint32 value;

		struct
		{
			uint8 flag[3];
			uint8 version;
		} s1;

	} version_flag;

} ISO_FULL_BOX64, *PISO_FULL_BOX64;

/** File Type Box */
typedef struct tag_iso_file_type_box
{
	/** box */
	ISO_BOX box;

	/** major brand */
	uint32  major_brand;
	/** minor version */
	uint32  minor_version;

	/** compatible brand */
	uint32* p_brand;
	/** compatible brand count */
	uint32  brand_count;

} ISO_FILE_TYPE_BOX, *PISO_FILE_TYPE_BOX;

/** Movie Box */
typedef struct tag_iso_movie_box
{
	/** box */
	ISO_BOX box;

} ISO_MOVIE_BOX, *PISO_MOVIE_BOX;

/** Media Data Box */
typedef struct tag_iso_media_data_box
{
	/** box */
	ISO_BOX box;

} ISO_MEDIA_DATA_BOX, *PISO_MEDIA_DATA_BOX;

/** Movie Header Box */
typedef struct tag_iso_movie_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** creation time */
	uint32 creation_time;
	/** modification time */
	uint32 modification_time;
	/** timescale */
	uint32 time_scale;
	/** duration */
	uint32 duration;

	/** rate */
	int32  rate;
	/** volume */
	int16  volume;
	/** padding */
	int16  padding;
	/** resv */
	uint32 resv[2];

	/** matrix */
	int32  matrix[9];
	/** pre define */
	int32  pre_define[6];
	/** next track id */
	uint32 next_track_id;

} ISO_MOVIE_HEADER_BOX, *PISO_MOVIE_HEADER_BOX;

/** Track Box */
typedef struct tag_iso_track_box
{
	/** box */
	ISO_BOX box;

} ISO_TRACK_BOX, *PISO_TRACK_BOX;

/** Track Header Box */
typedef struct tag_iso_track_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** creation time */
	uint32 creation_time;
	/** modification time */
	uint32 modification_time;
	/** track id */
	uint32 track_id;
	/** resv */
	uint32 resv;
	/** duration */
	uint32 duration;

	/** resv2 */
	uint32 resv2[2];
	/** layer */
	int16  layer;
	/** alternate group */
	int16  alternate_group;
	/** volume */
	int16  volume;
	/** padding */
	uint16 padding;

	/** matrix */
	int32  matrix[9];
	/** width */
	uint32 width;
	/** height */
	uint32 height;

} ISO_TRACK_HEADER_BOX, *PISO_TRACK_HEADER_BOX;

/** Media Box */
typedef struct tag_iso_media_box
{
	/** box */
	ISO_BOX box;

} ISO_MEDIA_BOX, *PISO_MEDIA_BOX;

/** Media Header Box */
typedef struct tag_iso_media_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** creation time */
	uint32 creation_time;
	/** modification time */
	uint32 modification_time;
	/** timescale */
	uint32 time_scale;
	/** duration */
	uint32 duration;

	/** language */
	uint16 language;
	/** pre define */
	uint16 pre_define;

} ISO_MEDIA_HEADER_BOX, *PISO_MEDIA_HEADER_BOX;

/** Handler Reference Box */
typedef struct tag_iso_handler_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** pre define */
	uint32 pre_define;
	/** handler type */
	uint32 handler_type;
	/** resv */
	uint32 resv[3];

	/** name */
	int8*  p_name;
	/** name size */
	int32  name_size;

} ISO_HANDLER_BOX, *PISO_HANDLER_BOX;

/** Media Information Box */
typedef struct tag_iso_media_info_box
{
	/** box */
	ISO_BOX box;

} ISO_MEDIA_INFO_BOX, *PISO_MEDIA_INFO_BOX;

/** Video Media Header Box */
typedef struct tag_iso_video_media_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** graphic mode */
	uint16 graph_mode;
	/** op color */
	uint16 op_color[3];

} ISO_VIDEO_MEDIA_HEADER_BOX, *PISO_VIDEO_MEDIA_HEADER_BOX;

/** Sound Media Header Box */
typedef struct tag_iso_sound_media_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** balance */
	int16  balance;
	/** resv */
	uint16 resv;

} ISO_SOUND_MEDIA_HEADER_BOX, *PISO_SOUND_MEDIA_HEADER_BOX;

/** Data Information Box */
typedef struct tag_iso_data_info_box
{
	/** box */
	ISO_BOX box;

} ISO_DATA_INFO_BOX, *PISO_DATA_INFO_BOX;

/** Data Entry URL Box */
typedef struct tag_iso_data_entry_url_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** location */
	int8*  p_location;
	/** location size */
	int32  location_size;

} ISO_DATA_ENTRY_URL_BOX, *PISO_DATA_ENTRY_URL_BOX;

/** Data Entry URN Box */
typedef struct tag_iso_data_entry_urn_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** name */
	int8*  p_name;
	/** name size */
	int32  name_size;
	/** location */
	int8*  p_location;
	/** location size */
	int32  location_size;

} ISO_DATA_ENTRY_URN_BOX, *PISO_DATA_ENTRY_URN_BOX;

/** Data Entry */
typedef struct tag_iso_data_entry
{
	/** entry type */
	int32  entry_type;

	union
	{
		/** url */
		ISO_DATA_ENTRY_URL_BOX url;
		/** urn */
		ISO_DATA_ENTRY_URN_BOX urn;

	} entry_box;

} ISO_DATA_ENTRY, *PISO_DATA_ENTRY;

/** Data Ref Box */
typedef struct tag_iso_data_ref_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry_count */
	uint32 entry_count;
	/** entry */
	ISO_DATA_ENTRY* p_entry;

} ISO_DATA_REF_BOX, *PISO_DATA_REF_BOX;

/** Sample Table Box */
typedef struct tag_iso_sample_table_box
{
	/** box */
	ISO_BOX box;

} ISO_SAMPLE_TABLE_BOX, *PISO_SAMPLE_TABLE_BOX;

/** Time to Sample Entry */
typedef struct tag_iso_time_to_sample_entry
{
	/** sample count */
	uint32 sample_count;
	/** sample delta */
	uint32 sample_delta;

} ISO_TIME_TO_SAMPLE_ENTRY, *PISO_TIME_TO_SAMPLE_ENTRY;

/** Decoding Time to Sample Box */
typedef struct tag_iso_time_to_sample_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_TIME_TO_SAMPLE_ENTRY* p_entry;

} ISO_TIME_TO_SAMPLE_BOX, *PISO_TIME_TO_SAMPLE_BOX;

/** Composition Time to Sample Entry */
typedef struct tag_iso_comp_time_to_sample_entry
{
	/** sample count */
	uint32 sample_count;
	/** sample offset */
	uint32 sample_offset;

} ISO_COMP_TIME_TO_SAMPLE_ENTRY, *PISO_COMP_TIME_TO_SAMPLE_ENTRY;

/** Composition Time to Sample Box */
typedef struct tag_iso_comp_time_to_sample_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_COMP_TIME_TO_SAMPLE_ENTRY* p_entry;

} ISO_COMP_TIME_TO_SAMPLE_BOX, *PISO_COMP_TIME_TO_SAMPLE_BOX;

/** Sample Entry */
typedef struct tag_iso_sample_entry
{
	/** box */
	ISO_BOX box;

	/** resv */
	uint8  resv[6];
	/** data reference index */
	uint16 data_ref_index;

} ISO_SAMPLE_ENTRY, *PISO_SAMPLE_ENTRY;

#pragma pack(push)
#pragma pack(1)

/** Visual Sample Entry */
typedef struct tag_iso_visual_sample_entry
{
	/** entry */
	ISO_SAMPLE_ENTRY entry;

	/** pre define */
	uint16 pre_define;
	/** padding */
	uint16 padding;
	/** pre define2 */
	uint32 pre_define2[3];

	/** width */
	uint16 width;
	/** height */
	uint16 height;

	/** horiz resolution */
	uint32 horiz_resolution;
	/** vert resolution */
	uint32 vert_resolution;
	/** resv */
	uint32 resv;
	/** frame count */
	uint16 frame_count;

	/** compressor name */
	int8   compressor_name[ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE];

	/** depth */
	uint16 depth;
	/** pre define */
	int16  pre_define3;

} ISO_VISUAL_SAMPLE_ENTRY, *PISO_VISUAL_SAMPLE_ENTRY;

#pragma pack(pop)

/** Visual Sample Description Box */
typedef struct tag_iso_visual_sample_desc_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_VISUAL_SAMPLE_ENTRY* p_entry;

} ISO_VISUAL_SAMPLE_DESC_BOX, *PISO_VISUAL_SAMPLE_DESC_BOX;

/** Audio Sample Entry */
typedef struct tag_iso_audio_sample_entry
{
	/** entry */
	ISO_SAMPLE_ENTRY entry;

	/** resv */
	uint32 resv[2];
	/** channel count */
	uint16 chn_count;
	/** sample size */
	uint16 sample_size;

	/** pre define */
	uint16 pre_define;
	/** padding */
	uint16 padding;
	/** sample rate */
	uint32 sample_rate;

} ISO_AUDIO_SAMPLE_ENTRY, *PISO_AUDIO_SAMPLE_ENTRY;

/** Audio Sample Description Box */
typedef struct tag_iso_audio_sample_desc_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_AUDIO_SAMPLE_ENTRY* p_entry;

} ISO_AUDIO_SAMPLE_DESC_BOX, *PISO_AUDIO_SAMPLE_DESC_BOX;

/** Sample Size Entry */
typedef struct tag_iso_sample_size_entry
{
	/** entry size */
	uint32 entry_size;

} ISO_SAMPLE_SIZE_ENTRY, *PISO_SAMPLE_SIZE_ENTRY;

/** Sample Size Box */
typedef struct tag_iso_sample_size_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** sample size */
	uint32 sample_size;
	/** sample count */
	uint32 sample_count;
	/** entry */
	ISO_SAMPLE_SIZE_ENTRY* p_entry;

} ISO_SAMPLE_SIZE_BOX, *PISO_SAMPLE_SIZE_BOX;

/** Sample to Chunk Entry */
typedef struct tag_iso_sample_to_chunk_entry
{
	/** first chunk */
	uint32 first_chunk;
	/** samples per chunk */
	uint32 sample_per_chunk;
	/** sample description index */
	uint32 sample_desc_index;

} ISO_SAMPLE_TO_CHUNK_ENTRY, *PISO_SAMPLE_TO_CHUNK_ENTRY;

/** Sample to Chunk Box */
typedef struct tag_iso_sample_to_chunk_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_SAMPLE_TO_CHUNK_ENTRY* p_entry;

} ISO_SAMPLE_TO_CHUNK_BOX, *PISO_SAMPLE_TO_CHUNK_BOX;

/** Chunk Offset Entry */
typedef struct tag_iso_chunk_offset_entry
{
	/** chunk offset */
	uint32 chunk_offset;

} ISO_CHUNK_OFFSET_ENTRY, *PISO_CHUNK_OFFSET_ENTRY;

/** Chunk Offset Box */
typedef struct tag_iso_chunk_offset_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_CHUNK_OFFSET_ENTRY* p_entry;

} ISO_CHUNK_OFFSET_BOX, *PISO_CHUNK_OFFSET_BOX;

/** Sync Sample Entry */
typedef struct tag_iso_sync_sample_entry
{
	/** sample number */
	uint32 sample_number;

} ISO_SYNC_SAMPLE_ENTRY, *PISO_SYNC_SAMPLE_ENTRY;

/** Sync Sample Box */
typedef struct tag_iso_sync_sample_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** entry count */
	uint32 entry_count;
	/** entry */
	ISO_SYNC_SAMPLE_ENTRY* p_entry;

} ISO_SYNC_SAMPLE_BOX, *PISO_SYNC_SAMPLE_BOX;

/** Free Space Box */
typedef struct tag_iso_free_space_box
{
	/** box */
	ISO_BOX box;

} ISO_FREE_SPACE_BOX, *PISO_FREE_SPACE_BOX;

/** Movie Extend Box */
typedef struct tag_iso_movie_extend_box
{
	/** box */
	ISO_BOX box;

} ISO_MOVIE_EXTEND_BOX, *PISO_MOVIE_EXTEND_BOX;

/** Movie Extend Header Box */
typedef struct tag_iso_movie_extend_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** fragment duration */
	uint32 fragment_duration;

} ISO_MOVIE_EXTEND_HEADER_BOX, *PISO_MOVIE_EXTEND_HEADER_BOX;

/** Track Extend Box */
typedef struct tag_iso_track_extend_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** track id */
	uint32 track_id;
	/** default sample description index */
	uint32 def_sample_desc_index;
	/** default sample duration */
	uint32 def_sample_duration;
	/** default sample size */
	uint32 def_sample_size;
	/** default sample flag */
	uint32 def_sample_flag;

} ISO_TRACK_EXTEND_BOX, *PISO_TRACK_EXTEND_BOX;

/** Movie Fragment Box */
typedef struct tag_iso_movie_fragment_box
{
	/** box */
	ISO_BOX box;

} ISO_MOVIE_FRAGMENT_BOX, *PISO_MOVIE_FRAGMENT_BOX;

/** Movie Fragment Header Box */
typedef struct tag_iso_movie_fragment_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** sequence number */
	uint32 seq_no;

} ISO_MOVIE_FRAGMENT_HEADER_BOX, *PISO_MOVIE_FRAGMENT_HEADER_BOX;

/** Track Fragment Box */
typedef struct tag_iso_track_fragment_box
{
	/** box */
	ISO_BOX box;

} ISO_TRACK_FRAGMENT_BOX, *PISO_TRACK_FRAGMENT_BOX;

/** Track Fragment Header Box */
typedef struct tag_iso_track_fragment_header_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** track id */
	uint32 track_id;

	/** base data offset */
	uint64 base_data_offset;
	/** sample description index */
	uint32 sample_desc_index;
	/** default sample duration */
	uint32 def_sample_duration;
	/** default sample size */
	uint32 def_sample_size;
	/** default sample flag */
	uint32 def_sample_flag;

} ISO_TRACK_FRAGMENT_HEADER_BOX, *PISO_TRACK_FRAGMENT_HEADER_BOX;

/** Track Fragment Sample */
typedef struct tag_iso_track_fragment_sample
{
	/** sample duration */
	uint32 sample_duration;
	/** sample size */
	uint32 sample_size;
	/** sample flag */
	uint32 sample_flag;
	/** sample composition time offset */
	uint32 sample_composition_time_offset;

} ISO_TRACK_FRAGMENT_SAMPLE, *PISO_TRACK_FRAGMENT_SAMPLE;

/** Track Fragment Run Box */
typedef struct tag_iso_track_fragment_run_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** sample count */
	uint32 sample_count;

	/** data offset */
	uint32 data_offset;
	/** first sample flag */
	uint32 first_sample_flag;

	/** sample */
	ISO_TRACK_FRAGMENT_SAMPLE* p_sample;

} ISO_TRACK_FRAGMENT_RUN_BOX, *PISO_TRACK_FRAGMENT_RUN_BOX;

/** Movie Fragment Random Access Box */
typedef struct tag_iso_movie_fragment_random_access_box
{
	/** box */
	ISO_BOX box;

} ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX, *PISO_MOVIE_FRAGMENT_RANDOM_ACCESS_BOX;

/** Track Fragment Random Access Entry */
typedef struct tag_iso_track_fragment_random_access_entry
{
	/** time */
	uint32 time;
	/** moof offset */
	uint32 moof_offset;

	/** traf number */
	uint32 traf_number;
	/** trun number */
	uint32 trun_number;
	/** sample number */
	uint32 sample_number;

} ISO_TRACK_FRAGMENT_RANDOM_ACCESS_ENTRY, *PISO_TRACK_FRAGMENT_RANDOM_ACCESS_ENTRY;

/** Track Fragment Random Access Box */
typedef struct tag_iso_track_fragment_random_access_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** track id */
	uint32 track_id;
	/** mixture */
	uint32 mixture;

	/** number of entry */
	uint32 entry_num;
	/** entry */
	ISO_TRACK_FRAGMENT_RANDOM_ACCESS_ENTRY* p_entry;

} ISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX, *PISO_TRACK_FRAGMENT_RANDOM_ACCESS_BOX;

/** Movie Fragment Random Access Offset Box */
typedef struct tag_iso_movie_fragment_random_access_offset_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** size */
	uint32 size;

} ISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX, *PISO_MOVIE_FRAGMENT_RANDOM_ACCESS_OFFSET_BOX;

#if 0
/** Sample Dependency Info */
typedef struct tag_iso_sample_dependency_info
{
	/** resv */
	uint32 resv;

} ISO_SAMPLE_DEPENDENCY_INFO, *PISO_SAMPLE_DEPENDENCY_INFO;

/** Sample Dependency Type Box */
typedef struct tag_iso_sample_dependency_type_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** sample count */
	uint32 sample_count;

	/** sample */
	ISO_TRACK_FRAGMENT_SAMPLE* p_sample;

} ISO_SAMPLE_DEPENDENCY_TYPE_BOX, *PISO_SAMPLE_DEPENDENCY_TYPE_BOX;
#endif

/** Meta Box */
typedef struct tag_iso_meta_box
{
	/** full box */
	ISO_FULL_BOX full_box;

	/** handler box */
	ISO_HANDLER_BOX handler_box;

} ISO_META_BOX, *PISO_META_BOX;

#endif ///__ISO_MEIDA_FILE_H__
