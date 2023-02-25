/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "valuetype.h"
#include "valuetype_io.h"
#include "valuetypeid.h"
#include "errors.h"

#include "internals.h"

#include <string.h>

int sbdf_vt_write(FILE* f, sbdf_valuetype v)
{
	int err;
	if (err = sbdf_write_int8(f, v.id))
	{
		return err;
	}

	return SBDF_OK;
}

int sbdf_vt_read(FILE* f, sbdf_valuetype* v)
{
	int err;

	if (!v)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	memset(v, 0, sizeof(sbdf_valuetype));

	if (err = sbdf_read_int8(f, &v->id))
	{
		return err;
	}

	return SBDF_OK;
}

int sbdf_vt_cmp(sbdf_valuetype lhs, sbdf_valuetype rhs)
{
	return lhs.id - rhs.id;
}


sbdf_valuetype sbdf_vt_bool()
{
	sbdf_valuetype result = { SBDF_BOOLTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_int()
{
	sbdf_valuetype result = { SBDF_INTTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_long()
{
	sbdf_valuetype result = { SBDF_LONGTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_float()
{
	sbdf_valuetype result = { SBDF_FLOATTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_double()
{
	sbdf_valuetype result = { SBDF_DOUBLETYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_datetime()
{
	sbdf_valuetype result = { SBDF_DATETIMETYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_date()
{
	sbdf_valuetype result = { SBDF_DATETYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_time()
{
	sbdf_valuetype result = { SBDF_TIMETYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_timespan()
{
	sbdf_valuetype result = { SBDF_TIMESPANTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_string()
{
	sbdf_valuetype result = { SBDF_STRINGTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_binary()
{
	sbdf_valuetype result = { SBDF_BINARYTYPEID };
	return result;
}

sbdf_valuetype sbdf_vt_decimal()
{
	sbdf_valuetype result = { SBDF_DECIMALTYPEID };
	return result;
}
