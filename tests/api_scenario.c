/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "test_c.h"

#include <stdio.h>

#include "all_io.h"

double column_1[] = { 14., 33.5, 96.2 };
int    column_2[] = { 42, 6, 19 };
char*  column_3[] = { "hello", "sbdf", "world" };
int column_3_lengths[] = { 5, 4, 5 }; /* the string lengths, may be omitted */

SBDF_TEST_C(api_scenario, write)
{
	/* note that error handling has been omitted to keep the example brief */
	int error = SBDF_OK;

	/* table metadata information */
	sbdf_metadata_head* table_meta_head = 0;
	sbdf_tablemetadata* table_meta = 0;
	sbdf_metadata_head* col_meta = 0;

	/* table and column slices */
	sbdf_tableslice* table_slice = 0;
	sbdf_columnslice* col_slice_1 = 0;
	sbdf_columnslice* col_slice_2 = 0;
	sbdf_columnslice* col_slice_3 = 0;

	/* value array and object helper */
	sbdf_valuearray* value_array_1 = 0;
	sbdf_valuearray* value_array_2 = 0;
	sbdf_valuearray* value_array_3 = 0;
	sbdf_object* values = 0;

	/* the output file */
	FILE* outfile = 0;

	/* create the table metadata head*/
	error = sbdf_md_create(&table_meta_head);

	/* add a metadata attribute to the table metadata head*/
	error = sbdf_md_add_str("table metadata", "string value", "string default value", table_meta_head);

	/* create the table metadata, storing a copy of the table metadata head */
	error = sbdf_tm_create(table_meta_head, &table_meta);

	/* destroy the table metadata head */
	sbdf_md_destroy(table_meta_head);

	/* create the metadata for the first column */
	error = sbdf_md_create(&col_meta);

	/* set the mandatory attributes for the first column metadata */
	error = sbdf_cm_set_values("first column", sbdf_vt_double(), col_meta);

	/* add the first column to the table metadata, storing a copy of the column metadata */
	error = sbdf_tm_add(col_meta, table_meta);

	/* destroy the first column metadata */
	sbdf_md_destroy(col_meta);

	/* create the metadata for the second column */
	error = sbdf_md_create(&col_meta);

	/* set the mandatory attributes for the second column metadata */
	error = sbdf_cm_set_values("second column", sbdf_vt_int(), col_meta);

	/* add the second column to the table metadata, storing a copy of the column metadata */
	error = sbdf_tm_add(col_meta, table_meta);

	/* destroy the second column metadata */
	sbdf_md_destroy(col_meta);

	/* create the metadata for the third column */
	error = sbdf_md_create(&col_meta);

	/* set the mandatory attributes for the second column metadata */
	error = sbdf_cm_set_values("third column", sbdf_vt_string(), col_meta);

	/* add the third column to the table metadata, storing a copy of the column metadata */
	error = sbdf_tm_add(col_meta, table_meta);

	/* destroy the third column metadata */
	sbdf_md_destroy(col_meta);

	/* create the table slice */
	error = sbdf_ts_create(table_meta, &table_slice);

	/* create the object for the first column */
	error = sbdf_obj_create_arr(sbdf_vt_double(), 3, column_1, 0, &values);

	/* create the value array for the first column, storing a copy of values */
	error = sbdf_va_create_dflt(values, &value_array_1);

	/* destroy the first column object */
	sbdf_obj_destroy(values);

	/* create a column slice for the first column, storing a reference to the value array */
	error = sbdf_cs_create(&col_slice_1, value_array_1);

	/* add the first column slice to the table slice */
	error = sbdf_ts_add(col_slice_1, table_slice);

	/* create the object for the second column */
	error = sbdf_obj_create_arr(sbdf_vt_int(), 3, column_2, 0, &values);

	/* create the value array for the second column, storing a copy of values */
	error = sbdf_va_create_dflt(values, &value_array_2);

	/* destroy the second column object */
	sbdf_obj_destroy(values);

	/* create a column slice for the second column, storing a reference to the value array */
	error = sbdf_cs_create(&col_slice_2, value_array_2);

	/* add the second column slice to the table slice */
	error = sbdf_ts_add(col_slice_2, table_slice);

	/* create the object for the third column */
	/* the lengths may be omitted. strlen will then be used to calculate the lengths */
	error = sbdf_obj_create_arr(sbdf_vt_string(), 3, column_3, column_3_lengths, &values);

	/* create the value array for the third column, storing a copy of values */
	error = sbdf_va_create_dflt(values, &value_array_3);

	/* destroy the third column object */
	sbdf_obj_destroy(values);

	/* create a column slice for the third column, storing a reference to the value array */
	error = sbdf_cs_create(&col_slice_3, value_array_3);

	/* add the third column slice to the table slice */
	error = sbdf_ts_add(col_slice_3, table_slice);

	/* open the out file */
	outfile = fopen("outfile.sbdf", "wb");

	/* write the file header */
	error = sbdf_fh_write_cur(outfile);

	/* write the table metadata */
	error = sbdf_tm_write(outfile, table_meta);

	/* write the table slice */
	error = sbdf_ts_write(outfile, table_slice);

	/* write the end of table marker */
	error = sbdf_ts_write_end(outfile);

	/* close outfile */
	fclose(outfile);

	/* destroy the first column slice */
	sbdf_cs_destroy(col_slice_1);

	/* destroy the second column slice */
	sbdf_cs_destroy(col_slice_2);

	/* destroy the third column slice */
	sbdf_cs_destroy(col_slice_3);

	/* destroy the value array for the first column */
	sbdf_va_destroy(value_array_1);

	/* destroy the value array for the second column */
	sbdf_va_destroy(value_array_2);

	/* destroy the value array for the third column */
	sbdf_va_destroy(value_array_3);

	/* destroy the table slice */
	sbdf_ts_destroy(table_slice);

	/* destroy the table metadata */
	sbdf_tm_destroy(table_meta);

	return 1;
}

SBDF_TEST_C(api_scenario, read)
{
	/* note that error handling has been omitted to keep the example brief */
	int error = SBDF_OK;
	int major_v, minor_v;
	FILE* input = 0;

	/* table metadata and table slice */
	sbdf_tablemetadata* table_meta = 0;
	sbdf_tableslice* table_slice = 0;

	/* iteration variables */
	sbdf_metadata* meta_iter = 0;
	int i = 0;
	int j = 0;

	/* open the sbdf file */
	input = fopen("outfile.sbdf", "rb");
	
	/* read the file header */
	error = sbdf_fh_read(input, &major_v, &minor_v);

	/* examine the version information */
	if (major_v != 1 || minor_v != 0)
	{
		fclose(input);
		return 0; /* unknown version */
	}

	/* read the table metadata */
	error = sbdf_tm_read(input, &table_meta);

	/* parse and display metadata information */
	printf("The table contains %d columns\n", table_meta->no_columns);
	for (meta_iter = table_meta->table_metadata->first; meta_iter; meta_iter = meta_iter->next)
	{
		printf("Table metadata '%s'\n", meta_iter->name);
	}

	for (i = 0; i < table_meta->no_columns; ++i)
	{
		char* col_name;
		sbdf_valuetype col_type;
		sbdf_cm_get_name(table_meta->column_metadata[i], &col_name);
		sbdf_cm_get_type(table_meta->column_metadata[i], &col_type);

		printf("Column %d name %s typeid %d\n", i, col_name, col_type.id);

		sbdf_str_destroy(col_name);

		for (meta_iter = table_meta->column_metadata[i]->first; meta_iter; meta_iter = meta_iter->next)
		{
			printf("Column %d metadata '%s'\n", i, meta_iter->name);
		}
	}

	/* read the table slices */
	while (!error)
	{
		error = sbdf_ts_read(input, table_meta, 0, &table_slice);
		if (error == SBDF_TABLEEND)
		{
			error = SBDF_OK;
			break;
		}

		for (i = 0; i < table_slice->no_columns; ++i)
		{
			sbdf_columnslice* cs = table_slice->columns[i];
			sbdf_object* values = 0;

			/* unpacks the value array values */
			error = sbdf_va_get_values(cs->values, &values);

			printf ("Column %d values ", i);

			for (j = 0; j < values->count; ++j)
			{
				if (values->type.id == SBDF_DOUBLETYPEID)
				{
					printf("%lf ", ((double*)values->data)[j]);
				}
				else if (values->type.id == SBDF_INTTYPEID)
				{
					printf("%d ", ((int*)values->data)[j]);
				}
				if (values->type.id == SBDF_STRINGTYPEID)
				{
					printf("%s ", ((char**)values->data)[j]);
				}
			}

			printf("\n");

			/* destroy the values object */
			sbdf_obj_destroy(values);
		}

		if (!error)
		{
			/* destroy the table slice, including the value arrays */
			sbdf_ts_destroy(table_slice);
		}
	}

	/* destroy the table metadata. this also destroys the column metadata*/
	sbdf_tm_destroy(table_meta);

	/* close the input file */
	fclose(input);

	return 1;
}
