/**
 * @file rtmp_url.h  RTMP URL
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.21
 *
 *
 */

#ifndef __RTMP_URL_H__
#define __RTMP_URL_H__


#include <public/gen_int.h>

/** max app name size */
#define RTMP_MAX_APP_NAME_SIZE                             (256)
/** max inst name size */
#define RTMP_MAX_INST_NAME_SIZE                            (256)
/** max url size */
#define RTMP_MAX_URL_SIZE                                  (1 * 1024)


/** url */
typedef struct tag_rtmp_url
{
	/** app name */
	int8   app_name[RTMP_MAX_APP_NAME_SIZE];
	/** app name size */
	int32  app_name_size;

	/** instance name */
	int8   instance_name[RTMP_MAX_INST_NAME_SIZE];
	/** instance name size */
	int32  instance_name_size;

	/** swf url */
	//int8   swf_url[RTMP_MAX_URL_SIZE];
	/** swf url size */
	//int32  swf_url_size;

	/** tc url */
	int8    tc_url[RTMP_MAX_URL_SIZE];
	/** tc url size */
	int32   tc_url_size;

	/** page url */
	//int8    page_url[RTMP_MAX_URL_SIZE];
	/** page url size */
	//int32   page_url_size;

	/** play path */
	int8   play_path[RTMP_MAX_URL_SIZE];
	/** play path size */
	int32  play_path_size;

} RTMP_URL, *PRTMP_URL;


#endif /// __RTMP_URL_H__
