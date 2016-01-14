
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "amf_const.h"
#include "amf_public.h"
#include "amf_demux.h"


/** max prop count */
#define AMF_DEMUX_MAX_PROP_NUM                             (64)

/** prop */
static AMF_OBJ_PROP  g_prop[AMF_DEMUX_MAX_PROP_NUM];
/** prop count */
static g_prop_count;

////////////////////////////// Outter Interface ///////////////////////////////
int32 amf_demux_number(AMF_NUMBER* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_src = NULL;
	uint8* p_dest = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_NUMBER_VALUE_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_src = p_buf;
	p_dest = (uint8*)&p_amf_type->value;

	p_dest[0] = p_src[7];
	p_dest[1] = p_src[6];
	p_dest[2] = p_src[5];
	p_dest[3] = p_src[4];
	p_dest[4] = p_src[3];
	p_dest[5] = p_src[2];
	p_dest[6] = p_src[1];
	p_dest[7] = p_src[0];

	total_size += AMF_DATA_NUMBER_VALUE_SIZE;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_bool(AMF_BOOL* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_BOOL_VALUE_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;

	p_amf_type->b_true = p_uint8[0];

	total_size += AMF_DATA_BOOL_VALUE_SIZE;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_string(AMF_STRING* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_STRING_LEN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;
	p_dest = (uint8*)&p_amf_type->data_size;

	/** swap endian */
	p_dest[0] = p_uint8[1];
	p_dest[1] = p_uint8[0];

	p_uint8 += AMF_DATA_STRING_LEN_SIZE;
	left_size -= AMF_DATA_STRING_LEN_SIZE;
	total_size += AMF_DATA_STRING_LEN_SIZE;

	need_size = p_amf_type->data_size;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_amf_type->p_data = (int8*)p_uint8;

	left_size -= need_size;
	total_size += need_size;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_calc_obj_size(AMF_OBJ* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 demux_size;
	uint32 total_size;

	AMF_OBJ_PROP* p_prop = NULL;
	uint32 prop_count;
	//uint32 total_prop_count;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;
	int8*  p_name = NULL;
	uint8* p_value = NULL;
	uint32 value_size;
	uint16 name_size;
	uint16 str_size;
	uint32 long_str_size;
	uint32 ecma_array_size;

	AMF_OBJ temp_obj;
	//AMF_OBJ_PROP temp_prop;

	uint32 i;
	int32  ret;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	total_size = 0;
	p_uint8 = p_buf;

	prop_count = 0;
	i = 0;

	while( 1 )
	{
		need_size = AMF_DATA_STRING_LEN_SIZE;
		if( left_size < need_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		/** swap endian */
		str_size = (((uint16)p_uint8[0]) << 8) | (p_uint8[1]);

		p_uint8 += AMF_DATA_STRING_LEN_SIZE;
		left_size -= AMF_DATA_STRING_LEN_SIZE;
		total_size += AMF_DATA_STRING_LEN_SIZE;

		if( left_size < str_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_name = (int8*)p_uint8;
		name_size = str_size;

		p_uint8 += str_size;
		left_size -= str_size;
		total_size += str_size;

		if( left_size < AMF_DATA_TYPE_SIZE )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ_END )
		{
			/** obj end */
			p_uint8 += AMF_DATA_TYPE_SIZE;
			left_size -= AMF_DATA_TYPE_SIZE;
			total_size += AMF_DATA_TYPE_SIZE;
			break;
		}

		p_value = p_uint8;
		value_size = AMF_DATA_TYPE_SIZE;
		need_size = AMF_DATA_TYPE_SIZE;

		switch( p_uint8[0] )
		{
		case AMF_DATA_TYPE_NUMBER:
			{
				need_size += AMF_DATA_NUMBER_VALUE_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += AMF_DATA_NUMBER_VALUE_SIZE;
			}
			break;

		case AMF_DATA_TYPE_BOOL:
			{
				need_size += AMF_DATA_BOOL_VALUE_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += AMF_DATA_BOOL_VALUE_SIZE;
			}
			break;

		case AMF_DATA_TYPE_STRING:
			{
				need_size += AMF_DATA_STRING_LEN_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				str_size = (((uint16)p_uint8[1]) << 8) | (p_uint8[2]);

				need_size += str_size;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += (AMF_DATA_STRING_LEN_SIZE + str_size);
			}
			break;

		case AMF_DATA_TYPE_NULL:
		case AMF_DATA_TYPE_UNDEF:
			{

			}
			break;

		case AMF_DATA_TYPE_REF:
			{
				need_size += AMF_DATA_REF_VALUE_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += AMF_DATA_REF_VALUE_SIZE;
			}
			break;

		case AMF_DATA_TYPE_ECMA_ARRAY:
			{
				need_size += AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				ecma_array_size = (((uint32)p_uint8[1]) << 24) | (((uint32)p_uint8[2]) << 16) | (((uint32)p_uint8[3]) << 8) | (p_uint8[4]);

				value_size += (AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE + ecma_array_size);

				//temp_obj.p_prop = &temp_prop;
				//temp_obj.prop_count = 1;
				demux_size = left_size;
				ret = amf_demux_obj(&temp_obj, p_uint8 + value_size, left_size - value_size, &demux_size);
				value_size += demux_size;
			}
			break;

		case AMF_DATA_TYPE_LONG_STRING:
			{
				need_size += AMF_DATA_LONG_STRING_LEN_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				long_str_size = (((uint32)p_uint8[1]) << 24) | (((uint32)p_uint8[2]) << 16) | (((uint32)p_uint8[3]) << 8) | (p_uint8[4]);

				need_size += long_str_size;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += (AMF_DATA_LONG_STRING_LEN_SIZE + long_str_size);
			}
			break;

		case AMF_DATA_TYPE_XML_DOC:
			{
				need_size += AMF_DATA_LONG_STRING_LEN_SIZE;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				long_str_size = (((uint32)p_uint8[1]) << 24) | (((uint32)p_uint8[2]) << 16) | (((uint32)p_uint8[3]) << 8) | (p_uint8[4]);

				need_size += long_str_size;
				if( left_size < need_size )
				{
					return GEN_E_NEED_MORE_DATA;
				}

				value_size += (AMF_DATA_LONG_STRING_LEN_SIZE + long_str_size);
			}
			break;

		default:
			{
				//log_debug(_T("[amf_demux::amf_demux_calc_obj_size] unknown prop value type = 0x%02x\n"), p_uint8[0]);
			}
			break;
		}

		p_uint8 += value_size;
		left_size -= value_size;
		total_size += value_size;

		i++;
	}

	p_amf_type->prop_count = i;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_obj(AMF_OBJ* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 demux_size;
	uint32 total_size;

	AMF_OBJ_PROP* p_prop = NULL;
	uint32 prop_count;
	uint32 total_prop_count;

	AMF_DATA amf_data;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;
	int8*  p_name = NULL;
	uint8* p_value = NULL;
	uint32 value_size;
	uint16 name_size;
	uint16 str_size;

	uint32 i;
	int32  ret;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	total_size = 0;
	p_uint8 = p_buf;

	total_prop_count = p_amf_type->prop_count;
	p_prop = p_amf_type->p_prop;

	prop_count = 0;
	i = 0;

	while( 1 )
	{
		need_size = AMF_DATA_STRING_LEN_SIZE;
		if( left_size < need_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		/** string size */
		str_size = (((uint16)p_uint8[0]) << 8) | (p_uint8[1]);

		p_uint8 += AMF_DATA_STRING_LEN_SIZE;
		left_size -= AMF_DATA_STRING_LEN_SIZE;
		total_size += AMF_DATA_STRING_LEN_SIZE;

		if( left_size < str_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_name = (int8*)p_uint8;
		name_size = str_size;

		p_uint8 += str_size;
		left_size -= str_size;
		total_size += str_size;

		if( left_size < AMF_DATA_TYPE_SIZE )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ_END )
		{
			/** obj end */
			p_uint8 += AMF_DATA_TYPE_SIZE;
			left_size -= AMF_DATA_TYPE_SIZE;
			total_size += AMF_DATA_TYPE_SIZE;
			break;
		}

		p_value = p_uint8;

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ )
		{
			amf_data.udata.obj.prop_count = 0;
			amf_data.udata.obj.p_prop = NULL;

		}else if( p_uint8[0] == AMF_DATA_TYPE_ECMA_ARRAY )
		{
			amf_data.udata.ecma_array.prop_count = 0;
			amf_data.udata.ecma_array.p_prop = NULL;
			//amf_data.udata.ecma_array.prop_count = AMF_DEMUX_MAX_PROP_NUM;
			//amf_data.udata.ecma_array.p_prop = g_prop;

		}else if( p_uint8[0] == AMF_DATA_TYPE_STRICT_ARRAY )
		{
			amf_data.udata.strict_array.value_count = 0;
			amf_data.udata.strict_array.p_value = NULL;
		}

		ret = amf_demux_data_type(&amf_data, p_uint8, left_size, &demux_size);
		value_size = demux_size;

		/** update prop */
		if( prop_count < total_prop_count )
		{
			if( p_prop )
			{
				p_prop->name.data_size = name_size;
				p_prop->name.p_data = p_name;
				p_prop->value_size = value_size;
				p_prop->p_value = p_value;

				p_prop++;
			}

			prop_count++;
		}

		p_uint8 += value_size;
		left_size -= value_size;
		total_size += value_size;

		i++;
	}

	p_amf_type->prop_count = i;
	*p_demux_size = total_size;

	if( i > total_prop_count )
	{
		/** need more prop set */
		return GEN_E_NEED_MORE_BUF;
	}

	return GEN_S_OK;
}

int32 amf_demux_ref(AMF_REF* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_REF_VALUE_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;

	p_amf_type->index = ((uint16)p_uint8[0] << 8) | (p_uint8[1]);

	total_size += AMF_DATA_REF_VALUE_SIZE;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_ecma_array(AMF_ECMA_ARRAY* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;
	uint32 demux_size;

	AMF_OBJ_PROP* p_prop = NULL;
	uint32 prop_count;
	uint32 total_prop_count;
	uint32 ecma_count;

	AMF_DATA amf_data;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;
	int8*  p_name = NULL;
	uint8* p_value = NULL;
	uint32 value_size;
	uint16 name_size;
	uint16 str_size;

	uint32 i;
	int32  ret;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;
	p_dest = (uint8*)&ecma_count;

	/** ecma count */
	p_dest[0] = p_uint8[3];
	p_dest[1] = p_uint8[2];
	p_dest[2] = p_uint8[1];
	p_dest[3] = p_uint8[0];

	p_uint8 += AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE;
	left_size -= AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE;
	total_size += AMF_DATA_ECMA_ARRAY_PROP_COUNT_SIZE;

	total_prop_count = p_amf_type->prop_count;
	p_prop = p_amf_type->p_prop;

	prop_count = 0;
	i = 0;

	while( 1 )
	{
		need_size = AMF_DATA_STRING_LEN_SIZE;
		if( left_size < need_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		/** string size */
		str_size = (((uint16)p_uint8[0]) << 8) | (p_uint8[1]);

		p_uint8 += AMF_DATA_STRING_LEN_SIZE;
		left_size -= AMF_DATA_STRING_LEN_SIZE;
		total_size += AMF_DATA_STRING_LEN_SIZE;

		if( left_size < str_size )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_name = (int8*)p_uint8;
		name_size = str_size;

		p_uint8 += str_size;
		left_size -= str_size;
		total_size += str_size;

		if( left_size < AMF_DATA_TYPE_SIZE )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ_END )
		{
			/** obj end */
			p_uint8 += AMF_DATA_TYPE_SIZE;
			left_size -= AMF_DATA_TYPE_SIZE;
			total_size += AMF_DATA_TYPE_SIZE;
			break;
		}

		p_value = p_uint8;

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ )
		{
			amf_data.udata.obj.prop_count = 0;
			amf_data.udata.obj.p_prop = NULL;

		}else if( p_uint8[0] == AMF_DATA_TYPE_ECMA_ARRAY )
		{
			amf_data.udata.ecma_array.prop_count = 0;
			amf_data.udata.ecma_array.p_prop = NULL;

		}else if( p_uint8[0] == AMF_DATA_TYPE_STRICT_ARRAY )
		{
			amf_data.udata.strict_array.value_count = 0;
			amf_data.udata.strict_array.p_value = NULL;
		}

		ret = amf_demux_data_type(&amf_data, p_uint8, left_size, &demux_size);
		value_size = demux_size;

		/** update prop */
		if( prop_count < total_prop_count )
		{
			if( p_prop )
			{
				p_prop->name.data_size = name_size;
				p_prop->name.p_data = p_name;
				p_prop->value_size = value_size;
				p_prop->p_value = p_value;

				p_prop++;
			}

			prop_count++;
		}

		p_uint8 += value_size;
		left_size -= value_size;
		total_size += value_size;

		i++;
	}

	p_amf_type->prop_count = i;
	*p_demux_size = total_size;

	if( i > total_prop_count )
	{
		/** need more prop set */
		return GEN_E_NEED_MORE_BUF;
	}

	return GEN_S_OK;
}

int32 amf_demux_strict_array(AMF_STRICT_ARRAY* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;
	uint32 demux_size;

	AMF_VALUE* p_amf_value = NULL;
	uint32 value_count;
	uint32 total_value_count;
	uint32 array_count;

	AMF_DATA amf_data;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;
	uint8* p_value = NULL;
	uint32 value_size;

	uint32 i;
	int32  ret;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_STRICT_ARRAY_VALUE_COUNT_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;
	p_dest = (uint8*)&array_count;

	/** array count */
	p_dest[0] = p_uint8[3];
	p_dest[1] = p_uint8[2];
	p_dest[2] = p_uint8[1];
	p_dest[3] = p_uint8[0];

	p_uint8 += AMF_DATA_STRICT_ARRAY_VALUE_COUNT_SIZE;
	left_size -= AMF_DATA_STRICT_ARRAY_VALUE_COUNT_SIZE;
	total_size += AMF_DATA_STRICT_ARRAY_VALUE_COUNT_SIZE;

	total_value_count = p_amf_type->value_count;
	p_amf_value = p_amf_type->p_value;

	value_count = 0;
	//i = 0;

	//while( 1 )
	for( i = 0; i < array_count; i++ )
	{
		if( left_size < AMF_DATA_TYPE_SIZE )
		{
			return GEN_E_NEED_MORE_DATA;
		}

		p_value = p_uint8;
		//value_size = 0;

		if( p_uint8[0] == AMF_DATA_TYPE_OBJ )
		{
			amf_data.udata.obj.prop_count = 0;
			amf_data.udata.obj.p_prop = NULL;

		}else if( p_uint8[0] == AMF_DATA_TYPE_ECMA_ARRAY )
		{
			amf_data.udata.ecma_array.prop_count = 0;
			amf_data.udata.ecma_array.p_prop = NULL;

		}else if( p_uint8[0] == AMF_DATA_TYPE_STRICT_ARRAY )
		{
			amf_data.udata.strict_array.value_count = 0;
			amf_data.udata.strict_array.p_value = NULL;
		}

		ret = amf_demux_data_type(&amf_data, p_uint8, left_size, &demux_size);
		p_uint8 += demux_size;
		left_size -= demux_size;
		total_size += demux_size;
		value_size = demux_size;

		/** update value */
		if( value_count < total_value_count )
		{
			if( p_amf_value )
			{
				p_amf_value->value_size = value_size;
				p_amf_value->p_value = p_value;

				p_amf_value++;
			}

			value_count++;
		}

		//i++;
	}

	p_amf_type->value_count = i;
	*p_demux_size = total_size;

	if( i > total_value_count )
	{
		/** need more value set */
		//return -4;
		return GEN_E_NEED_MORE_DATA;
	}

	return GEN_S_OK;
}

int32 amf_demux_long_string(AMF_LONG_STRING* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_LONG_STRING_LEN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;
	p_dest = (uint8*)&p_amf_type->data_size;

	/** swap endian */
	p_dest[0] = p_uint8[3];
	p_dest[1] = p_uint8[2];
	p_dest[2] = p_uint8[1];
	p_dest[3] = p_uint8[0];

	p_uint8 += AMF_DATA_LONG_STRING_LEN_SIZE;
	left_size -= AMF_DATA_LONG_STRING_LEN_SIZE;
	total_size += AMF_DATA_LONG_STRING_LEN_SIZE;

	need_size = p_amf_type->data_size;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_amf_type->p_data = (int8*)p_uint8;

	left_size -= need_size;
	total_size += need_size;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_xml_doc(AMF_XML_DOC* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_dest = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_LONG_STRING_LEN_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;
	p_dest = (uint8*)&p_amf_type->data_size;

	/** swap endian */
	p_dest[0] = p_uint8[3];
	p_dest[1] = p_uint8[2];
	p_dest[2] = p_uint8[1];
	p_dest[3] = p_uint8[0];

	p_uint8 += AMF_DATA_LONG_STRING_LEN_SIZE;
	left_size -= AMF_DATA_LONG_STRING_LEN_SIZE;
	total_size += AMF_DATA_LONG_STRING_LEN_SIZE;

	need_size = p_amf_type->data_size;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	p_amf_type->p_data = (int8*)p_uint8;

	left_size -= need_size;
	total_size += need_size;

	*p_demux_size = total_size;

	return GEN_S_OK;
}

int32 amf_demux_data_type(AMF_DATA* p_amf_type, uint8* p_buf, uint32 buf_size, uint32* p_demux_size)
{
	uint32 left_size = buf_size;
	uint32 need_size;
	uint32 demux_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	int32  ret;

	if( p_amf_type == NULL || p_buf == NULL || p_demux_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	*p_demux_size = 0;

	need_size = AMF_DATA_TYPE_SIZE;
	if( left_size < need_size )
	{
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;

	p_uint8 = p_buf;

	p_amf_type->type = p_uint8[0];

	p_uint8 += AMF_DATA_TYPE_SIZE;
	left_size -= AMF_DATA_TYPE_SIZE;
	total_size += AMF_DATA_TYPE_SIZE;

	demux_size = 0;
	ret = 0;
	switch( p_amf_type->type )
	{
	case AMF_DATA_TYPE_NUMBER:
		{
			ret = amf_demux_number(&p_amf_type->udata.number, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_BOOL:
		{
			ret = amf_demux_bool(&p_amf_type->udata.boolean, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_STRING:
		{
			ret = amf_demux_string(&p_amf_type->udata.string, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_OBJ:
		{
			ret = amf_demux_obj(&p_amf_type->udata.obj, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_NULL:
	case AMF_DATA_TYPE_UNDEF:
		{
			ret = 0;
		}
		break;

	case AMF_DATA_TYPE_ECMA_ARRAY:
		{
			ret = amf_demux_ecma_array(&p_amf_type->udata.ecma_array, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_STRICT_ARRAY:
		{
			ret = amf_demux_strict_array(&p_amf_type->udata.strict_array, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_LONG_STRING:
		{
			ret = amf_demux_long_string(&p_amf_type->udata.long_string, p_uint8, left_size, &demux_size);
		}
		break;

	case AMF_DATA_TYPE_XML_DOC:
		{
			ret = amf_demux_xml_doc(&p_amf_type->udata.xml_doc, p_uint8, left_size, &demux_size);
		}
		break;

	default:
		{
			ret = GEN_E_WRONG_FORMAT;
			//log_debug(_T("[amf_demux::amf_demux_data_type] unknown data type = 0x%x\n"), p_amf_type->type);
		}
		break;
	}

	if( ret == GEN_E_NEED_MORE_DATA )
	{
		return ret;
	}

	total_size += demux_size;
	*p_demux_size = total_size;

	return ret;
}
