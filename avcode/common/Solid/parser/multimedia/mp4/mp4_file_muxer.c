

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <public/gen_fourcc.h>
#include <public/gen_endian.h>
#include <public/gen_deque.h>
#include <public/bit_stream_writer.h>
#include <public/gen_file_const.h>
#include <public/gen_file.h>
#include <public/gen_error.h>
#include <Solid/public/sp_video_const.h>
#include <Solid/public/sp_video_format.h>
#include <Solid/public/sp_audio_format.h>
#include <Solid/public/sp_media_format.h>


#include "../../mpeg4/mpeg4_public.h"
#include "../../mpeg4/mpeg4_aac_const.h"
#include "../../h264/h264_public.h"
#include "../../h264/h264_nalu_demux.h"

#include "../iso/iso_media_file.h"
#include "mp4_avc_demux.h"
#include "mp4_mux.h"
#include "mp4_muxer.h"
#include "mp4_file_const.h"
#include "mp4_file_muxer.h"



/** max gop size */
#define MP4_FILE_MUXER_MAX_GOP_SIZE                        (1024)

/** max video data deque size */
#define MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE           (MP4_FILE_MUXER_MAX_GOP_SIZE)
/** video frag entry buf size */
#define MP4_FILE_MUXER_VIDEO_FRAG_ENTRY_BUF_SIZE           (MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE * sizeof(ISO_TRACK_FRAGMENT_SAMPLE))
/** max video data buf size */
#define MP4_FILE_MUXER_MAX_VIDEO_DATA_BUF_SIZE             (32 * 1024 * 1024)
/** max video index */
#define MP4_FILE_MUXER_MAX_VIDEO_INDEX                     (512 * 1024)
/** video time entry buf size */
#define MP4_FILE_MUXER_VIDEO_TIME_ENTRY_BUF_SIZE           (MP4_FILE_MUXER_MAX_VIDEO_INDEX * sizeof(ISO_TIME_TO_SAMPLE_ENTRY))
/** video sample size entry buf size */
#define MP4_FILE_MUXER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE    (MP4_FILE_MUXER_MAX_VIDEO_INDEX * sizeof(ISO_SAMPLE_SIZE_ENTRY))
/** video chunk offset entry buf size */
#define MP4_FILE_MUXER_VIDEO_CHUNK_OFFSET_ENTRY_BUF_SIZE   (MP4_FILE_MUXER_MAX_VIDEO_INDEX * sizeof(ISO_CHUNK_OFFSET_ENTRY))
/** max video sync entry num */
#define MP4_FILE_MUXER_MAX_VIDEO_SYNC_ENTRY_NUM            (256 * 1024)
/** video sync entry buf size */
#define MP4_FILE_MUXER_VIDEO_SYNC_ENTRY_BUF_SIZE           (MP4_FILE_MUXER_MAX_VIDEO_SYNC_ENTRY_NUM * sizeof(ISO_SYNC_SAMPLE_ENTRY))

/** max audio data deque size */
#define MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE           (16 * 1024)
/** audio frag entry buf size */
#define MP4_FILE_MUXER_AUDIO_FRAG_ENTRY_BUF_SIZE           (MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE * sizeof(ISO_TRACK_FRAGMENT_SAMPLE))
/** max audio data buf size */
#define MP4_FILE_MUXER_MAX_AUDIO_DATA_BUF_SIZE             (4 * 1024 * 1024)
/** max audio index */
#define MP4_FILE_MUXER_MAX_AUDIO_INDEX                     (1024 * 1024)
/** audio time entry buf size */
#define MP4_FILE_MUXER_AUDIO_TIME_ENTRY_BUF_SIZE           (MP4_FILE_MUXER_MAX_AUDIO_INDEX * sizeof(ISO_TIME_TO_SAMPLE_ENTRY))
/** audio sample size entry buf size */
#define MP4_FILE_MUXER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE    (MP4_FILE_MUXER_MAX_AUDIO_INDEX * sizeof(ISO_SAMPLE_SIZE_ENTRY))
/** chunk offset entry buf size */
#define MP4_FILE_MUXER_AUDIO_CHUNK_OFFSET_ENTRY_BUF_SIZE   (MP4_FILE_MUXER_MAX_AUDIO_INDEX * sizeof(ISO_CHUNK_OFFSET_ENTRY))

/** write buf size */
#define MP4_FILE_MUXER_WRITE_BUF_SIZE                      (256 * 1024)
/** max avc dec config buf size */
#define MP4_FILE_MUXER_MAX_AVC_DEC_CONFIG_BUF_SIZE         (1024)
/** padding buf size */
#define MP4_FILE_MUXER_PADDING_BUF_SIZE                    (512)
/** max string size */
#define MP4_FILE_MUXER_MAX_STR_SIZE                        (255)
/** max name size */
#define MP4_FILE_MUXER_MAX_NAME_SIZE                       (32)
/** dec spec info buf size */
#define MP4_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE               (32)
/** logo */
#define MP4_FILE_MUXER_LOGO_NAME                           "robert"

/** max track num */
#define MP4_FILE_MUXER_MAX_TRACK_NUM                       (2)
/** video track id */
#define MP4_FILE_MUXER_VIDEO_TRACK_ID                      (1)
/** audio track id */
#define MP4_FILE_MUXER_AUDIO_TRACK_ID                      (2)
/** max track id */
#define MP4_FILE_MUXER_MAX_TRACK_ID                        (MP4_FILE_MUXER_AUDIO_TRACK_ID)
/** max sps record */
#define MP4_FILE_MUXER_MAX_SPS_RECORD                      (2)
/** max pps record */
#define MP4_FILE_MUXER_MAX_PPS_RECORD                      (2)

/** uni time scale */
#define MP4_FILE_MUXER_UNI_TIME_SCALE                      (1000)
/** enable exception */
#define MP4_FILE_MUXER_ENABLE_EXCEPTION                    (1)

/** enable debug */
#define MP4_FILE_MUXER_ENABLE_DEBUG                        (0)

/** video */
typedef struct tag_mp4_file_video
{
	/** param */
	SP_VIDEO_FRAME param;
	/** fourcc */
	uint32 fourcc;

	/** init mp4 */
	int32  b_init_mp4;
	/** init frag mp4 */
	int32  b_init_frag_mp4;

	/** first frame */
	int32  b_first_frame;
	/** get key frame */
	int32  b_get_key_frame;

	/** profile */
	int32  profile;
	/** profile compat */
	int32  profile_compat;
	/** level */
	int32  level;

	/** get avc dec config */
	int32  b_get_avc_dec_config;
	/** avc dec config buf */
	uint8* p_avc_dec_config_buf;
	/** avc dec config buf size */
	int32  avc_dec_config_buf_size;
	/** avc dec config buf use size */
	int32  avc_dec_config_buf_use_size;

	/** get sps */
	int32  b_get_sps;
	/** sps */
	uint8* p_sps_buf;
	/** sps buf size */
	int32  sps_buf_size;
	/** sps nalu size */
	int32  sps_nalu_size;

	/** get pps */
	int32  b_get_pps;
	/** pps */
	uint8* p_pps_buf;
	/** pps buf size */
	int32  pps_buf_size;
	/** pps nalu size */
	int32  pps_nalu_size;

	/** frame count */
	int32  frame_count;
	/** total frame size */
	uint32 total_frame_size;
	/** sync frame ts, unit: timescale */
	int64  sync_frame_ts;
	/** last frame ts, unit: timescale */
	int64  last_frame_ts;
	/** total elapse time, unit: timescale */
	int64  total_elapse_time;
	/** total elapse time, unit: ms */
	int64  total_elapse_time_ms;

	/** time to sample entry */
	ISO_TIME_TO_SAMPLE_ENTRY* p_time_entry;
	/** video sample size entry */
	ISO_SAMPLE_SIZE_ENTRY* p_size_entry;
	/** video chunk offset entry */
	ISO_CHUNK_OFFSET_ENTRY* p_offset_entry;
	/** video sync sample entry */
	ISO_SYNC_SAMPLE_ENTRY* p_sync_entry;
	/** video sync sample entry count */
	int32  sync_entry_count;

	/** data buf */
	uint8*  p_data_buf;
	/** data buf size */
	uint32  data_buf_size;
	/** data buf use size */
	uint32  data_buf_use_size;
	/** data buf left size */
	uint32  data_buf_left_size;

	/** data deque */
	GEN_DEQUE data_deque;
	/** data deque total size */
	int32   data_deque_total_size;

	/** frag sample entry */
	ISO_TRACK_FRAGMENT_SAMPLE* p_frag_sample_entry;
	/** frag sample entry count */
	int32   frag_sample_entry_count;

} MP4_FILE_VIDEO, *PMP4_FILE_VIDEO;


/** audio */
typedef struct tag_mp4_file_audio
{
	/** param */
	SP_AUDIO_FRAME param;
	/** fourcc */
	uint32 fourcc;

	/** init mp4 */
	int32  b_init_mp4;
	/** init frag mp4 */
	int32  b_init_frag_mp4;

	/** first frame */
	int32  b_first_frame;

	/** frame count */
	int32  frame_count;
	/** total frame size */
	uint32 total_frame_size;
	/** sync frame ts, unit: timescale */
	int64  sync_frame_ts;
	/** last frame ts, unit: timescale */
	int64  last_frame_ts;
	/** total elapse time, unit: timescale */
	int64  total_elapse_time;
	/** total elapse time, unit: ms */
	int64  total_elapse_time_ms;
	/** total sample count */
	int64  total_sample_count;

	/** time to sample entry */
	ISO_TIME_TO_SAMPLE_ENTRY* p_time_entry;
	/** sample size entry */
	ISO_SAMPLE_SIZE_ENTRY* p_size_entry;
	/** sample size entry count */
	int32  size_entry_count;
	/** chunk offset entry */
	ISO_CHUNK_OFFSET_ENTRY* p_offset_entry;
	/** chunk offset entry count */
	int32  offset_entry_count;

	/** data buf */
	uint8*  p_data_buf;
	/** data buf size */
	uint32  data_buf_size;
	/** data buf use size */
	uint32  data_buf_use_size;
	/** data buf left size */
	uint32  data_buf_left_size;

	/** data deque */
	GEN_DEQUE data_deque;
	/** data deque total size */
	int32   data_deque_total_size;

	/** frag sample entry */
	ISO_TRACK_FRAGMENT_SAMPLE* p_frag_sample_entry;
	/** frag sample entry count */
	int32   frag_sample_entry_count;

} MP4_FILE_AUDIO, *PMP4_FILE_AUDIO;


/** mp4 file muxer */
typedef struct tag_mp4_file_muxer
{
	/** init */
	int32  b_init;

	/** file type */
	int32  file_type;

	/** callback */
	MP4_FILE_MUXER_CALLBACK callback;

	/** enable video */
	int32  b_enable_video;
	/** enable audio */
	int32  b_enable_audio;
	/** reserve audio */
	int32  b_reserve_audio;
	/** force sync */
	int32  b_force_sync;

	/** timescale */
	uint32 time_scale;

	/** start */
	int32  b_start;
	/** first fragment */
	int32  b_first_fragment;
	/** write movie box */
	int32  b_write_movie_box;

	/** get video */
	int32  b_get_video;
	/** get audio */
	int32  b_get_audio;
	/** track count */
	int32  track_count;
	/** track */
	int32  track[MP4_FILE_MUXER_MAX_TRACK_NUM];

	/** video */
	MP4_FILE_VIDEO video;
	/** audio */
	MP4_FILE_AUDIO audio;

	/** total frame size */
	uint32 total_frame_size;

	/** gop */
	int32  b_gop;
	/** gop count */
	uint32 gop_count;
	/** write gop count */
	uint32 write_gop_count;
	/** total gop count */
	uint32 total_gop_count;
	/** last gop ts, unit: timescale */
	int64  last_gop_ts;

	/** write buf */
	uint8* p_write_buf;
	/** write buf size */
	int32  write_buf_size;
	/** write buf use size */
	int32  write_buf_use_size;
	/** write buf left size */
	int32  write_buf_left_size;

	/** file */
	GEN_FILE_H file;
	/** total write size */
	uint32 total_write_size;
	/** media data offset */
	uint32 media_data_offset;
	/** media data box size */
	uint32 media_data_box_size;

#if MP4_FILE_MUXER_ENABLE_DEBUG
	FILE* p_test_file;
#endif

} MP4_FILE_MUXER, *PMP4_FILE_MUXER;


/////////////////////////////// Inner  Interface //////////////////////////////
/**  
 * @brief init video avc buf
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_avc_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	p_video->p_avc_dec_config_buf = (uint8*)malloc(MP4_FILE_MUXER_MAX_AVC_DEC_CONFIG_BUF_SIZE);
	if( p_video->p_avc_dec_config_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_video->avc_dec_config_buf_size = MP4_FILE_MUXER_MAX_AVC_DEC_CONFIG_BUF_SIZE;

	p_video->p_sps_buf = (uint8*)malloc(H264_MAX_SPS_NALU_SIZE);
	if( p_video->p_sps_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_video->sps_buf_size = H264_MAX_SPS_NALU_SIZE;

	p_video->p_pps_buf = (uint8*)malloc(H264_MAX_PPS_NALU_SIZE);
	if( p_video->p_pps_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_video->pps_buf_size = H264_MAX_PPS_NALU_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief deinit video avc buf
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video_avc_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	if( p_video->p_avc_dec_config_buf )
	{
		free(p_video->p_avc_dec_config_buf);
		p_video->p_avc_dec_config_buf = NULL;
	}

	if( p_video->p_sps_buf )
	{
		free(p_video->p_sps_buf);
		p_video->p_sps_buf = NULL;
	}

	if( p_video->p_pps_buf )
	{
		free(p_video->p_pps_buf);
		p_video->p_pps_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init video entry
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_entry(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	p_video->p_time_entry = (ISO_TIME_TO_SAMPLE_ENTRY*)malloc(MP4_FILE_MUXER_VIDEO_TIME_ENTRY_BUF_SIZE);
	if( p_video->p_time_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_video->p_size_entry = (ISO_SAMPLE_SIZE_ENTRY*)malloc(MP4_FILE_MUXER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( p_video->p_size_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_video->p_offset_entry = (ISO_CHUNK_OFFSET_ENTRY*)malloc(MP4_FILE_MUXER_VIDEO_CHUNK_OFFSET_ENTRY_BUF_SIZE);
	if( p_video->p_offset_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_video->p_sync_entry = (ISO_SYNC_SAMPLE_ENTRY*)malloc(MP4_FILE_MUXER_VIDEO_SYNC_ENTRY_BUF_SIZE);
	if( p_video->p_sync_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_video->sync_entry_count = 0;

	return GEN_S_OK;
}

/**  
 * @brief deinit video entry
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video_entry(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	if( p_video->p_time_entry )
	{
		free(p_video->p_time_entry);
		p_video->p_time_entry = NULL;
	}

	if( p_video->p_size_entry )
	{
		free(p_video->p_size_entry);
		p_video->p_size_entry = NULL;
	}

	if( p_video->p_offset_entry )
	{
		free(p_video->p_offset_entry);
		p_video->p_offset_entry = NULL;
	}

	if( p_video->p_sync_entry )
	{
		free(p_video->p_sync_entry);
		p_video->p_sync_entry = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init video data buf
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_data_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	p_video->p_data_buf = (uint8*)malloc(MP4_FILE_MUXER_MAX_VIDEO_DATA_BUF_SIZE);
	if( p_video->p_data_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_video->data_buf_size = MP4_FILE_MUXER_MAX_VIDEO_DATA_BUF_SIZE;
	p_video->data_buf_use_size = 0;
	p_video->data_buf_left_size = p_video->data_buf_size;

	return GEN_S_OK;
}

/**  
 * @brief deinit video data buf
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video_data_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	if( p_video->p_data_buf )
	{
		free(p_video->p_data_buf);
		p_video->p_data_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init video deque
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	//gen_deque_setup(&p_video->data_deque, NULL);
	gen_deque_init(&p_video->data_deque);
	gen_deque_set_array_param(&p_video->data_deque, MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE, MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE);
	p_video->data_deque_total_size = MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief deinit video deque
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	SP_MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	//if( m_pVideoDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(&p_video->data_deque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(&p_video->data_deque);
			free(p_frame);
			p_frame = NULL;
		}

		gen_deque_cleanup(&p_video->data_deque);
		//gen_deque_destroy(m_pVideoDeque);
		//m_pVideoDeque = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief clear video deque
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_clear_video_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	SP_MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	while( 1 )
	{
		ret = gen_deque_get_front_data(&p_video->data_deque, (void**)&p_frame);
		if( ret )
		{
			break;
		}

		gen_deque_pop_front(&p_video->data_deque);
		free(p_frame);
		p_frame = NULL;
	}

	//gen_deque_cleanup(&p_video->data_deque);

	return GEN_S_OK;
}

/**  
 * @brief init video frag
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_frag(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	p_video->p_frag_sample_entry = (ISO_TRACK_FRAGMENT_SAMPLE*)malloc( MP4_FILE_MUXER_VIDEO_FRAG_ENTRY_BUF_SIZE );
	if( p_video->p_frag_sample_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit video frag
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video_frag(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	if( p_video->p_frag_sample_entry )
	{
		free(p_video->p_frag_sample_entry);
		p_video->p_frag_sample_entry = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init video
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	int32  ret;

	p_video->b_init_mp4 = 0;
	p_video->b_init_frag_mp4 = 0;

	ret = mp4_file_muxer_init_video_avc_buf(p_muxer, p_video);
	if( ret )
	{
		return ret;
	}

	gen_deque_setup(&p_video->data_deque, NULL);

#if 0
	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */
		ret = mp4_file_muxer_init_video_entry(p_muxer, p_video);

	}else
	{
		/** frag mp4 */
		ret = mp4_file_muxer_init_video_data_buf(p_muxer, p_video);
		ret = mp4_file_muxer_init_video_deque(p_muxer, p_video);
		ret = mp4_file_muxer_init_video_frag(p_muxer, p_video);
	}
#endif

	return GEN_S_OK;
}

/**  
 * @brief deinit video
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_video(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	mp4_file_muxer_deinit_video_avc_buf(p_muxer, p_video);
	mp4_file_muxer_deinit_video_entry(p_muxer, p_video);
	mp4_file_muxer_deinit_video_data_buf(p_muxer, p_video);
	mp4_file_muxer_deinit_video_deque(p_muxer, p_video);
	mp4_file_muxer_deinit_video_frag(p_muxer, p_video);

	return GEN_S_OK;
}

/**  
 * @brief init audio entry
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio_entry(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	p_audio->p_time_entry = (ISO_TIME_TO_SAMPLE_ENTRY*)malloc(MP4_FILE_MUXER_AUDIO_TIME_ENTRY_BUF_SIZE);
	if( p_audio->p_time_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_audio->p_size_entry = (ISO_SAMPLE_SIZE_ENTRY*)malloc(MP4_FILE_MUXER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( p_audio->p_size_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_audio->p_offset_entry = (ISO_CHUNK_OFFSET_ENTRY*)malloc(MP4_FILE_MUXER_AUDIO_CHUNK_OFFSET_ENTRY_BUF_SIZE);
	if( p_audio->p_offset_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit audio entry
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_audio_entry(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	if( p_audio->p_time_entry )
	{
		free(p_audio->p_time_entry);
		p_audio->p_time_entry = NULL;
	}

	if( p_audio->p_size_entry )
	{
		free(p_audio->p_size_entry);
		p_audio->p_size_entry = NULL;
	}

	if( p_audio->p_offset_entry )
	{
		free(p_audio->p_offset_entry);
		p_audio->p_offset_entry = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init audio data buf
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio_data_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	p_audio->p_data_buf = (uint8*)malloc( MP4_FILE_MUXER_MAX_AUDIO_DATA_BUF_SIZE );
	if( p_audio->p_data_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_audio->data_buf_size = MP4_FILE_MUXER_MAX_AUDIO_DATA_BUF_SIZE;
	p_audio->data_buf_use_size = 0;
	p_audio->data_buf_left_size = p_audio->data_buf_size;

	return GEN_S_OK;
}

/**  
 * @brief deinit audio data buf
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_audio_data_buf(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	if( p_audio->p_data_buf )
	{
		free(p_audio->p_data_buf);
		p_audio->p_data_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init audio deque
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	//gen_deque_setup(&p_audio->data_deque, NULL);
	gen_deque_init(&p_audio->data_deque);
	gen_deque_set_array_param(&p_audio->data_deque, MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE, MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE);
	p_audio->data_deque_total_size = MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief deinit audio deque
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_audio_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	SP_MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	while( 1 )
	{
		ret = gen_deque_get_front_data(&p_audio->data_deque, (void**)&p_frame);
		if( ret )
		{
			break;
		}

		gen_deque_pop_front(&p_audio->data_deque);
		free(p_frame);
		p_frame = NULL;
	}

	gen_deque_cleanup(&p_audio->data_deque);

	return GEN_S_OK;
}

/**  
 * @brief clear audio deque
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_clear_audio_deque(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	SP_MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	while( 1 )
	{
		ret = gen_deque_get_front_data(&p_audio->data_deque, (void**)&p_frame);
		if( ret )
		{
			break;
		}

		gen_deque_pop_front(&p_audio->data_deque);
		free(p_frame);
		p_frame = NULL;
	}

	//gen_deque_cleanup(&p_audio->data_deque);

	return GEN_S_OK;
}

/**  
 * @brief init audio frag
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio_frag(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	p_audio->p_frag_sample_entry = (ISO_TRACK_FRAGMENT_SAMPLE*)malloc( MP4_FILE_MUXER_AUDIO_FRAG_ENTRY_BUF_SIZE );
	if( p_audio->p_frag_sample_entry == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	return GEN_S_OK;
}

/**  
 * @brief deinit audio frag
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_audio_frag(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	if( p_audio->p_frag_sample_entry )
	{
		free(p_audio->p_frag_sample_entry);
		p_audio->p_frag_sample_entry = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init audio
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	//int32  ret;

	p_audio->b_init_mp4 = 0;
	p_audio->b_init_frag_mp4 = 0;

	gen_deque_setup(&p_audio->data_deque, NULL);

#if 0
	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */
		ret = mp4_file_muxer_init_audio_entry(p_muxer, p_audio);

	}else
	{
		/** frag mp4 */
		ret = mp4_file_muxer_init_audio_data_buf(p_muxer, p_audio);
		ret = mp4_file_muxer_init_audio_deque(p_muxer, p_audio);
		ret = mp4_file_muxer_init_audio_frag(p_muxer, p_audio);
	}
#endif

	return GEN_S_OK;
}

/**  
 * @brief deinit audio
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_audio(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	mp4_file_muxer_deinit_audio_entry(p_muxer, p_audio);
	mp4_file_muxer_deinit_audio_data_buf(p_muxer, p_audio);
	mp4_file_muxer_deinit_audio_deque(p_muxer, p_audio);
	mp4_file_muxer_deinit_audio_frag(p_muxer, p_audio);

	return GEN_S_OK;
}

/**  
 * @brief init write buf
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_write_buf(MP4_FILE_MUXER* p_muxer)
{
	p_muxer->p_write_buf = (uint8*)malloc(MP4_FILE_MUXER_WRITE_BUF_SIZE);
	if( p_muxer->p_write_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	p_muxer->write_buf_size = MP4_FILE_MUXER_WRITE_BUF_SIZE;
	p_muxer->write_buf_use_size = 0;
	p_muxer->write_buf_left_size = p_muxer->write_buf_size;

	return GEN_S_OK;
}

/**  
 * @brief deinit write buf
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_write_buf(MP4_FILE_MUXER* p_muxer)
{
	if( p_muxer->p_write_buf )
	{
		free(p_muxer->p_write_buf);
		p_muxer->p_write_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init file
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_file(MP4_FILE_MUXER* p_muxer)
{
	p_muxer->file = gen_file_malloc();
	if( p_muxer->file == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}
	gen_file_init(p_muxer->file);

	return GEN_S_OK;
}

/**  
 * @brief deinit file
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_deinit_file(MP4_FILE_MUXER* p_muxer)
{
	if( p_muxer->file )
	{
		gen_file_free(p_muxer->file);
		p_muxer->file = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief init video status
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_video_status(MP4_FILE_MUXER* p_muxer, MP4_FILE_VIDEO* p_video)
{
	memset(&p_video->param, 0, sizeof(p_video->param));

	p_video->b_first_frame = 1;
	p_video->b_get_key_frame = 0;

	p_video->b_get_avc_dec_config = 0;
	p_video->b_get_sps = 0;
	p_video->b_get_pps = 0;

	p_video->frame_count = 0;
	p_video->total_frame_size = 0;
	p_video->sync_frame_ts = 0;
	p_video->last_frame_ts = 0;
	p_video->total_elapse_time = 0;
	p_video->total_elapse_time_ms = 0;

	p_video->sync_entry_count = 0;

	if( p_muxer->file_type == MP4_FILE_TYPE_FRAG_MP4 )
	{
		p_video->data_buf_use_size = 0;
		p_video->data_buf_left_size = p_video->data_buf_size;

		p_video->frag_sample_entry_count = 0;

#if MP4_FILE_MUXER_ENABLE_DEBUG
		if( p_muxer->p_test_file == NULL )
		{
			p_muxer->p_test_file = fopen("d:\\test\\test3.mp4", "wb");
		}
#endif
	}

	return GEN_S_OK;
}

/**  
 * @brief init audio status
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_audio_status(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio)
{
	memset(&p_audio->param, 0, sizeof(p_audio->param));

	p_audio->b_first_frame = 1;

	p_audio->frame_count = 0;
	p_audio->total_frame_size = 0;
	p_audio->sync_frame_ts = 0;
	p_audio->last_frame_ts = 0;
	p_audio->total_elapse_time = 0;
	p_audio->total_elapse_time_ms = 0;
	p_audio->total_sample_count = 0;

	if( p_muxer->file_type == MP4_FILE_TYPE_FRAG_MP4 )
	{
		p_audio->data_buf_use_size = 0;
		p_audio->data_buf_left_size = p_audio->data_buf_size;

		p_audio->frag_sample_entry_count = 0;
	}

	return GEN_S_OK;
}

/**  
 * @brief init status
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_init_status(MP4_FILE_MUXER* p_muxer)
{
	int32  ret;

	ret = mp4_file_muxer_init_video_status(p_muxer, &p_muxer->video);
	ret = mp4_file_muxer_init_audio_status(p_muxer, &p_muxer->audio);

	p_muxer->b_first_fragment = 0;
	p_muxer->b_write_movie_box = 0;

	p_muxer->b_get_video = 0;
	p_muxer->b_get_audio = 0;
	p_muxer->track_count = 0;

	p_muxer->total_frame_size = 0;
	p_muxer->write_buf_use_size = 0;
	p_muxer->write_buf_left_size = p_muxer->write_buf_size;
	p_muxer->total_write_size = 0;

	if( p_muxer->file_type == MP4_FILE_TYPE_FRAG_MP4 )
	{
		p_muxer->b_gop = 0;
		p_muxer->gop_count = 0;
		p_muxer->write_gop_count = 0;
		p_muxer->total_gop_count = 0;
		p_muxer->last_gop_ts = 0;
	}

	return GEN_S_OK;
}

/**  
 * @brief make audio dec spec info
 * @param [in] p_muxer, muxer
 * @param [in] p_audio, audio
 * @param [in] p_buf, input buf
 * @param [in/out] p_buf_size, total buf size/actual use size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_make_audio_dec_spec_info(MP4_FILE_MUXER* p_muxer, MP4_FILE_AUDIO* p_audio, uint8* p_buf, int32* p_buf_size)
{
	BIT_STREAM_WRITER bs;
	//uint32 value;

	int32  left_size = *p_buf_size;
	int32  total_mux_size = 0;

	//int32  ret;

	bs_writer_init(&bs, p_buf, left_size, 8);

	/** audio object type, 5 bit */
	switch( p_audio->param.format )
	{
	case SP_AUDIO_FORMAT_AAC_RAW:
	case SP_AUDIO_FORMAT_AAC_ADTS:
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
	switch( p_audio->param.param.sample_rate )
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
	switch( p_audio->param.param.chn )
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

	return GEN_S_OK;
}

/**  
 * @brief write file type box
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_write_file_type_box(MP4_FILE_MUXER* p_muxer)
{
	ISO_FILE_TYPE_BOX  file_type_box;
	ISO_MEDIA_DATA_BOX media_data_box;

	uint32 brand[2];

	uint8*  p_uint8 = NULL;
	int32   buf_left_size;
	int32   mux_size;

	uint32  write_size;

	int32   ret;

	p_uint8 = p_muxer->p_write_buf + p_muxer->write_buf_use_size;
	buf_left_size = p_muxer->write_buf_size;

	/** file type box */
	file_type_box.major_brand = MP4_BRAND_ISOM;
	file_type_box.minor_version = 0x0;
	brand[0] = hton_u32(MP4_BRAND_MP41);
	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		file_type_box.brand_count = 1;

	}else
	{
		brand[1] = hton_u32(MP4_BRAND_FRAG);
		file_type_box.brand_count = 2;
	}
	file_type_box.p_brand = brand;

	mux_size = buf_left_size;
	ret = mp4_make_file_type_box(&file_type_box, p_uint8, &mux_size);
	//ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->write_buf_use_size += mux_size;
	p_muxer->write_buf_left_size -= mux_size;
	p_muxer->media_data_offset = mux_size;
	p_muxer->total_write_size += mux_size;

	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** media data box */
		media_data_box.box.size = 0x0;
		mux_size = buf_left_size;
		ret = mp4_media_data_box_mux(&media_data_box, p_uint8, &mux_size);
		//ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->write_buf_use_size += mux_size;
		p_muxer->write_buf_left_size -= mux_size;
		p_muxer->media_data_box_size = mux_size;
		p_muxer->total_write_size += mux_size;

		ret = gen_file_write(p_muxer->file, (int8*)p_muxer->p_write_buf, p_muxer->write_buf_use_size, &write_size);
	}

	return GEN_S_OK;
}

/**  
 * @brief write movie box
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_write_movie_box(MP4_FILE_MUXER* p_muxer)
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

	ISO_MOVIE_EXTEND_BOX movie_extend_box;
	ISO_TRACK_EXTEND_BOX track_extend_box;

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
	uint32 movie_extend_box_offset;
	uint32 movie_extend_box_size;

	int32  mux_size;
	uint32 total_mux_size;

	time_t utc_time;
	uint32 creation_time;
	uint32 modification_time;
	uint32 total_entry_size;

	int8   str[MP4_FILE_MUXER_MAX_STR_SIZE + 1];

	uint8   dec_spec_buf[MP4_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE];
	int32   dec_spec_buf_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_buf_offset = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	int32  i;
	int32  ret;

	p_uint8 = p_muxer->p_write_buf + p_muxer->write_buf_use_size;
	buf_left_size = p_muxer->write_buf_left_size;

	/** movie box */
	movie_box_offset = p_muxer->total_write_size;
	total_mux_size = 0;

	movie_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_movie_box_mux(&movie_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	total_mux_size += mux_size;

	/** movie header box */
	utc_time = time(NULL);
	creation_time = utc_time + MP4_UTC_TIME_1904_TO_1970;
	modification_time = creation_time;

	movie_header_box.creation_time = creation_time;
	movie_header_box.modification_time = modification_time;

	if( p_muxer->b_get_video )
	{
		movie_header_box.time_scale = p_muxer->video.param.timescale;

	}else if( p_muxer->b_get_audio )
	{
		movie_header_box.time_scale = p_muxer->audio.param.timescale;

	}else
	{
		movie_header_box.time_scale = p_muxer->time_scale;
	}
	movie_header_box.duration = 0;
	movie_header_box.next_track_id = MP4_FILE_MUXER_MAX_TRACK_ID + 1;

	mux_size = buf_left_size;
	ret = mp4_movie_header_box_mux(&movie_header_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	total_mux_size += mux_size;

	memset(str, 0, MP4_FILE_MUXER_MAX_STR_SIZE + 1);
	str[0] = 0x0;

	/** track */
	for( i = 0; i < p_muxer->track_count; i++ )
	{
		/** track box */
		track_box_offset = p_muxer->total_write_size;
		track_box_size = 0;

		track_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_track_box_mux(&track_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** track header box */
		track_header_box.full_box.version_flag.value =
			(MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | (MP4_TRACK_ENABLE_FLAG | MP4_TRACK_IN_MOVIE_FLAG | MP4_TRACK_IN_PREVIEW_FLAG);
		track_header_box.creation_time = creation_time;
		track_header_box.modification_time = modification_time;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_header_box.track_id = MP4_FILE_MUXER_VIDEO_TRACK_ID;
			track_header_box.duration = 0;
			track_header_box.volume = 0x0;

		}else
		{
			track_header_box.track_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
			track_header_box.duration = 0;
			track_header_box.volume = 0x0100;
		}

		track_header_box.width = p_muxer->video.param.width << 16;
		track_header_box.height = p_muxer->video.param.height << 16;

		mux_size = buf_left_size;
		ret = mp4_track_header_box_mux(&track_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media box */
		media_box_offset = p_muxer->total_write_size;
		media_box_size = 0;

		media_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_media_box_mux(&media_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media header box */
		media_header_box.creation_time = creation_time;
		media_header_box.modification_time = modification_time;
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			media_header_box.time_scale = p_muxer->video.param.timescale;

		}else
		{
			media_header_box.time_scale = p_muxer->audio.param.timescale;
		}
		media_header_box.duration = 0;

		//chi
		media_header_box.language = 0x0D09;

		mux_size = buf_left_size;
		ret = mp4_media_header_box_mux(&media_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** handler ref box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_VIDE;

		}else
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_SOUN;
		}

		handler_box.p_name = str;
		handler_box.name_size = 1;

		mux_size = buf_left_size;
		ret = mp4_make_handler_box(&handler_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media info box */
		media_info_box_offset = p_muxer->total_write_size;
		media_info_box_size = 0;

		media_info_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_media_info_box_mux(&media_info_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** video media header box */
			mux_size = buf_left_size;
			ret = mp4_video_media_header_box_mux(&video_media_header_box, p_uint8, &mux_size);
			p_uint8 += mux_size;
			buf_left_size -= mux_size;
			p_muxer->total_write_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** sound media header box */
			mux_size = buf_left_size;
			ret = mp4_sound_media_header_box_mux(&sound_media_header_box, p_uint8, &mux_size);
			p_uint8 += mux_size;
			buf_left_size -= mux_size;
			p_muxer->total_write_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** data info box */
		data_info_box_offset = p_muxer->total_write_size;
		data_info_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_data_info_box_mux(&data_info_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
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
		ret = mp4_make_data_ref_box(&data_ref_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		data_info_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite data info box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + data_info_box_offset);
		data_info_box_size = hton_u32(data_info_box_size);
		*p_uint32 = data_info_box_size;

		/** sample table box */
		sample_table_box_offset = p_muxer->total_write_size;
		sample_table_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_sample_table_box_mux(&sample_table_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample desc box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** visual */
			if(	p_muxer->video.fourcc == GEN_FOURCC('a','v','c','1') )
			{
				/** h264 */
				avc_sample_desc_box.entry.visual.entry.data_ref_index = 0x1;
				avc_sample_desc_box.entry.visual.width = p_muxer->video.param.width;
				avc_sample_desc_box.entry.visual.height = p_muxer->video.param.height;
				memset(avc_sample_desc_box.entry.visual.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);
				avc_sample_desc_box.entry.config.record.profile = p_muxer->video.profile;
				avc_sample_desc_box.entry.config.record.profile_compat = p_muxer->video.profile_compat;
				avc_sample_desc_box.entry.config.record.level = p_muxer->video.level;
				avc_sample_desc_box.entry.config.record.len_size_minus_1 = 0x3;

				if( p_muxer->video.b_get_sps )
				{
					avc_sps_record.nalu_len = p_muxer->video.sps_nalu_size;
					avc_sps_record.p_nalu = p_muxer->video.p_sps_buf;

					avc_sample_desc_box.entry.config.record.sps_count = 1;
					avc_sample_desc_box.entry.config.record.p_sps = &avc_sps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.sps_count = 0;
					avc_sample_desc_box.entry.config.record.p_sps = NULL;
				}

				if( p_muxer->video.b_get_pps )
				{
					avc_pps_record.nalu_len = p_muxer->video.pps_nalu_size;
					avc_pps_record.p_nalu = p_muxer->video.p_pps_buf;

					avc_sample_desc_box.entry.config.record.pps_count = 1;
					avc_sample_desc_box.entry.config.record.p_pps = &avc_pps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.pps_count = 0;
					avc_sample_desc_box.entry.config.record.p_pps = NULL;
				}

				mux_size = buf_left_size;
				ret = mp4_make_avc_sample_desc_box(&avc_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				/** other video */
				visual_sample_entry.entry.box.type = p_muxer->video.param.fourcc;
				visual_sample_entry.entry.data_ref_index = 0x1;
				visual_sample_entry.width = p_muxer->video.param.width;
				visual_sample_entry.height = p_muxer->video.param.height;
				memset(visual_sample_entry.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);

				visual_sample_desc_box.entry_count = 1;
				visual_sample_desc_box.p_entry = &visual_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_visual_sample_desc_box(&visual_sample_desc_box, p_uint8, &mux_size);
			}

			p_uint8 += mux_size;
			buf_left_size -= mux_size;
			p_muxer->total_write_size += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** audio */
			if(	p_muxer->audio.fourcc == GEN_FOURCC('m','p','4','a') )
			{
				/** aac */
				aac_sample_entry.audio.entry.data_ref_index = 0x1;
				aac_sample_entry.audio.sample_rate = p_muxer->audio.param.param.sample_rate << 16;

				aac_sample_entry.es_box.es_desc.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.es_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
				aac_sample_entry.es_box.es_desc.stream_depend_flag = 0x0;
				aac_sample_entry.es_box.es_desc.url_flag = 0x0;
				aac_sample_entry.es_box.es_desc.ocr_stream_flag = 0x0;
				aac_sample_entry.es_box.es_desc.stream_priority = 0x0;
				aac_sample_entry.es_box.es_desc.dec_config.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.dec_config.obj_type = MPEG4_OBJECT_TYPE_AUDIO_MPEG4;
				aac_sample_entry.es_box.es_desc.dec_config.stream_type = MPEG4_STREAM_TYPE_AUDIO_STREAM;
				aac_sample_entry.es_box.es_desc.dec_config.buf_size = 0;
				aac_sample_entry.es_box.es_desc.dec_config.max_bitrate = 1500 * 1000;
				aac_sample_entry.es_box.es_desc.dec_config.avg_bitrate = 0;

				dec_spec_buf[0] = MPEG4_DEC_SPEC_INFO_TAG;
				dec_spec_buf_size = MP4_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE;
				ret = mp4_file_muxer_make_audio_dec_spec_info(p_muxer, &p_muxer->audio, dec_spec_buf + 2, &dec_spec_buf_size);
				dec_spec_buf[1] = dec_spec_buf_size;

				aac_sample_entry.es_box.es_desc.dec_config.p_vdec_spec_info = dec_spec_buf;
				aac_sample_entry.es_box.es_desc.dec_config.vdec_spec_info_size = dec_spec_buf_size + 2;

				aac_sample_entry.es_box.es_desc.sl_config.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.sl_config.predefine = MPEG4_PREDEFINE_MP4;

				aac_sample_desc_box.entry_count = 1;
				aac_sample_desc_box.p_entry = &aac_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_aac_sample_desc_box(&aac_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				/** other audio */
				audio_sample_entry.entry.box.type = p_muxer->audio.fourcc;
				audio_sample_entry.entry.data_ref_index = 0x1;
				audio_sample_entry.chn_count = p_muxer->audio.param.param.chn;
				audio_sample_entry.sample_rate = p_muxer->audio.param.param.sample_rate << 16;

				audio_sample_desc_box.entry_count = 1;
				audio_sample_desc_box.p_entry = &audio_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_audio_sample_desc_box(&audio_sample_desc_box, p_uint8, &mux_size);
			}

			p_uint8 += mux_size;
			buf_left_size -= mux_size;
			p_muxer->total_write_size += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** time to sample box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			time_to_sample_entry.sample_count = 0;
			time_to_sample_entry.sample_delta = 0;

		}else
		{
			time_to_sample_entry.sample_count = 0;
			time_to_sample_entry.sample_delta = 0;
		}

		time_to_sample_box.entry_count = 0;
		time_to_sample_box.p_entry = NULL;

		mux_size = buf_left_size;
		ret = mp4_make_time_to_sample_box(&time_to_sample_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample size box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = 0;
			sample_size_box.sample_count = 0;

		}else
		{
			total_entry_size = 0;
			sample_size_box.sample_count = 0;
		}

		sample_size_box.full_box.box.size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE + total_entry_size;
		sample_size_box.sample_size = 0x0;

		mux_size = buf_left_size;
		ret = mp4_sample_size_box_mux(&sample_size_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample to chunk box */
		sample_to_chunk_box.entry_count = 0;
		sample_to_chunk_box.p_entry = NULL;

		mux_size = buf_left_size;
		ret = mp4_make_sample_to_chunk_box(&sample_to_chunk_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** chunk offset box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = 0;
			chunk_offset_box.entry_count = 0;

		}else
		{
			total_entry_size = 0;
			chunk_offset_box.entry_count = 0;
		}

		chunk_offset_box.full_box.box.size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE + total_entry_size;

		mux_size = buf_left_size;
		ret = mp4_chunk_offset_box_mux(&chunk_offset_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite sample table box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + sample_table_box_offset);
		sample_table_box_size = hton_u32(sample_table_box_size);
		*p_uint32 = sample_table_box_size;

		/** rewrite media info box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + media_info_box_offset);
		media_info_box_size = hton_u32(media_info_box_size);
		*p_uint32 = media_info_box_size;

		/** rewrite media box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + media_box_offset);
		media_box_size = hton_u32(media_box_size);
		*p_uint32 = media_box_size;

		/** rewrite track box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + track_box_offset);
		track_box_size = hton_u32(track_box_size);
		*p_uint32 = track_box_size;
	}

	/** movie extend box */
	movie_extend_box_offset = p_muxer->total_write_size;
	movie_extend_box_size = 0;

	movie_extend_box.box.size = 0;

	mux_size = buf_left_size;
	ret = mp4_make_movie_extend_box(&movie_extend_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	movie_extend_box_size += mux_size;
	total_mux_size += mux_size;

	for( i = 0; i < p_muxer->track_count; i++ )
	{
		/** track extend box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_extend_box.track_id = MP4_FILE_MUXER_VIDEO_TRACK_ID;

		}else
		{
			track_extend_box.track_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
		}

		track_extend_box.def_sample_desc_index = 0x1;
		track_extend_box.def_sample_duration = 0;
		track_extend_box.def_sample_size = 0;
		track_extend_box.def_sample_flag = 0;

		mux_size = buf_left_size;
		ret = mp4_make_track_extend_box(&track_extend_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		movie_extend_box_size += mux_size;
		total_mux_size += mux_size;
	}

	/** rewrite movie extend box size */
	p_uint32 = (uint32*)(p_muxer->p_write_buf + movie_extend_box_offset);
	movie_extend_box_size = hton_u32(movie_extend_box_size);
	*p_uint32 = movie_extend_box_size;

	/** rewrite movie box size */
	p_uint32 = (uint32*)(p_muxer->p_write_buf + movie_box_offset);
	total_mux_size = hton_u32(total_mux_size);
	*p_uint32 = total_mux_size;

	p_muxer->write_buf_use_size = p_muxer->total_write_size;
	p_muxer->write_buf_left_size = buf_left_size;

	//log_debug(_T("[CMp4LiveWriter::WriteMovieBox] write %u bytes\n"), m_TotalWriteSize);

	return GEN_S_OK;
}

/**  
 * @brief write fragment
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_write_fragment(MP4_FILE_MUXER* p_muxer)
{
	ISO_MOVIE_FRAGMENT_BOX  movie_frag_box;
	ISO_MOVIE_FRAGMENT_HEADER_BOX movie_frag_header_box;

	ISO_TRACK_FRAGMENT_BOX track_frag_box;
	ISO_TRACK_FRAGMENT_HEADER_BOX track_frag_header_box;
	ISO_TRACK_FRAGMENT_RUN_BOX track_frag_run_box;

	ISO_MEDIA_DATA_BOX media_data_box;

	uint32 movie_frag_box_offset;
	uint32 movie_frag_box_size;
	uint32 track_frag_box_offset;
	uint32 track_frag_box_size;
	uint32 track_frag_run_box_offset;

	int32  mux_size;
	uint32 total_mux_size;

	uint32 uint32_data;

	//int8   file_path[MP4_LIVE_WRITER_MAX_PATH_SIZE];
	//int32  file_path_size;

	//DWORD dwWriteSize;

	SP_MEDIA_FRAME* p_frame = NULL;

	uint8*  p_uint8 = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	int32   i;
	int32   ret;

	p_muxer->write_buf_use_size = 0;
	p_muxer->write_buf_left_size = p_muxer->write_buf_size;
	p_muxer->total_write_size = 0;

	if( !p_muxer->b_write_movie_box )
	{
		/** write movie box */
		ret = mp4_file_muxer_write_file_type_box(p_muxer);
		ret = mp4_file_muxer_write_movie_box(p_muxer);

		if( p_muxer->callback.meta_data )
		{
			ret = p_muxer->callback.meta_data(p_muxer->p_write_buf, p_muxer->write_buf_use_size, p_muxer->callback.p_custom_param);
		}

#if MP4_FILE_MUXER_ENABLE_DEBUG
		if( p_muxer->p_test_file )
		{
			ret = fwrite(p_muxer->p_write_buf, 1, p_muxer->write_buf_use_size, p_muxer->p_test_file);
		}
#endif

		/** clear buf */
		p_muxer->write_buf_use_size = 0;
		p_muxer->write_buf_left_size = p_muxer->write_buf_size;
		p_muxer->total_write_size = 0;

		p_muxer->b_write_movie_box = 1;
	}

	p_uint8 = p_muxer->p_write_buf + p_muxer->write_buf_use_size;
	buf_left_size = p_muxer->write_buf_left_size;

	/** movie fragment box */
	movie_frag_box_offset = p_muxer->total_write_size;
	movie_frag_box_size = 0;
	total_mux_size = 0;

	movie_frag_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_make_movie_fragment_box(&movie_frag_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	movie_frag_box_size += mux_size;
	total_mux_size += mux_size;

	/** movie fragment header box */
	movie_frag_header_box.seq_no = p_muxer->write_gop_count;
	p_muxer->write_gop_count++;

	mux_size = buf_left_size;
	ret = mp4_make_movie_fragment_header_box(&movie_frag_header_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	movie_frag_box_size += mux_size;
	total_mux_size += mux_size;

	track_frag_run_box_offset = 0;

	for( i = 0; i < p_muxer->track_count; i++ )
	{
		/** track fragment box */
		track_frag_box_offset = p_muxer->total_write_size;
		track_frag_box_size = 0;

		track_frag_box.box.size = 0x0;
		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_box(&track_frag_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		/** track fragment header box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_frag_header_box.track_id = MP4_FILE_MUXER_VIDEO_TRACK_ID;

		}else
		{
			track_frag_header_box.track_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
		}

		track_frag_header_box.full_box.version_flag.value = ( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) );

		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_header_box(&track_frag_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		if( i == 0 )
		{
			/** first moof */
			track_frag_run_box_offset = p_muxer->total_write_size;
		}

		/** track fragment run box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			//track_frag_run_box_offset = p_muxer->total_write_size;

			track_frag_run_box.sample_count = p_muxer->video.frag_sample_entry_count;
			track_frag_run_box.p_sample = p_muxer->video.p_frag_sample_entry;

			track_frag_run_box.data_offset = 0;
			track_frag_run_box.first_sample_flag = 0x0;
			track_frag_run_box.full_box.version_flag.value = ( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET)
				| (MP4_DATA_OFFSET_PRESENT_FLAG | MP4_FIRST_SAMPLE_FLAG_PRESENT_FLAG | MP4_SAMPLE_DURATION_PRESENT_FLAG | MP4_SAMPLE_SIZE_PRESENT_FLAG | MP4_SAMPLE_COMPOSITION_TIME_OFFSET_PRESENT_FLAG) );

		}else
		{
			track_frag_run_box.sample_count = p_muxer->audio.frag_sample_entry_count;
			track_frag_run_box.p_sample = p_muxer->audio.p_frag_sample_entry;

			track_frag_run_box.data_offset = 0;
			track_frag_run_box.full_box.version_flag.value = ( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET)
				| (MP4_DATA_OFFSET_PRESENT_FLAG | MP4_SAMPLE_DURATION_PRESENT_FLAG | MP4_SAMPLE_SIZE_PRESENT_FLAG) );
		}

		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_run_box(&track_frag_run_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		p_muxer->total_write_size += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite track fragment box size */
		p_uint32 = (uint32*)(p_muxer->p_write_buf + track_frag_box_offset);
		track_frag_box_size = hton_u32(track_frag_box_size);
		*p_uint32 = track_frag_box_size;
	}

	/** rewrite track fragment run box's data offset section */
	p_uint32 = (uint32*)(p_muxer->p_write_buf + track_frag_run_box_offset + MP4_FULL_BOX_SIZE + 4);
	uint32_data = hton_u32(movie_frag_box_size + 8);
	*p_uint32 = uint32_data;

	/** rewrite movie fragment box size */
	p_uint32 = (uint32*)(p_muxer->p_write_buf + movie_frag_box_offset);
	movie_frag_box_size = hton_u32(movie_frag_box_size);
	*p_uint32 = movie_frag_box_size;

	/** media data box */
	media_data_box.box.size = MP4_MEDIA_DATA_BOX_SIZE + p_muxer->video.data_buf_use_size + p_muxer->audio.data_buf_use_size;

	mux_size = buf_left_size;
	ret = mp4_media_data_box_mux(&media_data_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;
	total_mux_size += mux_size;

	p_muxer->write_buf_use_size = p_muxer->total_write_size;

	if( p_muxer->callback.moof_data )
	{
		ret = p_muxer->callback.moof_data(p_muxer->p_write_buf, p_muxer->write_buf_use_size, p_muxer->callback.p_custom_param);
	}

#if MP4_FILE_MUXER_ENABLE_DEBUG
	if( p_muxer->p_test_file )
	{
		ret = fwrite(p_muxer->p_write_buf, 1, p_muxer->write_buf_use_size, p_muxer->p_test_file);
	}
#endif

	for( i = 0; i < p_muxer->track_count; i++ )
	{
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			if( p_muxer->callback.video_data )
			{
				ret = p_muxer->callback.video_data(p_muxer->video.p_data_buf, p_muxer->video.data_buf_use_size, p_muxer->callback.p_custom_param);
			}

#if MP4_FILE_MUXER_ENABLE_DEBUG
			if( p_muxer->p_test_file )
			{
				ret = fwrite(p_muxer->video.p_data_buf, 1, p_muxer->video.data_buf_use_size, p_muxer->p_test_file);
			}
#endif

		}else
		{
			if( p_muxer->callback.audio_data )
			{
				ret = p_muxer->callback.audio_data(p_muxer->audio.p_data_buf, p_muxer->audio.data_buf_use_size, p_muxer->callback.p_custom_param);
			}

#if MP4_FILE_MUXER_ENABLE_DEBUG
			if( p_muxer->p_test_file )
			{
				ret = fwrite(p_muxer->audio.p_data_buf, 1, p_muxer->audio.data_buf_use_size, p_muxer->p_test_file);
			}
#endif
		}
	}

#if 0
	if( m_WriteGopCount % MP4_LIVE_WRITER_GOP_PER_FILE == 0 )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
#endif

	return GEN_S_OK;
}

/**  
 * @brief stop mp4
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_stop_mp4(MP4_FILE_MUXER* p_muxer)
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
	//ISO_TIME_TO_SAMPLE_ENTRY time_to_sample_entry;
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
	uint32 total_entry_size;

	int32  b_open;
	uint32 write_size;

	int8   str[MP4_FILE_MUXER_MAX_STR_SIZE + 1];

	uint8  dec_spec_buf[MP4_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE];
	int32  dec_spec_buf_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_buf_offset = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	uint32  uint32_data;
	int64   int64_data;

	int32   i;
	int32   ret;

	ret = gen_file_is_open(p_muxer->file, &b_open);
	if( !b_open )
	{
		//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_stop_mp4] mp4 file not open!\n"));
		goto FIN;
	}

	/** mdat */
	ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, p_muxer->media_data_offset, NULL);

	uint32_data = p_muxer->total_frame_size + p_muxer->media_data_box_size;
	uint32_data = hton_u32(uint32_data);
	ret = gen_file_write(p_muxer->file, (int8*)&uint32_data, sizeof(uint32_data), &write_size);

	ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_END, 0, NULL);

	p_uint8 = p_muxer->p_write_buf;
	buf_left_size = p_muxer->write_buf_size;

	/** movie box */
	movie_box_offset = p_muxer->total_write_size;
	total_mux_size = 0;

	movie_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_movie_box_mux(&movie_box, p_uint8, &mux_size);
	ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
	p_muxer->total_write_size += mux_size;
	total_mux_size += mux_size;

	/** movie header box */
	utc_time = time(NULL);
	creation_time = utc_time + MP4_UTC_TIME_1904_TO_1970;
	modification_time = creation_time;

	movie_header_box.creation_time = creation_time;
	movie_header_box.modification_time = modification_time;

	if( p_muxer->b_get_video )
	{
		movie_header_box.time_scale = p_muxer->video.param.timescale;
		movie_header_box.duration = p_muxer->video.total_elapse_time;

	}else if( p_muxer->b_get_audio )
	{
		movie_header_box.time_scale = p_muxer->audio.param.timescale;
		movie_header_box.duration = p_muxer->audio.total_elapse_time;

	}else
	{
		movie_header_box.time_scale = p_muxer->time_scale;
		movie_header_box.duration = UINT32_MAX;
	}
	movie_header_box.next_track_id = MP4_FILE_MUXER_MAX_TRACK_ID + 1;

	mux_size = buf_left_size;
	ret = mp4_movie_header_box_mux(&movie_header_box, p_uint8, &mux_size);
	ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
	p_muxer->total_write_size += mux_size;
	total_mux_size += mux_size;

	memset(str, 0, MP4_FILE_MUXER_MAX_STR_SIZE + 1);
	str[0] = 0x00;

	/** track */
	for( i = 0; i < p_muxer->track_count; i++ )
	{
		/** track box */
		track_box_offset = p_muxer->total_write_size;
		track_box_size = 0;

		track_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_track_box_mux(&track_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** track header box */
		track_header_box.full_box.version_flag.value =
			(MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) | (MP4_TRACK_ENABLE_FLAG | MP4_TRACK_IN_MOVIE_FLAG | MP4_TRACK_IN_PREVIEW_FLAG);
		track_header_box.creation_time = creation_time;
		track_header_box.modification_time = modification_time;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_header_box.track_id = MP4_FILE_MUXER_VIDEO_TRACK_ID;
			track_header_box.duration = p_muxer->video.total_elapse_time;
			track_header_box.volume = 0x0;

		}else
		{
			track_header_box.track_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
			if( p_muxer->audio.param.timescale == movie_header_box.time_scale )
			{
				track_header_box.duration = p_muxer->audio.total_elapse_time;

			}else
			{
				int64_data = p_muxer->audio.total_elapse_time * movie_header_box.time_scale / p_muxer->audio.param.timescale;
				track_header_box.duration = int64_data;
			}

			track_header_box.volume = 0x0100;
		}

		track_header_box.width = p_muxer->video.param.width << 16;
		track_header_box.height = p_muxer->video.param.height << 16;

		mux_size = buf_left_size;
		ret = mp4_track_header_box_mux(&track_header_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media box */
		media_box_offset = p_muxer->total_write_size;
		media_box_size = 0;

		media_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_media_box_mux(&media_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media header box */
		media_header_box.creation_time = creation_time;
		media_header_box.modification_time = modification_time;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			media_header_box.time_scale = p_muxer->video.param.timescale;
			media_header_box.duration = p_muxer->video.total_elapse_time;

		}else
		{
			media_header_box.time_scale = p_muxer->audio.param.timescale;
			media_header_box.duration = p_muxer->audio.total_elapse_time;
		}

		//chi
		media_header_box.language = 0x0D09;

		mux_size = buf_left_size;
		ret = mp4_media_header_box_mux(&media_header_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** handler ref box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_VIDE;

		}else
		{
			handler_box.handler_type = MP4_HANDLER_TYPE_SOUN;
		}

		handler_box.p_name = str;
		handler_box.name_size = 1;

		mux_size = buf_left_size;
		ret = mp4_make_handler_box(&handler_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media info box */
		media_info_box_offset = p_muxer->total_write_size;
		media_info_box_size = 0;

		media_info_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_media_info_box_mux(&media_info_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** video media header box */
			mux_size = buf_left_size;
			ret = mp4_video_media_header_box_mux(&video_media_header_box, p_uint8, &mux_size);
			ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
			p_muxer->total_write_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** sound media header box */
			mux_size = buf_left_size;
			ret = mp4_sound_media_header_box_mux(&sound_media_header_box, p_uint8, &mux_size);
			ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
			p_muxer->total_write_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** data info box */
		data_info_box_offset = p_muxer->total_write_size;
		data_info_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_data_info_box_mux(&data_info_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
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
		ret = mp4_make_data_ref_box(&data_ref_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		data_info_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite data info box size */
		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, data_info_box_offset, NULL);

		data_info_box_size = hton_u32(data_info_box_size);
		ret = gen_file_write(p_muxer->file, (int8*)&data_info_box_size, sizeof(data_info_box_size), &write_size);

		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_END, 0, NULL);

		/** sample table box */
		sample_table_box_offset = p_muxer->total_write_size;
		sample_table_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_sample_table_box_mux(&sample_table_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample desc box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			/** visual */
			if(	p_muxer->video.fourcc == GEN_FOURCC('a','v','c','1') )
			{
				avc_sample_desc_box.entry.visual.entry.data_ref_index = 0x1;
				avc_sample_desc_box.entry.visual.width = p_muxer->video.param.width;
				avc_sample_desc_box.entry.visual.height = p_muxer->video.param.height;
				memset(avc_sample_desc_box.entry.visual.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);
				avc_sample_desc_box.entry.config.record.profile = p_muxer->video.profile;
				avc_sample_desc_box.entry.config.record.profile_compat = p_muxer->video.profile_compat;
				avc_sample_desc_box.entry.config.record.level = p_muxer->video.level;
				avc_sample_desc_box.entry.config.record.len_size_minus_1 = 0x3;

				if( p_muxer->video.b_get_sps )
				{
					avc_sps_record.nalu_len = p_muxer->video.sps_nalu_size;
					avc_sps_record.p_nalu = p_muxer->video.p_sps_buf;

					avc_sample_desc_box.entry.config.record.sps_count = 1;
					avc_sample_desc_box.entry.config.record.p_sps = &avc_sps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.sps_count = 0;
					avc_sample_desc_box.entry.config.record.p_sps = NULL;
				}

				if( p_muxer->video.b_get_pps )
				{
					avc_pps_record.nalu_len = p_muxer->video.pps_nalu_size;
					avc_pps_record.p_nalu = p_muxer->video.p_pps_buf;

					avc_sample_desc_box.entry.config.record.pps_count = 1;
					avc_sample_desc_box.entry.config.record.p_pps = &avc_pps_record;

				}else
				{
					avc_sample_desc_box.entry.config.record.pps_count = 0;
					avc_sample_desc_box.entry.config.record.p_pps = NULL;
				}

				mux_size = buf_left_size;
				ret = mp4_make_avc_sample_desc_box(&avc_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				visual_sample_entry.entry.box.type = p_muxer->video.fourcc;
				visual_sample_entry.entry.data_ref_index = 0x1;
				visual_sample_entry.width = p_muxer->video.param.width;
				visual_sample_entry.height = p_muxer->video.param.height;
				memset(visual_sample_entry.compressor_name, 0, ISO_MEDIA_FILE_COMPRESSOR_NAME_SIZE);

				visual_sample_desc_box.entry_count = 1;
				visual_sample_desc_box.p_entry = &visual_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_visual_sample_desc_box(&visual_sample_desc_box, p_uint8, &mux_size);
			}

			ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
			p_muxer->total_write_size += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

		}else
		{
			/** audio */
			if(	p_muxer->audio.fourcc == GEN_FOURCC('m','p','4','a') )
			{
				aac_sample_entry.audio.entry.data_ref_index = 0x1;
				aac_sample_entry.audio.sample_rate = p_muxer->audio.param.param.sample_rate << 16;

				aac_sample_entry.es_box.es_desc.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.es_id = MP4_FILE_MUXER_AUDIO_TRACK_ID;
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
				dec_spec_buf_size = MP4_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE;
				ret = mp4_file_muxer_make_audio_dec_spec_info(p_muxer, &p_muxer->audio, dec_spec_buf + 2, &dec_spec_buf_size);
				dec_spec_buf[1] = dec_spec_buf_size;

				aac_sample_entry.es_box.es_desc.dec_config.p_vdec_spec_info = dec_spec_buf;
				aac_sample_entry.es_box.es_desc.dec_config.vdec_spec_info_size = dec_spec_buf_size + 2;

				aac_sample_entry.es_box.es_desc.sl_config.base.size.bit_size = MPEG4_SIZE_ENCODING_8_BIT;
				aac_sample_entry.es_box.es_desc.sl_config.predefine = MPEG4_PREDEFINE_MP4;

				aac_sample_desc_box.entry_count = 1;
				aac_sample_desc_box.p_entry = &aac_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_aac_sample_desc_box(&aac_sample_desc_box, p_uint8, &mux_size);

			}else
			{
				audio_sample_entry.entry.box.type = p_muxer->audio.fourcc;
				audio_sample_entry.entry.data_ref_index = 0x1;
				audio_sample_entry.chn_count = p_muxer->audio.param.param.chn;
				audio_sample_entry.sample_rate = p_muxer->audio.param.param.sample_rate << 16;

				audio_sample_desc_box.entry_count = 1;
				audio_sample_desc_box.p_entry = &audio_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_audio_sample_desc_box(&audio_sample_desc_box, p_uint8, &mux_size);
			}

			ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
			p_muxer->total_write_size += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;
		}

		/** time to sample box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * p_muxer->video.frame_count;
			time_to_sample_box.entry_count = p_muxer->video.frame_count;
			time_to_sample_box.p_entry = p_muxer->video.p_time_entry;

			if( p_muxer->video.frame_count == 1 )
			{
				p_muxer->video.p_time_entry[p_muxer->video.frame_count - 1].sample_delta = hton_u32(40);

			}else
			{
				p_muxer->video.p_time_entry[p_muxer->video.frame_count - 1].sample_delta = p_muxer->video.p_time_entry[p_muxer->video.frame_count - 2].sample_delta;
			}

		}else
		{
			total_entry_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * p_muxer->audio.frame_count;
			time_to_sample_box.entry_count = p_muxer->audio.frame_count;
			time_to_sample_box.p_entry = p_muxer->audio.p_time_entry;

			if( p_muxer->audio.frame_count == 1 )
			{
				p_muxer->audio.p_time_entry[p_muxer->audio.frame_count - 1].sample_delta = hton_u32(48000 / 1024);

			}else
			{
				p_muxer->audio.p_time_entry[p_muxer->audio.frame_count - 1].sample_delta = p_muxer->audio.p_time_entry[p_muxer->audio.frame_count - 2].sample_delta;
			}
		}

		mux_size = buf_left_size;
		ret = mp4_make_time_to_sample_box(&time_to_sample_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->video.p_time_entry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->audio.p_time_entry, total_entry_size, &write_size);
		}
		p_muxer->total_write_size += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** sample size box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * p_muxer->video.frame_count;
			sample_size_box.sample_count = p_muxer->video.frame_count;

		}else
		{
			total_entry_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * p_muxer->audio.frame_count;
			sample_size_box.sample_count = p_muxer->audio.frame_count;
		}

		sample_size_box.full_box.box.size = MP4_SAMPLE_SIZE_BOX_MIN_SIZE + total_entry_size;
		sample_size_box.sample_size = 0x0;

		mux_size = buf_left_size;
		ret = mp4_sample_size_box_mux(&sample_size_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->video.p_size_entry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->audio.p_size_entry, total_entry_size, &write_size);
		}

		p_muxer->total_write_size += total_entry_size;
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
		ret = mp4_make_sample_to_chunk_box(&sample_to_chunk_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		total_entry_size = MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
		ret = gen_file_write(p_muxer->file, (int8*)&sample_to_chunk_entry, total_entry_size, &write_size);
		p_muxer->total_write_size += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** chunk offset box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_CHUNK_OFFSET_ENTRY_SIZE * p_muxer->video.frame_count;
			chunk_offset_box.entry_count = p_muxer->video.frame_count;

		}else
		{
			total_entry_size = MP4_CHUNK_OFFSET_ENTRY_SIZE * p_muxer->audio.frame_count;
			chunk_offset_box.entry_count = p_muxer->audio.frame_count;
		}

		chunk_offset_box.full_box.box.size = MP4_CHUNK_OFFSET_BOX_MIN_SIZE + total_entry_size;

		mux_size = buf_left_size;
		ret = mp4_chunk_offset_box_mux(&chunk_offset_box, p_uint8, &mux_size);
		ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
		p_muxer->total_write_size += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->video.p_offset_entry, total_entry_size, &write_size);

		}else
		{
			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->audio.p_offset_entry, total_entry_size, &write_size);
		}

		p_muxer->total_write_size += total_entry_size;
		sample_table_box_size += total_entry_size;
		media_info_box_size += total_entry_size;
		media_box_size += total_entry_size;
		track_box_size += total_entry_size;
		total_mux_size += total_entry_size;

		/** sync sample box */
		if( p_muxer->track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			total_entry_size = MP4_SYNC_SAMPLE_ENTRY_SIZE * p_muxer->video.sync_entry_count;
			sync_sample_box.entry_count = p_muxer->video.sync_entry_count;

			sync_sample_box.full_box.box.size = MP4_SYNC_SAMPLE_BOX_MIN_SIZE + total_entry_size;

			mux_size = buf_left_size;
			ret = mp4_sync_sample_box_mux(&sync_sample_box, p_uint8, &mux_size);
			ret = gen_file_write(p_muxer->file, (int8*)p_uint8, mux_size, &write_size);
			p_muxer->total_write_size += mux_size;
			sample_table_box_size += mux_size;
			media_info_box_size += mux_size;
			media_box_size += mux_size;
			track_box_size += mux_size;
			total_mux_size += mux_size;

			ret = gen_file_write(p_muxer->file, (int8*)p_muxer->video.p_sync_entry, total_entry_size, &write_size);
			p_muxer->total_write_size += total_entry_size;
			sample_table_box_size += total_entry_size;
			media_info_box_size += total_entry_size;
			media_box_size += total_entry_size;
			track_box_size += total_entry_size;
			total_mux_size += total_entry_size;
		}

		/** rewrite sample table box size */
		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, sample_table_box_offset, NULL);

		sample_table_box_size = hton_u32(sample_table_box_size);
		ret = gen_file_write(p_muxer->file, (int8*)&sample_table_box_size, sizeof(sample_table_box_size), &write_size);

		/** rewrite media info box size */
		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, media_info_box_offset, NULL);

		media_info_box_size = hton_u32(media_info_box_size);
		ret = gen_file_write(p_muxer->file, (int8*)&media_info_box_size, sizeof(media_info_box_size), &write_size);

		/** rewrite media box size */
		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, media_box_offset, NULL);

		media_box_size = hton_u32(media_box_size);
		ret = gen_file_write(p_muxer->file, (int8*)&media_box_size, sizeof(media_box_size), &write_size);

		/** rewrite track box size */
		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, track_box_offset, NULL);

		track_box_size = hton_u32(track_box_size);
		ret = gen_file_write(p_muxer->file, (int8*)&track_box_size, sizeof(track_box_size), &write_size);

		ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_END, 0, NULL);
	}

	/** rewrite movie box size */
	ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, movie_box_offset, NULL);

	total_mux_size = hton_u32(total_mux_size);
	ret = gen_file_write(p_muxer->file, (int8*)&total_mux_size, sizeof(total_mux_size), &write_size);

	//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_stop_mp4] total write %u bytes\n"), p_muxer->total_write_size);

	gen_file_close(p_muxer->file);

FIN:
	p_muxer->b_start = 0;

	return GEN_S_OK;
}

/**  
 * @brief stop frag mp4
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_muxer_stop_frag_mp4(MP4_FILE_MUXER* p_muxer)
{
	//MP4_FILE_MUXER* p_muxer = (MP4_FILE_MUXER*)muxer_h;
	MP4_FILE_VIDEO* p_video = NULL;
	MP4_FILE_AUDIO* p_audio = NULL;

	SP_MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	p_video = &p_muxer->video;
	p_audio = &p_muxer->audio;

	//if( m_pVideoDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(&p_video->data_deque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(&p_video->data_deque);
			free(p_frame);
			p_frame = NULL;
		}
	}

	//if( m_pAudioDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(&p_audio->data_deque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(&p_audio->data_deque);
			free(p_frame);
			p_frame = NULL;
		}
	}

#if MP4_FILE_MUXER_ENABLE_DEBUG
	if( p_muxer->p_test_file )
	{
		fclose(p_muxer->p_test_file);
		p_muxer->p_test_file = NULL;
	}
#endif

	p_muxer->b_start = 0;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
MP4_FILE_MUXER_H mp4_file_muxer_create()
{
	MP4_FILE_MUXER *p_muxer = NULL;

	p_muxer = (MP4_FILE_MUXER*)malloc( sizeof(MP4_FILE_MUXER) );

	if( p_muxer == NULL )
	{
		return NULL;
	}

	memset(p_muxer, 0, sizeof(MP4_FILE_MUXER));

	return p_muxer;
}

int32 mp4_file_muxer_destroy(MP4_FILE_MUXER_H muxer_h)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_init )
	{
		mp4_file_muxer_deinit(muxer_h);
	}

	free(p_muxer);

	return 0;
}

int32 mp4_file_muxer_init(MP4_FILE_MUXER_H muxer_h)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_init )
	{
		return GEN_S_OK;
	}

	p_muxer->file_type = MP4_FILE_TYPE_MP4;

	memset(&p_muxer->callback, 0, sizeof(p_muxer->callback));

	p_muxer->b_start = 0;
	p_muxer->b_enable_video = 1;
	p_muxer->b_enable_audio = 1;
	p_muxer->b_reserve_audio = 0;

	p_muxer->time_scale = MP4_FILE_MUXER_UNI_TIME_SCALE;

	p_muxer->total_write_size = 0;

	ret = mp4_file_muxer_init_video(p_muxer, &p_muxer->video);
	if( ret )
	{
		mp4_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = mp4_file_muxer_init_audio(p_muxer, &p_muxer->audio);
	if( ret )
	{
		mp4_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = mp4_file_muxer_init_write_buf(p_muxer);
	if( ret )
	{
		mp4_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = mp4_file_muxer_init_file(p_muxer);
	if( ret )
	{
		mp4_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

#if MP4_FILE_MUXER_ENABLE_DEBUG
	p_muxer->p_test_file = NULL;
#endif

	p_muxer->b_init = 1;

	return GEN_S_OK;
}

int32 mp4_file_muxer_deinit(MP4_FILE_MUXER_H muxer_h)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	//int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_start )
	{
		mp4_file_muxer_stop(p_muxer);
	}

	mp4_file_muxer_deinit_video(p_muxer, &p_muxer->video);
	mp4_file_muxer_deinit_audio(p_muxer, &p_muxer->audio);
	mp4_file_muxer_deinit_write_buf(p_muxer);
	mp4_file_muxer_deinit_file(p_muxer);

	p_muxer->b_init = 0;

	return GEN_S_OK;
}

int32 mp4_file_muxer_reset(MP4_FILE_MUXER_H muxer_h)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = mp4_file_muxer_clear_video_deque(p_muxer, &p_muxer->video);
	ret = mp4_file_muxer_clear_audio_deque(p_muxer, &p_muxer->audio);
	ret = mp4_file_muxer_init_status(p_muxer);
	ret = mp4_file_muxer_write_file_type_box(p_muxer);

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_file_type(MP4_FILE_MUXER_H muxer_h, int32 file_type)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	//int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_muxer->b_start )
	{
		return GEN_E_INVALID_STATUS;
	}

	switch( file_type )
	{
	case MP4_FILE_TYPE_MP4:
	case MP4_FILE_TYPE_FRAG_MP4:
		{
		}
		break;

	default:
		{
			return GEN_E_NOT_SUPPORT;
		}
	}

	p_muxer->file_type = file_type;

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_callback(MP4_FILE_MUXER_H muxer_h, MP4_FILE_MUXER_CALLBACK* p_callback)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	//int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

#if 0
	if( p_muxer->b_start )
	{
		return GEN_E_INVALID_STATUS;
	}
#endif

	p_muxer->callback = *p_callback;

	return GEN_S_OK;
}

int32 mp4_file_muxer_enable_video(MP4_FILE_MUXER_H muxer_h, int32 b_enable)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_muxer->b_enable_video = b_enable;

	return GEN_S_OK;
}

int32 mp4_file_muxer_enable_audio(MP4_FILE_MUXER_H muxer_h, int32 b_enable)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_muxer->b_enable_audio = b_enable;

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_reserve_audio(MP4_FILE_MUXER_H muxer_h, int32 b_reserve)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_muxer->b_reserve_audio = b_reserve;

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_force_sync(MP4_FILE_MUXER_H muxer_h, int32 b_force_sync)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_muxer->b_force_sync = b_force_sync;

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_time_scale(MP4_FILE_MUXER_H muxer_h, uint32 time_scale)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_muxer->time_scale = time_scale;

	return GEN_S_OK;
}

int32 mp4_file_muxer_set_avc_dec_config(MP4_FILE_MUXER_H muxer_h, uint8* p_data, int32 data_size)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	AVC_DEC_CONFIG_RECORD dec_config;
	AVC_SPS_RECORD sps_record[MP4_FILE_MUXER_MAX_SPS_RECORD];
	AVC_PPS_RECORD pps_record[MP4_FILE_MUXER_MAX_PPS_RECORD];

	uint32 left_size;
	uint32 demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_muxer == NULL || p_data == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	left_size = data_size;
	p_uint8 = p_data;

	dec_config.sps_count = MP4_FILE_MUXER_MAX_SPS_RECORD;
	dec_config.p_sps = sps_record;
	dec_config.pps_count = MP4_FILE_MUXER_MAX_PPS_RECORD;
	dec_config.p_pps = pps_record;
	demux_size = left_size;
	ret = mp4_avc_dec_config_record_demux(&dec_config, p_uint8, &demux_size);
	if( ret == 0 )
	{
		p_muxer->video.b_get_sps = 1;
		p_muxer->video.profile = dec_config.profile;
		p_muxer->video.profile_compat = dec_config.profile_compat;
		p_muxer->video.level = dec_config.level;
		memcpy(p_muxer->video.p_sps_buf, dec_config.p_sps[0].p_nalu, dec_config.p_sps[0].nalu_len);
		p_muxer->video.sps_nalu_size = dec_config.p_sps[0].nalu_len;

		p_muxer->video.b_get_pps = 1;
		memcpy(p_muxer->video.p_pps_buf, dec_config.p_pps[0].p_nalu, dec_config.p_pps[0].nalu_len);
		p_muxer->video.pps_nalu_size = dec_config.p_pps[0].nalu_len;
	}

	return GEN_S_OK;
}

int32 mp4_file_muxer_startA(MP4_FILE_MUXER_H muxer_h, int8* p_file_path, int32 file_path_size)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;
	GEN_FILE_OPEN_PARAM open_param;

	int32 ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_muxer->b_start )
	{
		mp4_file_muxer_stop(muxer_h);
	}

	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */

		/** video */
		if( p_muxer->video.b_init_frag_mp4 )
		{
			mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
			mp4_file_muxer_deinit_video_deque(p_muxer, &p_muxer->video);
			mp4_file_muxer_deinit_video_frag(p_muxer, &p_muxer->video);

			p_muxer->video.b_init_frag_mp4 = 0;
		}

		if( !p_muxer->video.b_init_mp4 )
		{
			ret = mp4_file_muxer_init_video_entry(p_muxer, &p_muxer->video);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			p_muxer->video.b_init_mp4 = 1;
		}

		/** audio */
		if( p_muxer->audio.b_init_frag_mp4 )
		{
			mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
			mp4_file_muxer_deinit_audio_deque(p_muxer, &p_muxer->audio);
			mp4_file_muxer_deinit_audio_frag(p_muxer, &p_muxer->audio);

			p_muxer->audio.b_init_frag_mp4 = 0;
		}

		if( !p_muxer->audio.b_init_mp4 )
		{
			ret = mp4_file_muxer_init_audio_entry(p_muxer, &p_muxer->audio);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			p_muxer->audio.b_init_mp4 = 1;
		}

		open_param.rw = GEN_FILE_OPEN_WRITE;
		open_param.share = GEN_FILE_SHARE_READ;
		open_param.create = GEN_FILE_CREATE_ALWAYS;
		open_param.open_flag = 0;
		ret = gen_file_set_param(p_muxer->file, GEN_FILE_PARAM_OPEN, (uint8*)&open_param, sizeof(open_param));

		ret = gen_file_openA(p_muxer->file, p_file_path, file_path_size);
		if( ret )
		{
			return GEN_E_FAIL;
		}

	}else
	{
		/** frag mp4 */

		/** video */
		if( p_muxer->video.b_init_mp4 )
		{
			mp4_file_muxer_deinit_video_entry(p_muxer, &p_muxer->video);

			p_muxer->video.b_init_mp4 = 0;
		}

		if( !p_muxer->video.b_init_frag_mp4 )
		{
			ret = mp4_file_muxer_init_video_data_buf(p_muxer, &p_muxer->video);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_video_deque(p_muxer, &p_muxer->video);
			if( ret )
			{
				mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_video_frag(p_muxer, &p_muxer->video);
			if( ret )
			{
				mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
				mp4_file_muxer_deinit_video_deque(p_muxer, &p_muxer->video);
				return GEN_E_FAIL;
			}

			p_muxer->video.b_init_frag_mp4 = 1;
		}

		/** audio */
		if( p_muxer->audio.b_init_mp4 )
		{
			mp4_file_muxer_deinit_audio_entry(p_muxer, &p_muxer->audio);

			p_muxer->audio.b_init_mp4 = 0;
		}

		if( !p_muxer->audio.b_init_frag_mp4 )
		{
			ret = mp4_file_muxer_init_audio_data_buf(p_muxer, &p_muxer->audio);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_audio_deque(p_muxer, &p_muxer->audio);
			if( ret )
			{
				mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_audio_frag(p_muxer, &p_muxer->audio);
			if( ret )
			{
				mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
				mp4_file_muxer_deinit_audio_deque(p_muxer, &p_muxer->audio);
				return GEN_E_FAIL;
			}

			p_muxer->audio.b_init_frag_mp4 = 1;
		}
	}

	ret = mp4_file_muxer_init_status(p_muxer);

	ret = mp4_file_muxer_write_file_type_box(p_muxer);

	p_muxer->b_start = 1;

	return GEN_S_OK;
}

int32 mp4_file_muxer_startW(MP4_FILE_MUXER_H muxer_h, int16* p_file_path, int32 file_path_size)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;
	GEN_FILE_OPEN_PARAM open_param;

	int32 ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( p_muxer->b_start )
	{
		mp4_file_muxer_stop(muxer_h);
	}

	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */

		/** video */
		if( p_muxer->video.b_init_frag_mp4 )
		{
			mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
			mp4_file_muxer_deinit_video_deque(p_muxer, &p_muxer->video);
			mp4_file_muxer_deinit_video_frag(p_muxer, &p_muxer->video);

			p_muxer->video.b_init_frag_mp4 = 0;
		}

		if( !p_muxer->video.b_init_mp4 )
		{
			ret = mp4_file_muxer_init_video_entry(p_muxer, &p_muxer->video);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			p_muxer->video.b_init_mp4 = 1;
		}

		/** audio */
		if( p_muxer->audio.b_init_frag_mp4 )
		{
			mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
			mp4_file_muxer_deinit_audio_deque(p_muxer, &p_muxer->audio);
			mp4_file_muxer_deinit_audio_frag(p_muxer, &p_muxer->audio);

			p_muxer->audio.b_init_frag_mp4 = 0;
		}

		if( !p_muxer->audio.b_init_mp4 )
		{
			ret = mp4_file_muxer_init_audio_entry(p_muxer, &p_muxer->audio);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			p_muxer->audio.b_init_mp4 = 1;
		}

		open_param.rw = GEN_FILE_OPEN_WRITE;
		open_param.share = GEN_FILE_SHARE_READ;
		open_param.create = GEN_FILE_CREATE_ALWAYS;
		open_param.open_flag = 0;
		ret = gen_file_set_param(p_muxer->file, GEN_FILE_PARAM_OPEN, (uint8*)&open_param, sizeof(open_param));

		ret = gen_file_openW(p_muxer->file, p_file_path, file_path_size);
		if( ret )
		{
			return GEN_E_FAIL;
		}

	}else
	{
		/** frag mp4 */

		/** video */
		if( p_muxer->video.b_init_mp4 )
		{
			mp4_file_muxer_deinit_video_entry(p_muxer, &p_muxer->video);

			p_muxer->video.b_init_mp4 = 0;
		}

		if( !p_muxer->video.b_init_frag_mp4 )
		{
			ret = mp4_file_muxer_init_video_data_buf(p_muxer, &p_muxer->video);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_video_deque(p_muxer, &p_muxer->video);
			if( ret )
			{
				mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_video_frag(p_muxer, &p_muxer->video);
			if( ret )
			{
				mp4_file_muxer_deinit_video_data_buf(p_muxer, &p_muxer->video);
				mp4_file_muxer_deinit_video_deque(p_muxer, &p_muxer->video);
				return GEN_E_FAIL;
			}

			p_muxer->video.b_init_frag_mp4 = 1;
		}

		/** audio */
		if( p_muxer->audio.b_init_mp4 )
		{
			mp4_file_muxer_deinit_audio_entry(p_muxer, &p_muxer->audio);

			p_muxer->audio.b_init_mp4 = 0;
		}

		if( !p_muxer->audio.b_init_frag_mp4 )
		{
			ret = mp4_file_muxer_init_audio_data_buf(p_muxer, &p_muxer->audio);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_audio_deque(p_muxer, &p_muxer->audio);
			if( ret )
			{
				mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
				return GEN_E_FAIL;
			}

			ret = mp4_file_muxer_init_audio_frag(p_muxer, &p_muxer->audio);
			if( ret )
			{
				mp4_file_muxer_deinit_audio_data_buf(p_muxer, &p_muxer->audio);
				mp4_file_muxer_deinit_audio_deque(p_muxer, &p_muxer->audio);
				return GEN_E_FAIL;
			}

			p_muxer->audio.b_init_frag_mp4 = 1;
		}
	}

	ret = mp4_file_muxer_init_status(p_muxer);

	ret = mp4_file_muxer_write_file_type_box(p_muxer);

	p_muxer->b_start = 1;

	return GEN_S_OK;
}

int32 mp4_file_muxer_stop(MP4_FILE_MUXER_H muxer_h)
{
	MP4_FILE_MUXER* p_muxer = (MP4_FILE_MUXER*)muxer_h;

	int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_muxer->b_start )
	{
		return GEN_S_OK;
	}

	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */
		ret = mp4_file_muxer_stop_mp4(p_muxer);

	}else
	{
		/** frag mp4 */
		ret = mp4_file_muxer_stop_frag_mp4(p_muxer);
	}

	return GEN_S_OK;
}

int32 mp4_file_muxer_input_frame_mp4(MP4_FILE_MUXER* p_muxer, SP_MEDIA_FRAME* p_frame)
{
	//MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	uint32 uint32_data;
	uint32 write_size;

	H264_NALU_PACK nalu_pack;
	uint8* p_uint8 = NULL;
	int32  parse_size;
	int32  total_parse_size;
	int32  left_size;
	int32  nalu_size;

	uint8* p_frame_data = NULL;
	uint32 frame_size;

	int32  ret;

	if( !p_muxer->video.b_get_key_frame )
	{
		if( p_frame->format == SP_MEDIA_FORMAT_VIDEO && p_frame->payload.video.type == SP_VIDEO_I_FRAME )
		{
			p_muxer->video.b_get_key_frame = 1;

		}else if( p_frame->format == SP_MEDIA_FORMAT_AUDIO && p_muxer->b_reserve_audio )
		{
			/** reserve audio */

		}else
		{
			return GEN_S_OK;
		}
	}

	switch( p_frame->format )
	{
	case SP_MEDIA_FORMAT_VIDEO:
		{
			if( !p_muxer->b_enable_video )
			{
				return GEN_S_OK;
			}

			if( !p_muxer->b_get_video )
			{
				p_muxer->b_get_video = 1;

				p_muxer->track[p_muxer->track_count] = MP4_TRACK_TYPE_VIDEO;
				p_muxer->track_count++;
				memcpy(&p_muxer->video.param, &p_frame->payload.video, sizeof(p_muxer->video.param));

				p_muxer->video.sync_frame_ts = p_frame->payload.video.ts;
				//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_mp4] video sync ts = %I64d\n"), p_muxer->video.sync_frame_ts);

				switch( p_muxer->video.param.format )
				{
				case SP_VIDEO_FORMAT_MPEG2:
					{
						//fourcc = MAKE_FOURCC('m','p','g','v');
						p_muxer->video.fourcc = GEN_FOURCC('M','P','E','G');
					}
					break;

				case SP_VIDEO_FORMAT_MPEG4:
					{
						//fourcc = MAKE_FOURCC('m','p','4','v');
						p_muxer->video.fourcc = GEN_FOURCC('X','V','I','D');
					}
					break;

				case SP_VIDEO_FORMAT_H264:
				case SP_VIDEO_FORMAT_H264_MP4:
					{
						p_muxer->video.fourcc = GEN_FOURCC('a','v','c','1');

						h264_nalu_pack_init(&nalu_pack);
						p_uint8 = p_frame->payload.video.p_data;
						total_parse_size = 0;
						left_size = p_frame->payload.video.data_size;

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
									if( !p_muxer->video.b_get_sps )
									{
										p_muxer->video.b_get_sps = 1;
										p_muxer->video.profile = nalu_pack.sps.profile;
										p_muxer->video.profile_compat = 0xD0;
										p_muxer->video.level = nalu_pack.sps.level;
										memcpy(p_muxer->video.p_sps_buf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										p_muxer->video.sps_nalu_size = nalu_size;
									}
								}

							}else if( nalu_pack.nalu_type == H264_NALU_PPS )
							{
								if( nalu_size <= H264_MAX_PPS_NALU_SIZE )
								{
									if( !p_muxer->video.b_get_pps )
									{
										p_muxer->video.b_get_pps = 1;
										memcpy(p_muxer->video.p_pps_buf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										p_muxer->video.pps_nalu_size = nalu_size;
									}
								}
							}
						}
					}
					break;

				default:
					{
						p_muxer->video.fourcc = GEN_FOURCC('m','p','g','v');
					}
					break;
				}

			}//end if( !p_muxer->b_get_video )

			if( p_muxer->video.frame_count < MP4_FILE_MUXER_MAX_VIDEO_INDEX )
			{
				p_muxer->video.p_time_entry[p_muxer->video.frame_count].sample_count = hton_u32(0x1);
				if( p_muxer->video.frame_count > 0 )
				{
					//uint32_data = p_frame->payload.video.ts - p_muxer->video.total_elapse_time;
					uint32_data = p_frame->payload.video.ts - p_muxer->video.last_frame_ts;
					p_muxer->video.p_time_entry[p_muxer->video.frame_count - 1].sample_delta = hton_u32(uint32_data);
				}

				p_muxer->video.p_size_entry[p_muxer->video.frame_count].entry_size = hton_u32(p_frame->payload.video.data_size);
				p_muxer->video.p_offset_entry[p_muxer->video.frame_count].chunk_offset = hton_u32(p_muxer->total_write_size);

				if( p_frame->payload.video.type == SP_VIDEO_I_FRAME
					&& p_muxer->video.sync_entry_count < MP4_FILE_MUXER_MAX_VIDEO_SYNC_ENTRY_NUM )
				{
					p_muxer->video.p_sync_entry[p_muxer->video.sync_entry_count].sample_number = hton_u32(p_muxer->video.frame_count + 1);
					p_muxer->video.sync_entry_count++;
				}
			}

			p_muxer->video.frame_count++;
			p_muxer->video.total_frame_size += p_frame->payload.video.data_size;
			p_muxer->video.last_frame_ts = p_frame->payload.video.ts;
			p_muxer->video.total_elapse_time = p_frame->payload.video.ts - p_muxer->video.sync_frame_ts;

			p_frame_data = p_frame->payload.video.p_data;
			frame_size = p_frame->payload.video.data_size;
		}
		break;

	case SP_MEDIA_FORMAT_AUDIO:
		{
			if( !p_muxer->b_enable_audio )
			{
				return GEN_S_OK;
			}

			if( !p_muxer->b_get_audio )
			{
				p_muxer->b_get_audio = 1;

				p_muxer->track[p_muxer->track_count] = MP4_TRACK_TYPE_AUDIO;
				p_muxer->track_count++;
				memcpy(&p_muxer->audio.param, &p_frame->payload.audio, sizeof(p_muxer->audio.param));

				p_muxer->audio.sync_frame_ts = p_frame->payload.audio.ts;
				//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_mp4] audio sync ts = %I64d\n"), p_muxer->audio.sync_frame_ts);

				switch( p_muxer->audio.param.format )
				{
				case SP_AUDIO_FORMAT_PCM:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('l','p','c','m');
					}
					break;

				case SP_AUDIO_FORMAT_ADPCM:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('a','p','c','m');
					}
					break;

				case SP_AUDIO_FORMAT_G711A:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('a','l','a','w');
						//m_AudioFourcc = MAKE_FOURCC(0x06,0x00,0x00,0x00);
					}
					break;

				case SP_AUDIO_FORMAT_G711U:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('u','l','a','w');
					}
					break;

				case SP_AUDIO_FORMAT_MPEG1:
				case SP_AUDIO_FORMAT_MPEG2:
					{
						//m_AudioFourcc = MAKE_FOURCC(0x50,0x00,0x00,0x00);
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','g','a');
					}
					break;

				case SP_AUDIO_FORMAT_AAC_RAW:
				case SP_AUDIO_FORMAT_AAC_ADTS:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','4','a');
					}
					break;

				default:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','g','a');
					}
					break;
				}
			}

			if( p_muxer->audio.frame_count < MP4_FILE_MUXER_MAX_AUDIO_INDEX )
			{
				p_muxer->audio.p_time_entry[p_muxer->audio.frame_count].sample_count = hton_u32(0x1);
				if( p_muxer->audio.frame_count > 0 )
				{
					//uint32_data = p_frame->payload.audio.ts - p_muxer->audio.total_elapse_time;
					uint32_data = p_frame->payload.audio.ts - p_muxer->audio.last_frame_ts;
					p_muxer->audio.p_time_entry[p_muxer->audio.frame_count - 1].sample_delta = hton_u32(uint32_data);
				}

				p_muxer->audio.p_size_entry[p_muxer->audio.frame_count].entry_size = hton_u32(p_frame->payload.audio.data_size);
				p_muxer->audio.p_offset_entry[p_muxer->audio.frame_count].chunk_offset  = hton_u32(p_muxer->total_write_size);
			}

			p_muxer->audio.frame_count++;
			p_muxer->audio.total_frame_size += p_frame->payload.audio.data_size;
			p_muxer->audio.last_frame_ts = p_frame->payload.audio.ts;
			p_muxer->audio.total_elapse_time = p_frame->payload.audio.ts - p_muxer->audio.sync_frame_ts;
			p_muxer->audio.total_sample_count += p_frame->payload.audio.sample_count;

			p_frame_data = p_frame->payload.audio.p_data;
			frame_size = p_frame->payload.audio.data_size;
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	/** rewrite media data box size */
	p_muxer->total_frame_size += frame_size;

#if 0
	ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_BEGIN, p_muxer->media_data_offset, NULL);

	uint32_data = p_muxer->total_frame_size + p_muxer->media_data_box_size;
	uint32_data = hton_u32(uint32_data);
	ret = gen_file_write(p_muxer->file, (int8*)&uint32_data, sizeof(uint32_data), &write_size);

	ret = gen_file_seek(p_muxer->file, GEN_FILE_SEEK_END, 0, NULL);
#endif

	/** write frame */
	ret = gen_file_write(p_muxer->file, p_frame_data, frame_size, &write_size);
	p_muxer->total_write_size += frame_size;

	return GEN_S_OK;
}

int32 mp4_file_muxer_input_frame_frag_mp4(MP4_FILE_MUXER* p_muxer, SP_MEDIA_FRAME* p_frame)
{
	MP4_FILE_VIDEO* p_video = &p_muxer->video;
	MP4_FILE_AUDIO* p_audio = &p_muxer->audio;

	H264_NALU_PACK nalu_pack;
	uint8* p_uint8 = NULL;
	int32  parse_size;
	int32  total_parse_size;
	int32  left_size;
	int32  nalu_size;

	int32  deque_size;
	int8*  p_block = NULL;
	SP_MEDIA_FRAME* p_temp_frame = NULL;
	SP_MEDIA_FRAME* p_next_frame = NULL;
	int8*  p_start = NULL;
	int32  b_next_gop;

	uint8* p_frame_data = NULL;
	uint32 frame_size;
	int64  audio_duration;

	int32  i;
	int32  ret;

	if( !p_muxer->video.b_get_key_frame )
	{
		if( p_frame->format == SP_MEDIA_FORMAT_VIDEO )
		{
			if( p_frame->payload.video.type == SP_VIDEO_I_FRAME )
			{

			}else
			{
				/** wait for first key frame */
				return GEN_S_OK;
			}

		}else if( p_frame->format == SP_MEDIA_FORMAT_AUDIO && p_muxer->b_reserve_audio )
		{
			/** reserve audio */

		}else
		{
			/** wait for first key frame */
			return GEN_S_OK;
		}
	}

	switch( p_frame->format )
	{
	case SP_MEDIA_FORMAT_VIDEO:
		{
			if( !p_muxer->b_enable_video )
			{
				return GEN_S_OK;
			}

			if( !p_muxer->b_get_video )
			{
				p_muxer->b_get_video = 1;

				p_muxer->track[p_muxer->track_count] = MP4_TRACK_TYPE_VIDEO;
				p_muxer->track_count++;
				memcpy(&p_muxer->video.param, &p_frame->payload.video, sizeof(p_muxer->video.param));

				p_muxer->video.sync_frame_ts = p_frame->payload.video.ts;
				//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] video sync ts = %I64d\n"), p_muxer->video.sync_frame_ts);

				switch( p_muxer->video.param.format )
				{
				case SP_VIDEO_FORMAT_MPEG2:
					{
						//fourcc = MAKE_FOURCC('m','p','g','v');
						p_muxer->video.fourcc = GEN_FOURCC('M','P','E','G');
					}
					break;

				case SP_VIDEO_FORMAT_MPEG4:
					{
						//fourcc = MAKE_FOURCC('m','p','4','v');
						p_muxer->video.fourcc = GEN_FOURCC('X','V','I','D');
					}
					break;

				case SP_VIDEO_FORMAT_H264:
				case SP_VIDEO_FORMAT_H264_MP4:
					{
						p_muxer->video.fourcc = GEN_FOURCC('a','v','c','1');

						h264_nalu_pack_init(&nalu_pack);
						p_uint8 = p_frame->payload.video.p_data;
						total_parse_size = 0;
						left_size = p_frame->payload.video.data_size;

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
									if( !p_muxer->video.b_get_sps )
									{
										p_muxer->video.b_get_sps = 1;
										p_muxer->video.profile = nalu_pack.sps.profile;
										p_muxer->video.profile_compat = 0xD0;
										p_muxer->video.level = nalu_pack.sps.level;
										memcpy(p_muxer->video.p_sps_buf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										p_muxer->video.sps_nalu_size = nalu_size;
									}
								}

							}else if( nalu_pack.nalu_type == H264_NALU_PPS )
							{
								if( nalu_size <= H264_MAX_PPS_NALU_SIZE )
								{
									if( !p_muxer->video.b_get_pps )
									{
										p_muxer->video.b_get_pps = 1;
										memcpy(p_muxer->video.p_pps_buf, nalu_pack.p_start + H264_BS_HEADER_SIZE, nalu_size);
										p_muxer->video.pps_nalu_size = nalu_size;
									}
								}
							}
						}
					}
					break;

				default:
					{
						p_muxer->video.fourcc = GEN_FOURCC('m','p','g','v');
					}
					break;
				}

			}//end if( !p_muxer->b_get_video )

			ret = gen_deque_get_size(&p_video->data_deque, &deque_size);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			if( deque_size >= p_video->data_deque_total_size )
			{
				return GEN_E_FAIL;
			}

			p_frame_data = p_frame->payload.video.p_data;
			frame_size = p_frame->payload.video.data_size;

			p_block = (int8*)malloc( sizeof(SP_MEDIA_FRAME) + frame_size + 4 );
			if( p_block == NULL )
			{
				return GEN_E_NOT_ENOUGH_MEM;
			}

			p_temp_frame = (SP_MEDIA_FRAME*)p_block;
			memcpy(p_temp_frame, p_frame, sizeof(SP_MEDIA_FRAME));

			p_start = p_block + sizeof(SP_MEDIA_FRAME);
			memcpy(p_start, p_frame_data, frame_size);
			p_temp_frame->payload.video.p_data = p_start;
			p_temp_frame->payload.video.data_size = frame_size;

			ret = gen_deque_push_back_data(&p_video->data_deque, p_block);

			p_muxer->video.frame_count++;
			p_muxer->video.total_frame_size += p_frame->payload.video.data_size;
			p_muxer->video.last_frame_ts = p_frame->payload.video.ts;
			p_muxer->video.total_elapse_time = p_frame->payload.video.ts - p_muxer->video.sync_frame_ts;
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] video frame ts = %I64d\n"), p_frame->payload.video.ts);
		}
		break;

	case SP_MEDIA_FORMAT_AUDIO:
		{
			if( !p_muxer->b_enable_audio )
			{
				return GEN_S_OK;
			}

			if( !p_muxer->b_get_audio )
			{
				p_muxer->b_get_audio = 1;

				p_muxer->track[p_muxer->track_count] = MP4_TRACK_TYPE_AUDIO;
				p_muxer->track_count++;
				memcpy(&p_muxer->audio.param, &p_frame->payload.audio, sizeof(p_muxer->audio.param));

				p_muxer->audio.sync_frame_ts = p_frame->payload.audio.ts;
				//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] audio format = %d, chn = %d, sample rate = %d, sync ts = %I64d\n"),
				//	p_frame->payload.audio.format, p_frame->payload.audio.param.chn, p_frame->payload.audio.param.sample_rate, p_muxer->audio.sync_frame_ts);

				switch( p_muxer->audio.param.format )
				{
				case SP_AUDIO_FORMAT_PCM:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('l','p','c','m');
					}
					break;

				case SP_AUDIO_FORMAT_ADPCM:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('a','p','c','m');
					}
					break;

				case SP_AUDIO_FORMAT_G711A:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('a','l','a','w');
						//m_AudioFourcc = MAKE_FOURCC(0x06,0x00,0x00,0x00);
					}
					break;

				case SP_AUDIO_FORMAT_G711U:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('u','l','a','w');
					}
					break;

				case SP_AUDIO_FORMAT_MPEG1:
				case SP_AUDIO_FORMAT_MPEG2:
					{
						//m_AudioFourcc = MAKE_FOURCC(0x50,0x00,0x00,0x00);
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','g','a');
					}
					break;

				case SP_AUDIO_FORMAT_AAC_RAW:
				case SP_AUDIO_FORMAT_AAC_ADTS:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','4','a');
						//m_AudioSamplePerFrame = 1024;
					}
					break;

				default:
					{
						p_muxer->audio.fourcc = GEN_FOURCC('m','p','g','a');
						//m_AudioSamplePerFrame = 1024;
					}
					break;
				}
			}

			ret = gen_deque_get_size(&p_audio->data_deque, &deque_size);
			if( ret )
			{
				return GEN_E_FAIL;
			}

			if( deque_size >= p_audio->data_deque_total_size )
			{
				return GEN_E_FAIL;
			}

			p_frame_data = p_frame->payload.audio.p_data;
			frame_size = p_frame->payload.audio.data_size;

			p_block = (int8*)malloc( sizeof(SP_MEDIA_FRAME) + frame_size + 4 );
			if( p_block == NULL )
			{
				return GEN_E_NOT_ENOUGH_MEM;
			}

			p_temp_frame = (SP_MEDIA_FRAME*)p_block;
			memcpy(p_temp_frame, p_frame, sizeof(SP_MEDIA_FRAME));

			p_start = p_block + sizeof(SP_MEDIA_FRAME);
			memcpy(p_start, p_frame_data, frame_size);
			p_temp_frame->payload.audio.p_data = p_start;
			p_temp_frame->payload.audio.data_size = frame_size;

			ret = gen_deque_push_back_data(&p_audio->data_deque, p_block);

			p_muxer->audio.frame_count++;
			p_muxer->audio.total_frame_size += p_frame->payload.audio.data_size;
			p_muxer->audio.last_frame_ts = p_frame->payload.audio.ts;
			p_muxer->audio.total_elapse_time = p_frame->payload.audio.ts - p_muxer->audio.sync_frame_ts;
			p_muxer->audio.total_sample_count += p_frame->payload.audio.sample_count;
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] audio frame ts = %I64d\n"), p_frame->payload.audio.ts);
		}
		break;

	default:
		{
			return GEN_E_INVALID_PARAM;
		}
	}

	if( p_frame->format == SP_MEDIA_FORMAT_VIDEO && p_frame->payload.video.type == SP_VIDEO_I_FRAME )
	{
		if( !p_video->b_get_key_frame )
		{
			p_video->b_get_key_frame = 1;

		}else
		{
			p_muxer->gop_count++;
			p_muxer->total_gop_count++;

			if( !p_muxer->b_gop )
			{
				p_muxer->b_gop = 1;
				p_muxer->last_gop_ts = p_video->total_elapse_time;
			}
		}
	}

	if( !p_muxer->b_gop )
	{
		return GEN_S_OK;
	}

	if( p_muxer->b_force_sync )
	{
		if( p_audio->param.timescale == p_video->param.timescale )
		{
			audio_duration = p_audio->total_elapse_time;

		}else
		{
			audio_duration = p_audio->total_elapse_time * p_video->param.timescale / p_audio->param.timescale;
		}

		if( audio_duration < p_muxer->last_gop_ts )
		{
			return GEN_S_OK;
		}
	}

	/** write video gop */
	p_video->data_buf_use_size = 0;
	p_video->data_buf_left_size = p_video->data_buf_size;
	p_video->frag_sample_entry_count = 0;
	b_next_gop = 0;
	while( 1 )
	{
		ret = gen_deque_get_front_data(&p_video->data_deque, (void**)&p_temp_frame);
		if( ret )
		{
			break;
		}

		if( p_temp_frame->payload.video.type == SP_VIDEO_I_FRAME )
		{
			if( b_next_gop )
			{
				/** find next gop */
				break;

			}else
			{
				b_next_gop = 1;
			}
		}

		p_frame_data = p_temp_frame->payload.video.p_data;
		frame_size = p_temp_frame->payload.video.data_size;

		if( frame_size < p_video->data_buf_left_size )
		{
			memcpy(p_video->p_data_buf + p_video->data_buf_use_size, p_frame_data, frame_size);
			p_video->data_buf_use_size += frame_size;
			p_video->data_buf_left_size -= frame_size;

		}else
		{
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] video size = %d too large\n"), frame_size);
			goto NEXT1;
		}

		if( p_video->frag_sample_entry_count < MP4_FILE_MUXER_MAX_VIDEO_DATA_DEQUE_SIZE )
		{
			/** next node index is 1  */
			ret = gen_deque_get_data(&p_video->data_deque, 1, (void**)&p_next_frame);
			if( ret == 0 )
			{
				p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_duration = p_next_frame->payload.video.ts - p_temp_frame->payload.video.ts;

			}else
			{
				p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_duration = 0;
			}

			p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_size = frame_size;
			p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_flag = 0;
			if( p_temp_frame->payload.video.flag & SP_VIDEO_FLAG_CTTS )
			{
				/** ctts */
				p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_composition_time_offset = p_temp_frame->payload.video.flag2;

			}else
			{
				/** no ctts */
				p_video->p_frag_sample_entry[p_video->frag_sample_entry_count].sample_composition_time_offset = 0;
			}

			p_video->frag_sample_entry_count++;

		}else
		{
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] video gop too large!\n"));
		}

NEXT1:
		gen_deque_pop_front(&p_video->data_deque);
		free(p_temp_frame);
		p_temp_frame = NULL;
	}

	/** write audio gop */
	p_audio->data_buf_use_size = 0;
	p_audio->data_buf_left_size = p_audio->data_buf_size;
	p_audio->frag_sample_entry_count = 0;
	while( 1 )
	{
		ret = gen_deque_get_front_data(&p_audio->data_deque, (void**)&p_temp_frame);
		if( ret )
		{
			break;
		}

		if( p_audio->param.timescale == p_video->param.timescale )
		{
			audio_duration = p_temp_frame->payload.audio.ts - p_audio->sync_frame_ts;

		}else
		{
			audio_duration = (p_temp_frame->payload.audio.ts - p_audio->sync_frame_ts) * p_video->param.timescale / p_audio->param.timescale;
		}

		if( audio_duration > p_muxer->last_gop_ts )
		{
			/** out of ts range */
			break;
		}

		p_frame_data = p_temp_frame->payload.audio.p_data;
		frame_size = p_temp_frame->payload.audio.data_size;

		if( frame_size < p_audio->data_buf_left_size )
		{
			memcpy(p_audio->p_data_buf + p_audio->data_buf_use_size, p_frame_data, frame_size);
			p_audio->data_buf_use_size += frame_size;
			p_audio->data_buf_left_size -= frame_size;

		}else
		{
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] audio size = %d too large\n"), frame_size);
			goto NEXT2;
		}

		if( p_audio->frag_sample_entry_count < MP4_FILE_MUXER_MAX_AUDIO_DATA_DEQUE_SIZE )
		{
			/** next node index is 1  */
			ret = gen_deque_get_data(&p_audio->data_deque, 1, (void**)&p_next_frame);
			if( ret == 0 )
			{
				p_audio->p_frag_sample_entry[p_audio->frag_sample_entry_count].sample_duration = p_next_frame->payload.audio.ts - p_temp_frame->payload.audio.ts;

			}else
			{
				/** keep frame for next gop */
				//p_audio->p_frag_sample_entry[p_audio->frag_sample_entry_count].sample_duration = 1024;
				break;
			}

			p_audio->p_frag_sample_entry[p_audio->frag_sample_entry_count].sample_size = frame_size;
			p_audio->p_frag_sample_entry[p_audio->frag_sample_entry_count].sample_flag = 0;
			p_audio->p_frag_sample_entry[p_audio->frag_sample_entry_count].sample_composition_time_offset = 0;

			p_audio->frag_sample_entry_count++;

		}else
		{
			//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_frag_mp4] audio gop too large!\n"));
		}

NEXT2:
		gen_deque_pop_front(&p_audio->data_deque);
		free(p_temp_frame);
		p_temp_frame = NULL;
	}

	ret = mp4_file_muxer_write_fragment(p_muxer);

	p_muxer->gop_count--;
	p_muxer->b_gop = 0;

	if( p_muxer->gop_count > 0 )
	{
		/** update last gop ts */
		b_next_gop = 0;
		i = 0;
		while( 1 )
		{
			ret = gen_deque_get_data(&p_video->data_deque, i, (void**)&p_temp_frame);
			if( ret )
			{
				break;
			}

			if( p_temp_frame->payload.video.type == SP_VIDEO_I_FRAME )
			{
				if( b_next_gop )
				{
					p_muxer->b_gop = 1;
					p_muxer->last_gop_ts = p_temp_frame->payload.video.ts - p_video->sync_frame_ts;
					break;

				}else
				{
					b_next_gop = 1;
				}
			}

			i++;
		}
	}

	return GEN_S_OK;
}

int32 mp4_file_muxer_input_frame(MP4_FILE_MUXER_H muxer_h, SP_MEDIA_FRAME* p_frame)
{
	MP4_FILE_MUXER *p_muxer = (MP4_FILE_MUXER*)muxer_h;

	int32  ret;

	if( p_muxer == NULL || p_frame == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_muxer->b_start )
	{
		return GEN_E_INVALID_STATUS;
	}

	if( p_muxer->file_type == MP4_FILE_TYPE_MP4 )
	{
		/** mp4 */
		ret = mp4_file_muxer_input_frame_mp4(p_muxer, p_frame);

	}else
	{
		/** frag mp4 */
		ret = mp4_file_muxer_input_frame_frag_mp4(p_muxer, p_frame);
	}

	return ret;
}
