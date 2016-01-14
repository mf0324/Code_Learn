/**
 * @file mpeg2_dvb_param.h   MPEG2 DVB Param
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.04.06
 *
 *
 */


#ifndef __MPEG2_DVB_PARAM_H__
#define __MPEG2_DVB_PARAM_H__

#include <public/gen_int.h>


/** SIT Section参数 */
typedef struct tag_mpeg2_dvb_sit_section_param
{
	/** service id */
	uint32 service_id;
	/** running status */
	int32  running_status;
	/** service loop length */
	int32  service_loop_len;
	/** descriptor */
	uint8* p_descriptor;

} MPEG2_DVB_SIT_SECTION_PARAM, *PMPEG2_DVB_SIT_SECTION_PARAM;

/** SIT参数 */
typedef struct tag_mpeg2_dvb_sit_param
{
	/** version_number */
	uint8  version_number;
	/** current_next_indicator */
	uint8  b_current_next;
	/** section number */
	uint8  section_number;
	/** last section number */
	uint8  last_section_number;

	/** transmission info loop length */
	int32  trans_info_loop_len;
	/** descriptor */
	uint8* p_descriptor;

	/** sit section */
	MPEG2_DVB_SIT_SECTION_PARAM* p_section;
	/** section num */
	int32  section_num;

} MPEG2_DVB_SIT_PARAM, *PMPEG2_DVB_SIT_PARAM;


#endif ///__MPEG2_DVB_PARAM_H__
