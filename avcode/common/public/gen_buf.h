/**
 * @file gen_buf.h  General Buf
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2013.06.14
 *
 *
 */

#ifndef __GEN_BUF_H__
#define __GEN_BUF_H__

#include <stdlib.h>

#include <public/gen_int.h>
#include <public/gen_def.h>
#include <public/gen_error.h>
#include <public/gen_buf_public.h>


#ifdef __cplusplus
extern "C"
{
#endif

/**  
 * @brief setup
 * @param [in] p_buf, buf
 * @return
 * 0: success
 * other: fail
 */
static inline int32 gen_buf_setup(GEN_BUF* p_buf);

/**  
 * @brief cleanup
 * @param [in] p_buf, buf
 * @return
 * 0: success
 * other: fail
 */
static inline int32 gen_buf_cleanup(GEN_BUF* p_buf);

/**  
 * @brief init
 * @param [in] p_buf, buf
 * @param [in] buf_size, buf size
 * @return
 * 0: success
 * other: fail
 */

#if !RUN_OS_64
static inline int32 gen_buf_init(GEN_BUF* p_buf, int32 buf_size);
#else
static inline int32 gen_buf_init(GEN_BUF* p_buf, int64 buf_size);
#endif

/**  
 * @brief deinit
 * @param [in] p_buf, buf
 * @return
 * 0: success
 * other: fail
 */
static inline int32 gen_buf_deinit(GEN_BUF* p_buf);


static inline int32 gen_buf_setup(GEN_BUF* p_buf)
{
	p_buf->p_buf = NULL;
	p_buf->buf_total_size = 0;

	return GEN_S_OK;
}

static inline int32 gen_buf_cleanup(GEN_BUF* p_buf)
{
	gen_buf_deinit(p_buf);

	return GEN_S_OK;
}

#if !RUN_OS_64
static inline int32 gen_buf_init(GEN_BUF* p_buf, int32 buf_size)
#else
static inline int32 gen_buf_init(GEN_BUF* p_buf, int64 buf_size)
#endif
{
	p_buf->p_buf = (uint8*)malloc( buf_size );
	if( p_buf->p_buf == NULL )
	{
		return GEN_E_NOT_ENOUGH_MEM;
	}

	p_buf->buf_total_size = buf_size;
	p_buf->buf_valid_size = buf_size;
	p_buf->buf_use_size = 0;
	p_buf->buf_left_size = p_buf->buf_valid_size;

	return GEN_S_OK;
}

static inline int32 gen_buf_deinit(GEN_BUF* p_buf)
{
	if( p_buf->p_buf )
	{
		free(p_buf->p_buf);
		p_buf->p_buf = NULL;
		p_buf->buf_total_size = 0;
	}

	return GEN_S_OK;
}

#ifdef __cplusplus
}
#endif


#endif ///__GEN_BUF_H__
