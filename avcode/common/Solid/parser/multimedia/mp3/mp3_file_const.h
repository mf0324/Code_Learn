/**
 * @file mp3_file_const.h   MP3 File Constant
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.09.11
 *
 *
 */

#ifndef __MP3_FILE_CONST_H__
#define __MP3_FILE_CONST_H__

/** 
 * @name mp3 tag
 * @{
 */

/** tag size */
#define MP3_TAG_HEADER_SIZE                                (10)
/** version size */
#define MP3_TAG_VERSION_SIZE                               (2)
/** flag size */
#define MP3_TAG_FLAG_SIZE                                  (1)
/** unsync */
#define MP3_TAG_FLAG_UNSYNC_BIT                            (7)
/** ext header */
#define MP3_TAG_FLAG_EXT_HEADER_BIT                        (6)
/** experiment */
#define MP3_TAG_FLAG_EXPERIMENT                            (5)
/** tag size */
#define MP3_TAG_SIZE                                       (4)
/** max tag size */
#define MP3_TAG_MAX_SIZE                                   (0x7F7F7F7F)
/** frame header size */
#define MP3_TAG_FRAME_HEADER_SIZE                          (10)
/** frame id size */
#define MP3_TAG_FRAME_ID_SIZE                              (4)
/** frame size */
#define MP3_TAG_FRAME_SIZE                                 (4)
/** frame flag size */
#define MP3_TAG_FRAME_FLAG_SIZE                            (2)


/** 
 * @}
 */


#endif ///__MP3_FILE_CONST_H__
