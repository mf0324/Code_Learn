/**
 * @file rtp.h   RTP库
 *
 * Copyright (C) 2007-2009 北清视通信息技术有限公司北京研发部
 * 
 *
 * @author 翁彦 <wengyan@bqvision.com>
 * @version 0.1
 * @date 2010.02.25
 *
 *
 */


#ifndef __RTP_H__
#define __RTP_H__

#include "../public/gen_int.h"

#include "rtp_public.h"

/** RTP头 */
typedef struct tag_rtp_header
{
	/** 标志 */
	uint8      flag;
	/** 掩码位和载荷类型 */
	uint8      mark_pt;
	/** 序列号 */
	uint16     seq;
	/** 时间戳 */
	uint32     timestamp;
	/** SSRC */
	uint32     ssrc;

} RTP_HEADER, *PRTP_HEADER;

/** RTP扩展头 */
typedef struct tag_rtp_ext_header
{
	/** profile */
	uint16     profile;
	/** 扩展数据长度(不包括4字节扩展头)，单位：4字节 */
	uint16     ext_data_len;

} RTP_EXT_HEADER, *PRTP_EXT_HEADER;

/** RTP头参数 */
typedef struct tag_rtp_header_param
{
	/** 版本 */
	uint8      version;
	/** 填充标志 */
	uint8      b_pad;
	/** 扩展标志 */
	uint8      b_ext;
	/** 掩码标志 */
	uint8      b_mark;
	/** 载荷类型 */
	uint8      payload_type;
	/** CSRC个数 */
	uint8      csrc_num;
	/** 序列号 */
	uint16     seq;
	/** 时间戳 */
	uint32     timestamp;
	/** SSRC */
	uint32     ssrc;
	/** CSRC数据 */
	uint8      *p_csrc_data;

} RTP_HEADER_PARAM, *PRTP_HEADER_PARAM;

/** RTP扩展头参数 */
typedef struct tag_rtp_ext_header_param
{
	/** Profile */
	uint16     profile;
	/** 扩展数据长度，单位：4字节 */
	uint16     ext_data_len;
	/** 扩展数据 */
	uint8      *p_ext_data;

} RTP_EXT_HEADER_PARAM, *PRTP_EXT_HEADER_PARAM;


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * @brief RTP打包
 * @param [in] p_param RTP头参数
 * @param [in] p_ext_param RTP扩展头参数
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_buf_len 缓冲区总长度/实际打包长度
 * @return
 * 0：成功
 * -1：失败
 * -2：缓冲区长度不足
 * -3：参数无效
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_len返回
 */
int32 rtp_mux(RTP_HEADER_PARAM *p_param, RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len);

/** 
 * @brief RTP解析
 * @param [out] p_param RTP头参数
 * @param [out] p_ext_param RTP扩展头参数
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_buf_len 缓冲区总长度/实际解析长度
 * @return
 * 0：成功
 * -1：失败
 * -2：输入数据不足以解析
 * -3：参数无效
 */
int32 rtp_demux(RTP_HEADER_PARAM *p_param, RTP_EXT_HEADER_PARAM *p_ext_param, int8 *p_buf, int32 *p_buf_len);


#ifdef __cplusplus
}
#endif

#endif ///__RTP_H__
