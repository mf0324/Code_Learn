/**
 * @file mp4_live_writer.h   MP4 Live Writer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.07.28
 *
 *
 */

#ifndef __MP4_LIVE_WRITER_H__
#define __MP4_LIVE_WRITER_H__

//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN
//#endif

#include <windows.h>

#include <public/gen_int.h>
#include <public/gen_deque.h>
#include <public/media_def.h>
#include "../iso/iso_media_file.h"


/** max track num */
#define MP4_LIVE_WRITER_MAX_TRACK_NUM                      (2)
/** max path size */
#define MP4_LIVE_WRITER_MAX_PATH_SIZE                      (512)



class CMp4LiveWriter
{
public:

	CMp4LiveWriter();
	~CMp4LiveWriter();

	int32 Init();
	int32 Deinit();
	int32 Reset();
	int32 EnableVideo(int32 b_enable);
	int32 EnableAudio(int32 b_enable);
	int32 SetTimescale(uint32 time_scale);
	int32 SetForceSync(int32 b_force_sync);
	int32 Start(int8 *p_file_path, int32 file_path_size);
	int32 Stop();
	int32 InputFrame(MEDIA_FRAME *p_frame);

	int32 Resync();
	int32 SetAvcDecConfig(uint8* p_buf, int32 p_buf_size);

	int32 WriteFileTypeBox();
	int32 WriteMovieBox();
	int32 WriteFragment();
	int32 MakeAudioDecSpecInfo(uint8* p_buf, int32* p_buf_size);

	/** 初始化标志 */
	int32  m_bInit;
	/** 内部复用器 */
	void*  m_Muxer;

	/** 是否使能视频 */
	int32  m_bEnableVideo;
	/** 是否使能音频 */
	int32  m_bEnableAudio;

	/** timescale */
	uint32 m_TimeScale;
	/** force sync */
	int32  m_bForceSync;

	/** 是否开始 */
	int32  m_bStart;
	/** 是否发现I帧 */
	int32  m_bFindKeyFrame;
	/** first video frame */
	int32  m_bFirstVideoFrame;
	/** first audio frame */
	int32  m_bFirstAudioFrame;
	/** first fragment */
	int32  m_bFirstFragment;
	/** write movie box */
	int32  m_bWriteMovieBox;

	/** 是否有视频 */
	int32  m_bHasVideo;
	/** 是否有音频 */
	int32  m_bHasAudio;
	/** Track个数 */
	int32  m_TrackNum;
	/** Track列表 */
	int32  m_Track[MP4_LIVE_WRITER_MAX_TRACK_NUM];

	/** 视频参数 */
	MEDIA_FRAME m_VideoFrame;
	/** 音频参数 */
	MEDIA_FRAME m_AudioFrame;

	/** gop */
	int32  m_bGop;
	/** gop count */
	uint32 m_GopCount;
	/** write gop count */
	uint32 m_WriteGopCount;
	/** total gop count */
	uint32 m_TotalGopCount;
	/** last gop ts, unit: ms */
	uint64 m_LastGopTs;

	/** get avc dec config */
	int32  m_bGetAvcDecConfig;
	/** avc dec config buf */
	uint8* m_pAvcDecConfigBuf;
	/** avc dec config buf size */
	int32  m_AvcDecConfigBufSize;
	/** avc dec config buf use size */
	int32  m_AvcDecConfigBufUseSize;

	/** profile */
	int32  m_Profile;
	/** profile compat */
	int32  m_ProfileCompat;
	/** level */
	int32  m_Level;

	/** get sps */
	int32  m_bGetSps;
	/** sps */
	uint8* m_pSPSBuf;
	/** sps buf size */
	int32  m_SPSBufSize;
	/** sps nalu size */
	int32  m_SPSNaluSize;

	/** get pps */
	int32  m_bGetPps;
	/** pps */
	uint8* m_pPPSBuf;
	/** pps buf size */
	int32  m_PPSBufSize;
	/** pps nalu size */
	int32  m_PPSNaluSize;

	/** 视频fourcc */
	uint32 m_VideoFourcc;
	/** 视频帧数 */
	int32  m_VideoFrameCount;
	/** 视频总时间, unit: scale */
	uint64 m_TotalVideoElapseTime;
	/** 视频总时间, unit: ms */
	uint64 m_TotalVideoElapseTimeMs;

	/** 音频fourcc */
	uint32 m_AudioFourcc;
	/** 音频帧数 */
	int32  m_AudioFrameCount;
	/** 音频总时间 */
	uint64 m_TotalAudioElapseTime;
	/** 音频总时间, unit: ms */
	uint64 m_TotalAudioElapseTimeMs;
	/** 音频每帧采样数 */
	int32  m_AudioSamplePerFrame;
	/** 音频帧总采样数 */
	uint64 m_TotalAudioSampleCount;
	/** 音频帧总采样持续时间, unit: timescale */
	uint64 m_TotalAudioSampleDuration;


	/** 写入缓冲区 */
	uint8* m_pWriteBuf;
	/** 写入缓冲区大小 */
	int32  m_WriteBufSize;
	/** 写入缓冲区已使用大小 */
	int32  m_WriteBufUseSize;
	/** 写入缓冲区未使用大小 */
	int32  m_WriteBufLeftSize;

	/** video buf */
	uint8*  m_pVideoBuf;
	/** video buf size */
	uint32  m_VideoBufSize;
	/** video buf use size */
	uint32  m_VideoBufUseSize;
	/** video buf left size */
	uint32  m_VideoBufLeftSize;

	/** video deque */
	GEN_DEQUE* m_pVideoDeque;
	/** video deque total size */
	int32   m_VideoDequeTotalSize;

	/** video sample size entry */
	ISO_TRACK_FRAGMENT_SAMPLE* m_pVideoSizeEntry;
	/** video sample size entry count */
	int32   m_VideoSizeEntryCount;

	/** audio buf */
	uint8*  m_pAudioBuf;
	/** audio buf size */
	uint32  m_AudioBufSize;
	/** audio buf use size */
	uint32  m_AudioBufUseSize;
	/** audio buf left size */
	uint32  m_AudioBufLeftSize;

	/** audio deque */
	GEN_DEQUE* m_pAudioDeque;
	/** audio deque total size */
	int32   m_AudioDequeTotalSize;

	/** audio sample size entry */
	ISO_TRACK_FRAGMENT_SAMPLE* m_pAudioSizeEntry;
	/** audio sample size entry count */
	int32   m_AudioSizeEntryCount;

	/** file count */
	uint32 m_FileCount;
	/** file dir path */
	int8   m_FileDirPath[MP4_LIVE_WRITER_MAX_PATH_SIZE];
	/** file handle */
	HANDLE m_hFile;
	/** total write size */
	uint32 m_TotalWriteSize;

};

#endif ///__MP4_LIVE_WRITER_H__
