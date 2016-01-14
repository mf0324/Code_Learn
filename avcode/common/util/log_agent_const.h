/**
 * @file log_agent_const.h  Log Agent Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.04.03
 *
 *
 */

#ifndef __LOG_AGENT_CONST_H__
#define __LOG_AGENT_CONST_H__

/**
 * @name log type
 *
 * @{
 */

/** stdout */
#define LA_LOG_TYPE_STDOUT                                 (0x1)
/** file */
#define LA_LOG_TYPE_FILE                                   (0x2)
/** sock */
#define LA_LOG_TYPE_SOCK                                   (0x4)

/** all */
#define LA_LOG_TYPE_ALL                                    (LA_LOG_TYPE_STDOUT | LA_LOG_TYPE_FILE | LA_LOG_TYPE_SOCK)


/**
 * @}
 */

/**
 * @name log level
 *
 * @{
 */

/** fatal */
#define LA_LOG_LEVEL_FATAL                                 (0x1)
/** error */
#define LA_LOG_LEVEL_ERROR                                 (0x2)
/** warn */
#define LA_LOG_LEVEL_WARN                                  (0x4)
/** normal */
#define LA_LOG_LEVEL_NORMAL                                (0x8)
/** info */
#define LA_LOG_LEVEL_INFO                                  (0x10)
/** debug */
#define LA_LOG_LEVEL_DEBUG                                 (0x20)
/** access */
#define LA_LOG_LEVEL_ACCESS                                (0x40)

/**
 * @}
 */

/**
 * @name split file rule
 *
 * @{
 */

/** by time */
#define LA_SPLIT_FILE_BY_TIME                              (0x1)
/** by size */
#define LA_SPLIT_FILE_BY_SIZE                              (0x2)

/**
 * @}
 */

#endif ///__LOG_AGENT_CONST_H__
