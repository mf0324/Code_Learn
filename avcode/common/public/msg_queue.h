/**
 * @file msg_queue.h   Msg Queue
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.09.16
 *
 *
 */

#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <public/gen_int.h>

/** msg */
typedef struct tag_msq_msg
{
	/** id */
	uint32 id;
	/** param1 */
	void*  param1;
	/** param2 */
	void*  param2;

} MSQ_MSG, *PMSQ_MSG;

/** msg queue handle */
typedef void*    MSQ_H;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief create
 * @return
 * not NULL: success
 * NULL: fail
 */
MSQ_H msq_create();

/**  
 * @brief destroy
 * @param [in] msq_h, msg queue
 * @return
 * 0: success
 * other: fail
 */
int32 msq_destroy(MSQ_H msq_h);

/**  
 * @brief init
 * @param [in] msq_h, msg queue
 * @return
 * 0: success
 * other: fail
 */
int32 msq_init(MSQ_H msq_h);

/**  
 * @brief deinit
 * @param [in] msq_h, msg queue
 * @return
 * 0: success
 * other: fail
 */
int32 msq_deinit(MSQ_H msq_h);

/**  
 * @brief reset
 * @param [in] msq_h, msg queue
 * @return
 * 0: success
 * other: fail
 */
int32 msq_reset(MSQ_H msq_h);

/**  
 * @brief set max size
 * @param [in] msq_h, msg queue
 * @param [in] max_size, max queue size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. after init, default size is 1024
 */
int32 msq_set_max_size(MSQ_H msq_h, int32 max_size);

/**  
 * @brief send msg
 * @param [in] msq_h, msg queue
 * @param [in] p_msg, msg
 * @param [in] b_wait, wait or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if msg queue is full and b_wait not zero, caller would be wait until send successfully
 * 2. this interface can be called by multithread
 */
int32 msq_send_msg(MSQ_H msq_h, MSQ_MSG *p_msg, int32 b_wait);

/**  
 * @brief get msg
 * @param [in] msq_h, msg queue
 * @param [in] p_msg, msg
 * @param [in] b_wait, wait or not
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. if msg queue is empty and b_wait not zero, caller would be wait until get msg
 * 2. this interface is multithread safe
 */
int32 msq_get_msg(MSQ_H msq_h, MSQ_MSG *p_msg, int32 b_wait);


#ifdef __cplusplus
}
#endif

#endif ///__MSG_QUEUE_H__
