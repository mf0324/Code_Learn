/**
 * @file sock_pool_const.h   Sock Pool Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.08.20
 *
 *
 */

#ifndef __SOCK_POOL_CONST_H__
#define __SOCK_POOL_CONST_H__

/** 
 * @name msg
 * @{
 */

/** msg base */
#define SOCK_POOL_MSG_BASE                                 (0)
/** add sock */
#define SOCK_POOL_MSG_ADD_SOCK                             (SOCK_POOL_MSG_BASE + 1)
/** delete sock */
#define SOCK_POOL_MSG_DEL_SOCK                             (SOCK_POOL_MSG_BASE + 2)
/** accept sock */
#define SOCK_POOL_MSG_ACCEPT_SOCK                          (SOCK_POOL_MSG_BASE + 3)
/** listen */
#define SOCK_POOL_MSG_LISTEN                               (SOCK_POOL_MSG_BASE + 10)
/** connect */
#define SOCK_POOL_MSG_CONNECT                              (SOCK_POOL_MSG_BASE + 20)
/** disconnect */
#define SOCK_POOL_MSG_DISCONNECT                           (SOCK_POOL_MSG_BASE + 30)
/** recv */
#define SOCK_POOL_MSG_RECV                                 (SOCK_POOL_MSG_BASE + 40)
/** send */
#define SOCK_POOL_MSG_SEND                                 (SOCK_POOL_MSG_BASE + 50)


/** 
 * @}
 */



#endif ///__SOCK_POOL_CONST_H__
