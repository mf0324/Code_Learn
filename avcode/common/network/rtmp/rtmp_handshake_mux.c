
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
#include "rtmp_handshake_mux.h"


/////////////////////////////// Inner Interface ///////////////////////////////
/**  
 * @brief make random data
 * @param [in] p_buf, input buf
 * @param [in] buf_size, input buf size
 * @return
 * 0: success
 * other: fail
 */
static int32 rtmp_make_random_data(uint8* p_buf, int32 buf_size)
{
	int32   i;

	if( buf_size < 0 )
	{
		return GEN_E_INVALID_PARAM;
	}

	for( i = 0; i < buf_size; i++ )
	{
		p_buf[i] = rand() % 256;
	}

	return GEN_S_OK;
}

////////////////////////////// Outter Interface ///////////////////////////////
int32 rtmp_mux_c1(RTMP_C1* p_c1, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;
	uint32  uint32_data;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	uint8   r0[RTMP_C1S1_DIGEST_SIZE];
	int32   r0_size;

	uint8   r1[RTMP_C1S1_DIGEST_SIZE];
	int32   r1_size;

	GEN_HMAC_STATE  hmac_state;
	int32   sha_buf_size;

	int32   ret;

	if( p_c1 == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = RTMP_C1_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	p_temp = c1_join;
	total_mux_size = 0;
	part1_size = 0;
	part2_size = 0;

	/** time */
	uint32_data = hton_u32(p_c1->time);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_TIME_SIZE;
	total_mux_size += RTMP_TIME_SIZE;
	left_size -= RTMP_TIME_SIZE;

	/** version */
	uint32_data = hton_u32(p_c1->version);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_VERSION_SIZE;
	total_mux_size += RTMP_VERSION_SIZE;
	left_size -= RTMP_VERSION_SIZE;

	/** digest block */
	/** offset */
	p_c1->digest.offset = rand();
	uint32_data = hton_u32((uint32)p_c1->digest.offset);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_C1S1_OFFSET_SIZE;
	total_mux_size += RTMP_C1S1_OFFSET_SIZE;
	left_size -= RTMP_C1S1_OFFSET_SIZE;

	/** r0 */
	ret = rtmp_calc_digest_random_size(&p_c1->digest, &r0_size);
	if( r0_size > 0 )
	{
		ret = rtmp_make_random_data(r0, r0_size);
		memcpy(p_uint8, r0, r0_size);
	}

	p_uint8 += r0_size;
	total_mux_size += r0_size;
	left_size -= r0_size;

	/** c1 join part1 */
	part1_size = total_mux_size;
	memcpy(c1_join, p_buf, part1_size);

	/** skip digest */
	p_uint8 += RTMP_C1_DIGEST_SIZE;
	total_mux_size += RTMP_C1_DIGEST_SIZE;
	left_size -= RTMP_C1_DIGEST_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_DIGEST_SIZE - RTMP_C1S1_OFFSET_SIZE - r0_size - RTMP_C1_DIGEST_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;
	}

	if( r1_size > 0 )
	{
		ret = rtmp_make_random_data(r1, r1_size);
		memcpy(p_uint8, r1, r1_size);
	}

	p_uint8 += r1_size;
	total_mux_size += r1_size;
	left_size -= r1_size;
	part2_size += r1_size;

	/** key block */
	p_c1->key.offset = rand();
	/** r0 */
	ret = rtmp_calc_key_random_size(&p_c1->key, &r0_size);
	if( r0_size > 0 )
	{
		ret = rtmp_make_random_data(r0, r0_size);
		memcpy(p_uint8, r0, r0_size);
	}

	p_uint8 += r0_size;
	total_mux_size += r0_size;
	left_size -= r0_size;
	part2_size += r0_size;

	/** key data */
	ret = rtmp_make_random_data(p_c1->key.key, RTMP_C1_KEY_SIZE);
	memcpy(p_uint8, p_c1->key.key, RTMP_C1_KEY_SIZE);
	p_uint8 += RTMP_C1_KEY_SIZE;
	total_mux_size += RTMP_C1_KEY_SIZE;
	left_size -= RTMP_C1_KEY_SIZE;
	part2_size += RTMP_C1_KEY_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_KEY_SIZE - r0_size  - RTMP_C1_KEY_SIZE - RTMP_C1S1_OFFSET_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;
	}

	if( r1_size > 0 )
	{
		ret = rtmp_make_random_data(r1, r1_size);
		memcpy(p_uint8, r1, r1_size);
	}

	p_uint8 += r1_size;
	total_mux_size += r1_size;
	left_size -= r1_size;
	part2_size += r1_size;

	/** offset */
	uint32_data = hton_u32((uint32)p_c1->key.offset);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_C1S1_OFFSET_SIZE;
	total_mux_size += RTMP_C1S1_OFFSET_SIZE;
	left_size -= RTMP_C1S1_OFFSET_SIZE;
	part2_size += RTMP_C1S1_OFFSET_SIZE;

	/** calc digest */
	memcpy(c1_join + part1_size, p_buf + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, rtmp_fp_key, 30);

	ret = gen_hmac_process(&hmac_state, c1_join, part1_size + part2_size);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, p_c1->digest.digest, &sha_buf_size);

	memcpy(p_buf + part1_size, p_c1->digest.digest, RTMP_C1_DIGEST_SIZE);

	if( total_mux_size != need_size )
	{
		return GEN_E_FAIL;
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_mux_c2(RTMP_C2* p_c2, RTMP_S1* p_s1, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;

	uint8   temp_key[RTMP_C1_DIGEST_SIZE];
	//int32   temp_key_size;

	GEN_HMAC_STATE  hmac_state;
	int32   sha_buf_size;

	int32   ret;

	if( p_c2 == NULL || p_s1 == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = RTMP_C2_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_mux_size = 0;

	/** make random data */
	ret = rtmp_make_random_data(p_c2->random, RTMP_C1S1_JOIN_SIZE);
	ret = rtmp_make_random_data(p_c2->digest, RTMP_C1_DIGEST_SIZE);

	/** calc temp key */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, rtmp_fp_key, 62);

	ret = gen_hmac_process(&hmac_state, p_s1->digest.digest, RTMP_C1_DIGEST_SIZE);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, temp_key, &sha_buf_size);

	/** calc digest */
	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, temp_key, RTMP_C1_DIGEST_SIZE);

	ret = gen_hmac_process(&hmac_state, p_c2->random, RTMP_C1S1_JOIN_SIZE);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, p_c2->digest, &sha_buf_size);

	/** copy */
	memcpy(p_uint8, p_c2->random, RTMP_C1S1_JOIN_SIZE);
	memcpy(p_uint8 + RTMP_C1S1_JOIN_SIZE, p_c2->digest, RTMP_C1_DIGEST_SIZE);

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_mux_s1(RTMP_S1* p_s1, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;
	uint8*  p_uint8_data = NULL;
	uint8*  p_temp = NULL;
	uint32  uint32_data;

	uint8   c1_join[RTMP_C1S1_JOIN_SIZE];
	int32   part1_size;
	int32   part2_size;

	uint8   r0[RTMP_C1S1_DIGEST_SIZE];
	int32   r0_size;

	uint8   r1[RTMP_C1S1_DIGEST_SIZE];
	int32   r1_size;

	GEN_HMAC_STATE  hmac_state;
	int32   sha_buf_size;

	int32   ret;

	if( p_s1 == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = RTMP_C1_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	p_temp = c1_join;
	total_mux_size = 0;
	part1_size = 0;
	part2_size = 0;

	/** time */
	uint32_data = hton_u32(p_s1->time);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_TIME_SIZE;
	total_mux_size += RTMP_TIME_SIZE;
	left_size -= RTMP_TIME_SIZE;

	/** version */
	uint32_data = hton_u32(p_s1->version);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_VERSION_SIZE;
	total_mux_size += RTMP_VERSION_SIZE;
	left_size -= RTMP_VERSION_SIZE;

	/** digest block */
	/** offset */
	p_s1->digest.offset = rand();
	uint32_data = hton_u32((uint32)p_s1->digest.offset);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_C1S1_OFFSET_SIZE;
	total_mux_size += RTMP_C1S1_OFFSET_SIZE;
	left_size -= RTMP_C1S1_OFFSET_SIZE;

	/** r0 */
	ret = rtmp_calc_digest_random_size(&p_s1->digest, &r0_size);
	if( r0_size > 0 )
	{
		ret = rtmp_make_random_data(r0, r0_size);
		memcpy(p_uint8, r0, r0_size);
	}

	p_uint8 += r0_size;
	total_mux_size += r0_size;
	left_size -= r0_size;

	/** c1 join part1 */
	part1_size = total_mux_size;
	memcpy(c1_join, p_buf, part1_size);

	/** skip digest */
	p_uint8 += RTMP_C1_DIGEST_SIZE;
	total_mux_size += RTMP_C1_DIGEST_SIZE;
	left_size -= RTMP_C1_DIGEST_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_DIGEST_SIZE - RTMP_C1S1_OFFSET_SIZE - r0_size - RTMP_C1_DIGEST_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;
	}

	if( r1_size > 0 )
	{
		ret = rtmp_make_random_data(r1, r1_size);
		memcpy(p_uint8, r1, r1_size);
	}

	p_uint8 += r1_size;
	total_mux_size += r1_size;
	left_size -= r1_size;
	part2_size += r1_size;

	/** key block */
	p_s1->key.offset = rand();
	/** r0 */
	ret = rtmp_calc_key_random_size(&p_s1->key, &r0_size);
	if( r0_size > 0 )
	{
		ret = rtmp_make_random_data(r0, r0_size);
		memcpy(p_uint8, r0, r0_size);
	}

	p_uint8 += r0_size;
	total_mux_size += r0_size;
	left_size -= r0_size;
	part2_size += r0_size;

	/** key data */
	ret = rtmp_make_random_data(p_s1->key.key, RTMP_C1_KEY_SIZE);
	memcpy(p_uint8, p_s1->key.key, RTMP_C1_KEY_SIZE);
	p_uint8 += RTMP_C1_KEY_SIZE;
	total_mux_size += RTMP_C1_KEY_SIZE;
	left_size -= RTMP_C1_KEY_SIZE;
	part2_size += RTMP_C1_KEY_SIZE;

	/** r1 */
	r1_size = RTMP_C1S1_KEY_SIZE - r0_size  - RTMP_C1_KEY_SIZE - RTMP_C1S1_OFFSET_SIZE;
	if( r1_size < 0 )
	{
		return GEN_E_FAIL;
	}

	if( r1_size > 0 )
	{
		ret = rtmp_make_random_data(r1, r1_size);
		memcpy(p_uint8, r1, r1_size);
	}

	p_uint8 += r1_size;
	total_mux_size += r1_size;
	left_size -= r1_size;
	part2_size += r1_size;

	/** offset */
	uint32_data = hton_u32((uint32)p_s1->key.offset);
	p_uint8_data = (uint8*)&uint32_data;
	p_uint8[0] = p_uint8_data[0];
	p_uint8[1] = p_uint8_data[1];
	p_uint8[2] = p_uint8_data[2];
	p_uint8[3] = p_uint8_data[3];
	p_uint8 += RTMP_C1S1_OFFSET_SIZE;
	total_mux_size += RTMP_C1S1_OFFSET_SIZE;
	left_size -= RTMP_C1S1_OFFSET_SIZE;
	part2_size += RTMP_C1S1_OFFSET_SIZE;

	/** calc digest */
	memcpy(c1_join + part1_size, p_buf + part1_size + RTMP_C1_DIGEST_SIZE, part2_size);

	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, rtmp_fp_key, 30);

	ret = gen_hmac_process(&hmac_state, c1_join, part1_size + part2_size);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, p_s1->digest.digest, &sha_buf_size);

	memcpy(p_buf + part1_size, p_s1->digest.digest, RTMP_C1_DIGEST_SIZE);

	if( total_mux_size != need_size )
	{
		return GEN_E_FAIL;
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 rtmp_mux_s2(RTMP_S2* p_s2, RTMP_C1* p_c1, uint8* p_buf, int32* p_buf_size)
{
	int32   left_size;
	int32   need_size;
	int32   total_mux_size;

	uint8*  p_uint8 = NULL;

	uint8   temp_key[RTMP_C1_DIGEST_SIZE];
	//int32   temp_key_size;

	GEN_HMAC_STATE  hmac_state;
	int32   sha_buf_size;

	int32   ret;

	if( p_s2 == NULL || p_c1 == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;
	*p_buf_size = 0;

	need_size = RTMP_S2_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_BUF;
	}

	p_uint8 = p_buf;
	total_mux_size = 0;

	/** make random data */
	ret = rtmp_make_random_data(p_s2->random, RTMP_C1S1_JOIN_SIZE);
	ret = rtmp_make_random_data(p_s2->digest, RTMP_C1_DIGEST_SIZE);

	/** calc temp key */
	gen_hmac_setup(&hmac_state);

	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, rtmp_fms_key, 68);

	ret = gen_hmac_process(&hmac_state, p_c1->digest.digest, RTMP_C1_DIGEST_SIZE);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, temp_key, &sha_buf_size);

	/** calc digest */
	hmac_state.sha_type = GEN_HMAC_SHA_TYPE_SHA256;
	ret = gen_hmac_init(&hmac_state, temp_key, RTMP_C1_DIGEST_SIZE);

	ret = gen_hmac_process(&hmac_state, p_s2->random, RTMP_C1S1_JOIN_SIZE);

	sha_buf_size = RTMP_C1_DIGEST_SIZE;
	ret = gen_hmac_done(&hmac_state, p_s2->digest, &sha_buf_size);

	/** copy */
	memcpy(p_uint8, p_s2->random, RTMP_C1S1_JOIN_SIZE);
	memcpy(p_uint8 + RTMP_C1S1_JOIN_SIZE, p_s2->digest, RTMP_C1_DIGEST_SIZE);

	*p_buf_size = need_size;

	return GEN_S_OK;
}
