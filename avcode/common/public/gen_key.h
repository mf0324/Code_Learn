/**
 * @file gen_key.h  General Key Define
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2010.06.11
 *
 *
 */

#ifndef __GEN_KEY_H__
#define __GEN_KEY_H__

#include <public/gen_int.h>

/** key pair */
typedef struct tag_key_pair
{
	/** key */
	void*  p_key;
	/** value */
	void*  p_value;

} KEY_PAIR, *PKEY_PAIR;


#endif ///__GEN_KEY_H__
