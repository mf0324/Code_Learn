/**
 * @file net_sock.h  Net Sock
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.06.10
 *
 *
 */


#ifndef __NET_SOCK_H__
#define __NET_SOCK_H__

#include <public/gen_int.h>
#include "network_public.h"

/** 无效 */
#define NET_SOCK_TYPE_INVALID                              (0)
/** TCP */
#define NET_SOCK_TYPE_TCP                                  (1)
/** UDP */
#define NET_SOCK_TYPE_UDP                                  (2)
/** SCTP */
#define NET_SOCK_TYPE_SCTP                                 (3)


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief 打开sock
 * @param [in/out] p_sock sock句柄
 * @param [in] type sock类型
 * @param [in] b_block 是否阻塞
 * @return
 * 0： 成功
 * -1：失败
 * @remark
 * 1、如无特殊说明，p_sock的addr字段各参数均为网络字节序，返回时p_sock的sock字段保存打开的socket
 * 2、addr字段的local_addr表示要绑定的IP，local_port表示要绑定的端口，端口为0表示不绑定
 */
int32 net_sock_open(NET_SOCK* p_sock, int32 type, int32 b_block);

/** 
 * @brief 关闭sock
 * @param [in] p_sock sock句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 net_sock_close(NET_SOCK* p_sock);

/** 
 * @brief 加入组播组
 * @param [in] p_sock sock句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 net_sock_join_group(NET_SOCK* p_sock);

/** 
 * @brief 退出组播组
 * @param [in] p_sock sock句柄
 * @return
 * 0：成功
 * -1：失败
 */
int32 net_sock_leave_group(NET_SOCK* p_sock);

/** 
 * @brief 加入源特定组播组
 * @param [in] p_sock sock句柄
 * @param [in] src_ip 组播组源IP，网络字节序
 * @return
 * 0：成功
 * -1：失败
 */
//int32 net_sock_join_src_group(NET_SOCK* p_sock, uint32 src_ip);

/** 
 * @brief 退出源特定组播组
 * @param [in] p_sock sock句柄
 * @param [in] src_ip 组播组源IP，网络字节序
 * @return
 * 0：成功
 * -1：失败
 */
//int32 net_sock_leave_src_group(NET_SOCK* p_sock, uint32 src_ip);

/** 
 * @brief 监听
 * @param [in] p_sock sock句柄
 * @param [in] max_connect 最大连接数
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、监听前必须先绑定到一个指定的端口
 */
int32 net_sock_listen(NET_SOCK* p_sock, int32 max_connect);

/** 
 * @brief 接受连接
 * @param [in] p_sock sock句柄
 * @param [in] p_accept 连接方句柄
 * @return
 * 0：成功
 * -1：失败
 * @remark
 * 1、接受连接前必须先监听
 */
int32 net_sock_accept(NET_SOCK* p_sock, NET_SOCK* p_accept);

/** 
 * @brief 连接远程地址
 * @param [in] p_sock sock句柄
 * @param [out] p_error_code 错误代码地址
 * @return
 * 0：成功
 * -1：失败
 */
int32 net_sock_connect(NET_SOCK* p_sock, int32* p_error_code);


#ifdef __cplusplus
}
#endif

#endif ///__NET_SOCK_H__
