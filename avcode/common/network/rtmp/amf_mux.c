
#include <stdlib.h>
#include <string.h>

#include <public/gen_def.h>
#include <public/gen_endian.h>
#include <public/gen_error.h>
#include <util/log_agent_const.h>
#include <util/log_agent.h>

#include "amf_const.h"
#include "amf_public.h"
#include "amf_mux.h"


////////////////////////////// Outter Interface ///////////////////////////////
int32 amf_mux_number(AMF_NUMBER* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_NUMBER_VALUE_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_NUMBER;
	p_uint8 += AMF_DATA_TYPE_SIZE;


	p_src = (uint8*)&p_amf_type->value;

	p_uint8[0] = p_src[7];
	p_uint8[1] = p_src[6];
	p_uint8[2] = p_src[5];
	p_uint8[3] = p_src[4];
	p_uint8[4] = p_src[3];
	p_uint8[5] = p_src[2];
	p_uint8[6] = p_src[1];
	p_uint8[7] = p_src[0];

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_bool(AMF_BOOL* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_BOOL_VALUE_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_BOOL;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	p_uint8[0] = p_amf_type->b_true;

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_string(AMF_STRING* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_STRING_LEN_SIZE + p_amf_type->data_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_STRING;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	/** string size */
	p_src = (uint8*)&p_amf_type->data_size;
	p_uint8[0] = p_src[1];
	p_uint8[1] = p_src[0];
	p_uint8 += AMF_DATA_STRING_LEN_SIZE;

	if( p_amf_type->data_size > 0 && p_amf_type->p_data )
	{
		memcpy(p_uint8, p_amf_type->p_data, p_amf_type->data_size);
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_calc_obj_size(AMF_OBJ* p_amf_type, uint32* p_size)
{
	AMF_OBJ_PROP* p_prop = NULL;
	uint32 need_size;
	uint32 i;

	if( p_amf_type == NULL || p_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	need_size = AMF_DATA_TYPE_SIZE;

	if( p_amf_type->prop_count > 0 && p_amf_type->p_prop )
	{
		for( i = 0; i < p_amf_type->prop_count; i++ )
		{
			p_prop = p_amf_type->p_prop + i;

			/** name */
			need_size += (AMF_DATA_STRING_LEN_SIZE + p_prop->name.data_size);
			
			/** value */
			need_size += p_prop->value_size;
		}
	}

	/** obj end */
	need_size += (AMF_DATA_STRING_LEN_SIZE + AMF_DATA_TYPE_SIZE);

	*p_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_obj(AMF_OBJ* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	AMF_OBJ_PROP* p_prop = NULL;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	uint32 i;
	int32  ret;

	if( p_amf_type == NULL || p_amf_type->p_prop == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	ret = amf_calc_obj_size(p_amf_type, &need_size);
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_OBJ;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	if( p_amf_type->prop_count > 0 && p_amf_type->p_prop )
	{
		for( i = 0; i < p_amf_type->prop_count; i++ )
		{
			p_prop = p_amf_type->p_prop + i;

			/** name */
			p_src = (uint8*)&p_prop->name.data_size;
			p_uint8[0] = p_src[1];
			p_uint8[1] = p_src[0];

			p_uint8 += AMF_DATA_STRING_LEN_SIZE;

			if( p_prop->name.p_data )
			{
				memcpy(p_uint8, p_prop->name.p_data, p_prop->name.data_size);
				p_uint8 += p_prop->name.data_size;
			}

			/** value */
			if( p_prop->value_size > 0 && p_prop->p_value )
			{
				memcpy(p_uint8, p_prop->p_value, p_prop->value_size);
				p_uint8 += p_prop->value_size;
			}
		}
	}

	/** obj end */
	p_uint8[0] = 0x00;
	p_uint8[1] = 0x00;
	p_uint8[2] = AMF_DATA_TYPE_OBJ_END;

	p_uint8 += (AMF_DATA_STRING_LEN_SIZE + AMF_DATA_TYPE_SIZE);

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_null(uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_NULL;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_undef(uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;

	if( p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_UNDEF;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_ref(AMF_REF* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_REF_VALUE_SIZE;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_REF;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	/** swap endian */
	p_src = (uint8*)&p_amf_type->index;
	p_uint8[0] = p_src[1];
	p_uint8[1] = p_src[0];

	p_uint8 += AMF_DATA_REF_VALUE_SIZE;

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_long_string(AMF_LONG_STRING* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_LONG_STRING_LEN_SIZE + p_amf_type->data_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_LONG_STRING;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	/** string size */
	p_src = (uint8*)&p_amf_type->data_size;
	p_uint8[0] = p_src[3];
	p_uint8[1] = p_src[2];
	p_uint8[2] = p_src[1];
	p_uint8[3] = p_src[0];
	p_uint8 += AMF_DATA_LONG_STRING_LEN_SIZE;

	if( p_amf_type->data_size > 0 && p_amf_type->p_data )
	{
		memcpy(p_uint8, p_amf_type->p_data, p_amf_type->data_size);
		p_uint8 += p_amf_type->data_size;
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}

int32 amf_mux_xml_doc(AMF_XML_DOC* p_amf_type, uint8* p_buf, uint32* p_buf_size)
{
	uint32 left_size;
	uint32 need_size;
	uint32 total_size;

	uint8* p_uint8 = NULL;
	uint8* p_src = NULL;

	if( p_amf_type == NULL || p_buf == NULL || p_buf_size == NULL )
	{
		return GEN_E_INVALID_PARAM;
	}

	left_size = *p_buf_size;

	need_size = AMF_DATA_TYPE_SIZE + AMF_DATA_LONG_STRING_LEN_SIZE + p_amf_type->data_size;
	if( left_size < need_size )
	{
		*p_buf_size = need_size;
		return GEN_E_NEED_MORE_DATA;
	}

	total_size = 0;
	p_uint8 = p_buf;

	p_uint8[0] = AMF_DATA_TYPE_LONG_STRING;
	p_uint8 += AMF_DATA_TYPE_SIZE;

	/** string size */
	p_src = (uint8*)&p_amf_type->data_size;
	p_uint8[0] = p_src[3];
	p_uint8[1] = p_src[2];
	p_uint8[2] = p_src[1];
	p_uint8[3] = p_src[0];
	p_uint8 += AMF_DATA_LONG_STRING_LEN_SIZE;

	if( p_amf_type->data_size > 0 && p_amf_type->p_data )
	{
		memcpy(p_uint8, p_amf_type->p_data, p_amf_type->data_size);
		p_uint8 += p_amf_type->data_size;
	}

	*p_buf_size = need_size;

	return GEN_S_OK;
}
