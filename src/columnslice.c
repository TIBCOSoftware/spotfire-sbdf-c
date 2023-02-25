/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "columnslice.h"
#include "columnslice_io.h"
#include "valuearray.h"
#include "valuearray_io.h"
#include "errors.h"
#include "sbdfstring.h"
#include "sectiontypeid.h"
#include "sectiontypeid_io.h"

#include "internals.h"

#include <stdlib.h>
#include <string.h>

/* stores a reference to values */
int sbdf_cs_create(sbdf_columnslice** out, sbdf_valuearray* values)
{
	sbdf_columnslice* t = 0;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(1, sizeof(sbdf_columnslice));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->values = values;

	*out = t;

	return SBDF_OK;
}

/* stores a reference to values */
int sbdf_cs_add_property(sbdf_columnslice* out, char const* name, sbdf_valuearray* values)
{
	int i, cap, error;
	char* nm;

	if (!out || !name || !values)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (sbdf_cs_row_cnt(out) != sbdf_va_row_cnt(values))
	{
		return SBDF_ERROR_ROW_COUNT_MISMATCH;
	}

	for (i = 0; i < out->prop_cnt; ++i)
	{
		if (!strcmp(name, out->property_names[i]))
		{
			return SBDF_ERROR_PROPERTY_ALREADY_EXISTS;
		}
	}

	cap = sbdf_calculate_array_capacity(out->prop_cnt);

	if (cap == out->prop_cnt)
	{
		int new_cap = sbdf_calculate_array_capacity(1 + out->prop_cnt);
		if ((error = sbdf_alloc((void**)&out->properties, new_cap * sizeof(void*))))
		{
			return error;
		}
		if ((error = sbdf_alloc((void**)&out->property_names, new_cap * sizeof(void*))))
		{
			return error;
		}
	}

	nm = sbdf_str_create(name);
	if (!nm)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	out->property_names[out->prop_cnt] = nm;
	out->properties[out->prop_cnt++] = values;

	return SBDF_OK;
}

int sbdf_cs_get_property(sbdf_columnslice* in, char const* name, sbdf_valuearray** out)
{
	int i;

	if (!in || !name || !out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	for (i = 0; i < in->prop_cnt; ++i)
	{
		if (!strcmp(name, in->property_names[i]))
		{
			*out = (sbdf_valuearray*)in->properties[i];
			return SBDF_OK;
		}
	}

	return SBDF_ERROR_PROPERTY_NOT_FOUND;
}

int sbdf_cs_row_cnt(sbdf_columnslice* in)
{
	if (!in)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	return sbdf_va_row_cnt(in->values);
}

void sbdf_cs_destroy(sbdf_columnslice* cs)
{
	if (cs)
	{
		int i;

		if (cs->owned)
		{
			sbdf_cs_destroy_all(cs);
			return;
		}

		if (cs->property_names)
		{
			for (i = 0; i < cs->prop_cnt; ++i)
			{
				sbdf_str_destroy(cs->property_names[i]);
			}

			free(cs->property_names);
		}

		if (cs->properties)
		{
			free((void*)cs->properties);
		}

		free(cs);
	}
}

void sbdf_cs_destroy_all(sbdf_columnslice* cs)
{
	if (cs)
	{
		int i;

		sbdf_va_destroy((sbdf_valuearray*)cs->values);

		if (cs->properties)
		{
			for (i = 0; i < cs->prop_cnt; ++i)
			{
				sbdf_va_destroy((sbdf_valuearray*)cs->properties[i]);
			}
		}

		cs->owned = 0;
	}

	sbdf_cs_destroy(cs);
}

int sbdf_cs_read(FILE* f, sbdf_columnslice** out)
{
	sbdf_columnslice* t;
	int error, v, i;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_sec_expect(f, SBDF_COLUMNSLICE_SECTIONID))
	{
		return error;
	}

	t = calloc(1, sizeof(sbdf_columnslice));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->owned = 1;

	if (error = sbdf_va_read(f, &t->values))
	{
		goto end;
	}

	if (error = sbdf_read_int32(f, &v))
	{
		goto end;
	}

	t->prop_cnt = v;
	/* TODO Verify that it is OK to have no properties */
	if (v > 0)
	{
		if (error = sbdf_alloc((void**)&t->properties, v * sizeof(void*)))
		{
			goto end;
		}

		if (error = sbdf_alloc((void**)&t->property_names, v * sizeof(void*)))
		{
			goto end;
		}

		for (i = 0; i < v; ++i)
		{
			if (error = sbdf_read_string(f, &t->property_names[i]))
			{
				goto end;
			}
			if (error = sbdf_va_read(f, &t->properties[i]))
			{
				goto end;
			}
		}
	}

end: if (error)
	{
		sbdf_cs_destroy(t);
	}
	else
	{
		*out = t;
	}

	return error;
}

int sbdf_cs_write(FILE* f, sbdf_columnslice const* in)
{
	int error, i;

	if (!in)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_sec_write(f, SBDF_COLUMNSLICE_SECTIONID))
	{
		return error;
	}

	if (error = sbdf_va_write(in->values, f))
	{
		return error;
	}

	if (error = sbdf_write_int32(f, in->prop_cnt))
	{
		return error;
	}

	for (i = 0; i < in->prop_cnt; ++i)
	{
		if (error = sbdf_write_string(f, in->property_names[i]))
		{
			return error;
		}
		if (error = sbdf_va_write(in->properties[i], f))
		{
			return error;
		}
	}

	return SBDF_OK;
}

int sbdf_cs_skip(FILE* f)
{
	int error, v;

	if (error = sbdf_sec_expect(f, SBDF_COLUMNSLICE_SECTIONID))
	{
		return error;
	}

	if (error = sbdf_va_skip(f))
	{
		return error;
	}

	if (error = sbdf_read_int32(f, &v))
	{
		return error;
	}

	if (v < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	while (v-- > 0)
	{
		if (error = sbdf_skip_string(f))
		{
			return error;
		}

		if (error = sbdf_va_skip(f))
		{
			return error;
		}
	}

	return SBDF_OK;
}
