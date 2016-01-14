/**
 * @file gen_endian.h  General Endian Exchange
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.06.26
 *
 *
 */

#ifndef __GEN_ENDIAN_H__
#define __GEN_ENDIAN_H__

#define swap_u16(x)                                ( (((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF) )
#define swap_u32(x)                                ( (((x) << 24) & 0xFF000000) | (((x) >> 24) & 0xFF) | (((x) << 8) & 0xFF0000) | (((x) >> 8) & 0xFF00) )
#define swap_u64(x)                                ( (((x) << 56) & 0xFF00000000000000) | \
													 (((x) << 40) & 0x00FF000000000000) | \
													 (((x) << 24) & 0x0000FF0000000000) | \
													 (((x) <<  8) & 0x000000FF00000000) | \
													 (((x) >>  8) & 0x00000000FF000000) | \
													 (((x) >> 24) & 0x0000000000FF0000) | \
													 (((x) >> 40) & 0x000000000000FF00) | \
													 (((x) >> 56) & 0x00000000000000FF) )

#ifdef __BIG_ENDIAN__

#define hton_u16(x)                                (x)
#define hton_u32(x)                                (x)
#define hton_u64(x)                                (x)

#else

#define hton_u16(x)                                swap_u16(x)
#define hton_u32(x)                                swap_u32(x)
#define hton_u64(x)                                swap_u64(x)

#endif// __BIG_ENDIAN__

/** same to hton_* func */
#define ntoh_u16(x)                                hton_u16(x)
#define ntoh_u32(x)                                hton_u32(x)
#define ntoh_u64(x)                                hton_u64(x)


#endif //__GEN_ENDIAN_H__
