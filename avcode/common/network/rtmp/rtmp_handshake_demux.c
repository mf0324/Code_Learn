
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include <crypto/hmac/gen_hmac_const.h>
#include <crypto/hmac/gen_hmac.h>

#include "rtmp_const.h"
#include "rtmp_util.h"
#include "rtmp_handshake_demux.h"


/////////////////////////////// Inner Interface ///////////////////////////////
/**  
 * @brief demux digest
 * @param [out] p_digest, digest
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
static int32 rtmp_demux_digest(RTMP_C1S1_DIGEST* p_digest, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;

	int32   r0_size;
	int32   r1_size;

	int32   ret;

	left_size = data_size;
	need_size = RTMP_C1S1_DIGEST_SIZE;

	if( left_size < need_size )
	{
		*p_demux_size = 0;
		return GEN_E_NEED_MORE_DATA;
	}

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** offset */
	p_uint8_data = (uint8*)&p_digest->offset;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];
	p_uint8 += RTMP_C1S1_OFFSET_SIZE;
	total_demux_size += RTMP_C1S1_OFFSET_SIZE;
	left_size -= RTMP_C1S1_OFFSET_SIZE;

	/** r0 */
	ret = rtmp_calc_digest_random_size(p_digest, &r0_size);
	if( r0_size > 0 )
	{
		p_digest->p_random0 = p_uint8;
		p_digest->random0_size = r0_size;

		p_uint8 += r0_size;
		total_demux_size += r0_size;
		left_size -= r0_size;

	}else
	{
		p_digest->p_random0 = NULL;
		p_digest->random0_size = 0;
	}

	/** digest */
	memcpy(p_digest->digest, p_uint8, RTMP_C1_DIGEST_SIZE);
	p_uint8 += RTMP_C1_DIGEST_SIZE;
	total_demux_size += RTMP_C1_DIGEST_SIZE;
	left_size -= RTMP_C1_DIGEST_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_DIGEST_SIZE - RTMP_C1S1_OFFSET_SIZE - r0_size - RTMP_C1_DIGEST_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;

	}else if( r0_size > 0 )
	{
		p_digest->p_random1 = p_uint8;
		p_digest->random1_size = r1_size;

		p_uint8 += r1_size;
		total_demux_size += r1_size;
		left_size -= r1_size;

	}else
	{
		p_digest->p_random1 = NULL;
		p_digest->random1_size = 0;
	}

	*p_demux_size = need_size;

	return GEN_S_OK;
}

/**  
 * @brief demux key
 * @param [out] p_key, key
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
static int32 rtmp_demux_key(RTMP_C1S1_KEY* p_key, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;

	int32   r0_size;
	int32   r1_size;

	int32   ret;

	left_size = data_size;
	need_size = RTMP_C1S1_KEY_SIZE;

	if( left_size < need_size )
	{
		*p_demux_size = 0;
		return GEN_E_NEED_MORE_DATA;
	}

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** offset */
	p_uint8 = p_data + RTMP_C1S1_KEY_SIZE - RTMP_C1S1_OFFSET_SIZE;
	p_uint8_data = (uint8*)&p_key->offset;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];

	/** r0 */
	p_uint8 = p_data;
	ret = rtmp_calc_key_random_size(p_key, &r0_size);
	if( r0_size > 0 )
	{
		p_key->p_random0 = p_uint8;
		p_key->random0_size = r0_size;

		p_uint8 += r0_size;
		total_demux_size += r0_size;
		left_size -= r0_size;

	}else
	{
		p_key->p_random0 = NULL;
		p_key->random0_size = 0;
	}

	/** key */
	memcpy(p_key->key, p_uint8, RTMP_C1_KEY_SIZE);
	p_uint8 += RTMP_C1_KEY_SIZE;
	total_demux_size += RTMP_C1_KEY_SIZE;
	left_size -= RTMP_C1_KEY_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_KEY_SIZE - r0_size  - RTMP_C1_KEY_SIZE - RTMP_C1S1_OFFSET_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;

	}else if( r0_size > 0 )
	{
		p_key->p_random1 = p_uint8;
		p_key->random1_size = r1_size;

		p_uint8 += r1_size;
		total_demux_size += r1_size;
		left_size -= r1_size;

	}else
	{
		p_key->p_random1 = NULL;
		p_key->random1_size = 0;
	}

	*p_demux_size = need_size;

	return GEN_S_OK;
}

/**  
 * @brief demux c1(schema0)
 * @param [out] p_c1, c1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
static int32 rtmp_demux_c1_schema0(RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	//int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	GEN_HMAC_STATE  hmac_state;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	p_temp = c1_join;
	part1_size = 0;
	part2_size = 0;

	/** time + version */
	demux_size = RTMP_TIME_SIZE + RTMP_VERSION_SIZE;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;
	part1_size += demux_size;

	/** key */
	ret = rtmp_demux_key(&p_c1->key, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** digest */
	ret = rtmp_demux_digest(&p_c1->digest, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** part1 */
	part1_size += (RTMP_C1S1_KEY_SIZE + RTMP_C1S1_OFFSET_SIZE + p_c1->digest.random0_size);
	memcpy(c1_join, p_data, part1_size);

	/** part2 */
	part2_size += p_c1->digest.random1_size;
	if( (part1_size + part2_size) != RTMP_C1S1_JOIN_SIZE )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("part1 = %d, part2 = %d error!\n"), part1_size, part2_size);
		return GEN_E_FAIL;
	}
	memcpy(c1_join + part1_size, p_data + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	/** calc digest */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fp_key, 30, c1_join, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	ret = memcmp(sha_buf, p_c1->digest.digest, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("c1 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	*p_demux_size = RTMP_C1_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief demux c1(schema1)
 * @param [out] p_c1, c1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 * 2. digest - key structure
 * 3. p_data including whole c1
 */
static int32 rtmp_demux_c1_schema1(RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	//int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	GEN_HMAC_STATE  hmac_state;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	p_temp = c1_join;
	part1_size = 0;
	part2_size = 0;

	/** time + version */
	demux_size = RTMP_TIME_SIZE + RTMP_VERSION_SIZE;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;
	part1_size += demux_size;

	/** digest */
	ret = rtmp_demux_digest(&p_c1->digest, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** part1 */
	part1_size += (RTMP_C1S1_OFFSET_SIZE + p_c1->digest.random0_size);
	memcpy(c1_join, p_data, part1_size);

	/** key */
	ret = rtmp_demux_key(&p_c1->key, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** part2 */
	part2_size += (RTMP_C1S1_KEY_SIZE + p_c1->digest.random1_size);
	if( (part1_size + part2_size) != RTMP_C1S1_JOIN_SIZE )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("part1 = %d, part2 = %d error!\n"), part1_size, part2_size);
		return GEN_E_FAIL;
	}
	memcpy(c1_join + part1_size, p_data + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	/** calc digest */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fp_key, 30, c1_join, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	ret = memcmp(sha_buf, p_c1->digest.digest, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("c1 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	*p_demux_size = RTMP_C1_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief demux s1(schema0)
 * @param [out] p_s1, s1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 */
static int32 rtmp_demux_s1_schema0(RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	//int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	GEN_HMAC_STATE  hmac_state;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	p_temp = c1_join;
	part1_size = 0;
	part2_size = 0;

	/** time + version */
	demux_size = RTMP_TIME_SIZE + RTMP_VERSION_SIZE;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;
	part1_size += demux_size;

	/** key */
	ret = rtmp_demux_key(&p_s1->key, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** digest */
	ret = rtmp_demux_digest(&p_s1->digest, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** part1 */
	part1_size += (RTMP_C1S1_KEY_SIZE + RTMP_C1S1_OFFSET_SIZE + p_s1->digest.random0_size);
	memcpy(c1_join, p_data, part1_size);

	/** part2 */
	part2_size += p_s1->digest.random1_size;
	if( (part1_size + part2_size) != RTMP_C1S1_JOIN_SIZE )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("part1 = %d, part2 = %d error!\n"), part1_size, part2_size);
		return GEN_E_FAIL;
	}
	memcpy(c1_join + part1_size, p_data + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	/** calc digest */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fms_key, 36, c1_join, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	ret = memcmp(sha_buf, p_s1->digest.digest, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s1 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s1 digest valid!\n"));

	*p_demux_size = RTMP_S1_SIZE;

	return GEN_S_OK;
}

/**  
 * @brief demux s1(schema1)
 * @param [out] p_s1, s1
 * @param [in] p_data, input data
 * @param [in] data_size, input data size
 * @param [out] p_demux_size, actual demux size
 * @return
 * 0: success
 * other: fail
 * @remark
 * 1. whether success or not, p_demux_size always would be update
 * 2. digest - key structure
 * 3. p_data including whole c1
 */
static int32 rtmp_demux_s1_schema1(RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	//int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	GEN_HMAC_STATE  hmac_state;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	*p_demux_size = 0;

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	p_temp = c1_join;
	part1_size = 0;
	part2_size = 0;

	/** time + version */
	demux_size = RTMP_TIME_SIZE + RTMP_VERSION_SIZE;

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;
	part1_size += demux_size;

	/** digest */
	ret = rtmp_demux_digest(&p_s1->digest, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	p_uint8 += demux_size;
	total_demux_size += demux_size;
	left_size -= demux_size;

	/** part1 */
	part1_size += (RTMP_C1S1_OFFSET_SIZE + p_s1->digest.random0_size);
	memcpy(c1_join, p_data, part1_size);

	/** key */
	ret = rtmp_demux_key(&p_s1->key, p_uint8, left_size, &demux_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** part2 */
	part2_size += (RTMP_C1S1_KEY_SIZE + p_s1->digest.random1_size);
	if( (part1_size + part2_size) != RTMP_C1S1_JOIN_SIZE )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("part1 = %d, part2 = %d error!\n"), part1_size, part2_size);
		return GEN_E_FAIL;
	}
	memcpy(c1_join + part1_size, p_data + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	/** calc digest */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fms_key, 36, c1_join, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	ret = memcmp(sha_buf, p_s1->digest.digest, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s1 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s1 digest valid!\n"));

	*p_demux_size = RTMP_S1_SIZE;

	return GEN_S_OK;
}

////////////////////////////// Outter Interface ///////////////////////////////
int32 rtmp_demux_c1(RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	int32   ret;

	if( p_c1 == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_C1_SIZE;
	if( data_size < need_size )
	{
		//*p_demux_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** time */
	p_uint8_data = (uint8*)&p_c1->time;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];
	p_uint8 += RTMP_TIME_SIZE;
	total_demux_size += RTMP_TIME_SIZE;
	left_size -= RTMP_TIME_SIZE;

	/** version */
	p_uint8_data = (uint8*)&p_c1->version;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];
	p_uint8 += RTMP_VERSION_SIZE;
	total_demux_size += RTMP_VERSION_SIZE;
	left_size -= RTMP_VERSION_SIZE;

	/** schema1 */
	ret = rtmp_demux_c1_schema1(p_c1, p_data, data_size, &demux_size);
	if( ret )
	{
		ret = rtmp_demux_c1_schema0(p_c1, p_data, data_size, &demux_size);
		if( ret )
		{
			return GEN_E_FAIL;
		}
	}

	//p_uint8 += demux_size;
	//total_demux_size += demux_size;
	//left_size -= demux_size;

	*p_demux_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_demux_c2(RTMP_C2* p_c2, RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	GEN_HMAC_STATE  hmac_state;

	uint8   temp_key[RTMP_C1_DIGEST_SIZE];
	int32   temp_key_size;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	if( p_c2 == NULL || p_s1 == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_C2_SIZE;
	if( data_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** temp key */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	temp_key_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fp_key, 62, p_s1->digest.digest, RTMP_C1_DIGEST_SIZE, temp_key, &temp_key_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** calc digest */
	//gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, temp_key, temp_key_size, p_uint8, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** cmp */
	ret = memcmp(sha_buf, p_data + RTMP_C1S1_JOIN_SIZE, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("c2 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("c2 digest valid!\n"));

	*p_demux_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_demux_s1(RTMP_S1* p_s1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   demux_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	int32   ret;

	if( p_s1 == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_S1_SIZE;
	if( data_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** time */
	p_uint8_data = (uint8*)&p_s1->time;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];
	p_uint8 += RTMP_TIME_SIZE;
	total_demux_size += RTMP_TIME_SIZE;
	left_size -= RTMP_TIME_SIZE;

	/** version */
	p_uint8_data = (uint8*)&p_s1->version;
	p_uint8_data[0] = p_uint8[3];
	p_uint8_data[1] = p_uint8[2];
	p_uint8_data[2] = p_uint8[1];
	p_uint8_data[3] = p_uint8[0];
	p_uint8 += RTMP_VERSION_SIZE;
	total_demux_size += RTMP_VERSION_SIZE;
	left_size -= RTMP_VERSION_SIZE;

	/** schema1 */
	ret = rtmp_demux_s1_schema1(p_s1, p_data, data_size, &demux_size);
	if( ret )
	{
		ret = rtmp_demux_s1_schema0(p_s1, p_data, data_size, &demux_size);
		if( ret )
		{
			return GEN_E_FAIL;
		}
	}

	*p_demux_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_demux_s2(RTMP_S2* p_s2, RTMP_C1* p_c1, uint8* p_data, int32 data_size, int32* p_demux_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_demux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;

	GEN_HMAC_STATE  hmac_state;

	uint8   temp_key[RTMP_C1_DIGEST_SIZE];
	int32   temp_key_size;

	uint8   sha_buf[RTMP_C1_DIGEST_SIZE];
	int32   sha_buf_size;

	int32   ret;

	if( p_s2 == NULL || p_c1 == NULL || p_data == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = RTMP_C2_SIZE;
	if( data_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_uint8 = p_data;
	total_demux_size = 0;
	left_size = data_size;

	/** temp key */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	temp_key_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, rtmp_fms_key, 68, p_c1->digest.digest, RTMP_C1_DIGEST_SIZE, temp_key, &temp_key_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** calc digest */
	//gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_calc(&hmac_state, temp_key, temp_key_size, p_uint8, RTMP_C1S1_JOIN_SIZE, sha_buf, &sha_buf_size);
	if( ret )
	{
		return GEN_E_FAIL;
	}

	/** cmp */
	ret = memcmp(sha_buf, p_data + RTMP_C1S1_JOIN_SIZE, RTMP_C1_DIGEST_SIZE);
	if( ret != 0 )
	{
		//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s2 digest cmp error!\n"));
		return GEN_E_FAIL;
	}

	//log_agent_debug(g_p_log_agent, _T(__FILE__), __LINE__, _T("s2 digest valid!\n"));

	*p_demux_size = need_size;

	return GEN_S_OK;
}
