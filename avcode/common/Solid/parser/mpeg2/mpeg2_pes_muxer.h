/**
 * @file mpeg2_pes_muxer.h   MPEG2 PES Muxer
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.08.25
 *
 *
 */


#ifndef __MPEG2_PES_MUXER_H__
#define __MPEG2_PES_MUXER_H__

#include <public/gen_int.h>

/** PES参数 */
typedef struct tag_mpeg2_pes_header_param
{
	/** stream id */
	uint8  stream_id;
	/** ext flag */
	uint8  b_ext;
	/** pack len */
	uint16 pack_len;

	/** scramble control */
	uint8  scramble_control;
	/** priority */
	uint8  b_priority;
	/** data alignment indicator */
	uint8  b_data_align;
	/** copyright */
	uint8  b_copyright;

	/** original */
	uint8  b_original;
	/** pts dts flag */
	uint8  pts_dts_flag;
	/** ESCR */
	uint8  b_escr;
	/** ES rate */
	uint8  b_es_rate;

	/** DSM trick mode flag */
	uint8  b_dsm_trick_mode;
	/** additional copy info flag */
	uint8  b_copy_info;
	/** crc flag */
	uint8  b_crc;
	/** header data len */
	uint8  header_data_len;

	/** pts(low 32 bit) */
	uint32 pts_low;
	/** pts(high 32 bit) */
	uint32 pts_high;
	/** dts(low 32 bit) */
	uint32 dts_low;
	/** dts(high 32 bit) */
	uint32 dts_high;

} MPEG2_PES_HEADER_PARAM, *PMPEG2_PES_HEADER_PARAM;


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief 计算PES头部数据(可选部分)长度
 *
 * @param [in] p_param PES头部参数
 * @param [out] p_header_data_size 头部数据长度，单位：字节
 * @return
 * 0：成功
 * -1：参数错误
 * @remark
 * 1、计算出来的头部数据长度仅包含可选部分的长度，切记！
 */
int32 mpeg2_calc_pes_header_data_size(MPEG2_PES_HEADER_PARAM *p_param, int32 *p_header_data_size);

/**  
 * @brief 计算PES头部长度
 *
 * @param [in] p_param PES头部参数
 * @param [out] p_header_size 头部长度，单位：字节
 * @return
 * 0：成功
 * -1：参数错误
 * @remark
 * 1、p_param中的header_data_len可以通过mpeg2_calc_pes_header_data_size计算出来
 */
int32 mpeg2_calc_pes_header_size(MPEG2_PES_HEADER_PARAM *p_param, int32 *p_header_size);

/**  
 * @brief 打包PES包头
 *
 * @param [in] p_param 打包参数句柄
 * @param [in] p_buf 数据缓冲地址
 * @param [in/out] p_buf_len 缓冲区长度/实际打包长度
 * @return
 * 0：成功
 * -1：参数错误
 * -2：缓冲区长度不足
 * @remark
 * 1、如果缓冲区长度不足，实际所需长度由p_buf_len返回
 */
int32 mpeg2_pes_mux(MPEG2_PES_HEADER_PARAM *p_param, uint8 *p_buf, int32 *p_buf_len);

/**  
 * @brief 生成PES填充包
 *
 * @param [in] p_buf 数据地址
 * @param [in] buf_len 填充长度
 * @return
 * 0：成功
 * -1：参数错误
 * @remark
 * 1、填充长度最小不少于MPEG2_PES_HEADER_MIN_SIZE，最大不能超过MPEG2_PES_HEADER_MIN_SIZE + MPEG2_PES_MAX_PACK_SIZE
 */
int32 mpeg2_pes_padding_mux(uint8 *p_buf, int32 buf_len);

#ifdef __cplusplus
}
#endif

#endif ///__MPEG2_PES_MUXER_H__
