/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "columnmetadata.h"

#include <stdlib.h>

#include "internals.h"
#include "metadata.h"
#include "errors.h"
#include "object.h"
#include "bytearray.h"
#include "valuetypeid.h"
#include "sbdfstring.h"

int sbdf_cm_set_values(char const* column_name, sbdf_valuetype data_type, sbdf_metadata_head* out)
{
	int error;
	sbdf_object* obj;

	error = sbdf_md_add_str(SBDF_COLUMNMETADATA_NAME, column_name, 0, out);
	if (error)
	{
		return error;
	}

	error = sbdf_valuetype_to_object(data_type, &obj);
	if (error)
	{
		return error;
	}

	error = sbdf_md_add(SBDF_COLUMNMETADATA_DATATYPE, obj, 0, out);
	if (error)
	{
		sbdf_obj_destroy(obj);
		return error;
	}

	sbdf_obj_destroy(obj);
	return SBDF_OK;
}

int sbdf_cm_get_type(sbdf_metadata_head* inp, sbdf_valuetype* out)
{
	int error;
	sbdf_object* obj;
	unsigned char* in;

	if (!inp || !out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_md_get(SBDF_COLUMNMETADATA_DATATYPE, inp, &obj))
	{
		return error;
	}

	if (obj->type.id != SBDF_BINARYTYPEID || obj->count != 1 || sbdf_ba_get_len(*(void**)obj->data) != 1 && sbdf_ba_get_len(*(void**)obj->data) != 3)
	{
		sbdf_obj_destroy(obj);
		return SBDF_ERROR_INCORRECT_METADATA;
	}

	in = *(unsigned char**)obj->data;
	out->id = *in++;
	sbdf_obj_destroy(obj);

	return SBDF_OK;
}

int sbdf_cm_get_name(sbdf_metadata_head* inp, char** out)
{
	int error;
	sbdf_object* obj;

	if (!inp || !out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_md_get(SBDF_COLUMNMETADATA_NAME, inp, &obj))
	{
		return error;
	}

	if (obj->type.id != SBDF_STRINGTYPEID)
	{
		sbdf_obj_destroy(obj);
		return SBDF_ERROR_INCORRECT_METADATA;
	}

	*out = sbdf_str_copy(*(char**)obj->data);

	sbdf_obj_destroy(obj);

	return SBDF_OK;
}

