

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <public/gen_file_const.h>
#include <public/media_video.h>
#include <util/log_debug.h>

#include "../../../public/media_video.h"
#include "../../../public/media_audio.h"
#include "../../mpeg4/mpeg4_public.h"
#include "../../mpeg4/mpeg4_aac_const.h"
#include "../../h264/h264_public.h"
#include "../../h264/h264_nalu_demux.h"

#include "mp4_avc_demux.h"
#include "mp4_muxer.h"
#include "mp4_file_writer.h"


/** 最大视频索引个数 */
#define MP4_FILE_WRITER_MAX_VIDEO_INDEX                    (512 * 1024)
/** 视频帧时间戳列表大小 */
#define MP4_FILE_WRITER_VIDEO_TIME_ENTRY_BUF_SIZE          (MP4_FILE_WRITER_MAX_VIDEO_INDEX * sizeof(ISO_TIME_TO_SAMPLE_ENTRY))
/** 视频帧长度列表大小 */
#define MP4_FILE_WRITER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE   (MP4_FILE_WRITER_MAX_VIDEO_INDEX * sizeof(ISO_SAMPLE_SIZE_ENTRY))
/** 视频帧偏移列表大小 */
#define MP4_FILE_WRITER_VIDEO_CHUNK_OFFSET_ENTRY_BUF_SIZE  (MP4_FILE_WRITER_MAX_VIDEO_INDEX * sizeof(ISO_CHUNK_OFFSET_ENTRY))
/** 最大视频关键帧个数 */
#define MP4_FILE_WRITER_MAX_VIDEO_SYNC_ENTRY_NUM           (128 * 1024)
/** 视频关键帧列表大小 */
#define MP4_FILE_WRITER_VIDEO_SYNC_ENTRY_BUF_SIZE          (MP4_FILE_WRITER_MAX_VIDEO_SYNC_ENTRY_NUM * sizeof(ISO_SYNC_SAMPLE_ENTRY))
/** 最大音频索引个数 */
#define MP4_FILE_WRITER_MAX_AUDIO_INDEX                    (512 * 1024)
/** 音频帧时间戳列表大小 */
#define MP4_FILE_WRITER_AUDIO_TIME_ENTRY_BUF_SIZE          (MP4_FILE_WRITER_MAX_AUDIO_INDEX * sizeof(ISO_TIME_TO_SAMPLE_ENTRY))
/** 音频帧长度列表大小 */
#define MP4_FILE_WRITER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE   (MP4_FILE_WRITER_MAX_AUDIO_INDEX * sizeof(ISO_SAMPLE_SIZE_ENTRY))
/** 音频帧偏移列表大小 */
#define MP4_FILE_WRITER_AUDIO_CHUNK_OFFSET_ENTRY_BUF_SIZE  (MP4_FILE_WRITER_MAX_AUDIO_INDEX * sizeof(ISO_CHUNK_OFFSET_ENTRY))

/** 写入缓冲区大小 */
#define MP4_FILE_WRITER_BUF_SIZE                           (256 * 1024)
/** max avc dec config buf size */
#define MP4_FILE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE        (1024)
/** 填充缓冲区大小 */
#define MP4_FILE_WRITER_PADDING_BUF_SIZE                   (512)
/** 最大字符串长度 */
#define MP4_FILE_WRITER_MAX_STR_SIZE                       (255)
/** 名称长度 */
#define MP4_FILE_WRITER_MAX_NAME_SIZE                      (32)
/** dec spec info buf size */
#define MP4_FILE_WRITER_MAX_DEC_SPEC_BUF_SIZE              (32)
/** 名称 */
#define MP4_FILE_WRITER_LOGO_NAME                          "robert"

/** 最大音频帧长度 */
//#define MP4_FILE_WRITER_MAX_AUDIO_FRAME                    (8 * 1024)

/** video track id */
#define MP4_FILE_WRITER_VIDEO_TRACK_ID                     (1)
/** audio track id */
#define MP4_FILE_WRITER_AUDIO_TRACK_ID                     (2)
/** max track id */
#define MP4_FILE_WRITER_MAX_TRACK_ID                       (MP4_FILE_WRITER_AUDIO_TRACK_ID)
/** max sps record */
#define MP4_FILE_WRITER_MAX_SPS_RECORD                     (2)
/** max pps record */
#define MP4_FILE_WRITER_MAX_PPS_RECORD                     (2)

/** uni time scale */
//#define MP4_FILE_WRITER_UNI_TIME_SCALE                     (MP4_TIMESCALE_MPEG2_TS)
#define MP4_FILE_WRITER_UNI_TIME_SCALE                     (1000)
/** use uni time scale */
//#define MP4_FILE_WRITER_USE_UNI_TIME_SCALE                 (0)
/** 是否处理异常 */
#define MP4_FILE_WRITER_ENABLE_EXCEPTION                   (1)




CMp4FileWriter::CMp4FileWriter()
{
	m_bInit = 0;

	m_Muxer = NULL;

	m_bStart = 0;

	m_pAvcDecConfigBuf = NULL;
	m_pSPSBuf = NULL;
	m_pPPSBuf = NULL;

	m_pWriteBuf = NULL;

	m_pVideoSizeEntry = NULL;
	m_pVideoOffsetEntry = NULL;
	m_pVideoSyncEntry = NULL;

	m_pAudioSizeEntry = NULL;
	m_pAudioOffsetEntry = NULL;

	m_hFile = NULL;
}

CMp4FileWriter::~CMp4FileWriter()
{
	if( m_bInit )
	{
		Deinit();
	}
}

int32 CMp4FileWriter::Init()
{
	if( m_bInit )
	{
		return 0;
	}

	m_Muxer = mp4_file_muxer_create();
	if( m_Muxer == NULL )
	{
		Deinit();
		return -1;
	}

	mp4_file_muxer_init(m_Muxer);

	m_bGetAvcDecConfig = 0;
	m_pAvcDecConfigBuf = (uint8*)malloc(MP4_FILE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE);
	if( m_pAvcDecConfigBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_AvcDecConfigBufSize = MP4_FILE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE;

	m_pSPSBuf = (uint8*)malloc(H264_MAX_SPS_NALU_SIZE);
	if( m_pSPSBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_SPSBufSize = H264_MAX_SPS_NALU_SIZE;

	m_pPPSBuf = (uint8*)malloc(H264_MAX_PPS_NALU_SIZE);
	if( m_pPPSBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_PPSBufSize = H264_MAX_PPS_NALU_SIZE;

	m_pWriteBuf = (uint8*)malloc(MP4_FILE_WRITER_BUF_SIZE);
	if( m_pWriteBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_WriteBufSize = MP4_FILE_WRITER_BUF_SIZE;

	m_pVideoTimeEntry = (ISO_TIME_TO_SAMPLE_ENTRY*)malloc(MP4_FILE_WRITER_VIDEO_TIME_ENTRY_BUF_SIZE);
	if( m_pVideoTimeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pVideoSizeEntry = (ISO_SAMPLE_SIZE_ENTRY*)malloc(MP4_FILE_WRITER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( m_pVideoSizeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pVideoOffsetEntry = (ISO_CHUNK_OFFSET_ENTRY*)malloc(MP4_FILE_WRITER_VIDEO_CHUNK_OFFSET_ENTRY_BUF_SIZE);
	if( m_pVideoOffsetEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pVideoSyncEntry = (ISO_SYNC_SAMPLE_ENTRY*)malloc(MP4_FILE_WRITER_VIDEO_SYNC_ENTRY_BUF_SIZE);
	if( m_pVideoSyncEntry == NULL )
	{
		Deinit();
		return -1;
	}
	m_VideoSyncEntryCount = 0;

	m_pAudioTimeEntry = (ISO_TIME_TO_SAMPLE_ENTRY*)malloc(MP4_FILE_WRITER_AUDIO_TIME_ENTRY_BUF_SIZE);
	if( m_pAudioTimeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pAudioSizeEntry = (ISO_SAMPLE_SIZE_ENTRY*)malloc(MP4_FILE_WRITER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( m_pAudioSizeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pAudioOffsetEntry = (ISO_CHUNK_OFFSET_ENTRY*)malloc(MP4_FILE_WRITER_AUDIO_CHUNK_OFFSET_ENTRY_BUF_SIZE);
	if( m_pAudioOffsetEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_hFile = gen_file_malloc();
	if( m_hFile == NULL )
	{
		Deinit();
		return -1;
	}
	gen_file_init(m_hFile);

	m_bStart = 0;
	m_bEnableVideo = 1;
	m_bEnableAudio = 1;

	m_TimeScale = MP4_FILE_WRITER_UNI_TIME_SCALE;


	m_TotalWriteSize = 0;

	m_bInit = 1;

	return 0;
}

int32 CMp4FileWriter::Deinit()
{
	if( m_bStart )
	{
		Stop();
	}

	if( m_Muxer )
	{
		mp4_file_muxer_destroy(m_Muxer);
		m_Muxer = NULL;
	}

	if( m_pAvcDecConfigBuf )
	{
		free(m_pAvcDecConfigBuf);
		m_pAvcDecConfigBuf = NULL;
	}

	if( m_pSPSBuf )
	{
		free(m_pSPSBuf);
		m_pSPSBuf = NULL;
	}

	if( m_pPPSBuf )
	{
		free(m_pPPSBuf);
		m_pPPSBuf = NULL;
	}

	if( m_pWriteBuf )
	{
		free(m_pWriteBuf);
		m_pWriteBuf = NULL;
	}

	if( m_pVideoTimeEntry )
	{
		free(m_pVideoTimeEntry);
		m_pVideoTimeEntry = NULL;
	}

	if( m_pVideoSizeEntry )
	{
		free(m_pVideoSizeEntry);
		m_pVideoSizeEntry = NULL;
	}

	if( m_pVideoOffsetEntry )
	{
		free(m_pVideoOffsetEntry);
		m_pVideoOffsetEntry = NULL;
	}

	if( m_pVideoSyncEntry )
	{
		free(m_pVideoSyncEntry);
		m_pVideoSyncEntry = NULL;
	}

	if( m_pAudioTimeEntry )
	{
		free(m_pAudioTimeEntry);
		m_pAudioTimeEntry = NULL;
	}

	if( m_pAudioSizeEntry )
	{
		free(m_pAudioSizeEntry);
		m_pAudioSizeEntry = NULL;
	}

	if( m_pAudioOffsetEntry )
	{
		free(m_pAudioOffsetEntry);
		m_pAudioOffsetEntry = NULL;
	}

	if( m_hFile )
	{
		gen_file_free(m_hFile);
		m_hFile = NULL;
	}


	m_bInit = 0;

	return 0;
}

int32 CMp4FileWriter::Reset()
{
	if( !m_bInit )
	{
		return -1;
	}

	if( m_Muxer )
	{
		mp4_file_muxer_reset(m_Muxer);
	}

	return 0;
}

int32 CMp4FileWriter::EnableVideo(int32 b_enable)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_bEnableVideo = b_enable;

	return 0;
}

int32 CMp4FileWriter::EnableAudio(int32 b_enable)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_bEnableAudio = b_enable;

	return 0;
}

int32 CMp4FileWriter::SetTimescale(uint32 time_scale)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_TimeScale = time_scale;

	return 0;
}

int32 CMp4FileWriter::Start(int8 *p_file_path, int32 file_path_size)
{
	GEN_FILE_OPEN_PARAM open_param;

	int32 ret;

	if( !m_bInit )
	{
		return -1;
	}

	if( m_bStart )
	{
		Stop();
	}

	open_param.rw = GEN_FILE_OPEN_WRITE;
	open_param.share = GEN_FILE_SHARE_READ;
	open_param.create = GEN_FILE_CREATE_ALWAYS;
	open_param.open_flag = 0;
	ret = gen_file_set_param(m_hFile, GEN_FILE_PARAM_OPEN, (uint8*)&open_param, sizeof(open_param));

	ret = gen_file_openA(m_hFile, p_file_path, file_path_size);
	if( ret )
	{
		return -1;
	}

	m_TotalWriteSize = 0;

	ret = WriteFileTypeBox();

	/** init status */
	m_bFindKeyFrame = 0;
	m_bFirstVideoFrame = 1;
	m_bFirstAudioFrame = 1;
	m_bFirstFragment = 0;
	m_bWriteMovieBox = 0;
	m_bHasVideo = 0;
	m_bHasAudio = 0;

	m_TrackNum = 0;

	memset(&m_VideoFrame, 0, sizeof(m_VideoFrame));
	memset(&m_AudioFrame, 0, sizeof(m_AudioFrame));

	m_bGetSps = 0;
	m_bGetPps = 0;

	m_VideoFrameCount = 0;
	m_TotalVideoElapseTime = 0;
	m_TotalVideoElapseTimeMs = 0;
	m_TotalVideoFrameSize = 0;

	m_VideoSyncEntryCount = 0;

	m_AudioFrameCount = 0;
	m_TotalAudioElapseTime = 0;
	m_TotalAudioElapseTime = 0;
	m_TotalAudioElapseTimeMs = 0;
	m_TotalAudioSampleCount = 0;
	m_TotalAudioSampleDuration = 0;
	m_TotalAudioFrameSize = 0;
	m_TotalFrameSize = 0;

	m_bStart = 1;

	return 0;
}

int32 CMp4FileWriter::Stop()
{
	ISO_MOVIE_BOX  movie_box;
	ISO_MOVIE_HEADER_BOX movie_header_box;

	ISO_TRACK_BOX track_box;
	ISO_TRACK_HEADER_BOX track_header_box;
	ISO_MEDIA_BOX media_box;
	ISO_MEDIA_HEADER_BOX media_header_box;
	ISO_HANDLER_BOX handler_box;

	ISO_MEDIA_INFO_BOX media_info_box;
	ISO_VIDEO_MEDIA_HEADER_BOX video_media_header_box;
	ISO_SOUND_MEDIA_HEADER_BOX sound_media_header_box;
	ISO_DATA_INFO_BOX data_info_box;
	ISO_DATA_REF_BOX data_ref_box;
	ISO_DATA_ENTRY data_entry;
	ISO_SAMPLE_TABLE_BOX sample_table_box;

	ISO_VISUAL_SAMPLE_DESC_BOX visual_sample_desc_box;
	ISO_VISUAL_SAMPLE_ENTRY visual_sample_entry;
	ISO_AUDIO_SAMPLE_DESC_BOX audio_sample_desc_box;
	ISO_AUDIO_SAMPLE_ENTRY audio_sample_entry;

	ISO_TIME_TO_SAMPLE_BOX time_to_sample_box;
	ISO_TIME_TO_SAMPLE_ENTRY time_to_sample_entry;
	ISO_SAMPLE_SIZE_BOX sample_size_box;
	ISO_SAMPLE_TO_CHUNK_BOX sample_to_chunk_box;
	ISO_CHUNK_OFFSET_BOX chunk_offset_box;
	ISO_SYNC_SAMPLE_BOX sync_sample_box;

	ISO_SAMPLE_TO_CHUNK_ENTRY sample_to_chunk_entry;

	AVC_SAMPLE_DESC_BOX avc_sample_desc_box;
	AVC_SPS_RECORD avc_sps_record;
	AVC_PPS_RECORD avc_pps_record;

	AAC_SAMPLE_DESC_BOX aac_sample_desc_box;
	AAC_SAMPLE_ENTRY  aac_sample_entry;

	uint32 movie_box_offset;
	uint32 track_box_offset;
	uint32 track_box_size;
	uint32 media_box_offset;
	uint32 media_box_size;
	uint32 media_info_box_offset;
	uint32 media_info_box_size;
	uint32 data_info_box_offset;
	uint32 data_info_box_size;
	uint32 sample_table_box_offset;
	uint32 sample_table_box_size;
	//uint32 sample_size_box_offset;
	//uint32 sample_size_box_size;

	int32  mux_size;
	uint32 total_mux_size;

	time_t utc_time;
	uint32 creation_time;
	uint32 modification_time;
	//uint32 time_scale;
	//uint32 duration;
	uint32 total_entry_size;

	int32  b_open;
	uint32 write_size;

	int8   str[MP4_FILE_WRITER_MAX_STR_SIZE + 1];

	uint8  dec_spec_buf[MP4_FILE_WRITER_MAX_DEC_SPEC_BUF_SIZE];
	int32  dec_spec_buf_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_buf_offset = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	int32  i;
	int32  ret;

	if( !m_bInit )
	{
		return -1;
	}

	if( !m_bStart )
	{
		return 0;
	}

	ret = gen_file_is_open(m_hFile, &b_open);
	if( !b_open )
	{
		goto FIN;
	}

	p_uint8 = m_pWriteBuf;
	buf_left_size = m_WriteBufSize;

	/** movie box */
	movie_box_offset = m_TotalWriteSize;
	total_mux_size = 0;

	movie_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_make_movie_box(m_Muxer, &movie_box, p_uint8, &mux_size);
	ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
	m_TotalWriteSize += mux_size;
	total_mux_size += mux_size;

	/** movie header box */
	utc_time = time(NULL);
	creation_time = utc_time + MP4_UTC_TIME_1904_TO_1970;
	modification_time = creation_time;

	movie_header_box.creation_time = creation_time;
	movie_header_box.modification_time = modification_time;

	if( m_bHasVideo )
	{
		movie_header_box.time_scale = m_TimeScale;
		movie_header_box.duration = m_TotalVideoElapseTime;

	}else if( m_bHasAudio )
	{
		movie_header_box.time_scale = m_TimeScale;
		movie_header_box.duration = m_TotalAudioElapseTime;

	}else
	{
		movie_header_box.time_scale = m_TimeScale;
		movie_header_box.duration = UINT32_MAX;
	}
	movie_header_box.next_track_id = MP4_FILE_WRITER_MAX_TRACK_ID + 1;

	mux_size = buf_left_size;
	ret = mp4_make_movie_header_box(m_Muxer, &movie_header_box, p_uint8, &mux_size);
	ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
	m_TotalWriteSize += mux_size;
	total_mux_size += mux_size;

	memset(str, 0, MP4_FILE_WRITER_MAX_STR_SIZE + 1);
	str[0] = 0x0;

	/** track */
	for( i = 0; i < m_TrackNum; i++ )
	{
		/** track box */
		track_box_offset = m_TotalWriteSize;
		track_box_size = 0;

		track_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_track_box(m_Muxer, &track_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** track header box */
		track_header_box.full_box.version_flag.value =
			(MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | (MP4_TRACK_ENABLE_FLAG | MP4_TRACK_IN_MOVIE_FLAG | MP4_TRACK_IN_PREVIEW_FLAG);
		track_header_box.creation_time = creation_time;
		track_header_box.modification_time = modification_time;

		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_header_box.track_id = MP4_FILE_WRITER_VIDEO_TRACK_ID;
			track_header_box.duration = m_TotalVideoElapseTime;
			track_header_box.volume = 0x0;

		}else
		{
			track_header_box.track_id = MP4_FILE_WRITER_AUDIO_TRACK_ID;
			track_header_box.duration = m_TotalAudioElapseTime;
			track_header_box.volume = 0x0100;
		}

		track_header_box.width = m_VideoFrame.param.video.width << 16;
		track_header_box.height = m_VideoFrame.param.video.height << 16;

		mux_size = buf_left_size;
		ret = mp4_make_track_header_box(m_Muxer, &track_header_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media box */
		media_box_offset = m_TotalWriteSize;
		media_box_size = 0;

		media_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_media_box(m_Muxer, &media_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media header box */
		media_header_box.creation_time = creation_time;
		media_header_box.modification_time = modification_time;

		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			media_header_box.time_scale = m_TimeScale;
			media_header_box.duration = m_TotalVideoElapseTime;

		}else
		{
			//media_header_box.time_scale = m_AudioFrame.param.audio.sample_rate;
			media_header_box.time_scale = m_TimeScale;
			media_header_box.duration = m_TotalAudioElapseTime;
		}

		//chi
		media_header_box.language = 0x0D09;

		mux_size = buf_left_size;
		ret = mp4_make_media_header_box(m_Muxer, &media_header_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** handler ref box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_VIDE;

		}else
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_SOUN;
		}

		handler_box.p_name = str;
		handler_box.name_size = 1;

		mux_size = buf_left_size;
		ret = mp4_make_handler_box(m_Muxer, &handler_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media info box */
		media_info_box_offset = m_TotalWriteSize;
		media_info_box_size = 0;

		media_info_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_media_info_box(m_Muxer, &media_info_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** video media header box */
			mux_size = buf_left_size;
			ret = mp4_make_video_media_header_box(m_Muxer, &video_media_header_box, p_uint8, &mux_size);
			ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
			m_TotalWriteSize += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** sound media header box */
			mux_size = buf_left_size;
			ret = mp4_make_sound_media_header_box(m_Muxer, &sound_media_header_box, p_uint8, &mux_size);
			ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
			m_TotalWriteSize += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** data info box */
		data_info_box_offset = m_TotalWriteSize;
		data_info_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_data_info_box(m_Muxer, &data_info_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		data_info_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** data ref box */
		data_entry.entry_type = MP4_DATA_ENTRY_TYPE_URL;
		data_entry.entry_box.url.full_box.version_flag.value =
			(MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | MP4_DATA_ENTRY_FLAG_SELF_CONTAIN;
		data_entry.entry_box.url.p_location = NULL;
		data_entry.entry_box.url.location_size = 0;

		data_ref_box.entry_count = 1;
		data_ref_box.p_entry = &data_entry;

		mux_size = buf_left_size;
		ret = mp4_make_data_ref_box(m_Muxer, &data_ref_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		data_info_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite data info box size */
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, data_info_box_offset, NULL);

		data_info_box_size = hton_u32(data_info_box_size);
		ret = gen_file_write(m_hFile, (int8*)&data_info_box_size, sizeof(data_info_box_size), &write_size);

		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_END, 0, NULL);

		/** sample table box */
		sample_table_box_offset = m_TotalWriteSize;
		sample_table_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_sample_table_box(m_Muxer, &sample_table_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample desc box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** visual */
			if(	m_VideoFourcc == MAKE_FOURCC('a','v','c','1') )
			{
				avc_sample_desc_box.entry.visual.entry.data_ref_index = 0x1;
				avc_sample_desc_box.entry.visual.width = m_VideoFrame.param.video.width;
				avc_sample_desc_box.entry.visual.height = m_VideoFrame.param.video.height;
				memset(avc_sample_desc_box.entry.visual.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);
				avc_sample_desc_box.entry.config.record.profile = m_Profile;
				avc_sample_desc_box.entry.config.record.profile_compat = m_ProfileCompat;
				avc_sample_desc_box.entry.config.record.level = m_Level;
				avc_sample_desc_box.entry.config.record.len_size_minus_1 = 0x3;

				if( m_bGetSps )
				{
					avc_sps_record.nalu_len = m_SPSNaluSize;
					avc_sps_record.p_nalu = m_pSPSBuf;

					avc_sample_desc_box.entry.config.record.sps_count = 1;
					avc_sample_desc_box.entry.config.record.p_sps = &avc_sps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.sps_count = 0;
					avc_sample_desc_box.entry.config.record.p_sps = NULL;
				}

				if( m_bGetPps )
				{
					avc_pps_record.nalu_len = m_PPSNaluSize;
					avc_pps_record.p_nalu = m_pPPSBuf;

					avc_sample_desc_box.entry.config.record.pps_count = 1;
					avc_sample_desc_box.entry.config.record.p_pps = &avc_pps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.pps_count = 0;
					avc_sample_desc_box.entry.config.record.p_pps = NULL;
				}

				mux_size = buf_left_size;
				ret = mp4_make_avc_sample_desc_box(m_Muxer, &avc_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				visual_sample_entry.entry.box.type = m_VideoFourcc;
				visual_sample_entry.entry.data_ref_index = 0x1;
				visual_sample_entry.width = m_VideoFrame.param.video.width;
				visual_sample_entry.height = m_VideoFrame.param.video.height;
				memset(visual_sample_entry.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);

				visual_sample_desc_box.entry_count = 1;
				visual_sample_desc_box.p_entry = &visual_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_visual_sample_desc_box(m_Muxer, &visual_sample_desc_box, p_uint8, &mux_size);
			}

			ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
			m_TotalWriteSize += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** audio */
			//if(	m_AudioFourcc == MAKE_FOURCC('m','p','4','a') )
			if( m_AudioFrame.param.audio.algo == MP_AUDIO_AAC_LC )
			{
				aac_sample_entry.audio.entry.data_ref_index = 0x1;
				aac_sample_entry.audio.sample_rate = m_AudioFrame.param.audio.sample_rate << 16;

				aac_sample_entry.es_box.es_desc.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.es_id = MP4_FILE_WRITER_AUDIO_TRACK_ID;
				aac_sample_entry.es_box.es_desc.stream_depend_flag = 0x0;
				aac_sample_entry.es_box.es_desc.url_flag = 0x0;
				aac_sample_entry.es_box.es_desc.ocr_stream_flag = 0x0;
				aac_sample_entry.es_box.es_desc.stream_priority = 0x0;
				aac_sample_entry.es_box.es_desc.dec_config.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.dec_config.obj_type = MPEG4_OBJECT_TYPE_AUDIO_MPEG4;
				aac_sample_entry.es_box.es_desc.dec_config.stream_type = MPEG4_STREAM_TYPE_AUDIO_STREAM;
				aac_sample_entry.es_box.es_desc.dec_config.buf_size = 15000;
				aac_sample_entry.es_box.es_desc.dec_config.max_bitrate = 1500 * 1000;
				aac_sample_entry.es_box.es_desc.dec_config.avg_bitrate = 0;

				dec_spec_buf[0] = MPEG4_DEC_SPEC_INFO_TAG;
				dec_spec_buf_size = MP4_FILE_WRITER_MAX_DEC_SPEC_BUF_SIZE;
				ret = MakeAudioDecSpecInfo(dec_spec_buf + 2, &dec_spec_buf_size);
				dec_spec_buf[1] = dec_spec_buf_size;

				aac_sample_entry.es_box.es_desc.dec_config.p_vdec_spec_info = dec_spec_buf;
				aac_sample_entry.es_box.es_desc.dec_config.vdec_spec_info_size = dec_spec_buf_size + 2;

				aac_sample_entry.es_box.es_desc.sl_config.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.sl_config.predefine = MPEG4_PREDEFINE_MP4;

				aac_sample_desc_box.entry_count = 1;
				aac_sample_desc_box.p_entry = &aac_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_aac_sample_desc_box(m_Muxer, &aac_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				audio_sample_entry.entry.box.type = m_AudioFourcc;
				audio_sample_entry.entry.data_ref_index = 0x1;
				audio_sample_entry.chn_count = m_AudioFrame.param.audio.chn;
				audio_sample_entry.sample_rate = m_AudioFrame.param.audio.sample_rate << 16;

				audio_sample_desc_box.entry_count = 1;
				audio_sample_desc_box.p_entry = &audio_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_audio_sample_desc_box(m_Muxer, &audio_sample_desc_box, p_uint8, &mux_size);
			}

			ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
			m_TotalWriteSize += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** time to sample box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * m_VideoFrameCount;
			time_to_sample_box.entry_count = m_VideoFrameCount;
			time_to_sample_box.p_entry = m_pVideoTimeEntry;

			if( m_VideoFrameCount == 1 )
			{
				m_pVideoTimeEntry[m_VideoFrameCount - 1].sample_delta = hton_u32(40);

			}else
			{
				m_pVideoTimeEntry[m_VideoFrameCount - 1].sample_delta = m_pVideoTimeEntry[m_VideoFrameCount - 2].sample_delta;
			}

		}else
		{
			total_entry_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * m_AudioFrameCount;
			time_to_sample_box.entry_count = m_AudioFrameCount;
			time_to_sample_box.p_entry = m_pAudioTimeEntry;

			if( m_AudioFrameCount == 1 )
			{
				m_pAudioTimeEntry[m_AudioFrameCount - 1].sample_delta = hton_u32(48000 / 1024);

			}else
			{
				m_pAudioTimeEntry[m_AudioFrameCount - 1].sample_delta = m_pAudioTimeEntry[m_AudioFrameCount - 2].sample_delta;
			}
		}

		mux_size = buf_left_size;
		ret = mp4_make_time_to_sample_box(m_Muxer, &time_to_sample_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		//total_entry_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE;
		//ret = gen_file_write(m_hFile, (int8*)&time_to_sample_entry, total_entry_size, &write_size);
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(m_hFile, (int8*)m_pVideoTimeEntry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(m_hFile, (int8*)m_pAudioTimeEntry, total_entry_size, &write_size);
		}
		m_TotalWriteSize += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** sample size box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * m_VideoFrameCount;
			sample_size_box.sample_count = m_VideoFrameCount;

		}else
		{
			total_entry_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * m_AudioFrameCount;
			sample_size_box.sample_count = m_AudioFrameCount;
		}

		sample_size_box.full_box.box.size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE + total_entry_size;
		sample_size_box.sample_size = 0x0;

		mux_size = buf_left_size;
		ret = mp4_make_sample_size_box(m_Muxer, &sample_size_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(m_hFile, (int8*)m_pVideoSizeEntry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(m_hFile, (int8*)m_pAudioSizeEntry, total_entry_size, &write_size);
		}

		m_TotalWriteSize += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** sample to chunk box */
		sample_to_chunk_entry.first_chunk = hton_u32(0x1);
		sample_to_chunk_entry.sample_per_chunk = hton_u32(0x1);
		sample_to_chunk_entry.sample_desc_index = hton_u32(0x1);

		sample_to_chunk_box.entry_count = 1;
		sample_to_chunk_box.p_entry = &sample_to_chunk_entry;

		mux_size = buf_left_size;
		ret = mp4_make_sample_to_chunk_box(m_Muxer, &sample_to_chunk_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		total_entry_size = MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
		ret = gen_file_write(m_hFile, (int8*)&sample_to_chunk_entry, total_entry_size, &write_size);
		m_TotalWriteSize += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** chunk offset box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_CHUNK_OFFSET_ENTRY_SIZE * m_VideoFrameCount;
			chunk_offset_box.entry_count = m_VideoFrameCount;

		}else
		{
			total_entry_size = MP4_CHUNK_OFFSET_ENTRY_SIZE * m_AudioFrameCount;
			chunk_offset_box.entry_count = m_AudioFrameCount;
		}

		chunk_offset_box.full_box.box.size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE + total_entry_size;

		mux_size = buf_left_size;
		ret = mp4_make_chunk_offset_box(m_Muxer, &chunk_offset_box, p_uint8, &mux_size);
		ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(m_hFile, (int8*)m_pVideoOffsetEntry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(m_hFile, (int8*)m_pAudioOffsetEntry, total_entry_size, &write_size);
		}

		m_TotalWriteSize += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** sync sample box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_SYNC_SAMPLE_ENTRY_SIZE * m_VideoSyncEntryCount;
			sync_sample_box.entry_count = m_VideoSyncEntryCount;

			sync_sample_box.full_box.box.size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE + total_entry_size;

			mux_size = buf_left_size;
			ret = mp4_make_sync_sample_box(m_Muxer, &sync_sample_box, p_uint8, &mux_size);
			ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
			m_TotalWriteSize += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

			ret = gen_file_write(m_hFile, (int8*)m_pVideoSyncEntry, total_entry_size, &write_size);
			m_TotalWriteSize += total_entry_size;
			sample_table_box_size += total_entry_size;
			media_info_box_size += total_entry_size;
			media_box_size += total_entry_size;
			track_box_size += total_entry_size;
			total_mux_size += total_entry_size;
		}

		/** rewrite sample table box size */
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, sample_table_box_offset, NULL);

		sample_table_box_size = hton_u32(sample_table_box_size);
		ret = gen_file_write(m_hFile, (int8*)&sample_table_box_size, sizeof(sample_table_box_size), &write_size);

		/** rewrite media info box size */
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, media_info_box_offset, NULL);

		media_info_box_size = hton_u32(media_info_box_size);
		ret = gen_file_write(m_hFile, (int8*)&media_info_box_size, sizeof(media_info_box_size), &write_size);

		/** rewrite media box size */
		//file_pos.QuadPart = media_box_offset;
		//ret = SetFilePointerEx(m_hFile, file_pos, NULL, FILE_BEGIN);
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, media_box_offset, NULL);

		media_box_size = hton_u32(media_box_size);
		ret = gen_file_write(m_hFile, (int8*)&media_box_size, sizeof(media_box_size), &write_size);

		/** rewrite track box size */
		//file_pos.QuadPart = track_box_offset;
		//ret = SetFilePointerEx(m_hFile, file_pos, NULL, FILE_BEGIN);
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, track_box_offset, NULL);

		track_box_size = hton_u32(track_box_size);
		ret = gen_file_write(m_hFile, (int8*)&track_box_size, sizeof(track_box_size), &write_size);

		//file_pos.QuadPart = 0;
		//ret = SetFilePointerEx(m_hFile, file_pos, NULL, FILE_END);
		ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_END, 0, NULL);
	}

	/** rewrite movie box size */
	//file_pos.QuadPart = movie_box_offset;
	//ret = SetFilePointerEx(m_hFile, file_pos, NULL, FILE_BEGIN);
	ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, movie_box_offset, NULL);

	total_mux_size = hton_u32(total_mux_size);
	ret = gen_file_write(m_hFile, (int8*)&total_mux_size, sizeof(total_mux_size), &write_size);

	log_debug(_T("[CMp4FileWriter::Stop] total write %u bytes\n"), m_TotalWriteSize);

	gen_file_close(m_hFile);

FIN:
	m_bStart = 0;

	return 0;
}

int32 CMp4FileWriter::InputFrame(MEDIA_FRAME *p_frame)
{
	uint32 uint32_data;
	uint32 write_size;

	H264_NALU_PACK nalu_pack;
	uint8* p_uint8 = NULL;
	int32  parse_size;
	int32  total_parse_size;
	int32  left_size;
	int32  nalu_size;

	int32 ret;

	if( p_frame == NULL )
	{
		return -1;
	}

	if( !m_bInit )
	{
		return -1;
	}

	if( !m_bStart )
	{
		return -1;
	}

	if( !m_bFindKeyFrame )
	{
		if( p_frame->type == MP_DATA_TYPE_VIDEO && p_frame->param.video.frame_type == MP_VIDEO_I_FRAME )
		{
			m_bFindKeyFrame = 1;

		}else
		{
			return 0;
		}
	}

	switch( p_frame->type )
	{
	case MP_DATA_TYPE_VIDEO:
		{
			if( !m_bEnableVideo )
			{
				return 0;
			}

			if( !m_bHasVideo )
			{
				m_bHasVideo = 1;

				m_Track[m_TrackNum] = MP4_TRACK_TYPE_VIDEO;
				m_TrackNum++;
				memcpy(&m_VideoFrame, p_frame, sizeof(MEDIA_FRAME));

				switch( m_VideoFrame.param.video.algo )
				{
				case MP_VIDEO_MPEG2:
					{
						//m_VideoFourcc = MAKE_FOURCC('m','p','g','v');
						m_VideoFourcc = MAKE_FOURCC('M','P','E','G');
					}
					break;

				case MP_VIDEO_MPEG4:
					{
						//m_VideoFourcc = MAKE_FOURCC('m','p','4','v');
						m_VideoFourcc = MAKE_FOURCC('X','V','I','D');
					}
					break;

				case MP_VIDEO_AVC:
					{
						m_VideoFourcc = MAKE_FOURCC('a','v','c','1');

						h264_nalu_pack_init(&nalu_pack);
						p_uint8 = (uint8*)p_frame->p_frame;
						total_parse_size = 0;
						left_size = p_frame->frame_size;

						//m_bGetSps = 0;
						//m_bGetPps = 0;
						while( 1 )
						{
							ret = h264_parse_nalu(&nalu_pack, p_uint8, left_size, &parse_size);
							p_uint8 += parse_size;
							total_parse_size += parse_size;
							left_size -= parse_size;

							if( ret )
							{
								break;
							}

							nalu_size = parse_size - H264_BS_HEADER_SIZE;
							if( nalu_pack.nalu_type == H264_NALU_SPS )
							{
								if( nalu_size <= H264_MAX_SPS_NALU_SIZE )
								{
									if( !m_bGetSps )
									{
										m_bGetSps = 1;
										m_Profile = nalu_pack.sps.profile;
										m_ProfileCompat = 0xD0;
										m_Level = nalu_pack.sps.level;
										memcpy(m_pSPSBuf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										m_SPSNaluSize = nalu_size;
									}
								}

							}else if( nalu_pack.nalu_type == H264_NALU_PPS )
							{
								if( nalu_size <= H264_MAX_PPS_NALU_SIZE )
								{
									if( !m_bGetPps )
									{
										m_bGetPps = 1;
										memcpy(m_pPPSBuf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										m_PPSNaluSize = nalu_size;
									}
								}
							}
						}
					}
					break;

				case MP_VIDEO_H264:
					{
						m_VideoFourcc = MAKE_FOURCC('h','2','6','4');
					}
					break;

				default:
					{
						m_VideoFourcc = MAKE_FOURCC('m','p','g','v');
					}
					break;
				}
			}

			if( m_VideoFrameCount < MP4_FILE_WRITER_MAX_VIDEO_INDEX )
			{
				m_pVideoTimeEntry[m_VideoFrameCount].sample_count = hton_u32(0x1);
				if( m_VideoFrameCount > 0 )
				{
					uint32_data = p_frame->frame_ts - m_TotalVideoElapseTime;
					m_pVideoTimeEntry[m_VideoFrameCount - 1].sample_delta = hton_u32(uint32_data);
				}

				m_pVideoSizeEntry[m_VideoFrameCount].entry_size = hton_u32(p_frame->frame_size);
				m_pVideoOffsetEntry[m_VideoFrameCount].chunk_offset = hton_u32(m_TotalWriteSize);

				if( p_frame->param.video.frame_type == MP_VIDEO_I_FRAME
					&& m_VideoSyncEntryCount < MP4_FILE_WRITER_MAX_VIDEO_SYNC_ENTRY_NUM )
				{
					m_pVideoSyncEntry[m_VideoSyncEntryCount].sample_number = hton_u32(m_VideoFrameCount + 1);
					m_VideoSyncEntryCount++;
				}
			}

			m_VideoFrameCount++;
			m_TotalVideoElapseTime = p_frame->frame_ts;
			m_TotalVideoFrameSize += p_frame->frame_size;
		}
		break;

	case MP_DATA_TYPE_AUDIO:
		{
			if( !m_bEnableAudio )
			{
				return 0;
			}

			if( !m_bHasAudio )
			{
				m_bHasAudio = 1;

				m_Track[m_TrackNum] = MP4_TRACK_TYPE_AUDIO;
				m_TrackNum++;
				memcpy(&m_AudioFrame, p_frame, sizeof(MEDIA_FRAME));

				switch( m_AudioFrame.param.audio.algo )
				{
				case MP_AUDIO_PCM:
					{
						m_AudioFourcc = MAKE_FOURCC('l','p','c','m');
						m_AudioSamplePerFrame = 1024;
					}
					break;

				case MP_AUDIO_ADPCM:
					{
						m_AudioFourcc = MAKE_FOURCC('a','p','c','m');
						m_AudioSamplePerFrame = 1024;
					}
					break;

				case MP_AUDIO_G711A:
					{
						m_AudioFourcc = MAKE_FOURCC('a','l','a','w');
						//m_AudioFourcc = MAKE_FOURCC(0x06,0x00,0x00,0x00);
						m_AudioSamplePerFrame = 240;
					}
					break;

				case MP_AUDIO_G711U:
					{
						m_AudioFourcc = MAKE_FOURCC('u','l','a','w');
						m_AudioSamplePerFrame = 240;
					}
					break;

				case MP_AUDIO_MP1:
				case MP_AUDIO_MP2:
				case MP_AUDIO_MP3:
					{
						//m_AudioFourcc = MAKE_FOURCC(0x50,0x00,0x00,0x00);
						m_AudioFourcc = MAKE_FOURCC('m','p','g','a');
						m_AudioSamplePerFrame = 1152;
					}
					break;

				case MP_AUDIO_AAC:
				case MP_AUDIO_AAC_LC:
					{
						m_AudioFourcc = MAKE_FOURCC('m','p','4','a');
						m_AudioSamplePerFrame = 1024 * m_TimeScale / m_AudioFrame.param.audio.sample_rate;
						//m_AudioSamplePerFrame = 1024;
					}
					break;

				default:
					{
						m_AudioFourcc = MAKE_FOURCC('m','p','g','a');
						m_AudioSamplePerFrame = 1024;
					}
					break;
				}
			}

			if( m_AudioFrameCount < MP4_FILE_WRITER_MAX_AUDIO_INDEX )
			{
				m_pAudioTimeEntry[m_AudioFrameCount].sample_count = hton_u32(0x1);
				if( m_AudioFrameCount > 0 )
				{
					uint32_data = p_frame->frame_ts - m_TotalAudioElapseTime;
					m_pAudioTimeEntry[m_AudioFrameCount - 1].sample_delta = hton_u32(uint32_data);
				}

				m_pAudioSizeEntry[m_AudioFrameCount].entry_size = hton_u32(p_frame->frame_size);
				m_pAudioOffsetEntry[m_AudioFrameCount].chunk_offset  = hton_u32(m_TotalWriteSize);
			}

			m_AudioFrameCount++;
			//m_TotalAudioElapseTime += m_AudioSamplePerFrame;
			m_TotalAudioElapseTime = p_frame->frame_ts;
			m_TotalAudioFrameSize += p_frame->frame_size;
		}
		break;

	default:
		{
			return -3;
		}
	}

	/** rewrite media data box size */
	m_TotalFrameSize += p_frame->frame_size;

	ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_BEGIN, m_MediaDataOffset, NULL);

	uint32_data = m_TotalFrameSize + m_MediaDataBoxSize;
	uint32_data = hton_u32(uint32_data);
	ret = gen_file_write(m_hFile, (int8*)&uint32_data, sizeof(uint32_data), &write_size);

	ret = gen_file_seek(m_hFile, GEN_FILE_SEEK_END, 0, NULL);

	/** write frame */
	ret = gen_file_write(m_hFile, p_frame->p_frame, p_frame->frame_size, &write_size);
	m_TotalWriteSize += p_frame->frame_size;

	return 0;
}

int32 CMp4FileWriter::SetAvcDecConfig(uint8* p_buf, int32 buf_size)
{
	AVC_DEC_CONFIG_RECORD dec_config;
	AVC_SPS_RECORD sps_record[MP4_FILE_WRITER_MAX_SPS_RECORD];
	AVC_PPS_RECORD pps_record[MP4_FILE_WRITER_MAX_PPS_RECORD];

	uint32 left_size;
	uint32 demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_buf == NULL )
	{
		return -1;
	}

	if( !m_bInit )
	{
		return -3;
	}

	left_size = buf_size;
	p_uint8 = p_buf;

	dec_config.sps_count = MP4_FILE_WRITER_MAX_SPS_RECORD;
	dec_config.p_sps = sps_record;
	dec_config.pps_count = MP4_FILE_WRITER_MAX_PPS_RECORD;
	dec_config.p_pps = pps_record;
	demux_size = left_size;
	ret = mp4_avc_dec_config_record_demux(&dec_config, p_uint8, &demux_size);
	if( ret == 0 )
	{
		m_bGetSps = 1;
		m_Profile = dec_config.profile;
		m_ProfileCompat = dec_config.profile_compat;
		m_Level = dec_config.level;
		memcpy(m_pSPSBuf, dec_config.p_sps[0].p_nalu, dec_config.p_sps[0].nalu_len);
		m_SPSNaluSize = dec_config.p_sps[0].nalu_len;

		m_bGetPps = 1;
		memcpy(m_pPPSBuf, dec_config.p_pps[0].p_nalu, dec_config.p_pps[0].nalu_len);
		m_PPSNaluSize = dec_config.p_pps[0].nalu_len;
	}

	return 0;
}

int32 CMp4FileWriter::WriteFileTypeBox()
{
	ISO_FILE_TYPE_BOX  file_type_box;
	ISO_MEDIA_DATA_BOX media_data_box;

	uint32 brand[2];

	uint8*  p_uint8 = NULL;
	int32   buf_left_size;
	int32   mux_size;

	uint32  write_size;

	int32 ret;

	p_uint8 = m_pWriteBuf;
	buf_left_size = m_WriteBufSize;

	/** file type box */
	file_type_box.major_brand = MP4_BRAND_ISOM;
	file_type_box.minor_version = 0x0;
	brand[0] = hton_u32(MP4_BRAND_MP41);
	file_type_box.p_brand = brand;
	file_type_box.brand_count = 1;

	mux_size = buf_left_size;
	ret = mp4_make_file_type_box(m_Muxer, &file_type_box, p_uint8, &mux_size);
	ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
	//p_uint8 += mux_size;
	//buf_left_size -= mux_size;
	m_MediaDataOffset = mux_size;
	m_TotalWriteSize += mux_size;

	/** media data box */
	media_data_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_make_media_data_box(m_Muxer, &media_data_box, p_uint8, &mux_size);
	ret = gen_file_write(m_hFile, (int8*)p_uint8, mux_size, &write_size);
	//p_uint8 += mux_size;
	//buf_left_size -= mux_size;
	m_MediaDataBoxSize = mux_size;
	m_TotalWriteSize += mux_size;

	return 0;
}

int32 CMp4FileWriter::WriteMovieBox()
{
	return 0;
}

int32 CMp4FileWriter::MakeAudioDecSpecInfo(uint8* p_buf, int32* p_buf_size)
{
	BIT_STREAM_WRITER bs;
	//uint32 value;

	int32  left_size = *p_buf_size;
	int32  total_mux_size = 0;

	//int32  ret;

	bs_writer_init(&bs, p_buf, left_size, 8);

	/** audio object type, 5 bit */
	switch( m_AudioFrame.param.audio.algo )
	{
	case MP_AUDIO_AAC_LC:
		{
			/** LC */
			bs_write_bits(&bs, MPEG4_AUDIO_OBJECT_TYPE_AAC_LC, 5);
		}
		break;

	default:
		{
			/** LC */
			bs_write_bits(&bs, MPEG4_AUDIO_OBJECT_TYPE_AAC_LC, 5);
		}
		break;
	}

	/** sample freq index, 4 bit */
	switch( m_AudioFrame.param.audio.sample_rate )
	{
	case 96000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_96000, 4);
		}
		break;

	case 64000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_64000, 4);
		}
		break;

	case 48000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_48000, 4);
		}
		break;

	case 44100:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_44100, 4);
		}
		break;

	case 32000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_32000, 4);
		}
		break;

	case 24000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_24000, 4);
		}
		break;

	case 22050:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_22050, 4);
		}
		break;

	case 16000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_16000, 4);
		}
		break;

	case 12000:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_12000, 4);
		}
		break;

	case 11025:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_11025, 4);
		}
		break;

	default:
		{
			bs_write_bits(&bs, MPEG4_AAC_SAMPLE_FREQ_INDEX_48000, 4);
		}
		break;
	}

	/** chn config, 4 bit */
	switch( m_AudioFrame.param.audio.chn )
	{
	case 1:
		{
			bs_write_bits(&bs, MPEG4_AAC_CHN_CONFIG_1, 4);
		}
		break;

	case 2:
		{
			bs_write_bits(&bs, MPEG4_AAC_CHN_CONFIG_2, 4);
		}
		break;

	default:
		{
			bs_write_bits(&bs, MPEG4_AAC_CHN_CONFIG_2, 4);
		}
		break;
	}

	/** frame length flag, 1 bit */
	bs_write_bits(&bs, 0x0, 1);

	/** depend on core coder, 1 bit */
	bs_write_bits(&bs, 0x0, 1);

	/** extension flag, 1 bit */
	bs_write_bits(&bs, 0x0, 1);

	bs_writer_get_use_bytes(&bs, &total_mux_size);

	*p_buf_size = total_mux_size;

	return 0;
}
