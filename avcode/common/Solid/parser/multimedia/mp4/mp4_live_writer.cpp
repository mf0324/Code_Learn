


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <public/gen_endian.h>
#include <public/bit_stream_writer.h>
#include <util/log_debug.h>

#include "../../mpeg4/mpeg4_public.h"
#include "../../mpeg4/mpeg4_aac_const.h"
#include "../../h264/h264_public.h"
#include "../../h264/h264_nalu_demux.h"

#include "mp4_avc_demux.h"
#include "mp4_muxer.h"
#include "mp4_live_writer.h"


/** 最大视频索引个数 */
#define MP4_LIVE_WRITER_MAX_VIDEO_INDEX                    (1024)
/** 最大GOP长度 */
#define MP4_LIVE_WRITER_MAX_GOP_SIZE                       (1024)
/** 最大视频队列长度 */
#define MP4_LIVE_WRITER_MAX_VIDEO_DEQUE_SIZE               (MP4_LIVE_WRITER_MAX_VIDEO_INDEX)
/** 视频帧长度列表大小 */
#define MP4_LIVE_WRITER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE   (MP4_LIVE_WRITER_MAX_VIDEO_INDEX * sizeof(ISO_TRACK_FRAGMENT_SAMPLE))
/** 最大音频索引个数 */
#define MP4_LIVE_WRITER_MAX_AUDIO_INDEX                    (32 * 1024)
/** 最大音频队列长度 */
#define MP4_LIVE_WRITER_MAX_AUDIO_DEQUE_SIZE               (MP4_LIVE_WRITER_MAX_AUDIO_INDEX)
/** 音频帧长度列表大小 */
#define MP4_LIVE_WRITER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE   (MP4_LIVE_WRITER_MAX_AUDIO_INDEX * sizeof(ISO_TRACK_FRAGMENT_SAMPLE))


/** 写入缓冲区大小 */
#define MP4_LIVE_WRITER_BUF_SIZE                           (256 * 1024)
/** 最大视频缓冲区大小 */
#define MP4_LIVE_WRITER_MAX_VIDEO_BUF_SIZE                 (64 * 1024 * 1024)
/** 最大音频缓冲区大小 */
#define MP4_LIVE_WRITER_MAX_AUDIO_BUF_SIZE                 (4 * 1024 * 1024)
/** max avc dec config buf size */
#define MP4_LIVE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE        (1024)
/** 填充缓冲区大小 */
#define MP4_LIVE_WRITER_PADDING_BUF_SIZE                   (512)
/** 最大字符串长度 */
#define MP4_LIVE_WRITER_MAX_STR_SIZE                       (255)
/** 名称长度 */
#define MP4_LIVE_WRITER_MAX_NAME_SIZE                      (32)
/** dec spec info buf size */
#define MP4_LIVE_WRITER_MAX_DEC_SPEC_BUF_SIZE              (32)
/** 名称 */
#define MP4_LIVE_WRITER_LOGO_NAME                          "robert"


/** video track id */
#define MP4_LIVE_WRITER_VIDEO_TRACK_ID                     (1)
/** audio track id */
#define MP4_LIVE_WRITER_AUDIO_TRACK_ID                     (2)
/** max track id */
#define MP4_LIVE_WRITER_MAX_TRACK_ID                       (MP4_LIVE_WRITER_AUDIO_TRACK_ID)
/** max sps record */
#define MP4_LIVE_WRITER_MAX_SPS_RECORD                     (2)
/** max pps record */
#define MP4_LIVE_WRITER_MAX_PPS_RECORD                     (2)

/** uni time scale */
//#define MP4_LIVE_WRITER_UNI_TIME_SCALE                     (MP4_TIMESCALE_MPEG2_TS)
//#define MP4_LIVE_WRITER_UNI_TIME_SCALE                     (10 * 1000 * 1000)
#define MP4_LIVE_WRITER_UNI_TIME_SCALE                     (1000)
/** reset movie box */
#define MP4_LIVE_WRITER_RESET_MOVIE_BOX                    (0)

/** gop per file */
#define MP4_LIVE_WRITER_GOP_PER_FILE                       (32)



CMp4LiveWriter::CMp4LiveWriter()
{
	m_bInit = 0;
	m_Muxer = NULL;

	m_bStart = 0;

	m_pAvcDecConfigBuf = NULL;
	m_pSPSBuf = NULL;
	m_pPPSBuf = NULL;

	m_pWriteBuf = NULL;

	m_pVideoBuf = NULL;
	m_pVideoDeque = NULL;
	m_pVideoSizeEntry = NULL;

	m_pAudioBuf = NULL;
	m_pAudioDeque = NULL;
	m_pAudioSizeEntry = NULL;

	m_hFile = INVALID_HANDLE_VALUE;
}

CMp4LiveWriter::~CMp4LiveWriter()
{
	if( m_bInit )
	{
		Deinit();
	}
}

int32 CMp4LiveWriter::Init()
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
	m_pAvcDecConfigBuf = (uint8*)malloc(MP4_LIVE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE);
	if( m_pAvcDecConfigBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_AvcDecConfigBufSize = MP4_LIVE_WRITER_MAX_AVC_DEC_CONFIG_BUF_SIZE;

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

	m_pWriteBuf = (uint8*)malloc(MP4_LIVE_WRITER_BUF_SIZE);
	if( m_pWriteBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_WriteBufSize = MP4_LIVE_WRITER_BUF_SIZE;

	m_pVideoBuf = (uint8*)malloc(MP4_LIVE_WRITER_MAX_VIDEO_BUF_SIZE);
	if( m_pVideoBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_VideoBufSize = MP4_LIVE_WRITER_MAX_VIDEO_BUF_SIZE;
	m_VideoBufUseSize = 0;
	m_VideoBufLeftSize = m_VideoBufSize;

	m_pVideoDeque = gen_deque_create(NULL);
	if( m_pVideoDeque == NULL )
	{
		Deinit();
		return -1;
	}
	gen_deque_init(m_pVideoDeque);
	gen_deque_set_array_param(m_pVideoDeque, MP4_LIVE_WRITER_MAX_VIDEO_DEQUE_SIZE, MP4_LIVE_WRITER_MAX_VIDEO_DEQUE_SIZE);
	m_VideoDequeTotalSize = MP4_LIVE_WRITER_MAX_VIDEO_DEQUE_SIZE;

	m_pVideoSizeEntry = (ISO_TRACK_FRAGMENT_SAMPLE*)malloc(MP4_LIVE_WRITER_VIDEO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( m_pVideoSizeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_pAudioBuf = (uint8*)malloc(MP4_LIVE_WRITER_MAX_AUDIO_BUF_SIZE);
	if( m_pAudioBuf == NULL )
	{
		Deinit();
		return -1;
	}
	m_AudioBufSize = MP4_LIVE_WRITER_MAX_AUDIO_BUF_SIZE;
	m_AudioBufUseSize = 0;
	m_AudioBufLeftSize = m_AudioBufSize;

	m_pAudioDeque = gen_deque_create(NULL);
	if( m_pAudioDeque == NULL )
	{
		Deinit();
		return -1;
	}
	gen_deque_init(m_pAudioDeque);
	gen_deque_set_array_param(m_pAudioDeque, MP4_LIVE_WRITER_MAX_AUDIO_DEQUE_SIZE, MP4_LIVE_WRITER_MAX_AUDIO_DEQUE_SIZE);
	m_AudioDequeTotalSize = MP4_LIVE_WRITER_MAX_AUDIO_DEQUE_SIZE;

	m_pAudioSizeEntry = (ISO_TRACK_FRAGMENT_SAMPLE*)malloc(MP4_LIVE_WRITER_AUDIO_SAMPLE_SIZE_ENTRY_BUF_SIZE);
	if( m_pAudioSizeEntry == NULL )
	{
		Deinit();
		return -1;
	}

	m_bStart = 0;
	m_bEnableVideo = 1;
	m_bEnableAudio = 1;

	m_TimeScale = MP4_LIVE_WRITER_UNI_TIME_SCALE;
	m_bForceSync = 1;

	m_FileCount = 0;
	m_FileDirPath[0] = '\0';
	m_hFile = INVALID_HANDLE_VALUE;
	m_TotalWriteSize = 0;

	m_bInit = 1;

	return 0;
}

int32 CMp4LiveWriter::Deinit()
{
	MEDIA_FRAME* p_frame = NULL;

	int32  ret;

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

	if( m_pVideoBuf )
	{
		free(m_pVideoBuf);
		m_pVideoBuf = NULL;
	}

	if( m_pVideoDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pVideoDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pVideoDeque);
			free(p_frame);
			p_frame = NULL;
		}

		gen_deque_destroy(m_pVideoDeque);
		m_pVideoDeque = NULL;
	}

	if( m_pVideoSizeEntry )
	{
		free(m_pVideoSizeEntry);
		m_pVideoSizeEntry = NULL;
	}

	if( m_pAudioBuf )
	{
		free(m_pAudioBuf);
		m_pAudioBuf = NULL;
	}

	if( m_pAudioDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pAudioDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pAudioDeque);
			free(p_frame);
			p_frame = NULL;
		}

		gen_deque_destroy(m_pAudioDeque);
		m_pAudioDeque = NULL;
	}

	if( m_pAudioSizeEntry )
	{
		free(m_pAudioSizeEntry);
		m_pAudioSizeEntry = NULL;
	}

	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_bInit = 0;

	return 0;
}

int32 CMp4LiveWriter::Reset()
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

int32 CMp4LiveWriter::EnableVideo(int32 b_enable)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_bEnableVideo = b_enable;

	return 0;
}

int32 CMp4LiveWriter::EnableAudio(int32 b_enable)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_bEnableAudio = b_enable;

	return 0;
}

int32 CMp4LiveWriter::SetTimescale(uint32 time_scale)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_TimeScale = time_scale;

	return 0;
}

int32 CMp4LiveWriter::SetForceSync(int32 b_force_sync)
{
	if( !m_bInit )
	{
		return -1;
	}

	m_bForceSync = b_force_sync;

	return 0;
}

int32 CMp4LiveWriter::Start(int8 *p_file_path, int32 file_path_size)
{
	//int32 ret;

	if( p_file_path == NULL || file_path_size > MP4_LIVE_WRITER_MAX_PATH_SIZE )
	{
		return -3;
	}

	if( !m_bInit )
	{
		return -1;
	}

	if( m_bStart )
	{
		Stop();
	}

	m_FileCount = 0;

	memcpy(m_FileDirPath, p_file_path, file_path_size);
	m_FileDirPath[file_path_size] = '\0';

	m_WriteBufUseSize = 0;
	m_WriteBufLeftSize = m_WriteBufSize;
	m_TotalWriteSize = 0;

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

	m_bGop = 0;
	m_GopCount = 0;
	m_WriteGopCount = 0;
	m_TotalGopCount = 0;
	m_LastGopTs = 0;

	m_bGetSps = 0;
	m_bGetPps = 0;

	m_VideoBufUseSize = 0;
	m_VideoBufLeftSize = m_VideoBufSize;

	m_VideoFrameCount = 0;
	m_TotalVideoElapseTime = 0;
	m_TotalVideoElapseTimeMs = 0;
	m_VideoSizeEntryCount = 0;

	m_AudioBufUseSize = 0;
	m_AudioBufLeftSize = m_AudioBufSize;

	m_AudioFrameCount = 0;
	m_TotalAudioElapseTime = 0;
	m_TotalAudioElapseTimeMs = 0;
	m_TotalAudioSampleCount = 0;
	m_TotalAudioSampleDuration = 0;
	m_AudioSizeEntryCount = 0;

	m_bStart = 1;

	return 0;
}

int32 CMp4LiveWriter::Stop()
{
	MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	if( !m_bInit )
	{
		return -1;
	}

	if( !m_bStart )
	{
		return 0;
	}

	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	log_debug(_T("[CMp4LiveWriter::Stop] total write %u bytes\n"), m_TotalWriteSize);

	if( m_pVideoDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pVideoDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pVideoDeque);
			free(p_frame);
			p_frame = NULL;
		}
	}

	if( m_pAudioDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pAudioDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pAudioDeque);
			free(p_frame);
			p_frame = NULL;
		}
	}

//FIN:
	m_bStart = 0;

	return 0;
}

int32 CMp4LiveWriter::InputFrame(MEDIA_FRAME *p_frame)
{
	H264_NALU_PACK nalu_pack;
	uint8* p_uint8 = NULL;
	int32  parse_size;
	int32  total_parse_size;
	int32  left_size;
	int32  nalu_size;

	//uint64 total_sample_duration;
	int32  deque_size;
	int8*  p_block = NULL;
	MEDIA_FRAME* p_temp_frame = NULL;
	int8*  p_start = NULL;
	int32  b_next_gop;

	int32  i;

	int32  ret;

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

	//log_debug(_T("[CMp4LiveWriter::InputFrame] frame type = %d size = %d ts = %I64d\n"), p_frame->type, p_frame->frame_size, p_frame->frame_ts);

	if( !m_bFindKeyFrame )
	{
		if( p_frame->type == MP_DATA_TYPE_VIDEO )
		{
			if( p_frame->param.video.frame_type == MP_VIDEO_I_FRAME )
			{
				//m_bFindKeyFrame = 1;

			}else
			{
				return 0;
			}

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

			ret = gen_deque_get_size(m_pVideoDeque, &deque_size);
			if( ret )
			{
				return -2;
			}

			if( deque_size >= m_VideoDequeTotalSize )
			{
				return -2;
			}

			p_block = (int8*)malloc( sizeof(MEDIA_FRAME) + p_frame->frame_size + 4 );
			if( p_block == NULL )
			{
				return -2;
			}

			p_temp_frame = (MEDIA_FRAME*)p_block;
			memcpy(p_temp_frame, p_frame, sizeof(MEDIA_FRAME));

			p_start = p_block + sizeof(MEDIA_FRAME);
			memcpy(p_start, p_frame->p_frame, p_frame->frame_size);
			p_temp_frame->p_frame = p_start;
			p_temp_frame->frame_size = p_frame->frame_size;

			//p_temp_frame->frame_ts = m_TotalVideoElapseTime;
			//p_temp_frame->frame_ts = m_TotalVideoElapseTimeMs;
			//p_temp_frame->user_ts = m_VideoDeltaTime;

			if( m_bFirstVideoFrame )
			{
				m_bFirstVideoFrame = 0;

				//p_temp_frame->frame_ts = p_frame->frame_ts;
				p_temp_frame->user_ts = 0;

			}else
			{
				p_temp_frame->user_ts = p_frame->frame_ts - m_TotalVideoElapseTime;
			}

			ret = gen_deque_push_back_data(m_pVideoDeque, p_block);

			m_VideoFrameCount++;
			//m_TotalVideoElapseTime += m_VideoDeltaTime;
			m_TotalVideoElapseTime = p_frame->frame_ts;

			m_TotalVideoElapseTimeMs = m_TotalVideoElapseTime * 1000 / m_TimeScale;
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
						m_AudioSamplePerFrame = 240;
					}
					break;

				case MP_AUDIO_G711U:
					{
						m_AudioFourcc = MAKE_FOURCC('u','l','a','w');
						m_AudioSamplePerFrame = 240;
					}
					break;

				case MP_AUDIO_MPEG1:
				case MP_AUDIO_MPEG2:
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

						//m_AudioSamplePerFrame = 1024 * MP4_LIVE_WRITER_UNI_TIME_SCALE / m_AudioFrame.param.audio.sample_rate;
						m_AudioSamplePerFrame = 1024;
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

			if( m_AudioFrame.param.audio.sample_rate == 0 )
			{
				m_AudioFrame.param.audio.sample_rate = 48000;
			}
			//total_sample_duration = (m_TotalAudioSampleCount + m_AudioSamplePerFrame) * MP4_LIVE_WRITER_UNI_TIME_SCALE / m_AudioFrame.param.audio.sample_rate;

			ret = gen_deque_get_size(m_pAudioDeque, &deque_size);
			if( ret )
			{
				return -2;
			}

			if( deque_size >= m_AudioDequeTotalSize )
			{
				return -2;
			}

			p_block = (int8*)malloc( sizeof(MEDIA_FRAME) + p_frame->frame_size + 4 );
			if( p_block == NULL )
			{
				return -2;
			}

			p_temp_frame = (MEDIA_FRAME*)p_block;
			memcpy(p_temp_frame, p_frame, sizeof(MEDIA_FRAME));

			p_start = p_block + sizeof(MEDIA_FRAME);
			memcpy(p_start, p_frame->p_frame, p_frame->frame_size);
			p_temp_frame->p_frame = p_start;
			p_temp_frame->frame_size = p_frame->frame_size;

			//p_temp_frame->frame_ts = m_TotalAudioElapseTime;
			//p_temp_frame->frame_ts = m_TotalAudioElapseTimeMs;
			//p_temp_frame->user_ts = total_sample_duration - m_TotalAudioSampleDuration;

			if( m_bFirstAudioFrame )
			{
				m_bFirstAudioFrame = 0;
				p_temp_frame->user_ts = 0;

			}else
			{
				p_temp_frame->user_ts = p_frame->frame_ts - m_TotalAudioElapseTime;
			}

			ret = gen_deque_push_back_data(m_pAudioDeque, p_block);

			m_AudioFrameCount++;
			m_TotalAudioSampleCount += m_AudioSamplePerFrame;
			m_TotalAudioSampleDuration = m_TotalAudioSampleCount * MP4_LIVE_WRITER_UNI_TIME_SCALE / m_AudioFrame.param.audio.sample_rate;
			//m_TotalAudioElapseTime = m_TotalAudioSampleCount * 1000 / m_AudioFrame.param.audio.sample_rate;
			//m_TotalAudioElapseTimeMs = m_TotalAudioElapseTime;
			m_TotalAudioElapseTime = p_frame->frame_ts;
			m_TotalAudioElapseTimeMs = m_TotalAudioElapseTime * 1000 / m_TimeScale;
		}
		break;

	default:
		{
			return -3;
		}
	}

	if( p_frame->type == MP_DATA_TYPE_VIDEO && p_frame->param.video.frame_type == MP_VIDEO_I_FRAME )
	{
		if( !m_bFindKeyFrame )
		{
			m_bFindKeyFrame = 1;

		}else
		{
			m_GopCount++;
			m_TotalGopCount++;

			if( !m_bGop )
			{
				m_bGop = 1;
				m_LastGopTs = m_TotalVideoElapseTimeMs;
			}
		}
	}

	if( !m_bGop )
	{
		return 0;
	}

	if( m_bForceSync )
	{
		if( m_TotalAudioElapseTimeMs < m_LastGopTs )
		{
			return 0;
		}
	}

	/** write video gop */
	m_VideoBufUseSize = 0;
	m_VideoBufLeftSize = m_VideoBufSize;
	m_VideoSizeEntryCount = 0;
	b_next_gop = 0;
	while( 1 )
	{
		ret = gen_deque_get_front_data(m_pVideoDeque, (void**)&p_temp_frame);
		if( ret )
		{
			break;
		}

		if( p_temp_frame->param.video.frame_type == MP_VIDEO_I_FRAME )
		{
			if( b_next_gop )
			{
				//free(p_frame);
				break;

			}else
			{
				b_next_gop = 1;
			}
		}

		if( p_temp_frame->frame_size < m_VideoBufLeftSize )
		{
			memcpy(m_pVideoBuf + m_VideoBufUseSize, p_temp_frame->p_frame, p_temp_frame->frame_size);
			m_VideoBufUseSize += p_temp_frame->frame_size;
			m_VideoBufLeftSize -= p_temp_frame->frame_size;

		}else
		{
			log_debug(_T("[CMp4LiveWriter::InputFrame] video size = %d too large\n"), p_temp_frame->frame_size);
			goto NEXT1;
		}

		if( m_VideoSizeEntryCount < MP4_LIVE_WRITER_MAX_VIDEO_INDEX )
		{
			m_pVideoSizeEntry[m_VideoSizeEntryCount].sample_duration = p_temp_frame->user_ts;
			m_pVideoSizeEntry[m_VideoSizeEntryCount].sample_size = p_temp_frame->frame_size;
			m_pVideoSizeEntry[m_VideoSizeEntryCount].sample_flag = 0;
			m_pVideoSizeEntry[m_VideoSizeEntryCount].sample_composition_time_offset = 0;

			//log_debug(_T("[CMp4LiveWriter::InputFrame] video frame %d user ts = %I64d, duration = %u\n"), m_VideoSizeEntryCount, p_temp_frame->user_ts, m_pVideoSizeEntry[m_VideoSizeEntryCount].sample_duration);

			m_VideoSizeEntryCount++;
		}

NEXT1:
		gen_deque_pop_front(m_pVideoDeque);
		free(p_temp_frame);
		p_temp_frame = NULL;
	}

	/** write audio gop */
	m_AudioBufUseSize = 0;
	m_AudioBufLeftSize = m_AudioBufSize;
	m_AudioSizeEntryCount = 0;
	while( 1 )
	{
		ret = gen_deque_get_front_data(m_pAudioDeque, (void**)&p_temp_frame);
		if( ret )
		{
			break;
		}

		if( p_temp_frame->frame_ts > m_LastGopTs )
		{
			break;
		}

		if( p_temp_frame->frame_size < m_AudioBufLeftSize )
		{
			memcpy(m_pAudioBuf + m_AudioBufUseSize, p_temp_frame->p_frame, p_temp_frame->frame_size);
			m_AudioBufUseSize += p_temp_frame->frame_size;
			m_AudioBufLeftSize -= p_temp_frame->frame_size;

		}else
		{
			goto NEXT2;
		}

		if( m_AudioSizeEntryCount < MP4_LIVE_WRITER_MAX_AUDIO_INDEX )
		{
			m_pAudioSizeEntry[m_AudioSizeEntryCount].sample_duration = p_temp_frame->user_ts;
			m_pAudioSizeEntry[m_AudioSizeEntryCount].sample_size = p_temp_frame->frame_size;
			m_pAudioSizeEntry[m_AudioSizeEntryCount].sample_flag = 0;
			m_pAudioSizeEntry[m_AudioSizeEntryCount].sample_composition_time_offset = 0;

			m_AudioSizeEntryCount++;
		}

NEXT2:
		gen_deque_pop_front(m_pAudioDeque);
		free(p_temp_frame);
		p_temp_frame = NULL;
	}

	ret = WriteFragment();

	m_GopCount--;
	m_bGop = 0;

	if( m_GopCount > 0 )
	{
		/** update last gop ts */
		b_next_gop = 0;
		i = 0;
		while( 1 )
		{
			//ret = gen_deque_get_front_data(m_pVideoDeque, (void**)&p_frame);
			ret = gen_deque_get_data(m_pVideoDeque, i, (void**)&p_temp_frame);
			if( ret )
			{
				break;
			}

			if( p_temp_frame->param.video.frame_type == MP_VIDEO_I_FRAME )
			{
				if( b_next_gop )
				{
					m_bGop = 1;
					m_LastGopTs = p_temp_frame->frame_ts;
					break;

				}else
				{
					b_next_gop = 1;
				}
			}

			i++;
		}
	}


	return 0;
}

int32 CMp4LiveWriter::Resync()
{
	MEDIA_FRAME* p_frame = NULL;

	int32  ret;

	if( m_pVideoDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pVideoDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pVideoDeque);
			free(p_frame);
			p_frame = NULL;
		}
	}

	if( m_pAudioDeque )
	{
		while( 1 )
		{
			ret = gen_deque_get_front_data(m_pAudioDeque, (void**)&p_frame);
			if( ret )
			{
				break;
			}

			gen_deque_pop_front(m_pAudioDeque);
			free(p_frame);
			p_frame = NULL;
		}
	}

	m_WriteBufUseSize = 0;
	m_WriteBufLeftSize = m_WriteBufSize;
	m_TotalWriteSize = 0;

	/** init status */
	m_bFindKeyFrame = 0;
	m_bFirstVideoFrame = 1;
	m_bFirstAudioFrame = 1;
	m_bFirstFragment = 0;
	//m_bWriteMovieBox = 0;
	//m_bHasVideo = 0;
	//m_bHasAudio = 0;

	//m_TrackNum = 0;

	//memset(&m_VideoFrame, 0, sizeof(m_VideoFrame));
	//memset(&m_AudioFrame, 0, sizeof(m_AudioFrame));

	m_bGop = 0;
	m_GopCount = 0;
	m_LastGopTs = 0;

	//m_bGetSps = 0;
	//m_bGetPps = 0;

	//m_VideoBufUseSize = 0;
	//m_VideoBufLeftSize = m_VideoBufSize;

	//m_VideoFrameCount = 0;
	m_TotalVideoElapseTime = 0;
	m_TotalVideoElapseTimeMs = 0;
	m_VideoSizeEntryCount = 0;

	//m_AudioBufUseSize = 0;
	//m_AudioBufLeftSize = m_AudioBufSize;

	//m_AudioFrameCount = 0;
	m_TotalAudioElapseTime = 0;
	m_TotalAudioElapseTimeMs = 0;
	m_TotalAudioSampleCount = 0;
	m_TotalAudioSampleDuration = 0;
	m_AudioSizeEntryCount = 0;

	return 0;
}

int32 CMp4LiveWriter::SetAvcDecConfig(uint8* p_buf, int32 buf_size)
{
	AVC_DEC_CONFIG_RECORD dec_config;
	AVC_SPS_RECORD sps_record[MP4_LIVE_WRITER_MAX_SPS_RECORD];
	AVC_PPS_RECORD pps_record[MP4_LIVE_WRITER_MAX_PPS_RECORD];

	//H264_NALU_PACK nalu_pack;

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

	dec_config.sps_count = MP4_LIVE_WRITER_MAX_SPS_RECORD;
	dec_config.p_sps = sps_record;
	dec_config.pps_count = MP4_LIVE_WRITER_MAX_PPS_RECORD;
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

int32 CMp4LiveWriter::WriteFileTypeBox()
{
	ISO_FILE_TYPE_BOX  file_type_box;

	uint32 brand[2];

	uint8*  p_uint8 = NULL;
	int32   buf_left_size;
	int32   mux_size;

	int32 ret;

	p_uint8 = m_pWriteBuf + m_WriteBufUseSize;
	buf_left_size = m_WriteBufLeftSize;

	/** file type box */
	file_type_box.major_brand = MP4_BRAND_ISOM;
	file_type_box.minor_version = 0x0;
	brand[0] = hton_u32(MP4_BRAND_MP41);
	brand[1] = hton_u32(MP4_BRAND_FRAG);
	file_type_box.p_brand = brand;
	file_type_box.brand_count = 2;

	mux_size = buf_left_size;
	ret = mp4_make_file_type_box(m_Muxer, &file_type_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_WriteBufUseSize += mux_size;
	m_WriteBufLeftSize -= mux_size;
	m_TotalWriteSize += mux_size;

	return 0;
}

int32 CMp4LiveWriter::WriteMovieBox()
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
	//ISO_SYNC_SAMPLE_BOX sync_sample_box;

	//ISO_SAMPLE_TO_CHUNK_ENTRY sample_to_chunk_entry;

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
	//uint32 time_scale;
	//uint32 duration;
	uint32 total_entry_size;

	int8   str[MP4_LIVE_WRITER_MAX_STR_SIZE + 1];

	uint8  dec_spec_buf[MP4_LIVE_WRITER_MAX_DEC_SPEC_BUF_SIZE];
	int32  dec_spec_buf_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_buf_offset = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	int32  i;
	int32  ret;

	p_uint8 = m_pWriteBuf + m_WriteBufUseSize;
	buf_left_size = m_WriteBufLeftSize;

	/** movie box */
	movie_box_offset = m_TotalWriteSize;
	total_mux_size = 0;

	movie_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_make_movie_box(m_Muxer, &movie_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	total_mux_size += mux_size;

	/** movie header box */
	utc_time = time(NULL);
	creation_time = utc_time + MP4_UTC_TIME_1904_TO_1970;
	modification_time = creation_time;

	movie_header_box.creation_time = creation_time;
	movie_header_box.modification_time = modification_time;

	movie_header_box.time_scale = m_TimeScale;
	movie_header_box.duration = 0;
	movie_header_box.next_track_id = MP4_LIVE_WRITER_MAX_TRACK_ID + 1;

	mux_size = buf_left_size;
	ret = mp4_make_movie_header_box(m_Muxer, &movie_header_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	total_mux_size += mux_size;

	memset(str, 0, MP4_LIVE_WRITER_MAX_STR_SIZE + 1);
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
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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
			track_header_box.track_id = MP4_LIVE_WRITER_VIDEO_TRACK_ID;
			track_header_box.duration = 0;
			track_header_box.volume = 0x0;

		}else
		{
			track_header_box.track_id = MP4_LIVE_WRITER_AUDIO_TRACK_ID;
			track_header_box.duration = 0;
			track_header_box.volume = 0x0100;
		}

		track_header_box.width = m_VideoFrame.param.video.width << 16;
		track_header_box.height = m_VideoFrame.param.video.height << 16;

		mux_size = buf_left_size;
		ret = mp4_make_track_header_box(m_Muxer, &track_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media box */
		media_box_offset = m_TotalWriteSize;
		media_box_size = 0;

		media_box.box.size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_media_box(m_Muxer, &media_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** media header box */
		media_header_box.creation_time = creation_time;
		media_header_box.modification_time = modification_time;
		media_header_box.time_scale = m_TimeScale;
		media_header_box.duration = 0;

		//chi
		media_header_box.language = 0x0D09;

		mux_size = buf_left_size;
		ret = mp4_make_media_header_box(m_Muxer, &media_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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
			p_uint8 += mux_size;
			buf_left_size -= mux_size;
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
			p_uint8 += mux_size;
			buf_left_size -= mux_size;
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
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		data_info_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite data info box size */
		p_uint32 = (uint32*)(m_pWriteBuf + data_info_box_offset);
		data_info_box_size = hton_u32(data_info_box_size);
		*p_uint32 = data_info_box_size;

		/** sample table box */
		sample_table_box_offset = m_TotalWriteSize;
		sample_table_box_size = 0;

		mux_size = buf_left_size;
		ret = mp4_make_sample_table_box(m_Muxer, &sample_table_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
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

			p_uint8 += mux_size;
			buf_left_size -= mux_size;
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
				aac_sample_entry.es_box.es_desc.es_id = MP4_LIVE_WRITER_AUDIO_TRACK_ID;
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
				dec_spec_buf_size = MP4_LIVE_WRITER_MAX_DEC_SPEC_BUF_SIZE;
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
				audio_sample_entry.sample_rate = m_AudioFrame.param.audio.sample_rate << 16;

				audio_sample_desc_box.entry_count = 1;
				audio_sample_desc_box.p_entry = &audio_sample_entry;

				mux_size = buf_left_size;
				ret = mp4_make_audio_sample_desc_box(m_Muxer, &audio_sample_desc_box, p_uint8, &mux_size);
			}

			p_uint8 += mux_size;
			buf_left_size -= mux_size;
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
		ret = mp4_make_time_to_sample_box(m_Muxer, &time_to_sample_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample size box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
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
		ret = mp4_make_sample_size_box(m_Muxer, &sample_size_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** sample to chunk box */
		sample_to_chunk_box.entry_count = 0;
		sample_to_chunk_box.p_entry = NULL;

		mux_size = buf_left_size;
		ret = mp4_make_sample_to_chunk_box(m_Muxer, &sample_to_chunk_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** chunk offset box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
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
		ret = mp4_make_chunk_offset_box(m_Muxer, &chunk_offset_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		sample_table_box_size += mux_size;
		media_info_box_size += mux_size;
		media_box_size += mux_size;
		track_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite sample table box size */
		p_uint32 = (uint32*)(m_pWriteBuf + sample_table_box_offset);
		sample_table_box_size = hton_u32(sample_table_box_size);
		*p_uint32 = sample_table_box_size;

		/** rewrite media info box size */
		p_uint32 = (uint32*)(m_pWriteBuf + media_info_box_offset);
		media_info_box_size = hton_u32(media_info_box_size);
		*p_uint32 = media_info_box_size;

		/** rewrite media box size */
		p_uint32 = (uint32*)(m_pWriteBuf + media_box_offset);
		media_box_size = hton_u32(media_box_size);
		*p_uint32 = media_box_size;

		/** rewrite track box size */
		p_uint32 = (uint32*)(m_pWriteBuf + track_box_offset);
		track_box_size = hton_u32(track_box_size);
		*p_uint32 = track_box_size;
	}

	/** movie extend box */
	movie_extend_box_offset = m_TotalWriteSize;
	movie_extend_box_size = 0;

	movie_extend_box.box.size = 0;

	mux_size = buf_left_size;
	ret = mp4_make_movie_extend_box(m_Muxer, &movie_extend_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	movie_extend_box_size += mux_size;
	total_mux_size += mux_size;

	for( i = 0; i < m_TrackNum; i++ )
	{
		/** track extend box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_extend_box.track_id = MP4_LIVE_WRITER_VIDEO_TRACK_ID;

		}else
		{
			track_extend_box.track_id = MP4_LIVE_WRITER_AUDIO_TRACK_ID;
		}

		track_extend_box.def_sample_desc_index = 0x1;
		track_extend_box.def_sample_duration = 0;
		track_extend_box.def_sample_size = 0;
		track_extend_box.def_sample_flag = 0;

		mux_size = buf_left_size;
		ret = mp4_make_track_extend_box(m_Muxer, &track_extend_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		movie_extend_box_size += mux_size;
		total_mux_size += mux_size;
	}

	/** rewrite movie extend box size */
	p_uint32 = (uint32*)(m_pWriteBuf + movie_extend_box_offset);
	movie_extend_box_size = hton_u32(movie_extend_box_size);
	*p_uint32 = movie_extend_box_size;

	/** rewrite movie box size */
	p_uint32 = (uint32*)(m_pWriteBuf + movie_box_offset);
	total_mux_size = hton_u32(total_mux_size);
	*p_uint32 = total_mux_size;

	m_WriteBufUseSize = m_TotalWriteSize;
	m_WriteBufLeftSize = buf_left_size;

	//log_debug(_T("[CMp4LiveWriter::WriteMovieBox] write %u bytes\n"), m_TotalWriteSize);

//FIN:

	return 0;
}

int32 CMp4LiveWriter::WriteFragment()
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

	//uint32 total_entry_size;
	uint32 uint32_data;

	int8   file_path[MP4_LIVE_WRITER_MAX_PATH_SIZE];
	int32  file_path_size;
	//int8   file_name[256];

	//LARGE_INTEGER file_pos;
	DWORD dwWriteSize;

	MEDIA_FRAME* p_frame = NULL;

	uint8*  p_uint8 = NULL;
	uint32* p_uint32 = NULL;
	int32   buf_left_size;

	int32  i;
	int32  ret;

	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		file_path_size = sprintf(file_path, "%s\\%d.mp4", m_FileDirPath, m_FileCount);
		m_hFile = CreateFileA(
			file_path,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if( m_hFile == INVALID_HANDLE_VALUE )
		{
			return -1;
		}

		m_FileCount++;
		m_bWriteMovieBox = 0;
	}

	m_WriteBufUseSize = 0;
	m_WriteBufLeftSize = m_WriteBufSize;
	m_TotalWriteSize = 0;

	if( !m_bWriteMovieBox )
	{
		/** write movie box */
		ret = WriteFileTypeBox();
		ret = WriteMovieBox();

		m_bWriteMovieBox = 1;
	}

	p_uint8 = m_pWriteBuf + m_WriteBufUseSize;
	buf_left_size = m_WriteBufLeftSize;

	/** movie fragment box */
	movie_frag_box_offset = m_TotalWriteSize;
	movie_frag_box_size = 0;
	total_mux_size = 0;

	movie_frag_box.box.size = 0x0;
	mux_size = buf_left_size;
	ret = mp4_make_movie_fragment_box(m_Muxer, &movie_frag_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	movie_frag_box_size += mux_size;
	total_mux_size += mux_size;

	/** movie fragment header box */
	movie_frag_header_box.seq_no = m_WriteGopCount;
	m_WriteGopCount++;

	mux_size = buf_left_size;
	ret = mp4_make_movie_fragment_header_box(m_Muxer, &movie_frag_header_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	movie_frag_box_size += mux_size;
	total_mux_size += mux_size;

	track_frag_run_box_offset = 0;

	for( i = 0; i < m_TrackNum; i++ )
	{
		/** track fragment box */
		track_frag_box_offset = m_TotalWriteSize;
		track_frag_box_size = 0;

		track_frag_box.box.size = 0x0;
		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_box(m_Muxer, &track_frag_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		/** track fragment header box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_frag_header_box.track_id = MP4_LIVE_WRITER_VIDEO_TRACK_ID;

		}else
		{
			track_frag_header_box.track_id = MP4_LIVE_WRITER_AUDIO_TRACK_ID;
		}

		track_frag_header_box.full_box.version_flag.value = ( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET) );

		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_header_box(m_Muxer, &track_frag_header_box, p_uint8, &mux_size);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		/** track fragment run box */
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			track_frag_run_box_offset = m_TotalWriteSize;

			track_frag_run_box.sample_count = m_VideoSizeEntryCount;
			track_frag_run_box.p_sample = m_pVideoSizeEntry;
			m_VideoSizeEntryCount = 0;

			track_frag_run_box.data_offset = 0;

		}else
		{
			track_frag_run_box.sample_count = m_AudioSizeEntryCount;
			track_frag_run_box.p_sample = m_pAudioSizeEntry;
			m_AudioSizeEntryCount = 0;

			track_frag_run_box.data_offset = 0;
		}
		track_frag_run_box.full_box.version_flag.value = ( (MP4_FULL_BOX_VERSION_0 << MP4_FULL_BOX_VERSION_BIT_OFFSET)
															| (MP4_DATA_OFFSET_PRESENT_FLAG | MP4_SAMPLE_DURATION_PRESENT_FLAG | MP4_SAMPLE_SIZE_PRESENT_FLAG) );

		mux_size = buf_left_size;
		ret = mp4_make_track_fragment_run_box(m_Muxer, &track_frag_run_box, p_uint8, &mux_size);
		//ret = WriteFile(m_hFile, m_pWriteBuf, mux_size, &dwWriteSize, NULL);
		p_uint8 += mux_size;
		buf_left_size -= mux_size;
		m_TotalWriteSize += mux_size;
		track_frag_box_size += mux_size;
		movie_frag_box_size += mux_size;
		total_mux_size += mux_size;

		/** rewrite track fragment box size */
		p_uint32 = (uint32*)(m_pWriteBuf + track_frag_box_offset);
		track_frag_box_size = hton_u32(track_frag_box_size);
		*p_uint32 = track_frag_box_size;
	}

	/** rewrite track fragment run box's data offset section */
	p_uint32 = (uint32*)(m_pWriteBuf + track_frag_run_box_offset + MP4_FULL_BOX_SIZE + 4);
	uint32_data = hton_u32(movie_frag_box_size + 8);
	*p_uint32 = uint32_data;

	/** rewrite movie fragment box size */
	p_uint32 = (uint32*)(m_pWriteBuf + movie_frag_box_offset);
	movie_frag_box_size = hton_u32(movie_frag_box_size);
	*p_uint32 = movie_frag_box_size;

	/** media data box */
	media_data_box.box.size = MP4_MEDIA_DATA_BOX_SIZE + m_VideoBufUseSize + m_AudioBufUseSize;

	mux_size = buf_left_size;
	ret = mp4_make_media_data_box(m_Muxer, &media_data_box, p_uint8, &mux_size);
	p_uint8 += mux_size;
	buf_left_size -= mux_size;
	m_TotalWriteSize += mux_size;
	total_mux_size += mux_size;

	m_WriteBufUseSize = m_TotalWriteSize;

	ret = WriteFile(m_hFile, m_pWriteBuf, m_WriteBufUseSize, &dwWriteSize, NULL);

	for( i = 0; i < m_TrackNum; i++ )
	{
		if( m_Track[i] == MP4_TRACK_TYPE_VIDEO )
		{
			mux_size = m_VideoBufUseSize;
			ret = WriteFile(m_hFile, m_pVideoBuf, mux_size, &dwWriteSize, NULL);
			m_VideoBufUseSize = 0;
			m_VideoBufLeftSize = m_VideoBufSize;

		}else
		{
			mux_size = m_AudioBufUseSize;
			ret = WriteFile(m_hFile, m_pAudioBuf, mux_size, &dwWriteSize, NULL);
			m_AudioBufUseSize = 0;
			m_AudioBufLeftSize = m_AudioBufSize;
		}

		m_TotalWriteSize += mux_size;
		total_mux_size += mux_size;
	}

	if( m_WriteGopCount % MP4_LIVE_WRITER_GOP_PER_FILE == 0 )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	return 0;
}

int32 CMp4LiveWriter::MakeAudioDecSpecInfo(uint8* p_buf, int32* p_buf_size)
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

