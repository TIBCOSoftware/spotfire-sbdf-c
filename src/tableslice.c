/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "tableslice.h"

#include <stdlib.h>

#include "columnslice.h"
#include "columnslice_io.h"
#include "tableslice_io.h"
#include "metadata.h"
#include "tablemetadata.h"
#include "errors.h"
#include "sectiontypeid.h"
#include "sectiontypeid_io.h"

#include "internals.h"

int sbdf_ts_create(sbdf_tablemetadata* head, sbdf_tableslice** out)
{
	sbdf_tableslice* t;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(1, sizeof(sbdf_tableslice));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->table_metadata = head;
	t->no_columns = 0;
	t->owned = 0;
	t->columns = 0;

	*out = t;

	return SBDF_OK;
}

int sbdf_ts_add(sbdf_columnslice* col, sbdf_tableslice* table)
{
	int cap, error;

	if (!col || !table)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	cap = sbdf_calculate_array_capacity(table->no_columns);

	if (cap == table->no_columns)
	{
		int new_cap = sbdf_calculate_array_capacity(1 + table->no_columns);
		if (error = sbdf_alloc((void**)&table->columns, new_cap * sizeof(void*)))
		{
			return error;
		}
	}

	table->columns[table->no_columns++] = col;

	return SBDF_OK;
}

void sbdf_ts_destroy(sbdf_tableslice* slice)
{
	if (slice)
	{
		if (slice->owned)
		{
			/* the metadata is never owned */
			if (slice->columns)
			{
				int i;
				for (i = 0; i < slice->no_columns; ++i)
				{
					sbdf_cs_destroy(slice->columns[i]);
				}
			}
		}

		if (slice->columns)
		{
			free(slice->columns);
		}

		free(slice);
	}
}

int sbdf_ts_read(FILE* f, sbdf_tablemetadata const* meta, char* subset, sbdf_tableslice** out)
{
	sbdf_tableslice* t;
	int error, v, i, column_count;

	if (!meta || !out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_sec_read(f, &v))
	{
		return error;
	}

	if (v == SBDF_TABLEEND_SECTIONID)
	{
		return SBDF_TABLEEND;
	}
	else if (v != SBDF_TABLESLICE_SECTIONID)
	{
		return SBDF_ERROR_UNEXPECTED_SECTION_ID;
	}

	if (error = sbdf_read_int32(f, &column_count))
	{
		return error;
	}

	if (column_count < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	if (column_count != meta->no_columns)
	{
		return SBDF_ERROR_COLUMN_COUNT_MISMATCH;
	}

	t = calloc(1, sizeof(sbdf_tableslice));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->columns = calloc(column_count, sizeof(void*));
	if (!t->columns)
	{
		free(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->owned = 1;
	t->table_metadata = (sbdf_tablemetadata*)meta;
	t->no_columns = column_count;

	for (i = 0; i < t->no_columns; ++i)
	{
		if (!subset || subset[i])
		{
			error = sbdf_cs_read(f, t->columns + i);
		}
		else
		{
			error = sbdf_cs_skip(f);
		}

		if (error)
		{
			sbdf_ts_destroy(t);
			return error;
		}
	}

	*out = t;

	return SBDF_OK;
}

int sbdf_ts_write(FILE* f, sbdf_tableslice const* slice)
{
	int error, i;

	if (!slice)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	error = sbdf_sec_write(f, SBDF_TABLESLICE_SECTIONID);
	if (error)
	{
		return error;
	}
	error = sbdf_write_int32(f, slice->no_columns);
	if (error)
	{
		return error;
	}

	for (i = 0; i < slice->no_columns; ++i)
	{
		error = sbdf_cs_write(f, slice->columns[i]);
		if (error)
		{
			return error;
		}
	}

	return SBDF_OK;
}

int sbdf_ts_skip(FILE* f, sbdf_tablemetadata const* meta)
{
	int error;
	sbdf_tableslice* slice;
	char* subset;

	if (!meta)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	subset = calloc(meta->no_columns, 1);
	if (!subset)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	if (error = sbdf_ts_read(f, meta, subset, &slice))
	{
		free(subset);
		return error;
	}

	sbdf_ts_destroy(slice);
	free(subset);

	return SBDF_OK;
}

int sbdf_ts_write_end(FILE* f)
{
	return sbdf_sec_write(f, SBDF_TABLEEND_SECTIONID);
}
