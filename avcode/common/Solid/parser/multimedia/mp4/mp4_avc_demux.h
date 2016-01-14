/**
 * @file mp4_avc_demux.h   MP4 AVC Demux
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.11.02
 *
 *
 */


#ifndef __MP4_AVC_DEMUX_H__
#define __MP4_AVC_DEMUX_H__

#include <public/gen_int.h>
#include "mp4_avc_file.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 解析Decoder Config Record
 * @param [in/out] p_record Record句柄
 * @param [in] p_buf 输入数据地址
 * @param [in/out] p_buf_size 输入长度/实际解析长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：输入数据不足以解析
 * -3：参数取值错误
 * -4：输入的sps、pps集合不足以存放解析后的数据
 * @remark
 * 1、如果输入的sps、pps集合不足，实际所需个数由sps_count、pps_count返回
 */
int32 mp4_avc_dec_config_record_demux(AVC_DEC_CONFIG_RECORD *p_record, uint8 *p_buf, uint32 *p_buf_size);




#ifdef __cplusplus
}
#endif

#endif ///__MP4_AVC_DEMUX_H__
