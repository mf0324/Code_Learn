/**
 * @file frame_parser_interface.h  Frame Parser Interface
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.02.27
 *
 *
 */

#ifndef __FRAME_PARSER_INTERFACE_H__
#define __FRAME_PARSER_INTERFACE_H__

#include <Solid/public/sp_media_frame.h>

/**  
 * @brief destroy
 * @param [in] h, frame parser handle
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*gen_frame_parser_destroy)(void* h);

/**  
 * @brief init
 * @param [in] h, frame parser handle
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*gen_frame_parser_init)(void* h);

/**  
 * @brief deinit
 * @param [in] h, frame parser handle
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*gen_frame_parser_deinit)(void* h);

/**  
 * @brief reset
 * @param [in] h, frame parser handle
 * @return
 * 0: success
 * -1: fail
 */
typedef int32 (*gen_frame_parser_reset)(void* h);

/**
 * @brief parse frame
 * @param [in] h, frame parser handle
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_frame, output frame
 * @param [out] p_parse_size, parse size
 * @return
 * 0: success
 * -1: fail
 * -2: need more data
 * @remark
 * 1.whether success or not, p_parse_size always return actual parse size
 * 2.when return success, caller must call this func again with p_data set to null, until it return -2.
 */
typedef int32 (*gen_frame_parser_parse_frame)(void* h, uint8* p_data, int32 data_size, SP_MEDIA_FRAME* p_frame, int32* p_parse_size);


/** frame parser interface */
typedef struct tag_gen_frame_parser_interface
{
	/**	destroy */
	gen_frame_parser_destroy destroy;
	/**	init */
	gen_frame_parser_init init;
	/** deinit */
	gen_frame_parser_deinit deinit;
	/** reset */
	gen_frame_parser_reset reset;
	/** parse frame */
	gen_frame_parser_parse_frame parse_frame;

} GEN_FRAME_PARSER_INTERFACE, *PGEN_FRAME_PARSER_INTERFACE;



#endif /// __FRAME_PARSER_INTERFACE_H__
