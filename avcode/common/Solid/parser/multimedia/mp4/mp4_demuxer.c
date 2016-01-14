
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <Windows.h>

#include <public/gen_endian.h>
#include <public/gen_file.h>
#include <util/log_debug.h>

#include "mp4_demux.h"
#include "mp4_demuxer.h"


/** max track num */
#define MP4_DEMUXER_MAX_TRACK_NUM                          (2)
/** max file path size */
#define MP4_DEMUXER_MAX_FILE_PATH_SIZE                     (1024)
/** file buf size */
#define MP4_DEMUXER_FILE_BUF_SIZE                          (2 * 1024)
/** read entry num */
#define MP4_DEMUXER_READ_ENTRY_NUM                         (1024)



/** mp4 file */
typedef struct tag_mp4_file
{
	/** file handle */
	//HANDLE m_hFile;
	GEN_FILE_H  file_h;

	/** file stat */
	MP4_FILE_STAT stat;

	/** track count */
	uint32 track_count;
	/** track */
	MP4_TRACK track[MP4_DEMUXER_MAX_TRACK_NUM];

	/** has video */
	int32  b_has_video;
	/** video track */
	MP4_VIDEO_TRACK video;

	/** has audio */
	int32  b_has_audio;
	/** audio track */
	MP4_AUDIO_TRACK audio;

} MP4_FILE, *PMP4_FILE;


/** mp4 file demuxer */
typedef struct tag_mp4_file_demuxer
{
	/** init */
	int32  b_init;

	/** demux */
	int32  b_demux;
	/** file */
	MP4_FILE file;

} MP4_FILE_DEMUXER, *PMP4_FILE_DEMUXER;

/**  
 * @brief 解析MP4 video stts
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析video stsz成功才能调用该接口！
 */
static int32 mp4_demux_video_stts(MP4_FILE_DEMUXER* p_demuxer, ISO_TIME_TO_SAMPLE_BOX* p_box, int64 entry_offset)
{
	ISO_TIME_TO_SAMPLE_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 left_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 sample_count;
	uint32 sample_delta;
	uint32 total_sample_time;

	int32  b_error;
	int32  error_code;

	uint32 i, j, k;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * MP4_DEMUXER_READ_ENTRY_NUM;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_TIME_TO_SAMPLE_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.video.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	if ( p_demuxer->file.video.time_scale == 0 )
	{
		p_demuxer->file.video.time_scale = UINT32_MAX;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_TIME_TO_SAMPLE_ENTRY*)p_demux_buf;
	left_entry_size = total_entry_size;
	i = 0;
	total_sample_time = 0;

	while( left_entry_size > 0 )
	{
		if( left_entry_size >= demux_buf_size )
		{
			next_read_size = demux_buf_size;

		}else
		{
			next_read_size = left_entry_size;
		}

		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		entry_count = next_read_size / MP4_TIME_TO_SAMPLE_ENTRY_SIZE;

		j = 0;
		while( j < entry_count )
		{
			sample_count = ntoh_u32(p_entry[j].sample_count);
			sample_delta = ntoh_u32(p_entry[j].sample_delta);

			k = 0;
			while( k < sample_count )
			{
				if( i >= p_demuxer->file.video.total_sample_count )
				{
					b_error = 1;
					error_code = -3;
					goto FIN;
				}

				p_demuxer->file.video.p_sample[i].sample_time = total_sample_time;
				p_demuxer->file.video.p_sample[i].sample_time_ms = total_sample_time * 1000 / p_demuxer->file.video.time_scale;

				i++;
				k++;
				total_sample_time += sample_delta;
			}

			j++;
		}

		left_entry_size -= next_read_size;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 video stsz
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 */
static int32 mp4_demux_video_stsz(MP4_FILE_DEMUXER* p_demuxer, ISO_SAMPLE_SIZE_BOX* p_box, int64 entry_offset)
{
	ISO_SAMPLE_SIZE_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 left_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	int32  b_error;
	int32  error_code;

	uint32 i, j;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * MP4_DEMUXER_READ_ENTRY_NUM;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->sample_count * MP4_SAMPLE_SIZE_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_SAMPLE_SIZE_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	/** init sample */
	p_demuxer->file.video.total_sample_count = p_box->sample_count;
	p_demuxer->file.video.p_sample = (MP4_SAMPLE*)malloc( sizeof(MP4_SAMPLE) * p_box->sample_count );
	if( p_demuxer->file.video.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_SAMPLE_SIZE_ENTRY*)p_demux_buf;
	left_entry_size = total_entry_size;
	i = 0;

	while( left_entry_size > 0 )
	{
		if( left_entry_size >= demux_buf_size )
		{
			next_read_size = demux_buf_size;

		}else
		{
			next_read_size = left_entry_size;
		}

		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		entry_count = next_read_size / MP4_SAMPLE_SIZE_ENTRY_SIZE;

		j = 0;
		while( j < entry_count )
		{
			p_demuxer->file.video.p_sample[i].sample_number = i;
			p_demuxer->file.video.p_sample[i].sample_size = ntoh_u32(p_entry[j].entry_size);

			i++;
			j++;
		}

		left_entry_size -= next_read_size;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 video stsc
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析video stsz成功才能调用该接口！
 */
static int32 mp4_demux_video_stsc(MP4_FILE_DEMUXER* p_demuxer, ISO_SAMPLE_TO_CHUNK_BOX* p_box, int64 entry_offset)
{
	ISO_SAMPLE_TO_CHUNK_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 chunk_number;
	uint32 first_chunk;
	uint32 next_chunk;
	uint32 sample_per_chunk;

	int32  b_error;
	int32  error_code;

	uint32 i, j;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	if( p_box->entry_count < 1 )
	{
		/** invalid entry count */
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.video.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_SAMPLE_TO_CHUNK_ENTRY*)p_demux_buf;
	next_read_size = demux_buf_size;
	i = 0;

	/** read first entry */
	ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
	if( !ret || dwReadSize < next_read_size )
	{
		b_error = 1;
		error_code = -4;
		goto FIN;
	}
	first_chunk = ntoh_u32(p_entry->first_chunk);
	sample_per_chunk = ntoh_u32(p_entry->sample_per_chunk);

	if( first_chunk != 1 )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	chunk_number = 0;
	entry_count = 1;
	while( entry_count < p_box->entry_count )
	{
		/** read next entry */
		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		next_chunk = ntoh_u32(p_entry->first_chunk) - 1;

		while( chunk_number < next_chunk )
		{
			for( j = 0; j < sample_per_chunk; j++ )
			{
				p_demuxer->file.video.p_sample[i].chunk_number = chunk_number;
				i++;
			}

			chunk_number++;
		}

		sample_per_chunk = ntoh_u32(p_entry->sample_per_chunk);
		entry_count++;
	}

	/** last entry */
	j = 0;
	while( i < p_demuxer->file.video.total_sample_count )
	{
		p_demuxer->file.video.p_sample[i].chunk_number = chunk_number;
		j++;

		if( j >= sample_per_chunk )
		{
			chunk_number++;
			j = 0;
		}

		i++;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 video stco
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析video stsz、video stsc成功才能调用该接口!
 */
static int32 mp4_demux_video_stco(MP4_FILE_DEMUXER* p_demuxer, ISO_CHUNK_OFFSET_BOX* p_box, int64 entry_offset)
{
	ISO_CHUNK_OFFSET_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 chunk_offset;
	uint32 sample_offset;

	int32  b_error;
	int32  error_code;

	int32  i;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_CHUNK_OFFSET_ENTRY_SIZE;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_CHUNK_OFFSET_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_CHUNK_OFFSET_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.video.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	p_demuxer->file.video.total_chunk_count = p_box->entry_count;
	i = p_demuxer->file.video.total_sample_count - 1;
	if( i < 0 || p_demuxer->file.video.p_sample[i].chunk_number != (p_box->entry_count - 1) )
	{
		/** chunk do not map */
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_CHUNK_OFFSET_ENTRY*)p_demux_buf;
	next_read_size = demux_buf_size;
	i = 0;
	entry_count = 0;

	while( entry_count < p_box->entry_count )
	{
		/** read entry */
		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		chunk_offset = ntoh_u32(p_entry->chunk_offset);
		sample_offset = chunk_offset;

		while( i < p_demuxer->file.video.total_sample_count )
		{
			if( p_demuxer->file.video.p_sample[i].chunk_number != entry_count )
			{
				/** next entry */
				break;
			}

			p_demuxer->file.video.p_sample[i].sample_offset = sample_offset;
			sample_offset += p_demuxer->file.video.p_sample[i].sample_size;

			i++;
		}

		entry_count++;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 video stss
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 */
static int32 mp4_demux_video_stss(MP4_FILE_DEMUXER* p_demuxer, ISO_SYNC_SAMPLE_BOX* p_box, int64 entry_offset)
{
	ISO_SYNC_SAMPLE_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 left_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	int32  b_error;
	int32  error_code;

	uint32 i, j;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_SYNC_SAMPLE_ENTRY_SIZE * MP4_DEMUXER_READ_ENTRY_NUM;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_SYNC_SAMPLE_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_SYNC_SAMPLE_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	/** init sync sample */
	p_demuxer->file.video.total_sync_sample_count = p_box->entry_count;
	p_demuxer->file.video.p_sync_sample = (MP4_SYNC_SAMPLE*)malloc( sizeof(MP4_SYNC_SAMPLE) * p_box->entry_count );
	if( p_demuxer->file.video.p_sync_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_SYNC_SAMPLE_ENTRY*)p_demux_buf;
	left_entry_size = total_entry_size;
	i = 0;

	while( left_entry_size > 0 )
	{
		if( left_entry_size >= demux_buf_size )
		{
			next_read_size = demux_buf_size;

		}else
		{
			next_read_size = left_entry_size;
		}

		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		entry_count = next_read_size / MP4_SYNC_SAMPLE_ENTRY_SIZE;

		j = 0;
		while( j < entry_count )
		{
			p_demuxer->file.video.p_sync_sample[i].sample_number = ntoh_u32(p_entry[j].sample_number) - 1;

			i++;
			j++;
		}

		left_entry_size -= next_read_size;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 audio stts
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析audio stsz成功才能调用该接口！
 */
static int32 mp4_demux_audio_stts(MP4_FILE_DEMUXER* p_demuxer, ISO_TIME_TO_SAMPLE_BOX* p_box, int64 entry_offset)
{
	ISO_TIME_TO_SAMPLE_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 left_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 sample_count;
	uint32 sample_delta;
	uint32 total_sample_time;

	int32  b_error;
	int32  error_code;

	uint32 i, j, k;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_TIME_TO_SAMPLE_ENTRY_SIZE * MP4_DEMUXER_READ_ENTRY_NUM;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_TIME_TO_SAMPLE_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_TIME_TO_SAMPLE_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.audio.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	if ( p_demuxer->file.audio.time_scale == 0 )
	{
		p_demuxer->file.audio.time_scale = UINT32_MAX;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_TIME_TO_SAMPLE_ENTRY*)p_demux_buf;
	left_entry_size = total_entry_size;
	i = 0;
	total_sample_time = 0;

	while( left_entry_size > 0 )
	{
		if( left_entry_size >= demux_buf_size )
		{
			next_read_size = demux_buf_size;

		}else
		{
			next_read_size = left_entry_size;
		}

		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		entry_count = next_read_size / MP4_TIME_TO_SAMPLE_ENTRY_SIZE;

		j = 0;
		while( j < entry_count )
		{
			sample_count = ntoh_u32(p_entry[j].sample_count);
			sample_delta = ntoh_u32(p_entry[j].sample_delta);

			k = 0;
			while( k < sample_count )
			{
				if( i >= p_demuxer->file.audio.total_sample_count )
				{
					b_error = 1;
					error_code = -3;
					goto FIN;
				}

				p_demuxer->file.audio.p_sample[i].sample_time = total_sample_time;
				p_demuxer->file.audio.p_sample[i].sample_time_ms = total_sample_time * 1000 / p_demuxer->file.audio.time_scale;

				i++;
				k++;
				total_sample_time += sample_delta;
			}

			j++;
		}

		left_entry_size -= next_read_size;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 audio stsz
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 */
static int32 mp4_demux_audio_stsz(MP4_FILE_DEMUXER* p_demuxer, ISO_SAMPLE_SIZE_BOX* p_box, int64 entry_offset)
{
	ISO_SAMPLE_SIZE_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 left_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	int32  b_error;
	int32  error_code;

	uint32 i, j;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_SAMPLE_SIZE_ENTRY_SIZE * MP4_DEMUXER_READ_ENTRY_NUM;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->sample_count * MP4_SAMPLE_SIZE_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_SAMPLE_SIZE_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	/** init sample */
	p_demuxer->file.audio.total_sample_count = p_box->sample_count;
	p_demuxer->file.audio.p_sample = (MP4_SAMPLE*)malloc( sizeof(MP4_SAMPLE) * p_box->sample_count );
	if( p_demuxer->file.video.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_SAMPLE_SIZE_ENTRY*)p_demux_buf;
	left_entry_size = total_entry_size;
	i = 0;

	while( left_entry_size > 0 )
	{
		if( left_entry_size >= demux_buf_size )
		{
			next_read_size = demux_buf_size;

		}else
		{
			next_read_size = left_entry_size;
		}

		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		entry_count = next_read_size / MP4_SAMPLE_SIZE_ENTRY_SIZE;

		j = 0;
		while( j < entry_count )
		{
			p_demuxer->file.audio.p_sample[i].sample_number = i;
			p_demuxer->file.audio.p_sample[i].sample_size = ntoh_u32(p_entry[j].entry_size);

			i++;
			j++;
		}

		left_entry_size -= next_read_size;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 audio stsc
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析audio stsz成功才能调用该接口！
 */
static int32 mp4_demux_audio_stsc(MP4_FILE_DEMUXER* p_demuxer, ISO_SAMPLE_TO_CHUNK_BOX* p_box, int64 entry_offset)
{
	ISO_SAMPLE_TO_CHUNK_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 chunk_number;
	uint32 first_chunk;
	uint32 next_chunk;
	uint32 sample_per_chunk;

	int32  b_error;
	int32  error_code;

	uint32 i, j;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	if( p_box->entry_count < 1 )
	{
		/** invalid entry count */
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_SAMPLE_TO_CHUNK_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_SAMPLE_TO_CHUNK_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.audio.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_SAMPLE_TO_CHUNK_ENTRY*)p_demux_buf;
	next_read_size = demux_buf_size;
	i = 0;

	/** read first entry */
	ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
	if( !ret || dwReadSize < next_read_size )
	{
		b_error = 1;
		error_code = -4;
		goto FIN;
	}
	first_chunk = ntoh_u32(p_entry->first_chunk);
	sample_per_chunk = ntoh_u32(p_entry->sample_per_chunk);

	if( first_chunk != 1 )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	chunk_number = 0;
	entry_count = 1;
	while( entry_count < p_box->entry_count )
	{
		/** read next entry */
		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		next_chunk = ntoh_u32(p_entry->first_chunk) - 1;

		while( chunk_number < next_chunk )
		{
			for( j = 0; j < sample_per_chunk; j++ )
			{
				p_demuxer->file.audio.p_sample[i].chunk_number = chunk_number;
				i++;
			}

			chunk_number++;
		}

		sample_per_chunk = ntoh_u32(p_entry->sample_per_chunk);
		entry_count++;
	}

	/** last entry */
	j = 0;
	while( i < p_demuxer->file.audio.total_sample_count )
	{
		p_demuxer->file.audio.p_sample[i].chunk_number = chunk_number;
		j++;

		if( j >= sample_per_chunk )
		{
			chunk_number++;
			j = 0;
		}

		i++;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}

/**  
 * @brief 解析MP4 audio stco
 * @param [in] p_demuxer 解析器句柄
 * @param [in] p_box Box句柄
 * @param [in] entry_offset Entry偏移
 * @return
 * 0：成功
 * -1：参数错误
 * -2：内存不足
 * -3：文件格式错误
 * -4：无法打开文件
 * -5：文件太大
 * @remark
 * 1、必须先解析audio stsz、audio stsc成功才能调用该接口!
 */
static int32 mp4_demux_audio_stco(MP4_FILE_DEMUXER* p_demuxer, ISO_CHUNK_OFFSET_BOX* p_box, int64 entry_offset)
{
	ISO_CHUNK_OFFSET_ENTRY* p_entry = NULL;
	uint8* p_demux_buf = NULL;
	uint32 demux_buf_size;
	uint32 total_entry_size;
	uint32 next_read_size;
	uint32 entry_count;
	DWORD  dwReadSize;

	uint32 chunk_offset;
	uint32 sample_offset;

	int32  b_error;
	int32  error_code;

	int32  i;
	int32  ret;

	b_error = 0;

	demux_buf_size = MP4_CHUNK_OFFSET_ENTRY_SIZE;
	p_demux_buf = (uint8*)malloc(demux_buf_size);
	if( p_demux_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	total_entry_size = p_box->entry_count * MP4_CHUNK_OFFSET_ENTRY_SIZE;
	if( total_entry_size != (p_box->full_box.box.size - MP4_CHUNK_OFFSET_BOX_MIN_SIZE) )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	if( p_demuxer->file.audio.p_sample == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	p_demuxer->file.audio.total_chunk_count = p_box->entry_count;
	i = p_demuxer->file.audio.total_sample_count - 1;
	if( i < 0 || p_demuxer->file.audio.p_sample[i].chunk_number != (p_box->entry_count - 1) )
	{
		/** chunk do not map */
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	/** set pos */
	ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&entry_offset), NULL, FILE_BEGIN);
	if( !ret )
	{
		b_error = 1;
		error_code = -3;
		goto FIN;
	}

	p_entry = (ISO_CHUNK_OFFSET_ENTRY*)p_demux_buf;
	next_read_size = demux_buf_size;
	i = 0;
	entry_count = 0;

	while( entry_count < p_box->entry_count )
	{
		/** read entry */
		ret = ReadFile(p_demuxer->file.m_hFile, p_demux_buf, next_read_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < next_read_size )
		{
			b_error = 1;
			error_code = -4;
			goto FIN;
		}
		chunk_offset = ntoh_u32(p_entry->chunk_offset);
		sample_offset = chunk_offset;

		while( i < p_demuxer->file.audio.total_sample_count )
		{
			if( p_demuxer->file.audio.p_sample[i].chunk_number != entry_count )
			{
				/** next entry */
				break;
			}

			p_demuxer->file.audio.p_sample[i].sample_offset = sample_offset;
			sample_offset += p_demuxer->file.audio.p_sample[i].sample_size;

			i++;
		}

		entry_count++;
	}

FIN:
	if( p_demux_buf )
	{
		free(p_demux_buf);
		p_demux_buf = NULL;
	}

	if( b_error )
	{
		return error_code;

	}else
	{
		return 0;
	}
}


/**  
 * @brief mp4 file parse
 * @param [in] p_demuxer, demuxer handle
 * @return
 * 0: success
 * -1: invalid param
 * -2: not enough memory
 * -3: wrong file format
 * -4: can not open file
 * -5: file too large
 * @remark
 */
static int32 mp4_file_parse(MP4_FILE_DEMUXER* p_demuxer)
{
	MP4_BOX_INFO box_info;

	ISO_MOVIE_HEADER_BOX movie_header_box;
	ISO_TRACK_HEADER_BOX track_header_box;
	ISO_MEDIA_HEADER_BOX media_header_box;
	ISO_HANDLER_BOX handler_box;

	ISO_VISUAL_SAMPLE_DESC_BOX visual_sample_desc_box;
	ISO_VISUAL_SAMPLE_ENTRY visual_sample_entry;
	ISO_AUDIO_SAMPLE_DESC_BOX audio_sample_desc_box;
	ISO_AUDIO_SAMPLE_ENTRY audio_sample_entry;

	ISO_TIME_TO_SAMPLE_BOX video_time_to_sample_box;
	ISO_TIME_TO_SAMPLE_BOX audio_time_to_sample_box;
	ISO_SAMPLE_SIZE_BOX video_sample_size_box;
	ISO_SAMPLE_SIZE_BOX audio_sample_size_box;
	ISO_SAMPLE_TO_CHUNK_BOX video_sample_to_chunk_box;
	ISO_SAMPLE_TO_CHUNK_BOX audio_sample_to_chunk_box;
	ISO_CHUNK_OFFSET_BOX video_chunk_offset_box;
	ISO_CHUNK_OFFSET_BOX audio_chunk_offset_box;
	ISO_SYNC_SAMPLE_BOX video_sync_sample_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	int32  b_error;
	int32  b_continue;
	int32  b_demux;
	int32  b_skip;

	int32  b_find_track_header;
	int32  b_find_media_header;
	int32  b_find_handler;
	int32  b_video_stts;
	int32  b_video_stsz;
	int32  b_video_stsc;
	int32  b_video_stco;
	int32  b_video_stss;
	int32  b_audio_stts;
	int32  b_audio_stsz;
	int32  b_audio_stsc;
	int32  b_audio_stco;

	uint32 read_size;

	int64  file_size;
	int64  total_read_size;
	int64  video_stts_offset;
	int64  video_stsz_offset;
	int64  video_stsc_offset;
	int64  video_stco_offset;
	int64  video_stss_offset;
	int64  audio_stts_offset;
	int64  audio_stsz_offset;
	int64  audio_stsc_offset;
	int64  audio_stco_offset;

	uint32 buf_total_size;
	uint32 buf_use_size;
	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	uint32 total_demux_size;

	int64  box_size;
	int64  skip_size;

	uint32 track_id;
	uint32 track_type;
	uint32 width;
	uint32 height;
	uint32 time_scale;
	uint32 duration;

	int32  error_code;
	int32  ret;

	b_error = 0;

	p_file_buf = (uint8*)malloc(MP4_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = -2;
		goto FIN;
	}

	/** get file size */
	//ret = GetFileSizeEx(p_demuxer->file.m_hFile, (LARGE_INTEGER*)&file_size);
	ret = gen_file_get_size(p_demuxer->file.file_h, &file_size);
	if( !ret )
	{
		b_error = 1;
		error_code = -4;
		goto FIN;
	}

	if( file_size >= UINT32_MAX )
	{
		b_error = 1;
		error_code = -5;
		goto FIN;
	}

	p_demuxer->file.stat.track_count = 0;
	p_demuxer->file.track_count = 0;
	p_demuxer->file.b_has_video = 0;
	p_demuxer->file.b_has_audio = 0;

	b_continue = 1;
	b_find_track_header = 0;
	b_find_media_header = 0;
	b_find_handler = 0;
	b_video_stts = 0;
	b_video_stsz = 0;
	b_video_stsc = 0;
	b_video_stco = 0;
	b_video_stss = 0;
	b_audio_stts = 0;
	b_audio_stsz = 0;
	b_audio_stsc = 0;
	b_audio_stco = 0;

	undemux_size = 0;
	buf_total_size = MP4_DEMUXER_FILE_BUF_SIZE;
	buf_use_size = 0;
	buf_left_size = MP4_DEMUXER_FILE_BUF_SIZE;
	total_read_size = 0;
	total_demux_size = 0;

	while( b_continue )
	{
		ret = ReadFile(p_demuxer->file.m_hFile, p_file_buf + buf_use_size, buf_left_size, &dwReadSize, NULL);
		if( !ret || dwReadSize < buf_left_size )
		{
			/** file end */
			b_continue = 0;
		}

		buf_use_size += dwReadSize;
		buf_left_size -= dwReadSize;
		total_read_size += dwReadSize;

		b_demux = 1;
		p_uint8 = p_file_buf;
		undemux_size = buf_use_size;
		total_demux_size = 0;

		while( b_demux )
		{
			ret = mp4_demux_box(&box_info, p_uint8, undemux_size, &demux_size);
			if( ret )
			{
				/** need more */
				break;
			}

			if( box_info.param.box.size == 0 )
			{
				/** not support */
				b_error = 1;
				error_code = -3;
				b_continue = 0;
				goto FIN;
				//break;
			}

			if( box_info.format == MP4_DEMUX_BOX_FORMAT_ISO_BOX )
			{
				box_size = box_info.param.box.size;

			}else
			{
				box_size = box_info.param.box64.large_size;
			}

			if( box_size < demux_size )
			{
				/** invalid box size */
				b_error = 1;
				error_code = -3;
				b_continue = 0;
				goto FIN;
				//break;
			}

			b_skip = 0;
			switch( box_info.param.box.type )
			{
			case MP4_MOVIE_BOX_TYPE:
			case MP4_MEDIA_INFO_BOX_TYPE:
			case MP4_SAMPLE_TABLE_BOX_TYPE:
				{

				}
				break;

			case MP4_MEDIA_BOX_TYPE:
				{
					b_find_media_header = 0;
					b_find_handler = 0;
				}
				break;

			case MP4_TRACK_BOX_TYPE:
				{
					b_find_track_header = 0;
					track_type = 0x0;
					p_demuxer->file.stat.track_count++;
				}
				break;

			case MP4_MOVIE_HEADER_BOX_TYPE:
				{
					ret = mp4_demux_movie_header_box(&movie_header_box, p_uint8, undemux_size, &demux_size);
					if( ret == 0 )
					{
						p_demuxer->file.stat.time_scale = movie_header_box.time_scale;
						p_demuxer->file.stat.duration = movie_header_box.duration;
						if( p_demuxer->file.stat.time_scale != 0 )
						{
							/** calc duration */
							p_demuxer->file.stat.duration_ms = (movie_header_box.duration / movie_header_box.time_scale) * 1000;

						}else
						{
							/** invalid time scale */
							b_error = 1;
							error_code = -3;
							b_continue = 0;
							b_demux = 0;
						}

					}else
					{
						/** need more */
						b_demux = 0;
					}
				}
				break;

			case MP4_TRACK_HEADER_BOX_TYPE:
				{
					ret = mp4_demux_track_header_box(&track_header_box, p_uint8, undemux_size, &demux_size);
					if( ret == 0 )
					{
						b_find_track_header = 1;

						track_id = track_header_box.track_id;
						width = track_header_box.width;
						height = track_header_box.height;

					}else
					{
						/** need more */
						b_demux = 0;
					}
				}
				break;

			case MP4_MEDIA_HEADER_BOX_TYPE:
				{
					ret = mp4_demux_media_header_box(&media_header_box, p_uint8, undemux_size, &demux_size);
					if( ret == 0 )
					{
						b_find_media_header = 1;

						time_scale = media_header_box.time_scale;
						duration = media_header_box.duration;

					}else
					{
						/** need more */
						b_demux = 0;
					}
				}
				break;

			case MP4_HANDLER_BOX_TYPE:
				{
					ret = mp4_demux_handler_box(&handler_box, p_uint8, undemux_size, &demux_size);
					if( ret == 0 )
					{
						b_find_handler = 1;
						track_type = handler_box.handler_type;

						if( handler_box.handler_type == MP4_HANDLER_TYPE_VIDE )
						{
							/** video */
							p_demuxer->file.b_has_video = 1;

							if( b_find_track_header )
							{
								if( p_demuxer->file.track_count < MP4_DEMUXER_MAX_TRACK_NUM )
								{
									p_demuxer->file.track[p_demuxer->file.track_count].track_id = track_id;
									p_demuxer->file.track[p_demuxer->file.track_count].track_type = MP4_TRACK_TYPE_VIDEO;
									p_demuxer->file.track_count++;
								}

								p_demuxer->file.video.track_id = track_id;
								p_demuxer->file.video.width = (width >> 16) & 0xFFFF;
								p_demuxer->file.video.height = (height >> 16) & 0xFFFF;
							}

							if( b_find_media_header )
							{
								p_demuxer->file.video.time_scale = time_scale;
								p_demuxer->file.video.duration = duration;

								if( time_scale != 0 )
								{
									p_demuxer->file.video.duration_ms = (duration / time_scale) * 1000;
								}
							}

						}else if( handler_box.handler_type == MP4_HANDLER_TYPE_SOUN )
						{
							/** audio */
							p_demuxer->file.b_has_audio = 1;

							if( b_find_track_header )
							{
								if( p_demuxer->file.track_count < MP4_DEMUXER_MAX_TRACK_NUM )
								{
									p_demuxer->file.track[p_demuxer->file.track_count].track_id = track_id;
									p_demuxer->file.track[p_demuxer->file.track_count].track_type = MP4_TRACK_TYPE_AUDIO;
									p_demuxer->file.track_count++;
								}

								p_demuxer->file.audio.track_id = track_id;
							}

							if( b_find_media_header )
							{
								p_demuxer->file.audio.time_scale = time_scale;
								p_demuxer->file.audio.duration = duration;

								if( time_scale != 0 )
								{
									p_demuxer->file.audio.duration_ms = (duration / time_scale) * 1000;
								}

								p_demuxer->file.audio.sample_rate = time_scale;
							}
						}

					}else
					{
						/** need more */
						b_demux = 0;
					}
				}
				break;

			case MP4_SAMPLE_DESC_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							visual_sample_desc_box.entry_count = 1;
							visual_sample_desc_box.p_entry = &visual_sample_entry;
							ret = mp4_demux_visual_sample_desc_box(&visual_sample_desc_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								p_demuxer->file.video.fourcc = visual_sample_desc_box.p_entry[0].entry.box.type;

							}else
							{
								b_demux = 0;
							}

						}else if( track_type == MP4_HANDLER_TYPE_SOUN )
						{
							audio_sample_desc_box.entry_count = 1;
							audio_sample_desc_box.p_entry = &audio_sample_entry;
							ret = mp4_demux_audio_sample_desc_box(&audio_sample_desc_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								p_demuxer->file.audio.fourcc = audio_sample_desc_box.p_entry[0].entry.box.type;
								p_demuxer->file.audio.sample_rate = (audio_sample_desc_box.p_entry[0].sample_rate >> 16) & 0xFFFF;
								p_demuxer->file.audio.sample_size = audio_sample_desc_box.p_entry[0].sample_size;
								p_demuxer->file.audio.chn_count = audio_sample_desc_box.p_entry[0].chn_count;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			case MP4_TIME_TO_SAMPLE_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							ret = mp4_demux_time_to_sample_box(&video_time_to_sample_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_video_stts = 1;
								video_stts_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else if( track_type == MP4_HANDLER_TYPE_SOUN )
						{
							ret = mp4_demux_time_to_sample_box(&audio_time_to_sample_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_audio_stts = 1;
								audio_stts_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			case MP4_SAMPLE_SIZE_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							ret = mp4_demux_sample_size_box(&video_sample_size_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_video_stsz = 1;
								video_stsz_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else if( track_type == MP4_HANDLER_TYPE_SOUN )
						{
							ret = mp4_demux_sample_size_box(&audio_sample_size_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_audio_stsz = 1;
								audio_stsz_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			case MP4_SAMPLE_TO_CHUNK_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							ret = mp4_demux_sample_to_chunk_box(&video_sample_to_chunk_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_video_stsc = 1;
								video_stsc_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else if( track_type == MP4_HANDLER_TYPE_SOUN )
						{
							ret = mp4_demux_sample_to_chunk_box(&audio_sample_to_chunk_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_audio_stsc = 1;
								audio_stsc_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			case MP4_CHUNK_OFFSET_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							ret = mp4_demux_chunk_offset_box(&video_chunk_offset_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_video_stco = 1;
								video_stco_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else if( track_type == MP4_HANDLER_TYPE_SOUN )
						{
							ret = mp4_demux_chunk_offset_box(&audio_chunk_offset_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_audio_stco = 1;
								audio_stco_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			case MP4_SYNC_SAMPLE_BOX_TYPE:
				{
					if( b_find_handler )
					{
						if( track_type == MP4_HANDLER_TYPE_VIDE )
						{
							ret = mp4_demux_sync_sample_box(&video_sync_sample_box, p_uint8, undemux_size, &demux_size);
							if( ret == 0 )
							{
								b_video_stss = 1;
								video_stss_offset = total_read_size - buf_use_size + total_demux_size + demux_size;

								b_skip = 1;
								skip_size = box_size;

							}else
							{
								b_demux = 0;
							}

						}else
						{
							b_skip = 1;
							skip_size = box_size;
						}

					}else
					{
						b_skip = 1;
						skip_size = box_size;
					}
				}
				break;

			default:
				{
					/** unknown box */
					b_skip = 1;
					skip_size = box_size;
				}
				break;
			}

			if( b_skip )
			{
				/** skip */
				if( undemux_size >= skip_size )
				{
					/** skip in buf */
					p_uint8 += skip_size;
					undemux_size -= skip_size;
					total_demux_size += skip_size;

				}else
				{
					/** skip in file */
					skip_size -= undemux_size;

					ret = SetFilePointerEx(p_demuxer->file.m_hFile, *((LARGE_INTEGER*)&skip_size), NULL, FILE_CURRENT);
					if( !ret )
					{
						b_error = 1;
						error_code = -3;
						goto FIN;
					}
					total_read_size += skip_size;

					total_demux_size += undemux_size;
					undemux_size = 0;
					b_demux = 0;
				}

			}else
			{
				/** continue demux */
				p_uint8 += demux_size;
				undemux_size -= demux_size;
				total_demux_size += demux_size;
			}

		}//end while( b_demux )

		if( total_demux_size == 0 )
		{
			/** box bigger than demux buf size */
			log_warn(_T("[mp4_demuxer::mp4_file_parse] box overflow!\n"));
			b_error = 1;
			error_code = -3;
			b_continue = 0;

		}else if( undemux_size > 0 )
		{
			/** copy undemux size */
			memcpy(p_file_buf, p_file_buf + total_demux_size, undemux_size);
		}

		buf_use_size = undemux_size;
		buf_left_size = buf_total_size - buf_use_size;
	}

	/** get video sample info */
	if( p_demuxer->file.b_has_video )
	{
		/** video stsz */
		if( !b_video_stsz )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}
		
		ret = mp4_demux_video_stsz(p_demuxer, &video_sample_size_box, video_stsz_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** video stts */
		if( !b_video_stts )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_video_stts(p_demuxer, &video_time_to_sample_box, video_stts_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** video stsc */
		if( !b_video_stsc )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_video_stsc(p_demuxer, &video_sample_to_chunk_box, video_stsc_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** video stco */
		if( !b_video_stco )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_video_stco(p_demuxer, &video_chunk_offset_box, video_stco_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** video stss */
		if( b_video_stss )
		{
			ret = mp4_demux_video_stss(p_demuxer, &video_sync_sample_box, video_stss_offset);
			if( ret )
			{
				b_error = 1;
				error_code = -3;
				goto FIN;
			}
		}
	}

	/** get audio sample info */
	if( p_demuxer->file.b_has_audio )
	{
		/** audio stsz */
		if( !b_audio_stsz )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_audio_stsz(p_demuxer, &audio_sample_size_box, audio_stsz_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** audio stts */
		if( !b_audio_stts )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_audio_stts(p_demuxer, &audio_time_to_sample_box, audio_stts_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** audio stsc */
		if( !b_audio_stsc )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_audio_stsc(p_demuxer, &audio_sample_to_chunk_box, audio_stsc_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		/** audio stco */
		if( !b_audio_stco )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}

		ret = mp4_demux_audio_stco(p_demuxer, &audio_chunk_offset_box, audio_stco_offset);
		if( ret )
		{
			b_error = 1;
			error_code = -3;
			goto FIN;
		}
	}

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	if( b_error )
	{
		return error_code;
	}

	return 0;
}

///////////////////////////////// Outter Interface ////////////////////////////
MP4_FILE_DEMUXER_H mp4_file_demuxer_create()
{
	MP4_FILE_DEMUXER *p_demuxer = NULL;
	p_demuxer = (MP4_FILE_DEMUXER*)malloc( sizeof(MP4_FILE_DEMUXER) );

	if( p_demuxer == NULL )
	{
		return NULL;
	}

	memset(p_demuxer, 0, sizeof(MP4_FILE_DEMUXER));

	return p_demuxer;
}

int32 mp4_file_demuxer_destroy(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	if( p_demuxer == NULL )
	{
		return -1;
	}

	if( p_demuxer->b_init )
	{
		mp4_file_demuxer_deinit(p_demuxer);
	}

	free(p_demuxer);

	return 0;
}

int32 mp4_file_demuxer_init(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  ret;

	if( p_demuxer == NULL )
	{
		return -1;
	}

	if( p_demuxer->b_init )
	{
		return 0;
	}

	memset(&p_demuxer->file, 0, sizeof(p_demuxer->file));
	p_demuxer->file.file_h = gen_file_malloc();
	ret = gen_file_init(p_demuxer->file.file_h);
	p_demuxer->b_demux = 0;

	p_demuxer->b_init = 1;

	return 0;
}

int32 mp4_file_demuxer_deinit(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	if( p_demuxer == NULL )
	{
		return -1;
	}

	if( p_demuxer->file.video.p_sample )
	{
		free(p_demuxer->file.video.p_sample);
		p_demuxer->file.video.p_sample = NULL;
	}

	if( p_demuxer->file.video.p_sync_sample )
	{
		free(p_demuxer->file.video.p_sync_sample);
		p_demuxer->file.video.p_sync_sample = NULL;
	}

	if( p_demuxer->file.audio.p_sample )
	{
		free(p_demuxer->file.audio.p_sample);
		p_demuxer->file.audio.p_sample = NULL;
	}

	if( p_demuxer->file.file_h )
	{
		gen_file_free(p_demuxer->file.file_h);
		p_demuxer->file.file_h = NULL;
	}

	p_demuxer->b_init = 0;

	return 0;
}

int32 mp4_file_demuxer_reset(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	if( p_demuxer == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	if( p_demuxer->file.video.p_sample )
	{
		free(p_demuxer->file.video.p_sample);
		p_demuxer->file.video.p_sample = NULL;
	}

	if( p_demuxer->file.video.p_sync_sample )
	{
		free(p_demuxer->file.video.p_sync_sample);
		p_demuxer->file.video.p_sync_sample = NULL;
	}

	if( p_demuxer->file.audio.p_sample )
	{
		free(p_demuxer->file.audio.p_sample);
		p_demuxer->file.audio.p_sample = NULL;
	}

	if( p_demuxer->file.file_h )
	{
		gen_file_close(p_demuxer->file.file_h);
		//p_demuxer->file.file_h = NULL;
	}

	p_demuxer->b_demux = 0;

	return 0;
}

int32 mp4_file_demuxer_demux(MP4_FILE_DEMUXER_H demuxer_h, int8* p_file_path, int32 file_path_size)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	GEN_FILE_OPEN_PARAM  open_param;

	int32  b_error;
	int32  error_code;

	int32  ret;

	if( p_demuxer == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	b_error = 0;
	ret = mp4_file_demuxer_reset(demuxer_h);

	open_param.rw = GEN_FILE_OPEN_READ;
	open_param.share = GEN_FILE_SHARE_READ;
	open_param.create = GEN_FILE_CREATE_EXIST;
	open_param.open_flag = 0;
	ret = gen_file_set_param(p_demuxer->file.file_h, GEN_FILE_PARAM_OPEN, (uint8*)&open_param, sizeof(open_param));

	ret = gen_file_openA(p_demuxer->file.file_h, p_file_path, file_path_size);
	if( ret )
	{
		b_error = 1;
		error_code = -4;
		goto FIN;
	}

	ret = mp4_file_parse(p_demuxer);
	if( ret == 0 )
	{
		p_demuxer->b_demux = 1;

	}else
	{
		b_error = 1;
		error_code = ret;
		goto FIN;
	}

FIN:
	gen_file_close(p_demuxer->file.file_h);

	if( b_error )
	{
		return error_code;
	}

	return 0;
}

int32 mp4_file_demuxer_get_file_stat_info(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_STAT* p_stat)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  ret;

	if( p_demuxer == NULL || p_stat == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	if( !p_demuxer->b_demux )
	{
		return -3;
	}

	memcpy(p_stat, &p_demuxer->file.stat, sizeof(p_demuxer->file.stat));

	return 0;
}

int32 mp4_file_demuxer_get_track_stat_info(MP4_FILE_DEMUXER_H demuxer_h, MP4_TRACK_STAT* p_stat)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  ret;

	if( p_demuxer == NULL || p_stat == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	if( !p_demuxer->b_demux )
	{
		return -3;
	}

	p_stat->track_count = p_demuxer->file.track_count;
	p_stat->p_track = p_demuxer->file.track;

	return 0;
}

int32 mp4_file_demuxer_get_video_track(MP4_FILE_DEMUXER_H demuxer_h, MP4_VIDEO_TRACK* p_track)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  ret;

	if( p_demuxer == NULL || p_track == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	if( !p_demuxer->b_demux )
	{
		return -3;
	}

	if( !p_demuxer->file.b_has_video )
	{
		return -4;
	}

	memcpy(p_track, &p_demuxer->file.video, sizeof(p_demuxer->file.video));

	return 0;
}

int32 mp4_file_demuxer_get_audio_track(MP4_FILE_DEMUXER_H demuxer_h, MP4_AUDIO_TRACK* p_track)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  ret;

	if( p_demuxer == NULL || p_track == NULL )
	{
		return -1;
	}

	if( !p_demuxer->b_init )
	{
		return -1;
	}

	if( !p_demuxer->b_demux )
	{
		return -3;
	}

	if( !p_demuxer->file.b_has_audio )
	{
		return -4;
	}

	memcpy(p_track, &p_demuxer->file.audio, sizeof(p_demuxer->file.audio));

	return 0;
}
