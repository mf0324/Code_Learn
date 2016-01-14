/**
 * @file gen_file_const.h   Gen File Constant
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.08
 *
 *
 */


#ifndef __GEN_FILE_CONST_H__
#define __GEN_FILE_CONST_H__

/** 
 * @name FILE
 * @{
 */

/** open */
#define GEN_FILE_PARAM_OPEN                                (1)

/** open read */
#define GEN_FILE_OPEN_READ                                 (0x1)
/** open write */
#define GEN_FILE_OPEN_WRITE                                (0x2)
/** open read write */
#define GEN_FILE_OPEN_READ_WRITE                           (GEN_FILE_OPEN_READ | GEN_FILE_OPEN_WRITE)

/** share read */
#define GEN_FILE_SHARE_READ                                (0x1)
/** share write */
#define GEN_FILE_SHARE_WRITE                               (0x2)
/** share del */
#define GEN_FILE_SHARE_DEL                                 (0x4)

/** create always */
#define GEN_FILE_CREATE_ALWAYS                             (0x1)
/** create new */
#define GEN_FILE_CREATE_NEW                                (0x2)
/** open always */
#define GEN_FILE_CREATE_OPEN                               (0x4)
/** open existing */
#define GEN_FILE_CREATE_EXIST                              (0x8)
/** truncate existing */
#define GEN_FILE_CREATE_TRUNC                              (0x10)

/** backup */
#define GEN_FILE_FLAG_BACKUP                               (0x1)
/** del on close */
#define GEN_FILE_FLAG_DEL_ON_CLOSE                         (0x2)
/** no buf */
#define GEN_FILE_FLAG_NO_BUF                               (0x4)
/** open no recall */
#define GEN_FILE_FLAG_OPEN_NO_RECALL                       (0x8)
/** overlapped */
#define GEN_FILE_FLAG_OVERLAP                              (0x10)
/** random access */
#define GEN_FILE_FLAG_RANDOM_ACCESS                        (0x20)
/** seq scan */
#define GEN_FILE_FLAG_SEQ_SCAN                             (0x40)
/** write through */
#define GEN_FILE_FLAG_WRITE_THROUGH                        (0x80)


/** begin */
#define GEN_FILE_SEEK_BEGIN                                (1)
/** end */
#define GEN_FILE_SEEK_END                                  (2)
/** current */
#define GEN_FILE_SEEK_CURRENT                              (3)


/** 
 * @}
 */


#endif ///__GEN_FILE_CONST_H__
