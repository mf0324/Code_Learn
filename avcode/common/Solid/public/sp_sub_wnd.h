/**
 * @file sp_sub_wnd.h  SP Sub Wnd
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.16
 *
 *
 */

#ifndef __SP_SUB_WND_H__
#define __SP_SUB_WND_H__

#include <public/gen_int.h>
#include <Solid/public/sp_media_wnd.h>
#include <Solid/public/sp_render_area.h>

/**
 * @name option
 *
 * @{
 */

/** wnd option */
#define SP_RENDER_OPTION_WND                               (0x1)
/** area option */
#define SP_RENDER_OPTION_AREA                              (0x2)

/** all option */
#define SP_RENDER_OPTION_ALL                               (SP_RENDER_OPTION_WND | \
															SP_RENDER_OPTION_AREA)

/**
 * @}
 */


/** Sub Wnd */
typedef struct tag_sp_sub_wnd
{
	/** index */
	int32  index;
	/** option */
	uint32 option;

	/** wnd */
	SP_MEDIA_WND  wnd;

	/** area */
	SP_RENDER_AREA  area;

} SP_SUB_WND, *PSP_SUB_WND;


#endif ///__SP_SUB_WND_H__
