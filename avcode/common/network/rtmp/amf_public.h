/**
 * @file amf_public.h   AMF Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2011.10.18
 *
 *
 */


#ifndef __AMF_PUBLIC_H__
#define __AMF_PUBLIC_H__

#include <public/gen_int.h>



/** Number */
typedef struct tag_amf_number
{
	/** value */
	double value;

} AMF_NUMBER, *PAMF_NUMBER;

/** Bool */
typedef struct tag_amf_bool
{
	/** true */
	uint8  b_true;
	/** padding */
	uint8  padding[3];

} AMF_BOOL, *PAMF_BOOL;

/** String */
typedef struct tag_amf_string
{
	/** data size */
	uint16 data_size;
	/** data */
	int8*  p_data;

} AMF_STRING, *PAMF_STRING;

/** Object Property */
typedef struct tag_amf_obj_prop
{
	/** name */
	AMF_STRING  name;

	/** value size */
	uint32 value_size;
	/** value */
	uint8* p_value;

} AMF_OBJ_PROP, *PAMF_OBJ_PROP;

/** Object */
typedef struct tag_amf_obj
{
	/** prop count */
	uint32 prop_count;
	/** prop */
	AMF_OBJ_PROP* p_prop;

} AMF_OBJ, *PAMF_OBJ;

/** Reference */
typedef struct tag_amf_ref
{
	/** index */
	uint16 index;
	/** padding */
	uint16 padding;

} AMF_REF, *PAMF_REF;

/** ECMA Array */
typedef struct tag_amf_ecma_array
{
	/** prop count */
	uint32 prop_count;
	/** prop */
	AMF_OBJ_PROP* p_prop;

} AMF_ECMA_ARRAY, *PAMF_ECMA_ARRAY;

/** Value */
typedef struct tag_amf_value
{
	/** value size */
	uint32 value_size;
	/** value */
	uint8* p_value;

} AMF_VALUE, *PAMF_VALUE;

/** Strict Array */
typedef struct tag_amf_strict_array
{
	/** value count */
	uint32 value_count;
	/** value */
	AMF_VALUE* p_value;

} AMF_STRICT_ARRAY, *PAMF_STRICT_ARRAY;

/** Long String */
typedef struct tag_amf_long_string
{
	/** data size */
	uint32 data_size;
	/** data */
	int8*  p_data;

} AMF_LONG_STRING, *PAMF_LONG_STRING;

/** XML Document */
typedef struct tag_amf_xml_doc
{
	/** data size */
	uint32 data_size;
	/** data */
	int8*  p_data;

} AMF_XML_DOC, *PAMF_XML_DOC;

/** Typed Object */
typedef struct tag_amf_typed_obj
{
	/** class name */
	AMF_STRING class_name;

	/** obj prop */
	AMF_OBJ_PROP obj_prop;

} AMF_TYPED_OBJ, *PAMF_TYPED_OBJ;


/** amf data */
typedef struct tag_amf_data
{
	/** type */
	int32  type;

	union
	{
		/** number */
		AMF_NUMBER number;
		/** bool */
		AMF_BOOL boolean;
		/** string */
		AMF_STRING string;
		/** object */
		AMF_OBJ obj;
		/** ref */
		AMF_REF ref;
		/** ecma array */
		AMF_ECMA_ARRAY ecma_array;
		/** strict array */
		AMF_STRICT_ARRAY strict_array;
		/** long string */
		AMF_LONG_STRING long_string;
		/** xml doc */
		AMF_XML_DOC xml_doc;
		/** typed object */
		AMF_TYPED_OBJ typed_obj;

	} udata;

} AMF_DATA, *PAMF_DATA;



#endif ///__AMF_PUBLIC_H__
