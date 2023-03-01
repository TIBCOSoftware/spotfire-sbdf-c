/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "metadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tablemetadata.h"
#include "tablemetadata_io.h"
#include "errors.h"
#include "sectiontypeid.h"
#include "sectiontypeid_io.h"
#include "valuetype.h"
#include "object.h"
#include "object_io.h"
#include "valuetype_io.h"
#include "sbdfstring.h"

#include "internals.h"

void sbdf_tm_destroy(sbdf_tablemetadata* tmd)
{
	int i;
	if (tmd)
	{
		if (tmd->table_metadata)
		{
			sbdf_md_destroy(tmd->table_metadata);
		}

		if (tmd->column_metadata)
		{
			for (i = 0; i < tmd->no_columns; ++i)
			{
				sbdf_md_destroy(tmd->column_metadata[i]);
			}

			free(tmd->column_metadata);
		}
		free(tmd);
	}
}

int sbdf_tm_create(sbdf_metadata_head* table_metadata, sbdf_tablemetadata** out)
{
	int error;
	sbdf_tablemetadata* t;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(sizeof(sbdf_tablemetadata), 1);
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	error = sbdf_md_create(&t->table_metadata);
	if (error)
	{
		free(t);
		return error;
	}

	error = sbdf_md_copy(table_metadata, t->table_metadata);
	if (error)
	{
		sbdf_tm_destroy(t);
		return error;
	}

	sbdf_md_set_immutable(t->table_metadata);

	*out = t;
	return SBDF_OK;
}

int sbdf_tm_add(sbdf_metadata_head* columndata, sbdf_tablemetadata* tabledata)
{
	int oldcap;
	int error;
	sbdf_metadata_head* t;

	if (!tabledata)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	oldcap = sbdf_calculate_array_capacity(tabledata->no_columns);

	error = sbdf_md_create(&t);
	if (error)
	{
		return error;
	}

	error = sbdf_md_copy(columndata, t);
	if (error)
	{
		sbdf_md_destroy(t);
		return error;
	}

	sbdf_md_set_immutable(t);

	if (tabledata->no_columns == oldcap)
	{
		int newcap = sbdf_calculate_array_capacity(1 + tabledata->no_columns);
		if (tabledata->column_metadata)
		{
			sbdf_metadata_head** ptr = realloc(tabledata->column_metadata, sizeof(sbdf_metadata_head*) * newcap);
			if (!ptr)
			{
				sbdf_md_destroy(t);
				return SBDF_ERROR_OUT_OF_MEMORY;
			}
			tabledata->column_metadata = ptr;
		}
		else
		{
			tabledata->column_metadata = malloc(sizeof(sbdf_metadata_head*) * newcap);
			if (!tabledata->column_metadata)
			{
				sbdf_md_destroy(t);
				return SBDF_ERROR_OUT_OF_MEMORY;
			}
		}
	}

	tabledata->column_metadata[tabledata->no_columns++] = t;

	return SBDF_OK;
}

int sbdf_read_metadata_values(FILE* in, sbdf_valuetype vt, sbdf_metadata* out)
{
	int error, v;
	sbdf_object* value;
	sbdf_object* default_value;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	value = default_value = 0;

	if (error = sbdf_read_int8(in, &v))
	{
		return error;
	}

	if (v)
	{
        if (v != 1)
        {
            return SBDF_ERROR_ARRAY_LENGTH_MUST_BE_1;
        }

		if (error = sbdf_obj_read(in, vt, &value))
		{
			return error;
		}
	}

	if (error = sbdf_read_int8(in, &v))
	{
		sbdf_obj_destroy(value);
		return error;
	}

	if (v)
	{
        if (v != 1)
        {
            return SBDF_ERROR_ARRAY_LENGTH_MUST_BE_1;
        }

		if (error = sbdf_obj_read(in, vt, &default_value))
		{
			sbdf_obj_destroy(value);
			return error;
		}
	}

	out->value = value;
	out->default_value = default_value;

	return SBDF_OK;
}

int sbdf_tm_read(FILE* in, sbdf_tablemetadata** out)
{
	sbdf_tablemetadata* t;
	int error;
	int v;
	int count, column_cnt;
	char** metadataname;
	sbdf_valuetype* metadatatype;
	sbdf_object** metadatadefault;
	int metadatacount;
	int i, j;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	error = SBDF_OK;
	t = 0;
	metadataname = 0;
	metadatacount = 0;
	metadatatype = 0;
	metadatadefault = 0;

	if (error = sbdf_sec_expect(in, SBDF_TABLEMETADATA_SECTIONID))
	{
		return error;
	}

	if (error = sbdf_read_int32(in, &count))
	{
		return error;
	}

	if (count < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	t = calloc(1, sizeof(sbdf_tablemetadata));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	if (error = sbdf_md_create(&t->table_metadata))
	{
		free(t);
		return error;
	}

	{
		sbdf_metadata* prev = 0;
		while (count--)
		{
			sbdf_valuetype vt;
			sbdf_metadata* cur;
			char* name;

			name = 0;
			cur = 0;

			if (error = sbdf_read_string(in, &name))
			{
				goto end;
			}

			if (error = sbdf_vt_read(in, &vt))
			{
				sbdf_str_destroy(name);
				goto end;
			}

			cur = calloc(1, sizeof(sbdf_metadata));
			if (!cur)
			{
				error = SBDF_ERROR_OUT_OF_MEMORY;
				sbdf_str_destroy(name);
				goto end;
			}
			cur->name = name;

			if (prev)
			{
				prev->next = cur;
			}
			else
			{
				t->table_metadata->first = cur;
			}

			if (error = sbdf_read_metadata_values(in, vt, cur))
			{
				goto end;
			}

			prev = cur;
		}
	}

	{
		if (error = sbdf_read_int32(in, &column_cnt))
		{
			goto end;
		}

		t->no_columns = column_cnt;
		t->column_metadata = calloc(column_cnt, sizeof(void*));
		if (!t->column_metadata)
		{
			error = SBDF_ERROR_OUT_OF_MEMORY;
			goto end;
		}

		if (error = sbdf_read_int32(in, &metadatacount))
		{
			error = SBDF_ERROR_OUT_OF_MEMORY;
			goto end;
		}
		
		metadataname = calloc(metadatacount, sizeof(void*));
		if (!metadataname)
		{
			error = SBDF_ERROR_OUT_OF_MEMORY;
			goto end;
		}
		metadatatype = calloc(metadatacount, sizeof(sbdf_valuetype));
		if (!metadatatype)
		{
			error = SBDF_ERROR_OUT_OF_MEMORY;
			goto end;
		}

		metadatadefault = calloc(metadatacount, sizeof(sbdf_object*));
		if (!metadatadefault)
		{
			error = SBDF_ERROR_OUT_OF_MEMORY;
			goto end;
		}

		for (i = 0; i < metadatacount; ++i)
		{
			if (error = sbdf_read_string(in, metadataname + i))
			{
				goto end;
			}

			if (error = sbdf_vt_read(in, metadatatype + i))
			{
				goto end;
			}

			if (error = sbdf_read_int8(in, &v))
			{
				goto end;
			}

			if (v)
			{
				if (error = sbdf_obj_read(in, metadatatype[i], metadatadefault + i))
				{
					goto end;
				}
			}
		}

		for (i = 0; i < t->no_columns; ++i)
		{
			if (error = sbdf_md_create(t->column_metadata + i))
			{
				goto end;
			}

			for (j = 0; j < metadatacount; ++j)
			{
				if (error = sbdf_read_int8(in, &v))
				{
					goto end;
				}

				if (v)
				{
					/* we have metadata */
					sbdf_object* value = 0;

					if (error = sbdf_obj_read(in, metadatatype[j], &value))
					{
						goto end;
					}

					/* add it to column i */
					if (error = sbdf_md_add(metadataname[j], value, metadatadefault[j], t->column_metadata[i]))
					{
						sbdf_obj_destroy(value);
						goto end;
					}

					sbdf_obj_destroy(value);
				}
				else
				{
					/* no data */
				}
			}
		}
	}

end: if (error)
	{
		if (t)
		{
			sbdf_tm_destroy(t);
		}
	}
	else
	{
		t->table_metadata->modifiable = 0;
		for (i = 0; i < t->no_columns; ++i)
		{
			t->column_metadata[i]->modifiable = 0;
		}
		*out = t;
	}

	if (metadatatype)
	{
		free(metadatatype);
		metadatatype = 0;
	}

	if (metadataname)
	{
		for (i = 0; i < metadatacount; ++i)
		{
			sbdf_str_destroy(metadataname[i]);
		}
		free(metadataname);
		metadataname = 0;
	}

	if (metadatadefault)
	{
		for (i = 0; i < metadatacount; ++i)
		{
			sbdf_obj_destroy(metadatadefault[i]);
		}
		free(metadatadefault);
		metadatadefault = 0;
	}

	return error;
}

struct metadata_sort
{
	sbdf_metadata const* meta;
	int order;
};

static int compare_metadata_sort_by_order(struct metadata_sort const* lhs, struct metadata_sort const* rhs)
{
	return lhs->order - rhs->order;
}

static int compare_metadata_sort_by_name(struct metadata_sort const* lhs, struct metadata_sort const* rhs)
{
	int name_diff = strcmp(lhs->meta->name, rhs->meta->name);
	if (name_diff)
	{
		return name_diff;
	}
	
	/* use sort order a second parameter so that items with the lowest order come first and are kept when folding */
	return compare_metadata_sort_by_order(lhs, rhs);
}

int sbdf_tm_write(FILE* out, sbdf_tablemetadata const* in)
{
	int error, count;
	sbdf_metadata const* meta;
	struct metadata_sort* array;
	int array_size;
	int array_capacity;
	int i;

	if (!in)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_sec_write(out, SBDF_TABLEMETADATA_SECTIONID))
	{
		return error;
	}

	count = sbdf_md_cnt(in->table_metadata);
	if (error = sbdf_write_int32(out, count))
	{
		return error;
	}

	for (meta = in->table_metadata->first; meta; meta = meta->next)
	{
		if (error = sbdf_write_string(out, meta->name))
		{
			return error;
		}

		if (error = sbdf_vt_write(out, meta->value->type))
		{
			return error;
		}

		if (meta->value)
		{
			if (error = sbdf_write_int8(out, 1))
			{
				return error;
			}
			if (error = sbdf_obj_write(meta->value, out))
			{
				return error;
			}
		}
		else if (error = sbdf_write_int8(out, 0))
		{
			return error;
		}

		if (meta->default_value)
		{
			if (error = sbdf_write_int8(out, 1))
			{
				return error;
			}
			if (error = sbdf_obj_write(meta->default_value, out))
			{
				return error;
			}
		}
		else if (error = sbdf_write_int8(out, 0))
		{
			return error;
		}
	}

	if (error = sbdf_write_int32(out, in->no_columns))
	{
		return error;
	}

	array = 0;
	array_capacity = array_size = 0;

	for (i = 0; i < in->no_columns; ++i)
	{
		for (meta = in->column_metadata[i]->first; meta; meta = meta->next)
		{
			if (array_size == array_capacity)
			{
				array_capacity = sbdf_calculate_array_capacity(1 + array_size);
				if (array)
				{
					array = realloc((struct metadata_sort*)array, sizeof(struct metadata_sort) * array_capacity);
				}
				else
				{
					array = malloc(sizeof(struct metadata_sort) * array_capacity);
				}

				if (!array)
				{
					return SBDF_ERROR_OUT_OF_MEMORY;
				}
			}

			{
				struct metadata_sort item;
				item.meta = meta;
				item.order = array_size;

				array[array_size++] = item;
			}
		}
	}

	qsort((void*)array, array_size, sizeof(struct metadata_sort), compare_metadata_sort_by_name);

	/* fold duplicate values */
	{
		count = 0;
		for (i = 0; i < array_size; ++i)
		{
			if (i == 0 || sbdf_md_compare_names(&array[i - 1].meta, &array[i].meta))
			{
				if (count != i)
				{
					array[count++] = array[i];
				}
				else
				{
					++count;
				}
			}
			else
			{
				/* verify equality of array + i - 1 and array + i */
				if (sbdf_vt_cmp(array[i - 1].meta->value->type, array[i].meta->value->type))
				{
					/* value types differ */
					error = SBDF_ERROR_INCORRECT_METADATA;
					goto end;
				}
				else if (!sbdf_obj_eq(array[i - 1].meta->default_value, array[i].meta->default_value))
				{
					/* default values differ */
					error = SBDF_ERROR_INCORRECT_METADATA;
					goto end;
				}
			}
		}
		array_size = count;
	}

	/* restore original sorting order */
	qsort((void*)array, array_size, sizeof(struct metadata_sort), compare_metadata_sort_by_order);

	if (error = sbdf_write_int32(out, count))
	{
		goto end;
	}

	/* write names, datatypes and default values */
	for (i = 0; i < array_size; ++i)
	{
		if (error = sbdf_write_string(out, array[i].meta->name))
		{
			goto end;
		}
		
		if (error = sbdf_vt_write(out, array[i].meta->value->type))
		{
			goto end;
		}

		if (array[i].meta->default_value)
		{
			if ((error = sbdf_write_int8(out, 1)) || (error = sbdf_obj_write(array[i].meta->default_value, out)))
			{
				goto end;
			}
		}
		else if (error = sbdf_write_int8(out, 0))
		{
			goto end;
		}
	}

	/* write column values */
	for (i = 0; i < in->no_columns; ++i)
	{
		int i2;

		for (i2 = 0; i2 < array_size; ++i2)
		{
			for (meta = in->column_metadata[i]->first; meta; meta = meta->next)
			{
				if (!sbdf_md_compare_names(&array[i2].meta, &meta))
				{
					break;
				}
			}

			if (meta)
			{
				if (error = sbdf_write_int8(out, 1))
				{
					goto end;
				}

				if (error = sbdf_obj_write(meta->value, out))
				{
					return error;
				}
			}
			else if (error = sbdf_write_int8(out, 0))
			{
				goto end;
			}
		}
	}

end: if (array)
	 {
		 free((void*)array);
		 array = 0;
	 }
	return error;
}
