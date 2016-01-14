/**
 * @file riff_public.h   RIFF Constant
 * @author 翁彦 <robert_3000@sina.com>
 * @version 0.1
 * @date 2009.10.26
 *
 *
 */


#ifndef __RIFF_PUBLIC_H__
#define __RIFF_PUBLIC_H__

/** 
 * @name RIFF
 * @{
 */

/** LIST头长度 */
#define RIFF_LIST_HEADER_SIZE                      (12)
/** CHUNK头长度 */
#define RIFF_CHUNK_HEADER_SIZE                     (8)

/** RIFF */
#define RIFF_ID_RIFF                               (0x52494646)
/** LIST */
#define RIFF_ID_LIST                               (0x4C495354)

/** 'AVI ' */
#define RIFF_FOURCC_AVI                            (0x41564920)
/** 'AVIX' */
#define RIFF_FOURCC_AVIX                           (0x41564958)

/** 
 * @}
 */

#endif ///__RIFF_PUBLIC_H__
