/**
 * @file rtcp.h   RTCP库
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

#ifndef __RTCP_H__
#define __RTCP_H__

#include "../public/gen_int.h"

#include "rtp_public.h"

/** RTCP头 */
typedef struct tag_rtcp_header
{
	/** 标志 */
	uint8      flag;
	/** PAYLOAD TYPE */
	uint8      pt;
	/** RTCP包长度减1，单位：4字节 */
	uint16     len;

} RTCP_HEADER, *PRTCP_HEADER;

/** RTCP Report Block */
typedef struct tag_rtcp_rb
{
	/** SSRC */
	uint32     ssrc;
	/** fraction lost */
	uint8      frac_lost;
	/** packet lost */
	uint8      pack_lost[3];
	/** extended highest sequence number received */
	uint32     ext_sn;
	/** 包接收间隔抖动 */
	uint32     jitter;
	/** last SR */
	uint32     last_sr;
	/** delay since last SR */
	uint32     delay_sr;

} RTCP_RB, *PRTCP_RB;

#define RTCP_RB_SIZE                               (sizeof(RTCP_RB))

/** RTCP SR */
typedef struct tag_rtcp_sr
{
	/** SSRC */
	uint32     ssrc;

	/** NTP timestamp, high 32 bit */
	uint32     ntp_ts_high;
	/** NTP timestamp, low 32 bit */
	uint32     ntp_ts_low;
	/** RTP timestamp */
	uint32     rtp_ts;
	/** 已发送包个数 */
	uint32     pack_send;
	/** 已发送字节数 */
	uint32     bytes_send;

} RTCP_SR, *PRTCP_SR;

#define RTCP_SR_SIZE                               (sizeof(RTCP_SR))

/** RTCP RR */
typedef struct tag_rtcp_rr
{
	/** SSRC */
	uint32     ssrc;

} RTCP_RR, *PRTCP_RR;

#define RTCP_RR_SIZE                               (sizeof(RTCP_RR))

/** RTCP SDES item */
typedef struct tag_rtcp_item
{
	/** item */
	uint8      type;
	/** len */
	uint8      len;
	/** 保留 */
	uint16     resv;
	/** content */
	int8       *p_text;

} RTCP_ITEM, *PRTCP_ITEM;

/** RTCP SDES chunk */
typedef struct tag_rtcp_chunk
{
	/** SSRC */
	uint32     ssrc;

} RTCP_CHUNK, *PRTCP_CHUNK;

#define RTCP_CHUNK_SIZE                            (sizeof(RTCP_CHUNK))

/** RTCP SDES */
typedef struct tag_rtcp_sdes
{
	/** 描述块 */
	RTCP_CHUNK *p_chunk;

} RTCP_SDES, *PRTCP_SDES;

#define RTCP_SDES_SIZE                             (sizeof(RTCP_SDES))

/** RTCP BYE */
typedef struct tag_rtcp_bye
{
	/** 标志 */
	uint8      flag;
	/** PAYLOAD TYPE */
	uint8      pt;
	/** RTCP包长度减1，单位：4字节 */
	uint16     len;

	/** SSRC列表 */
	uint32     *p_ssrc;

} RTCP_BYE, *PRTCP_BYE;

#define RTCP_BYE_SIZE                              (sizeof(RTCP_BYE))

/** RTCP SR参数 */
typedef struct tag_rtcp_sr_param
{
	/** SSRC */
	uint32     ssrc;

	/** NTP时间戳, 高32位 */
	uint32     ntp_ts_high;
	/** NTP时间戳, 低32位 */
	uint32     ntp_ts_low;
	/** RTP时间戳 */
	uint32     rtp_ts;
	/** 已发送包个数 */
	uint32     pack_send;
	/** 已发送字节数 */
	uint32     bytes_send;

	/** report block */
	RTCP_RB    report_block[RTCP_MAX_SSRC_NUM];

} RTCP_SR_PARAM, *PRTCP_SR_PARAM;

/** RTCP RR参数 */
typedef struct tag_rtcp_rr_param
{
	/** SSRC */
	uint32     ssrc;

	/** report block */
	RTCP_RB    report_block[RTCP_MAX_SSRC_NUM];

} RTCP_RR_PARAM, *PRTCP_RR_PARAM;

/** RTCP SDES chunk */
typedef struct tag_rtcp_chunk_param
{
	/** SSRC */
	uint32     ssrc;
	/** item个数 */
	int32      item_cnt;
	/** item列表 */
	RTCP_ITEM  item[RTCP_MAX_ITEM];

} RTCP_CHUNK_PARAM, *PRTCP_CHUNK_PARAM;

/** RTCP SDES参数 */
typedef struct tag_rtcp_sdes_param
{
	/** chunk列表 */
	RTCP_CHUNK_PARAM chunk[RTCP_MAX_SSRC_NUM];

} RTCP_SDES_PARAM, *PRTCP_SDES_PARAM;

/** RTCP BYE参数 */
typedef struct tag_rtcp_bye_param
{
	/** SSRC列表 */
	uint32     ssrc[RTCP_MAX_SSRC_NUM];

} RTCP_BYE_PARAM, *PRTCP_BYE_PARAM;

/** RTCP包参数 */
typedef struct tag_rtcp_pack
{
	/** 版本 */
	uint8      version;
	/** 填充标志 */
	uint8      b_pad;
	/** SSRC/CSRC个数 */
	uint8      ssrc_cnt;
	/** 载荷类型 */
	uint8      pt;
	/** 包长度，单位：字节 */
	uint16     len;
	/** 保留 */
	uint16     resv;

	union 
	{
		RTCP_SR_PARAM    sr;
		RTCP_RR_PARAM    rr;
		RTCP_SDES_PARAM  sdes;
		RTCP_BYE_PARAM   bye;

	} body;

} RTCP_PACK, *PRTCP_PACK;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 构造RTCP消息
 *
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_len 缓冲区总长度/实际生成长度
 * @param [out] p_pack RTCP参数句柄
 * @return
 * 0：成功
 * -1：失败
 * -2：缓冲区长度不足，实际所需长度由p_len返回
 * -3：RTCP参数无效
 */
int32 rtcp_pack(int8 *p_buf, int32 *p_len, RTCP_PACK *p_pack);

/**
 * @brief 解析RTCP消息
 *
 * @param [in] p_buf 缓冲区地址
 * @param [in/out] p_len 缓冲区总长度/实际解析长度
 * @param [out] p_pack RTCP参数句柄
 * @return
 * 0：成功
 * -1：失败
 * -2：缓冲区长度不足，实际所需长度由p_len返回
 */
int32 rtcp_unpack(int8 *p_buf, int32 *p_len, RTCP_PACK *p_pack);

#ifdef __cplusplus
}
#endif

#endif// __RTCP_H__
