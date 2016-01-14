/**
 * @file mpeg2_pes_public.h  MPEG2 PES Public
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2008.12.19
 *
 *
 */

#ifndef __MPEG2_PES_PUBLIC_H__
#define __MPEG2_PES_PUBLIC_H__

#include <public/gen_int.h>

#include "mpeg2_public.h"

/** descriptor */
typedef struct tag_mpeg2_descriptor
{
	/** tag */
	int32  tag;

	/** data */
	uint8* p_data;
	/** data size */
	int32  data_size;

} MPEG2_DESCRIPTOR, *PMPEG2_DESCRIPTOR;

/** es stream */
typedef struct tag_mpeg2_es_stream
{
	/** stream type */
	int32  stream_type;
	/** es id */
	int32  es_id;

	/** descriptor */
	uint8* p_descriptor;
	/** descriptor size */
	int32  descriptor_size;

} MPEG2_ES_STREAM, *PMPEG2_ES_STREAM;

/** ps map */
typedef struct tag_mpeg2_ps_map
{
	/** descriptor */
	uint8  *p_descriptor;
	/** descriptor size */
	int32  descriptor_len;

	/** es map */
	uint8  *p_es_map;
	/** es map size */
	int32  es_map_len;

} MPEG2_PS_MAP, *PMPEG2_PS_MAP;

/** ps directory */
typedef struct tag_mpeg2_ps_dir
{
	/** prev directory offset */
	int64  prev_dir_offset;
	/** next directory offset */
	int64  next_dir_offset;

	/** au count */
	int32  au_num;
	/** au */
	uint8  *p_au;
	/** au size */
	int32  au_len;

} MPEG2_PS_DIR, *PMPEG2_PS_DIR;

/** pes header */
typedef struct tag_mpeg2_pes_header
{
	/** start pos */
	uint8  *p_start;
	/** start code */
	uint32 start_code;

	/** pes pack len */
	int32  pes_pack_len;
	/** header data len */
	int32  header_data_len;

	/** pes pack data */
	uint8* p_pack_data;
	/** pes pack data len */
	int32  pack_data_len;

	/** pts dts flag */
	uint32 pts_dts_flag;
	/** dts */
	int64  dts;
	/** pts */
	int64  pts;

} MPEG2_PES_HEADER, *PMPEG2_PES_HEADER;

/** pes pack */
typedef struct tag_mpeg2_pes_pack
{
	/** start pos */
	uint8  *p_start;
	/** start code */
	uint32 start_code;

	/** pes pack len */
	int32  pes_pack_len;
	/** payload data */
	uint8  *p_data;

} MPEG2_PES_PACK, *PMPEG2_PES_PACK;



#endif ///__MPEG2_PES_PUBLIC_H__
