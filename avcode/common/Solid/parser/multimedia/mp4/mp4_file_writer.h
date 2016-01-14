/**
 * @file mp4_file_writer.h   MP4 File Writer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.12
 *
 *
 */

#ifndef __MP4_FILE_WRITER_H__
#define __MP4_FILE_WRITER_H__

#include <public/gen_int.h>
#include <public/media_def.h>
#include <public/gen_file.h>
#include "../iso/iso_media_file.h"

#include "../include/IMultiMediaFileWriter.h"

/** 最大Track个数 */
#define MP4_FILE_WRITER_MAX_TRACK_NUM                      (2)


class CMp4FileWriter : public IMultiMediaFileWriter
{
public:

	CMp4FileWriter();
	~CMp4FileWriter();

	int32 Init();
	int32 Deinit();
	int32 Reset();
	int32 EnableVideo(int32 b_enable);
	int32 EnableAudio(int32 b_enable);
	int32 SetTimescale(uint32 time_scale);
	//int32 SetVideoFps(int32 fps);
	//int32 SetAudioFrameTs(int32 frame_ts);
	int32 Start(int8 *p_file_path, int32 file_path_size);
	int32 Stop();
	int32 InputFrame(MEDIA_FRAME *p_frame);

	int32 SetAvcDecConfig(uint8* p_buf, int32 p_buf_size);

	int32 WriteFileTypeBox();
	int32 WriteMovieBox();
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
	int32  m_Track[MP4_FILE_WRITER_MAX_TRACK_NUM];

	/** 视频参数 */
	MEDIA_FRAME m_VideoFrame;
	/** 音频参数 */
	MEDIA_FRAME m_AudioFrame;

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
	/** 视频总时间, unit: timescale */
	int64  m_TotalVideoElapseTime;
	/** 视频总时间, unit: ms */
	int64  m_TotalVideoElapseTimeMs;
	/** 视频帧累计长度 */
	uint32 m_TotalVideoFrameSize;

	/** 音频fourcc */
	uint32 m_AudioFourcc;
	/** 音频帧数 */
	int32  m_AudioFrameCount;
	/** 音频总时间, unit: timescale */
	int64  m_TotalAudioElapseTime;
	/** 音频总时间, unit: ms */
	int64  m_TotalAudioElapseTimeMs;
	/** 音频每帧采样数 */
	int32  m_AudioSamplePerFrame;
	/** 音频帧总采样数 */
	int64  m_TotalAudioSampleCount;
	/** 音频帧总采样持续时间, unit: timescale */
	int64  m_TotalAudioSampleDuration;
	/** 音频帧累计长度 */
	uint32 m_TotalAudioFrameSize;
	/** 所有帧累计长度 */
	uint32 m_TotalFrameSize;

	/** 写入缓冲区 */
	uint8* m_pWriteBuf;
	/** 写入缓冲区大小 */
	int32  m_WriteBufSize;
	/** 写入缓冲区已使用大小 */
	int32  m_WriteBufUseSize;
	/** 写入缓冲区未使用大小 */
	int32  m_WriteBufLeftSize;

	/** video time to sample entry */
	ISO_TIME_TO_SAMPLE_ENTRY* m_pVideoTimeEntry;
	/** video sample size entry */
	ISO_SAMPLE_SIZE_ENTRY* m_pVideoSizeEntry;
	/** video sample size entry count */
	//int32  m_VideoSizeEntryCount;
	/** video chunk offset entry */
	ISO_CHUNK_OFFSET_ENTRY* m_pVideoOffsetEntry;
	/** video chunk offset entry count */
	//int32  m_VideoOffsetEntryCount;
	/** video sync sample entry */
	ISO_SYNC_SAMPLE_ENTRY* m_pVideoSyncEntry;
	/** video sync sample entry count */
	int32  m_VideoSyncEntryCount;

	/** audio time to sample entry */
	ISO_TIME_TO_SAMPLE_ENTRY* m_pAudioTimeEntry;
	/** audio sample size entry */
	ISO_SAMPLE_SIZE_ENTRY* m_pAudioSizeEntry;
	/** audio sample size entry count */
	int32  m_AudioSizeEntryCount;
	/** audio chunk offset entry */
	ISO_CHUNK_OFFSET_ENTRY* m_pAudioOffsetEntry;
	/** audio chunk offset entry count */
	int32  m_AudioOffsetEntryCount;

	/** file */
	GEN_FILE_H m_hFile;
	/** total write size */
	uint32 m_TotalWriteSize;
	/** media data offset */
	uint32 m_MediaDataOffset;
	/** media data box size */
	uint32 m_MediaDataBoxSize;
};

#endif ///__MP4_FILE_WRITER_H__
