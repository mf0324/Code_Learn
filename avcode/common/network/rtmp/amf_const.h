/**
 * @file amf_const.h   AMF Constant
 * @author Robert Weng <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.18
 *
 *
 */


#ifndef __AMF_CONST_H__
#define __AMF_CONST_H__

/** 
 * @name AMF
 * @{
 */

/** Number */
#define AMF_DATA_TYPE_NUMBER                               (0x00)
/** Boolean */
#define AMF_DATA_TYPE_BOOL                                 (0x01)
/** String */
#define AMF_DATA_TYPE_STRING                               (0x02)
/** Object */
#define AMF_DATA_TYPE_OBJ                                  (0x03)
/** Movie Clip */
#define AMF_DATA_TYPE_MOVIE_CLIP                           (0x04)
/** Null */
#define AMF_DATA_TYPE_NULL                                 (0x05)
/** Undefined */
#define AMF_DATA_TYPE_UNDEF                                (0x06)
/** Reference */
#define AMF_DATA_TYPE_REF                                  (0x07)
/** Ecma Array */
#define AMF_DATA_TYPE_ECMA_ARRAY                           (0x08)
/** Object End */
#define AMF_DATA_TYPE_OBJ_END                              (0x09)
/** Strict Array */
#define AMF_DATA_TYPE_STRICT_ARRAY                         (0x0A)
/** Date */
#define AMF_DATA_TYPE_DATE                                 (0x0B)
/** Long String */
#define AMF_DATA_TYPE_LONG_STRING                          (0x0C)
/** Unsupported */
#define AMF_DATA_TYPE_UNSUPPORT                            (0x0D)
/** Record Set */
#define AMF_DATA_TYPE_RECORD_SET                           (0x0E)
/** Xml Document */
#define AMF_DATA_TYPE_XML_DOC                              (0x0F)
/** Typed Object */
#define AMF_DATA_TYPE_TYPED_OBJ                            (0x10)
/** Avmplus Object */
#define AMF_DATA_TYPE_AVMPLUS_OBJ                          (0x11)

/** UTF-8 String Len Size */
#define AMF_UTF8_STRING_LEN_SIZE                           (2)
/** UTF-8 Long String Len Size */
#define AMF_UTF8_LONG_STRING_LEN_SIZE                      (4)
/** UTF-8 Empty Size */
#define AMF_UTF8_EMPTY_SIZE                                (2)
/** UTF-8 Empty Value */
#define AMF_UTF8_EMPTY_VALUE                               (0x0000)

/** Data Type Size */
#define AMF_DATA_TYPE_SIZE                                 (1)

/** Number Value Size */
#define AMF_DATA_NUMBER_VALUE_SIZE                         (8)
/** Bool Value Size */
#define AMF_DATA_BOOL_VALUE_SIZE                           (1)
/** String Length Size */
#define AMF_DATA_STRING_LEN_SIZE                           (AMF_UTF8_STRING_LEN_SIZE)
/** Max String Length */
#define AMF_DATA_MAX_STRING_LEN                            (0xFFFF)
/** String Value Min Size */
#define AMF_DATA_STRING_VALUE_MIN_SIZE                     (AMF_DATA_STRING_LEN_SIZE)
/** Object Property Name Min Size */
#define AMF_DATA_OBJ_NAME_MIN_SIZE                         (AMF_DATA_STRING_LEN_SIZE)
/** Object Property Value Min Size */
#define AMF_DATA_OBJ_VALUE_MIN_SIZE                        (AMF_DATA_TYPE_SIZE)
/** Null Value Size */
#define AMF_DATA_NULL_VALUE_SIZE                           (0)
/** Undef Value Size */
#define AMF_DATA_UNDEF_VALUE_SIZE                          (0)
/** Ref Value Size */
#define AMF_DATA_REF_VALUE_SIZE                            (2)
/** ECMA Array Prop Count Size */
#define AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE                (4)
/** Object End Value Size */
#define AMF_DATA_OBJ_END_VALUE_SIZE                        (0)
/** Object End Marker Size */
#define AMF_DATA_OBJ_END_MARKER_SIZE                       (3)
/** Strict Array Value Count Size */
#define AMF_DATA_STRICT_ARRAY_VALUE_COUNT_SIZE             (4)
/** Date Value Size */
#define AMF_DATA_DATE_VALUE_SIZE                           (8 + 2)
/** Long String Length Size */
#define AMF_DATA_LONG_STRING_LEN_SIZE                      (AMF_UTF8_LONG_STRING_LEN_SIZE)
/** Xml Document Value Min Size */
#define AMF_DATA_XML_DOC_VALUE_MIN_SIZE                    (AMF_UTF8_LONG_STRING_LEN_SIZE)



/** 
 * @}
 */

#endif ///__AMF_CONST_H__
