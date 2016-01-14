

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <public/gen_fourcc.h>
#include <public/gen_endian.h>
#include <public/gen_deque.h>
#include <public/gen_buf.h>
#include <public/bit_stream_writer.h>
#include <public/gen_file_const.h>
#include <public/gen_file.h>
#include <public/gen_error.h>

#include <network/rtmp/flv_const.h>
#include <network/rtmp/flv_mux.h>

#include <Solid/public/sp_video_const.h>
#include <Solid/public/sp_video_format.h>
#include <Solid/public/sp_audio_format.h>
#include <Solid/public/sp_media_format.h>

#include <util/log_agent.h>

//#include "../../mpeg4/mpeg4_public.h"
//#include "../../mpeg4/mpeg4_aac_const.h"
#include "../../h264/h264_public.h"
#include "../../h264/h264_nalu_demux.h"

#include "../mp4/mp4_avc_mux.h"
#include "../mp4/mp4_avc_demux.h"
#include "flv_file_muxer.h"


/** max video data buf size */
#define FLV_FILE_MUXER_MAX_VIDEO_DATA_BUF_SIZE             (32 * 1024 * 1024)
/** max video index */
#define FLV_FILE_MUXER_MAX_VIDEO_INDEX                     (512 * 1024)
/** max video sync entry num */
#define FLV_FILE_MUXER_MAX_VIDEO_SYNC_ENTRY_NUM            (256 * 1024)

/** max audio data buf size */
#define FLV_FILE_MUXER_MAX_AUDIO_DATA_BUF_SIZE             (4 * 1024 * 1024)
/** max audio index */
#define FLV_FILE_MUXER_MAX_AUDIO_INDEX                     (1024 * 1024)

/** write buf size */
#define FLV_FILE_MUXER_WRITE_BUF_SIZE                      (256 * 1024)
/** max avc dec config buf size */
#define FLV_FILE_MUXER_MAX_AVC_DEC_CONFIG_BUF_SIZE         (1024)
/** max string size */
#define FLV_FILE_MUXER_MAX_STR_SIZE                        (255)
/** dec spec info buf size */
#define FLV_FILE_MUXER_MAX_DEC_SPEC_BUF_SIZE               (32)

/** max sps record */
#define FLV_FILE_MUXER_MAX_SPS_RECORD                      (2)
/** max pps record */
#define FLV_FILE_MUXER_MAX_PPS_RECORD                      (2)


/** video */
typedef struct tag_flv_file_video
{
	/** param */
	SP_VIDEO_FRAME param;

	/** init mp4 */
	//int32  b_init_mp4;

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
	GEN_BUF  avc_dec_config;

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

} FLV_FILE_VIDEO, *PFLV_FILE_VIDEO;


/** audio */
typedef struct tag_flv_file_audio
{
	/** param */
	SP_AUDIO_FRAME param;

	/** init mp4 */
	//int32  b_init_mp4;

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

} FLV_FILE_AUDIO, *PFLV_FILE_AUDIO;


/** flv file muxer */
typedef struct tag_flv_file_muxer
{
	/** init */
	int32  b_init;

	/** enable video */
	int32  b_enable_video;
	/** enable audio */
	int32  b_enable_audio;
	/** force sync */
	int32  b_force_sync;

	/** start */
	int32  b_start;

	/** get video */
	int32  b_get_video;
	/** get audio */
	int32  b_get_audio;

	/** video */
	FLV_FILE_VIDEO video;
	/** audio */
	FLV_FILE_AUDIO audio;

	/** total frame size */
	//uint32 total_frame_size;

	/** write buf */
	GEN_BUF  write;

	/** file */
	GEN_FILE_H file;
	/** total write size */
	uint32 total_write_size;

} FLV_FILE_MUXER, *PFLV_FILE_MUXER;


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
static int32 flv_file_muxer_init_video_avc_buf(FLV_FILE_MUXER* p_muxer, FLV_FILE_VIDEO* p_video)
{
	int32   ret;

	gen_buf_setup(&p_video->avc_dec_config);
	ret = gen_buf_init(&p_video->avc_dec_config, FLV_FILE_MUXER_MAX_AVC_DEC_CONFIG_BUF_SIZE);
	if( ret )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

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
static int32 flv_file_muxer_deinit_video_avc_buf(FLV_FILE_MUXER* p_muxer, FLV_FILE_VIDEO* p_video)
{
	gen_buf_deinit(&p_video->avc_dec_config);

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
 * @brief init video
 * @param [in] p_muxer, muxer
 * @param [in] p_video, video
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 flv_file_muxer_init_video(FLV_FILE_MUXER* p_muxer, FLV_FILE_VIDEO* p_video)
{
	int32  ret;

	//p_video->b_init_mp4 = 0;

	ret = flv_file_muxer_init_video_avc_buf(p_muxer, p_video);
	if( ret )
	{
		return ret;
	}

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
static int32 flv_file_muxer_deinit_video(FLV_FILE_MUXER* p_muxer, FLV_FILE_VIDEO* p_video)
{
	flv_file_muxer_deinit_video_avc_buf(p_muxer, p_video);
	//mp4_file_muxer_deinit_video_data_buf(p_muxer, p_video);

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
static int32 flv_file_muxer_init_audio(FLV_FILE_MUXER* p_muxer, FLV_FILE_AUDIO* p_audio)
{
	//int32  ret;

	//p_audio->b_init_mp4 = 0;

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
static int32 flv_file_muxer_deinit_audio(FLV_FILE_MUXER* p_muxer, FLV_FILE_AUDIO* p_audio)
{

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
static int32 flv_file_muxer_init_write_buf(FLV_FILE_MUXER* p_muxer)
{
	int32   ret;

	gen_buf_setup(&p_muxer->write);
	ret = gen_buf_init(&p_muxer->write, FLV_FILE_MUXER_WRITE_BUF_SIZE);
	if( ret )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

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
static int32 flv_file_muxer_deinit_write_buf(FLV_FILE_MUXER* p_muxer)
{
	gen_buf_deinit(&p_muxer->write);

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
static int32 flv_file_muxer_init_file(FLV_FILE_MUXER* p_muxer)
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
static int32 flv_file_muxer_deinit_file(FLV_FILE_MUXER* p_muxer)
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
static int32 flv_file_muxer_init_video_status(FLV_FILE_MUXER* p_muxer, FLV_FILE_VIDEO* p_video)
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
static int32 flv_file_muxer_init_audio_status(FLV_FILE_MUXER* p_muxer, FLV_FILE_AUDIO* p_audio)
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
static int32 flv_file_muxer_init_status(FLV_FILE_MUXER* p_muxer)
{
	int32  ret;

	ret = flv_file_muxer_init_video_status(p_muxer, &p_muxer->video);
	ret = flv_file_muxer_init_audio_status(p_muxer, &p_muxer->audio);

	p_muxer->b_get_video = 0;
	p_muxer->b_get_audio = 0;

	//p_muxer->total_frame_size = 0;
	p_muxer->write.buf_use_size = 0;
	p_muxer->write.buf_left_size = p_muxer->write.buf_valid_size;
	p_muxer->total_write_size = 0;

	return GEN_S_OK;
}


/**  
 * @brief write header
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 flv_file_muxer_write_header(FLV_FILE_MUXER* p_muxer)
{
	FLV_HEADER  flv_header;

	uint8*  p_uint8 = NULL;
	int32   left_size;
	int32   mux_size;

	uint32  uint32_data;
	uint32  write_size;

	int32   ret;

	p_muxer->write.buf_use_size = 0;
	p_muxer->write.buf_left_size = 	p_muxer->write.buf_valid_size;

	p_uint8 = p_muxer->write.p_buf + p_muxer->write.buf_use_size;
	left_size = p_muxer->write.buf_valid_size;

	//flv_header.type_flag = (FLV_HEADER_AUDIO_TAG_PRESENT << FLV_HEADER_AUDIO_FLAG_BIT_POS) | (FLV_HEADER_VIDEO_TAG_PRESENT << FLV_HEADER_VIDEO_FLAG_BIT_POS);
	flv_header.type_flag = (FLV_HEADER_VIDEO_TAG_PRESENT << FLV_HEADER_VIDEO_FLAG_BIT_POS);
	mux_size = left_size;
	ret = flv_mux_header(&flv_header, p_uint8, &mux_size);
	p_uint8 += mux_size;
	left_size -= mux_size;
	p_muxer->write.buf_use_size += mux_size;
	p_muxer->write.buf_left_size -= mux_size;
	p_muxer->total_write_size += mux_size;

	ret = gen_file_write(p_muxer->file, p_muxer->write.p_buf, p_muxer->write.buf_use_size, &write_size);

	return GEN_S_OK;
}

/**  
 * @brief stop
 * @param [in] p_muxer, muxer
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 flv_file_muxer_stop_inner(FLV_FILE_MUXER* p_muxer)
{
	int32  mux_size;
	uint32 total_mux_size;

	int32  b_open;
	uint32 write_size;

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

	//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_stop_mp4] total write %u bytes\n"), p_muxer->total_write_size);

	gen_file_close(p_muxer->file);

FIN:
	p_muxer->b_start = 0;

	return GEN_S_OK;
}

/////////////////////////////// Outter Interface //////////////////////////////
FLV_FILE_MUXER_H flv_file_muxer_create()
{
	FLV_FILE_MUXER* p_muxer = NULL;

	p_muxer = (FLV_FILE_MUXER*)malloc( sizeof(FLV_FILE_MUXER) );

	if( p_muxer == NULL )
	{
		return NULL;
	}

	memset(p_muxer, 0, sizeof(FLV_FILE_MUXER));

	return p_muxer;
}

int32 flv_file_muxer_destroy(FLV_FILE_MUXER_H h)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_init )
	{
		flv_file_muxer_deinit(h);
	}

	free(p_muxer);

	return GEN_S_OK;
}

int32 flv_file_muxer_init(FLV_FILE_MUXER_H h)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

	int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_init )
	{
		return GEN_S_OK;
	}

	p_muxer->b_start = 0;
	p_muxer->b_enable_video = 1;
	p_muxer->b_enable_audio = 1;

	p_muxer->total_write_size = 0;

	ret = flv_file_muxer_init_video(p_muxer, &p_muxer->video);
	if( ret )
	{
		flv_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = flv_file_muxer_init_audio(p_muxer, &p_muxer->audio);
	if( ret )
	{
		flv_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = flv_file_muxer_init_write_buf(p_muxer);
	if( ret )
	{
		flv_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	ret = flv_file_muxer_init_file(p_muxer);
	if( ret )
	{
		flv_file_muxer_deinit(p_muxer);
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_muxer->b_init = 1;

	return GEN_S_OK;
}

int32 flv_file_muxer_deinit(FLV_FILE_MUXER_H h)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

	//int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_muxer->b_start )
	{
		flv_file_muxer_stop(p_muxer);
	}

	flv_file_muxer_deinit_video(p_muxer, &p_muxer->video);
	flv_file_muxer_deinit_audio(p_muxer, &p_muxer->audio);
	flv_file_muxer_deinit_write_buf(p_muxer);
	flv_file_muxer_deinit_file(p_muxer);

	p_muxer->b_init = 0;

	return GEN_S_OK;
}

int32 flv_file_muxer_reset(FLV_FILE_MUXER_H h)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

	int32  ret;

	if( p_muxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_muxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	ret = flv_file_muxer_init_status(p_muxer);

	return GEN_S_OK;
}

int32 flv_file_muxer_enable_video(FLV_FILE_MUXER_H h, int32 b_enable)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

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

int32 flv_file_muxer_enable_audio(FLV_FILE_MUXER_H h, int32 b_enable)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

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

int32 flv_file_muxer_set_avc_dec_config(FLV_FILE_MUXER_H h, uint8* p_data, int32 data_size)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

	AVC_DEC_CONFIG_RECORD  dec_config;
	AVC_SPS_RECORD sps_record[FLV_FILE_MUXER_MAX_SPS_RECORD];
	AVC_PPS_RECORD pps_record[FLV_FILE_MUXER_MAX_PPS_RECORD];

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

	dec_config.sps_count = FLV_FILE_MUXER_MAX_SPS_RECORD;
	dec_config.p_sps = sps_record;
	dec_config.pps_count = FLV_FILE_MUXER_MAX_PPS_RECORD;
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

int32 flv_file_muxer_startA(FLV_FILE_MUXER_H h, int8* p_file_path, int32 file_path_size)
{
	FLV_FILE_MUXER *p_muxer = (FLV_FILE_MUXER*)h;
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
		flv_file_muxer_stop(h);
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

	ret = flv_file_muxer_init_status(p_muxer);

	ret = flv_file_muxer_write_header(p_muxer);

	p_muxer->b_start = 1;

	return GEN_S_OK;
}

int32 flv_file_muxer_startW(FLV_FILE_MUXER_H h, int16* p_file_path, int32 file_path_size)
{
	FLV_FILE_MUXER *p_muxer = (FLV_FILE_MUXER*)h;
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
		flv_file_muxer_stop(h);
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

	ret = flv_file_muxer_init_status(p_muxer);

	ret = flv_file_muxer_write_header(p_muxer);

	p_muxer->b_start = 1;

	return GEN_S_OK;
}

int32 flv_file_muxer_stop(FLV_FILE_MUXER_H h)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

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

	ret = flv_file_muxer_stop_inner(p_muxer);

	return GEN_S_OK;
}

int32 flv_file_muxer_input_frame_inner(FLV_FILE_MUXER* p_muxer, SP_MEDIA_FRAME* p_frame)
{
	H264_NALU_PACK        nalu_pack;
	H264_NALU             nalu;
	AVC_DEC_CONFIG_RECORD avc_dec_config;
	AVC_SPS_RECORD        avc_sps_record;
	AVC_PPS_RECORD        avc_pps_record;

	FLV_TAG_HEADER        tag_header;
	FLV_VIDEO_TAG_HEADER  video_tag_header;

	uint8*  p_uint8 = NULL;
	int32   parse_size;
	int32   total_parse_size;
	int32   left_size;
	int32   mux_size;
	int32   total_mux_size;
	int32   nalu_size;
	int32   tag_size;

	uint8*  p_frame_data = NULL;
	uint32  frame_size;
	uint32  rtmp_ts;

	uint32  uint32_data;
	uint32  write_size;

	int32   ret;

	if( !p_muxer->video.b_get_key_frame )
	{
		if( p_frame->format == SP_MEDIA_FORMAT_VIDEO && p_frame->payload.video.type == SP_VIDEO_I_FRAME )
		{
			p_muxer->video.b_get_key_frame = 1;

		}else if( p_frame->format == SP_MEDIA_FORMAT_AUDIO )
		{
			/** audio */

		}else
		{
			return GEN_S_OK;
		}
	}

	p_muxer->write.buf_use_size = 0;
	p_muxer->write.buf_left_size = 	p_muxer->write.buf_valid_size;
	total_mux_size = 0;

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

				memcpy(&p_muxer->video.param, &p_frame->payload.video, sizeof(p_muxer->video.param));

				//p_muxer->video.sync_frame_ts = p_frame->payload.video.ts;
				p_muxer->video.sync_frame_ts = p_frame->ts;
				log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("video sync ts = %I64d\n"), p_muxer->video.sync_frame_ts);

				switch( p_muxer->video.param.format )
				{
				case SP_VIDEO_FORMAT_H264:
				case SP_VIDEO_FORMAT_H264_MP4:
					{
						h264_nalu_pack_init(&nalu_pack);
						p_uint8 = p_frame->payload.video.p_data;
						total_parse_size = 0;
						left_size = p_frame->payload.video.data_size;

						while( 1 )
						{
							ret = h264_nalu_demux(&nalu_pack, p_uint8, left_size, &parse_size);
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
										//p_muxer->video.profile_compat = 0xD0;
										p_muxer->video.profile_compat = 0x00;
										p_muxer->video.level = nalu_pack.sps.level;

										//ret = h264_nalu_find_header(&nalu, p_uint8, left_size, &parse_size);
										ret = h264_nalu_find_header(&nalu, nalu_pack.p_start, parse_size + left_size, &parse_size);
										if( ret == 0 )
										{
											if( nalu.nalu_size > 0 )
											{
												nalu_size = nalu.nalu_size - nalu.start_code_size;

											}else
											{
												nalu_size = (parse_size - nalu.start_code_size);
											}

										}else
										{
											nalu_size += (parse_size);
										}

										memcpy(p_muxer->video.p_sps_buf, nalu_pack.p_start + nalu.start_code_size, nalu_size);
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

										//ret = h264_nalu_find_header(&nalu, p_uint8, left_size, &parse_size);
										ret = h264_nalu_find_header(&nalu, nalu_pack.p_start, parse_size + left_size, &parse_size);
										if( ret == 0 )
										{
											if( nalu.nalu_size > 0 )
											{
												nalu_size = nalu.nalu_size - nalu.start_code_size;

											}else
											{
												nalu_size = (parse_size - nalu.start_code_size);
											}

										}else
										{
											nalu_size += (parse_size);
										}

										memcpy(p_muxer->video.p_pps_buf, nalu_pack.p_start + nalu.start_code_size, nalu_size);
										p_muxer->video.pps_nalu_size = nalu_size;
									}
								}
							}
						}

						/** avc dec config */
						avc_dec_config.profile = p_muxer->video.profile;
						avc_dec_config.profile_compat = p_muxer->video.profile_compat;
						avc_dec_config.level = p_muxer->video.level;
						avc_dec_config.len_size_minus_1 = 0x3;

						if( p_muxer->video.b_get_sps )
						{
							avc_sps_record.nalu_len = p_muxer->video.sps_nalu_size;
							avc_sps_record.p_nalu = p_muxer->video.p_sps_buf;

							avc_dec_config.sps_count = 1;
							avc_dec_config.p_sps = &avc_sps_record;

						}else
						{
							avc_dec_config.sps_count = 0;
							avc_dec_config.p_sps = NULL;
						}

						if( p_muxer->video.b_get_pps )
						{
							avc_pps_record.nalu_len = p_muxer->video.pps_nalu_size;
							avc_pps_record.p_nalu = p_muxer->video.p_pps_buf;

							avc_dec_config.pps_count = 1;
							avc_dec_config.p_pps = &avc_pps_record;

						}else
						{
							avc_dec_config.pps_count = 0;
							avc_dec_config.p_pps = NULL;
						}

						ret = mp4_avc_calc_dec_config_record_total_size(&avc_dec_config, &mux_size);

						if( p_muxer->video.b_get_sps )
						{
							/** write avc dec config */
							p_uint8 = p_muxer->write.p_buf + p_muxer->write.buf_use_size;
							left_size = p_muxer->write.buf_left_size;
							tag_size = 0;

							/** tag header */
							tag_header.tag_type = FLV_TAG_TYPE_VIDEO;
							tag_header.data_size = FLV_AVC_VIDEO_TAG_HEADER_SIZE + mux_size;
							tag_header.ts = 0x0;
							tag_header.stream_id = 0;

							mux_size = left_size;
							ret = flv_mux_tag_header(&tag_header, p_uint8, &mux_size);
							p_uint8 += mux_size;
							left_size -= mux_size;
							tag_size += mux_size;
							p_muxer->write.buf_use_size += mux_size;
							p_muxer->write.buf_left_size -= mux_size;

							/** video tag header */
							video_tag_header.codec_id = FLV_VIDEO_CODEC_ID_AVC;
							video_tag_header.avc_pack_type = FLV_VIDEO_AVC_SEQ_HEADER;
							video_tag_header.frame_type = FLV_VIDEO_KEY_FRAME;
							video_tag_header.cts_offset = 0x0;

							mux_size = left_size;
							ret = flv_mux_video_tag_header(&video_tag_header, p_uint8, &mux_size);
							p_uint8 += mux_size;
							left_size -= mux_size;
							tag_size += mux_size;
							p_muxer->write.buf_use_size += mux_size;
							p_muxer->write.buf_left_size -= mux_size;

							/** avc dec config */
							mux_size = left_size;
							ret = mp4_avc_dec_config_record_mux(&avc_dec_config, p_uint8, &mux_size);
							p_uint8 += mux_size;
							left_size -= mux_size;
							tag_size += mux_size;
							p_muxer->write.buf_use_size += mux_size;
							p_muxer->write.buf_left_size -= mux_size;

							/** prev tag size */
							uint32_data = hton_u32(tag_size);
							mux_size = sizeof(uint32_data);
							memcpy(p_uint8, &uint32_data, mux_size);
							p_uint8 += mux_size;
							left_size -= mux_size;
							tag_size += mux_size;
							p_muxer->write.buf_use_size += mux_size;
							p_muxer->write.buf_left_size -= mux_size;

							/** write tag */
							ret = gen_file_write(p_muxer->file, p_muxer->write.p_buf, p_muxer->write.buf_use_size, &write_size);
							//total_mux_size += write_size;
							p_muxer->total_write_size += write_size;

							p_muxer->write.buf_use_size = 0;
							p_muxer->write.buf_left_size = 	p_muxer->write.buf_valid_size;
						}

					}
					break;

				default:
					{
					}
					break;
				}



			}//end if( !p_muxer->b_get_video )

			if( p_frame->timescale )
			{
				/** transform to ms */
				rtmp_ts = (p_frame->ts - p_muxer->video.sync_frame_ts) * 1000 / p_frame->timescale;

			}else
			{
				rtmp_ts = 0x0;
			}

			p_muxer->video.frame_count++;
			p_muxer->video.total_frame_size += p_frame->payload.video.data_size;
			p_muxer->video.last_frame_ts = p_frame->payload.video.ts;
			p_muxer->video.total_elapse_time = rtmp_ts;

			p_frame_data = p_frame->payload.video.p_data;
			frame_size = p_frame->payload.video.data_size;

			/** write header */
			p_uint8 = p_muxer->write.p_buf + p_muxer->write.buf_use_size;
			left_size = p_muxer->write.buf_left_size;
			tag_size = 0;

			/** tag header */
			tag_header.tag_type = FLV_TAG_TYPE_VIDEO;
			tag_header.data_size = FLV_AVC_VIDEO_TAG_HEADER_SIZE + frame_size;
			tag_header.ts = rtmp_ts;
			tag_header.stream_id = 0;

			mux_size = left_size;
			ret = flv_mux_tag_header(&tag_header, p_uint8, &mux_size);
			p_uint8 += mux_size;
			left_size -= mux_size;
			tag_size += mux_size;
			p_muxer->write.buf_use_size += mux_size;
			p_muxer->write.buf_left_size -= mux_size;

			/** video tag header */
			video_tag_header.codec_id = FLV_VIDEO_CODEC_ID_AVC;
			video_tag_header.avc_pack_type = FLV_VIDEO_AVC_NALU;

			if( p_frame->payload.video.type == SP_VIDEO_I_FRAME )
			{
				video_tag_header.frame_type = FLV_VIDEO_KEY_FRAME;
				video_tag_header.cts_offset = 0x0;

			}else
			{
				video_tag_header.frame_type = FLV_VIDEO_INTER_FRAME;
				video_tag_header.cts_offset = 0x0;
			}

			mux_size = left_size;
			ret = flv_mux_video_tag_header(&video_tag_header, p_uint8, &mux_size);
			p_uint8 += mux_size;
			left_size -= mux_size;
			tag_size += mux_size;
			p_muxer->write.buf_use_size += mux_size;
			p_muxer->write.buf_left_size -= mux_size;
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

				memcpy(&p_muxer->audio.param, &p_frame->payload.audio, sizeof(p_muxer->audio.param));

				p_muxer->audio.sync_frame_ts = p_frame->payload.audio.ts;
				//log_debug(_T("[mp4_file_muxer::mp4_file_muxer_input_frame_mp4] audio sync ts = %I64d\n"), p_muxer->audio.sync_frame_ts);

				switch( p_muxer->audio.param.format )
				{
				case SP_AUDIO_FORMAT_PCM:
					{
					}
					break;

				case SP_AUDIO_FORMAT_ADPCM:
					{
					}
					break;

				case SP_AUDIO_FORMAT_G711A:
					{
					}
					break;

				case SP_AUDIO_FORMAT_G711U:
					{
					}
					break;

				case SP_AUDIO_FORMAT_MPEG1:
				case SP_AUDIO_FORMAT_MPEG2:
					{

					}
					break;

				case SP_AUDIO_FORMAT_AAC_RAW:
				case SP_AUDIO_FORMAT_AAC_ADTS:
					{
					}
					break;

				default:
					{
					}
					break;
				}
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

	/** write tag header */
	ret = gen_file_write(p_muxer->file, p_muxer->write.p_buf, p_muxer->write.buf_use_size, &write_size);
	//total_mux_size += write_size;
	p_muxer->total_write_size += write_size;

	/** write frame */
	ret = gen_file_write(p_muxer->file, p_frame_data, frame_size, &write_size);
	tag_size += frame_size;
	p_muxer->total_write_size += frame_size;

	/** prev tag size */
	//uint32_data = total_mux_size;
	uint32_data = hton_u32(tag_size);
	ret = gen_file_write(p_muxer->file, (uint8*)&uint32_data, sizeof(uint32_data), &write_size);
	p_muxer->total_write_size += write_size;

	return GEN_S_OK;
}

int32 flv_file_muxer_input_frame(FLV_FILE_MUXER_H h, SP_MEDIA_FRAME* p_frame)
{
	FLV_FILE_MUXER* p_muxer = (FLV_FILE_MUXER*)h;

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

	ret = flv_file_muxer_input_frame_inner(p_muxer, p_frame);

	return ret;
}
