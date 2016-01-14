/**
 * @file sp_audio_render_public.h  SP Audio Render Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.04.27
 *
 *
 */

#ifndef __SP_AUDIO_RENDER_PUBLIC_H__
#define __SP_AUDIO_RENDER_PUBLIC_H__

#include <public/gen_int.h>


/** id size */
#define SP_AUDIO_RENDER_DEV_ID_SIZE                        (16)
/** max string size */
#define SP_AUDIO_RENDER_MAX_STRING_SIZE                    (256)


/** dsound dev */
typedef struct tag_dsound_dev
{
	/** id(guid) */
	uint8  id[SP_AUDIO_RENDER_DEV_ID_SIZE];
	/** desc */
	int8   desc[SP_AUDIO_RENDER_MAX_STRING_SIZE];
	/** name */
	int8   name[SP_AUDIO_RENDER_MAX_STRING_SIZE];

} DSOUND_DEV, *PDSOUND_DEV;

/** dsound render capability */
typedef struct tag_dsound_render_cap
{
	/**	capability */
	uint32 cap;
	/**	capability2 */
	uint32 cap2;

} DSOUND_RENDER_CAP, *PDSOUND_RENDER_CAP;

/** audio render capability */
typedef struct tag_audio_render_cap
{
	/**	render type */
	int32  render_type;

	/** render */
	union
	{
		/** dsound */
		DSOUND_RENDER_CAP dsound;

	} render;

} AUDIO_RENDER_CAP, *PAUDIO_RENDER_CAP;

/** audio render dev */
typedef struct tag_audio_render_dev
{
	/**	render type */
	int32  render_type;

	/** render */
	union
	{
		/** dsound */
		DSOUND_DEV dsound;

	} render;

} AUDIO_RENDER_DEV, *PAUDIO_RENDER_DEV;


#endif /// __SP_AUDIO_RENDER_PUBLIC_H__
