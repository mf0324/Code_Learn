/**
 * @file mpeg4_video_def.h   MPEG4 Video Define
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.05.11
 *
 *
 */


#ifndef __MPEG4_VIDEO_DEF_H__
#define __MPEG4_VIDEO_DEF_H__

#include <public/gen_int.h>


/** Visual Object Sequence */
typedef struct tag_mpeg4_vos
{
	/** profile & level */
	uint32 profile;
	
} MPEG4_VOS, *PMPGE4_VOS;

/** Visual Object */
typedef struct tag_mpeg4_visual_object
{
	/** is visual object */
	uint8  is_visual_obj;
	/** verid */
	uint8  verid;
	/** priority */
	uint8  priority;
	/** type */
	uint8  type;

	/** video signal type */
	uint8  signal_type;
	/** video format */
	uint8  video_format;
	/** video range */
	uint8  video_range;
	/** color description */
	uint8  color_desc;

} MPEG4_VISUAL_OBJECT, *PMPGE4_VISUAL_OBJECT;

/** Video Object */
typedef struct tag_mpeg4_video_object
{
	/** start code */
	uint32 start_code;

} MPEG4_VIDEO_OBJECT, *PMPGE4_VIDEO_OBJECT;

/** Video Object Layer */
typedef struct tag_mpeg4_vol
{
	/** video object type indication */
	uint8  video_obj_type_indication;
	/** is object layer */
	uint8  is_obj_layer;
	/** verid */
	uint8  verid;
	/** priority */
	uint8  priority;

	/** aspect ratio */
	uint8  aspect_ratio;
	/** vol control param */
	uint8  vol_ctrl_param;
	/** chroma format */
	uint8  chroma_format;
	/** vbv param */
	uint8  vbv_param;

	/** shape */
	uint8  shape;
	/** shape ext */
	uint8  shape_ext;
	/** vop time increment resolution */
	uint16 vop_time_inc_res;

	/** width */
	uint16 width;
	/** height */
	uint16 height;

	/** interlace */
	int32  b_interlace;

} MPEG4_VOL, *PMPGE4_VOL;

/** vop */
typedef struct tag_mpeg4_vop
{
	/** frame type */
	int32 frame_type;

} MPEG4_VOP, *PMPGE4_VOP;

/** ves */
typedef struct tag_mpeg4_ves_state
{
	/** start addr */
	uint8  *p_start;
	/** start code */
	int32  start_code;

	/** vos */
	MPEG4_VOS  vos;
	/** visual object */
	MPEG4_VISUAL_OBJECT  vo;
	/** vol */
	MPEG4_VOL vol;
	/** vop */
	MPEG4_VOP vop;

} MPEG4_VES_STATE, *PMPEG4_VES_STATE;


#endif ///__MPEG4_VIDEO_DEF_H__
