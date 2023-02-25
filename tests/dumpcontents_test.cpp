/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "test.h"

#include "all_io.h"

void dumpcontents(char const* fn, FILE* dest)
{
	printf("Dumping contents of %s\n", fn);
	int error = SBDF_OK;
	int major_v, minor_v;
	FILE* input = 0;

	/* table metadata and table slice */
	sbdf_tablemetadata* table_meta = 0;
	sbdf_tableslice* table_slice = 0;

	/* iteration variables */
	int i = 0;

	/* open the sbdf file */
	input = fopen(fn, "rb");
	
	/* read the file header */
	error = sbdf_fh_read(input, &major_v, &minor_v);

	/* examine the version information */
	if (major_v != 1 || minor_v != 0)
	{
		fclose(input);
		return; /* unknown version */
	}

	/* read the table metadata */
	error = sbdf_tm_read(input, &table_meta);

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
			fprintf(dest, "Column %d\n", i);
			sbdf_columnslice* cs = table_slice->columns[i];
			sbdf_object* values = 0;

			/* unpacks the value array values */
			error = sbdf_va_get_values(cs->values, &values);
			fprintf(dest, "Values: ");
			
			print_object(dest, values);

			// replaced, error, missing

			for (int prop = 0; prop < 3; ++prop)
			{
				static char const* props[] = { SBDF_ISINVALID_VALUEPROPERTY, SBDF_ERRORCODE_VALUEPROPERTY, SBDF_REPLACEDVALUE_VALUEPROPERTY };
				sbdf_valuearray* va;
				sbdf_object* pobj;

				if (SBDF_OK == sbdf_cs_get_property(cs, props[prop], &va))
				{
					/* unpacks the value array values */
					error = sbdf_va_get_values(va, &pobj);

					fprintf(dest, "%s: ", props[prop]);

					print_object(dest, pobj);

					// destroys the values
					sbdf_obj_destroy(pobj);
				}
			}

			/* destroy the values object */
			sbdf_obj_destroy(values);
		}

		sbdf_ts_destroy(table_slice);
	}

	/* destroy the table metadata. this also destroys the column metadata*/
	sbdf_tm_destroy(table_meta);

	fclose(input);
}

SBDF_TEST(dumpcontents, test)
{
	FILE* dest = fopen("dump file.txt", "w");
	dumpcontents(TESTSDIR "/samples/100.sbdf", dest);
	fclose(dest);
}
