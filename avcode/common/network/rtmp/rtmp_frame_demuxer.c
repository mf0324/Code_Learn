
#include <stdlib.h>
#include <string.h>

#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include <Solid/public/sp_video_format.h>
#include <Solid/public/sp_audio_format.h>
#include <Solid/public/sp_media_format.h>
#include <Solid/parser/multimedia/mp4/mp4_avc_demux.h>
#include <Solid/parser/h264/h264_nalu_demux.h>

#include "rtmp_const.h"
#include "flv_const.h"
#include "flv_demux.h"
#include "rtmp_frame_demuxer.h"

/** max sps record */
#define RTMP_FRAME_DEMUXER_MAX_SPS_RECORD                  (32)
/** max pps record */
#define RTMP_FRAME_DEMUXER_MAX_PPS_RECORD                  (32)

/** print log */
#define RTMP_FRAME_DEMUXER_PRINT_LOG                       (0)


/** RTMP Frame Demuxer */
typedef struct tag_rtmp_frame_demuxer
{
	/** init */
	int32   b_init;

	/** get video */
	int32   b_get_video;
	/** video frame */
	SP_MEDIA_FRAME video_frame;
	/** video frame ts */
	int32   b_video_frame_ts;
	/** fore video ts */
	uint32  fore_video_ts;
	/** video ts overflow count */
	int32   video_ts_overflow_count;
	/** video ts total overflow, unit: ms */
	int64   video_ts_total_overflow;

	/** get audio */
	int32   b_get_audio;
	/** audio frame */
	SP_MEDIA_FRAME audio_frame;
	/** audio frame ts */
	int32   b_audio_frame_ts;
	/** fore audio ts */
	uint32  fore_audio_ts;
	/** audio ts overflow count */
	int32   audio_ts_overflow_count;
	/** audio ts total overflow, unit: ms */
	int64   audio_ts_total_overflow;

} RTMP_FRAME_DEMUXER, *PRTMP_FRAME_DEMUXER;

/**  
 * @brief demux avc dec config
 * @param [in] p_demuxer, demuxer
 * @param [in] p_buf, input data
 * @param [in/out] p_buf_size, input data size/actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 */
int32 rtmp_frame_demux_avc_dec_config(RTMP_FRAME_DEMUXER* p_demuxer, uint8* p_buf, uint32* p_buf_size)
{
	AVC_DEC_CONFIG_RECORD dec_config;
	AVC_SPS_RECORD sps_record[RTMP_FRAME_DEMUXER_MAX_SPS_RECORD];
	AVC_PPS_RECORD pps_record[RTMP_FRAME_DEMUXER_MAX_PPS_RECORD];

	H264_NALU_PACK nalu_pack;

	uint32 left_size;
	uint32 demux_size;
	uint8* p_uint8 = NULL;

	int32  ret;

	left_size = *p_buf_size;
	p_uint8 = p_buf;
	*p_buf_size = 0;

	dec_config.sps_count = RTMP_FRAME_DEMUXER_MAX_SPS_RECORD;
	dec_config.p_sps = sps_record;
	dec_config.pps_count = RTMP_FRAME_DEMUXER_MAX_PPS_RECORD;
	dec_config.p_pps = pps_record;
	demux_size = left_size;
	ret = mp4_avc_dec_config_record_demux(&dec_config, p_uint8, &demux_size);
	if( ret )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	h264_nalu_pack_init(&nalu_pack);
	demux_size = dec_config.p_sps[0].nalu_len;
	ret = h264_demux_sps(&nalu_pack, dec_config.p_sps[0].p_nalu, &demux_size);
	if( ret )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_demuxer->video_frame.payload.video.width = nalu_pack.sps.width;
	p_demuxer->video_frame.payload.video.height = nalu_pack.sps.height;

	return GEN_S_OK;
}

////////////////////////////// Outter Interface ///////////////////////////////
RTMP_FRAME_DEMUXER_H rtmp_frame_demuxer_create()
{
	RTMP_FRAME_DEMUXER *p_demuxer = NULL;
	p_demuxer = (RTMP_FRAME_DEMUXER*)malloc( sizeof(RTMP_FRAME_DEMUXER) );

	if( p_demuxer == NULL )
	{
		return NULL;
	}

	memset(p_demuxer, 0, sizeof(RTMP_FRAME_DEMUXER));

	return p_demuxer;
}


int32 rtmp_frame_demuxer_destroy(RTMP_FRAME_DEMUXER_H h)
{
	RTMP_FRAME_DEMUXER *p_demuxer = (RTMP_FRAME_DEMUXER*)h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		rtmp_frame_demuxer_deinit(p_demuxer);
	}

	free(p_demuxer);

	return GEN_S_OK;
}

int32 rtmp_frame_demuxer_init(RTMP_FRAME_DEMUXER_H h)
{
	RTMP_FRAME_DEMUXER *p_demuxer = (RTMP_FRAME_DEMUXER*)h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		return GEN_S_OK;
	}

	p_demuxer->b_get_video = 0;
	p_demuxer->b_video_frame_ts = 0;
	//p_demuxer->video_ts_overflow_count = 0;
	//p_demuxer->video_ts_total_overflow = 0;

	p_demuxer->b_get_audio = 0;
	p_demuxer->b_audio_frame_ts = 0;
	//p_demuxer->audio_ts_overflow_count = 0;
	//p_demuxer->audio_ts_total_overflow = 0;

	p_demuxer->b_init = 1;

	return GEN_S_OK;
}


int32 rtmp_frame_demuxer_deinit(RTMP_FRAME_DEMUXER_H rtmp_frame_demuxer)
{
	RTMP_FRAME_DEMUXER *p_demuxer = (RTMP_FRAME_DEMUXER*)rtmp_frame_demuxer;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_demuxer->b_init = 0;

	return GEN_S_OK;
}


int32 rtmp_frame_demuxer_reset(RTMP_FRAME_DEMUXER_H rtmp_frame_demuxer)
{
	RTMP_FRAME_DEMUXER *p_demuxer = (RTMP_FRAME_DEMUXER*)rtmp_frame_demuxer;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_demuxer->b_get_video = 0;
	p_demuxer->b_video_frame_ts = 0;
	//p_demuxer->video_ts_overflow_count = 0;
	//p_demuxer->video_ts_total_overflow = 0;

	p_demuxer->b_get_audio = 0;
	p_demuxer->b_audio_frame_ts = 0;
	//p_demuxer->audio_ts_overflow_count = 0;
	//p_demuxer->audio_ts_total_overflow = 0;

	return GEN_S_OK;
}

int32 rtmp_frame_demux(RTMP_FRAME_DEMUXER_H rtmp_frame_demuxer, RTMP_MSG* p_msg, SP_MEDIA_FRAME* p_frame)
{
	RTMP_FRAME_DEMUXER*  p_demuxer = (RTMP_FRAME_DEMUXER*)rtmp_frame_demuxer;

	FLV_VIDEO_TAG_HEADER video_header;
	FLV_AUDIO_TAG_HEADER audio_header;

	int32   b_find;

	uint32  left_size;
	uint32  demux_size;

	uint8*  p_uint8 = NULL;
	uint32  audio_obj_type;
	uint32  sample_freq_index;
	uint32  chn_config;

	int32   ret;

	if( p_demuxer == NULL || p_msg == NULL || p_frame == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	b_find = 0;

	//p_frame->flag = 0x0;
	memset(p_frame, 0, sizeof(*p_frame));

	switch( p_msg->msg_type )
	{
	case RTMP_MSG_TYPE_AUDIO:
		{
			if( p_msg->payload_size < 1 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_uint8 = p_msg->p_payload;
			left_size = p_msg->payload_size;

			demux_size = left_size;
			ret = flv_demux_audio_tag_header(&audio_header, p_uint8, &demux_size);
			if( ret == 0 )
			{
				p_uint8 += demux_size;
				left_size -= demux_size;

				if( audio_header.format == FLV_AUDIO_FORMAT_AAC )
				{
					if( audio_header.aac_pack_type == FLV_AUDIO_AAC_SEQ_HEADER )
					{
						if( !p_demuxer->b_get_audio )
						{
							if( p_msg->payload_size >= 2 )
							{
								/** audio obj type, 5 bit */
								audio_obj_type = ((p_uint8[0] >> 3) & 0x1F);
								switch( audio_obj_type )
								{
								case 1:
									{
										/** AAC Main */
										//p_demuxer->audio_frame.payload.audio.format = MP_AUDIO_AAC_LC;
										p_demuxer->audio_frame.payload.audio.format = SP_AUDIO_FORMAT_AAC_RAW;
									}
									break;

								case 2:
									{
										/** AAC LC */
										//p_demuxer->audio_frame.param.audio.algo = MP_AUDIO_AAC_LC;
										p_demuxer->audio_frame.payload.audio.format = SP_AUDIO_FORMAT_AAC_RAW;
									}
									break;

								default:
									{
										//p_demuxer->audio_frame.param.audio.algo = MP_AUDIO_AAC_LC;
										p_demuxer->audio_frame.payload.audio.format = SP_AUDIO_FORMAT_AAC_RAW;
									}
									break;
								}

								/** sample freq index, 4 bit */
								sample_freq_index = ((p_uint8[0] & 0x7) << 1) | ((p_uint8[1] >> 7) & 0x1);
								switch( sample_freq_index )
								{
								case 0:
									{
										/** 96000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 96000;
									}
									break;

								case 2:
									{
										/** 64000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 64000;
									}
									break;

								case 3:
									{
										/** 48000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 48000;
									}
									break;

								case 4:
									{
										/** 44100 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 44100;
									}
									break;

								case 5:
									{
										/** 32000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 32000;
									}
									break;

								case 6:
									{
										/** 24000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 24000;
									}
									break;

								case 7:
									{
										/** 22050 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 22050;
									}
									break;

								case 8:
									{
										/** 16000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 16000;
									}
									break;

								case 9:
									{
										/** 12000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 12000;
									}
									break;

								case 0xA:
									{
										/** 11025 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 11025;
									}
									break;

								default:
									{
										/** 48000 */
										p_demuxer->audio_frame.payload.audio.param.sample_rate = 48000;
									}
									break;
								}

								/** chn config, 4 bit */
								chn_config = ((p_uint8[1] >> 3) & 0xF);
								switch( chn_config )
								{
								case 1:
									{
										/** single chn */
										p_demuxer->audio_frame.payload.audio.param.chn = 1;
									}
									break;

								case 2:
									{
										/** dual chn */
										p_demuxer->audio_frame.payload.audio.param.chn = 2;
									}
									break;

								default:
									{
										/** dual chn */
										p_demuxer->audio_frame.payload.audio.param.chn = 2;
									}
									break;
								}

								p_demuxer->audio_frame.payload.audio.param.sample_bit = SP_AUDIO_SAMPLE_SIZE_16BIT;
								p_demuxer->audio_frame.payload.audio.sample_count = 1024;
								p_demuxer->audio_frame.payload.audio.flag = 0x0;
								p_demuxer->audio_frame.payload.audio.timescale = p_demuxer->audio_frame.payload.audio.param.sample_rate;
								p_demuxer->audio_frame.payload.audio.duration = 1024;
								p_demuxer->audio_frame.payload.audio.ts = 0;

								p_demuxer->b_get_audio = 1;

							}//end if( p_msg->payload_size >= 2 )

						}// if( !p_demuxer->b_get_audio )

					}else if( audio_header.aac_pack_type == FLV_AUDIO_AAC_RAW )
					{
						if( p_demuxer->b_get_audio )
						{
							b_find = 1;

							memcpy(&p_frame->payload.audio, &p_demuxer->audio_frame.payload.audio, sizeof(p_demuxer->audio_frame.payload.audio));
							p_frame->format = SP_MEDIA_FORMAT_AUDIO;
							p_frame->p_data = p_uint8;
							p_frame->data_size = left_size;
							p_frame->payload.audio.p_data = p_uint8;
							p_frame->payload.audio.data_size = left_size;

							if( !p_demuxer->b_audio_frame_ts )
							{
								p_demuxer->b_audio_frame_ts = 1;
								p_demuxer->audio_ts_overflow_count = 0;
								p_demuxer->audio_ts_total_overflow = 0;

							}else
							{
								if( p_msg->ts < p_demuxer->fore_audio_ts )
								{
									/** overflow */
									p_demuxer->audio_ts_overflow_count++;
									p_demuxer->audio_ts_total_overflow += UINT32_OVERFLOW;
									//log_debug(_T("[rtmp_frame_demuxer::rtmp_frame_demux] audio ts overflow count = %d\n"), p_demuxer->audio_ts_overflow_count);
								}
							}

							p_frame->timescale = 1000;
							p_frame->ts = p_msg->ts + p_demuxer->audio_ts_total_overflow;
							p_frame->payload.audio.timescale = 1000;
							p_frame->payload.audio.ts = p_frame->ts;
							p_demuxer->fore_audio_ts = p_msg->ts;

#if RTMP_FRAME_DEMUXER_PRINT_LOG
							//log_debug(_T("[rtmp_frame_demuxer::rtmp_frame_demux] audio msg ts = %u frame ts = %I64d\n"), p_msg->ts, p_frame->ts);
#endif
						}
					}
				}
			}
		}
		break;

	case RTMP_MSG_TYPE_VIDEO:
		{
			if( p_msg->payload_size < 5 )
			{
				return GEN_E_NEED_MORE_DATA;
			}

			p_uint8 = p_msg->p_payload;
			left_size = p_msg->payload_size;

			demux_size = left_size;
			ret = flv_demux_video_tag_header(&video_header, p_uint8, &demux_size);
			if( ret == 0 )
			{
				p_uint8 += demux_size;
				left_size -= demux_size;

				if( video_header.codec_id == FLV_VIDEO_CODEC_ID_AVC )
				{
#if RTMP_FRAME_DEMUXER_PRINT_LOG
					//log_debug(_T("[rtmp_frame_demuxer::rtmp_frame_demux] video ctts offset = %d\n"), video_header.cts_offset);
#endif

					p_frame->payload.video.flag |= SP_VIDEO_FLAG_CTTS;
					p_frame->payload.video.flag2 = video_header.cts_offset;

					if( video_header.avc_pack_type == FLV_VIDEO_AVC_SEQ_HEADER )
					{
						if( !p_demuxer->b_get_video )
						{
							demux_size = left_size;
							ret = rtmp_frame_demux_avc_dec_config(p_demuxer, p_uint8, &demux_size);
							if( ret == 0 )
							{
								p_demuxer->b_get_video = 1;
							}

						}// if( !p_demuxer->b_get_video )

					}else if( video_header.avc_pack_type == FLV_VIDEO_AVC_NALU )
					{
						if( p_demuxer->b_get_video )
						{
							b_find = 1;

							p_frame->format = SP_MEDIA_FORMAT_VIDEO;
							p_frame->p_data = p_uint8;
							p_frame->data_size = left_size;
							p_frame->payload.video.p_data = p_uint8;
							p_frame->payload.video.data_size = left_size;
							p_frame->flag = 0;

							if( !p_demuxer->b_video_frame_ts )
							{
								p_demuxer->b_video_frame_ts = 1;
								p_demuxer->video_ts_overflow_count = 0;
								p_demuxer->video_ts_total_overflow = 0;

							}else
							{
								if( p_msg->ts < p_demuxer->fore_video_ts )
								{
									/** overflow */
									p_demuxer->video_ts_overflow_count++;
									p_demuxer->video_ts_total_overflow += UINT32_OVERFLOW;
									//log_debug(_T("[rtmp_frame_demuxer::rtmp_frame_demux] video ts overflow count = %d\n"), p_demuxer->video_ts_overflow_count);
								}
							}

							p_frame->timescale = 1000;
							p_frame->ts = p_msg->ts + p_demuxer->video_ts_total_overflow;
							p_frame->payload.video.timescale = 1000;
							p_frame->payload.video.ts = p_frame->ts;
							p_demuxer->fore_video_ts = p_msg->ts;

#if RTMP_FRAME_DEMUXER_PRINT_LOG
							//log_debug(_T("[rtmp_frame_demuxer::rtmp_frame_demux] video msg ts = %u frame ts = %I64d\n"), p_msg->ts, p_frame->ts);
#endif

							p_frame->payload.video.format = SP_VIDEO_FORMAT_H264_MP4;
							if( video_header.frame_type == FLV_VIDEO_KEY_FRAME )
							{
								p_frame->payload.video.type = SP_VIDEO_I_FRAME;

							}else
							{
								p_frame->payload.video.type = SP_VIDEO_P_FRAME;
							}
							p_frame->payload.video.width = p_demuxer->video_frame.payload.video.width;
							p_frame->payload.video.height = p_demuxer->video_frame.payload.video.height;
						}
					}

				}else
				{

				}

			}
		}
		break;

	default:
		{
		}
		break;
	}

	if( !b_find )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	return GEN_S_OK;
}
