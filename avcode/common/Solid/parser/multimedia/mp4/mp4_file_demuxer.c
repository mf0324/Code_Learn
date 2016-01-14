
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <public/gen_endian.h>
#include <public/gen_file_const.h>
#include <public/gen_file.h>
#include <public/gen_error.h>
//#include <util/log_debug.h>

#include "mp4_demux.h"
#include "mp4_file_tree.h"
#include "mp4_file_demuxer.h"


/** file buf size */
#define MP4_FILE_DEMUXER_FILE_BUF_SIZE                     (4 * 1024)
/** max track num */
#define MP4_FILE_DEMUXER_MAX_TRACK_NUM                     (4)


/** mp4 file */
typedef struct tag_mp4_file
{
	/** file handle */
	GEN_FILE_H  file_h;

	/** file tree */
	MP4_FILE_TREE* p_tree;

	/** file stat */
	MP4_FILE_STAT stat;

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



/////////////////////////////// Inner Interface ///////////////////////////////
/**  
 * @brief demux sample table box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_sample_table_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;

	uint32 read_size;

	int64  current_offset;

	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;
	int64  box_left_size;

	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	current_offset = p_box_node->box.offset + MP4_SAMPLE_TABLE_BOX_SIZE;
	p_parent = p_box_node;
	box_size = p_box_node->box.payload.box.size;
	box_left_size = p_box_node->box.payload.box.size - MP4_SAMPLE_TABLE_BOX_SIZE;
	total_demux_size = 0;

	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	while( total_demux_size < box_left_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_sample_table_box] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_sample_table_box] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_sample_table_box] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_SAMPLE_DESC_BOX_TYPE:
		case MP4_TIME_TO_SAMPLE_BOX_TYPE:
		case MP4_COMPOSITION_TIME_TO_SAMPLE_BOX_TYPE:
		case MP4_SAMPLE_TO_CHUNK_BOX_TYPE:
		case MP4_SAMPLE_SIZE_BOX_TYPE:
		case MP4_CHUNK_OFFSET_BOX_TYPE:
		case MP4_SYNC_SAMPLE_BOX_TYPE:
			{
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_sample_table_box] demux box offset 0x%I64x unknown type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < box_left_size )

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief demux media info box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_media_info_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;

	uint32 read_size;

	int64  current_offset;

	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;
	int64  box_left_size;

	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	current_offset = p_box_node->box.offset + MP4_MEDIA_INFO_BOX_SIZE;
	p_parent = p_box_node;
	box_size = p_box_node->box.payload.box.size;
	box_left_size = p_box_node->box.payload.box.size - MP4_MEDIA_INFO_BOX_SIZE;
	total_demux_size = 0;

	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	while( total_demux_size < box_left_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_info_box] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_info_box] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_info_box] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_VIDEO_MEDIA_HEADER_BOX_TYPE:
		case MP4_SOUND_MEDIA_HEADER_BOX_TYPE:
		case MP4_DATA_INFO_BOX_TYPE:
			{
			}
			break;

		case MP4_SAMPLE_TABLE_BOX_TYPE:
			{
				/** stbl */
				ret = mp4_file_demux_sample_table_box(p_demuxer, p_node);
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_info_box] demux offset 0x%I64x box unknown type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < box_left_size )

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief demux media box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_media_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;
	//int32  b_continue;

	uint32 read_size;

	int64  current_offset;

	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;
	int64  box_left_size;

	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	current_offset = p_box_node->box.offset + MP4_MEDIA_BOX_SIZE;
	p_parent = p_box_node;
	box_size = p_box_node->box.payload.box.size;
	box_left_size = p_box_node->box.payload.box.size - MP4_MEDIA_BOX_SIZE;
	total_demux_size = 0;

	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	while( total_demux_size < box_left_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_box] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_box] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_box] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_MEDIA_HEADER_BOX_TYPE:
		case MP4_HANDLER_BOX_TYPE:
			{
			}
			break;

		case MP4_MEDIA_INFO_BOX_TYPE:
			{
				/** minf */
				ret = mp4_file_demux_media_info_box(p_demuxer, p_node);
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_media_box] demux offset 0x%I64x box unknown type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < box_left_size )

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief demux track box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_track_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;
	//int32  b_continue;

	uint32 read_size;

	int64  current_offset;

	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;
	int64  box_left_size;

	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	current_offset = p_box_node->box.offset + MP4_TRACK_BOX_SIZE;
	p_parent = p_box_node;
	box_size = p_box_node->box.payload.box.size;
	box_left_size = p_box_node->box.payload.box.size - MP4_TRACK_BOX_SIZE;
	total_demux_size = 0;

	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	while( total_demux_size < box_left_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_track_box] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_track_box] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_track_box] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_TRACK_HEADER_BOX_TYPE:
			{
			}
			break;

		case MP4_MEDIA_BOX_TYPE:
			{
				/** mdia */
				ret = mp4_file_demux_media_box(p_demuxer, p_node);
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_track_box] demux offset 0x%I64x box unknown type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < box_left_size )

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief demux movie header box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_movie_header_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	uint32 read_size;

	uint32 buf_total_size;
	//uint32 buf_use_size;
	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	//uint32 total_demux_size;

	//int64  box_size;

	int32  b_error;
	int32  error_code;

	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}
	buf_total_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;
	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	if( p_box_node->box.payload.box.size > buf_total_size )
	{
		b_error = 1;
		error_code = GEN_E_NEED_MORE_BUF;
		goto FIN;
	}

	ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, p_box_node->box.offset, NULL);
	if( ret )
	{
		//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_header_box] seek error = %d!\n"), ret);
		b_error = 1;
		error_code = GEN_E_INNER_ERROR;
		goto FIN;
	}

	ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
	if( ret < 0 )
	{
		/** read file error */
		b_error = 1;
		error_code = GEN_E_READ_FAIL;
		goto FIN;
	}

	p_uint8 = p_file_buf;
	undemux_size = read_size;

	ret = mp4_demux_movie_header_box(&p_demuxer->file.stat.mvhd_box, p_uint8, undemux_size, &demux_size);
	if( ret == 0 )
	{
		if( p_demuxer->file.stat.mvhd_box.time_scale != 0 )
		{
			p_demuxer->file.stat.duration_ms = (p_demuxer->file.stat.mvhd_box.duration / p_demuxer->file.stat.mvhd_box.time_scale) * 1000;

		}else
		{
			/** invalid time scale */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
		}

	}else
	{
		/** need more */
		b_error = 1;
		error_code = GEN_E_NEED_MORE_DATA;
	}

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}

/**  
 * @brief demux movie box
 * @param [in] p_demuxer, demuxer
 * @param [in] p_box_node, box node
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_demux_movie_box(MP4_FILE_DEMUXER* p_demuxer, MP4_FILE_BOX_NODE* p_box_node)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;
	//int32  b_continue;

	uint32 read_size;

	int64  current_offset;

	uint32 buf_total_size;
	uint32 buf_use_size;
	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;
	int64  box_left_size;

	int32  error_code;
	int32  ret;

	b_error = 0;
	error_code = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	//b_continue = 1;
	current_offset = p_box_node->box.offset + MP4_MOVIE_BOX_SIZE;
	p_parent = p_box_node;
	box_size = p_box_node->box.payload.box.size;
	box_left_size = p_box_node->box.payload.box.size - MP4_MOVIE_BOX_SIZE;

	buf_total_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;
	buf_use_size = 0;
	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;
	total_demux_size = 0;

	while( total_demux_size < box_left_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_box] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_box] read error = %d!\n"), ret);
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_box] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_box] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_MOVIE_HEADER_BOX_TYPE:
			{
				/** mvhd */
				ret = mp4_file_demux_movie_header_box(p_demuxer, p_node);
			}
			break;

		case MP4_MOVIE_EXTEND_BOX_TYPE:
		case MP4_OBJECT_DESC_BOX_TYPE:
			{
				//ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, NULL);
			}
			break;

		case MP4_TRACK_BOX_TYPE:
			{
				/** trak */
				ret = mp4_file_demux_track_box(p_demuxer, p_node);
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_demux_movie_box] demux offset 0x%I64x unknown box type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < box_left_size )

FIN:
	if( p_file_buf )
	{
		free(p_file_buf);
		p_file_buf = NULL;
	}

	return GEN_S_OK;
}


/**  
 * @brief mp4 file parse
 * @param [in] p_demuxer, demuxer handle
 * @return
 * 0: success
 * other: fail
 * @remark
 */
static int32 mp4_file_parse(MP4_FILE_DEMUXER* p_demuxer)
{
	MP4_BOX box;
	MP4_FILE_BOX file_box;

	uint8* p_file_buf = NULL;
	uint8* p_uint8 = NULL;

	MP4_FILE_BOX_NODE* p_parent = NULL;
	MP4_FILE_BOX_NODE* p_node = NULL;

	int32  b_error;
	int32  b_continue;

	uint32 read_size;

	int64  current_offset;
	int64  file_size;

	//uint32 buf_total_size;
	//uint32 buf_use_size;
	uint32 buf_left_size;

	uint32 demux_size;
	uint32 undemux_size;
	int64  total_demux_size;

	int64  box_size;

	int32  error_code;
	int32  i;
	int32  ret;

	b_error = 0;

	p_file_buf = (uint8*)malloc(MP4_FILE_DEMUXER_FILE_BUF_SIZE);
	if( p_file_buf == NULL )
	{
		b_error = 1;
		error_code = GEN_E_NOT_ENOUGH_MEM;
		goto FIN;
	}

	/** get file size */
	ret = gen_file_get_size(p_demuxer->file.file_h, &file_size);
	if( ret )
	{
		b_error = 1;
		error_code = GEN_E_FAIL;
		goto FIN;
	}
	//log_debug(_T("[mp4_file_demuxer::mp4_file_parse] file size is 0x%I64x\n"), file_size);

	if( file_size >= UINT32_MAX )
	{
		b_error = 1;
		error_code = GEN_E_INPUT_OVERFLOW;
		goto FIN;
	}

	b_continue = 1;
	current_offset = 0;
	p_parent = p_demuxer->file.p_tree->p_root;
	total_demux_size = 0;

	buf_left_size = MP4_FILE_DEMUXER_FILE_BUF_SIZE;

	while( total_demux_size < file_size )
	{
		ret = gen_file_seek64(p_demuxer->file.file_h, GEN_FILE_SEEK_BEGIN, current_offset, NULL);
		if( ret )
		{
			//log_debug(_T("[mp4_file_demuxer::mp4_file_parse] seek error = %d!\n"), ret);
			break;
		}

		ret = gen_file_read(p_demuxer->file.file_h, p_file_buf, buf_left_size, &read_size);
		if( ret < 0 )
		{
			/** read file error */
			break;
		}

		p_uint8 = p_file_buf;
		undemux_size = read_size;

		ret = mp4_demux_box(&box, p_uint8, undemux_size, &demux_size);
		if( ret )
		{
			/** need more data */
			//log_debug(_T("[mp4_file_demuxer::mp4_file_parse] demux offset 0x%I64x box error = %d!\n"), current_offset, ret);
			break;
		}

		if( box.payload.box.size == 0 )
		{
			/** not support */
			b_error = 1;
			error_code = GEN_E_NOT_SUPPORT;
			break;
		}

		if( box.format == MP4_BOX_FORMAT_ISO_BOX )
		{
			box_size = box.payload.box.size;

		}else
		{
			box_size = box.payload.box64.large_size;
		}

		if( box_size < demux_size )
		{
			/** invalid box size */
			b_error = 1;
			error_code = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[mp4_file_demuxer::mp4_file_parse] demux offset 0x%I64x box size error = %d!\n"), current_offset, ret);
			break;
		}

		file_box.offset = current_offset;
		file_box.format = box.format;
		file_box.payload.full_box = box.payload.full_box;

		ret = mp4_file_tree_insert_box(p_demuxer->file.p_tree, &file_box, p_parent, &p_node);

		switch( box.payload.box.type )
		{
		case MP4_FILE_TYPE_BOX_TYPE:
		case MP4_MEDIA_DATA_BOX_TYPE:
		case MP4_FREE_BOX_TYPE:
		case MP4_META_BOX_TYPE:
			{

			}
			break;

		case MP4_MOVIE_BOX_TYPE:
			{
				/** moov */
				ret = mp4_file_demux_movie_box(p_demuxer, p_node);
			}
			break;

		case MP4_MOVIE_FRAGMENT_BOX_TYPE:
			{
				/** moof */
				//ret = mp4_file_demux_movie_frag_box(p_demuxer, p_node);
			}
			break;

		default:
			{
				//log_debug(_T("[mp4_file_demuxer::mp4_file_parse] demux box offset 0x%I64x unknown type = 0x%08x!\n"), current_offset, box.payload.box.type);
			}
			break;
		}

		current_offset += box_size;
		total_demux_size += box_size;

	}//end while( total_demux_size < file_size )

#if 0

	log_debug(_T("[mp4_file_demuxer::mp4_file_parse] file timesacle = %u, duration = %u ms\n"), p_demuxer->file.stat.mvhd_box.time_scale, p_demuxer->file.stat.duration_ms);
	log_debug(_T("[mp4_file_demuxer::mp4_file_parse] tree size is %d\n"), p_demuxer->file.p_tree->tree_size);

	i = 0;
	p_node = p_demuxer->file.p_tree->p_head->p_next;
	while( p_node != p_demuxer->file.p_tree->p_rear )
	{
		p_uint8 = (uint8*)&p_node->box.payload.box.type;
		log_debug(_T("[mp4_file_demuxer::mp4_file_parse] box %d type = %c%c%c%c, offset = 0x%I64x, size = 0x%x\n"), i, p_uint8[3], p_uint8[2], p_uint8[1], p_uint8[0], p_node->box.offset, p_node->box.payload.box.size);

		i++;
		p_node = p_node->p_next;
	}

#endif

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

	return GEN_S_OK;
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
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		mp4_file_demuxer_deinit(p_demuxer);
	}

	free(p_demuxer);

	return GEN_S_OK;
}

int32 mp4_file_demuxer_init(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	//int32  ret;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( p_demuxer->b_init )
	{
		return GEN_S_OK;
	}

	memset(&p_demuxer->file, 0, sizeof(p_demuxer->file));

	p_demuxer->b_demux = 0;

	p_demuxer->b_init = 1;

	return GEN_S_OK;
}

int32 mp4_file_demuxer_deinit(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	p_demuxer->b_init = 0;

	return GEN_S_OK;
}

int32 mp4_file_demuxer_reset(MP4_FILE_DEMUXER_H demuxer_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	if( p_demuxer == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	p_demuxer->b_demux = 0;

	return GEN_S_OK;
}

int32 mp4_file_demuxer_demux(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_TREE* p_tree, GEN_FILE_H file_h)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	int32  b_error;
	int32  error_code;

	int32  ret;

	if( p_demuxer == NULL || p_tree == NULL || file_h == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	b_error = 0;
	p_demuxer->file.file_h = file_h;
	p_demuxer->file.p_tree = p_tree;
	p_demuxer->file.stat.track_count = 0;

	ret = mp4_file_parse(p_demuxer);
	if( ret == 0 )
	{
		p_demuxer->b_demux = 1;

	}else
	{
		b_error = 1;
		error_code = ret;
		//goto FIN;
	}

	if( b_error )
	{
		return error_code;
	}

	return GEN_S_OK;
}

int32 mp4_file_demuxer_get_file_stat(MP4_FILE_DEMUXER_H demuxer_h, MP4_FILE_STAT* p_stat)
{
	MP4_FILE_DEMUXER *p_demuxer = (MP4_FILE_DEMUXER*)demuxer_h;

	//int32  ret;

	if( p_demuxer == NULL || p_stat == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	if( !p_demuxer->b_init )
	{
		return GEN_E_NOT_INIT;
	}

	if( !p_demuxer->b_demux )
	{
		return GEN_E_INVALID_STATUS;
	}

	memcpy(p_stat, &p_demuxer->file.stat, sizeof(p_demuxer->file.stat));

	return GEN_S_OK;
}
