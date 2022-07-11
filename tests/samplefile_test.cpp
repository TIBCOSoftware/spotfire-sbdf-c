/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "test.h"

#include <vector>
#include <string>
#include <algorithm>

#include "all_io.h"

#if _MSC_VER
#include <windows.h>

static std::vector<std::string> get_sample_files()
{
	std::vector<std::string> result;
	std::string const prefix = TESTSDIR "\\samples";
	WIN32_FIND_DATAA t;

	HANDLE hdl = FindFirstFileA((prefix + "*.sbdf").c_str(), &t);
	if (hdl != INVALID_HANDLE_VALUE)
	{
		do
		{
			result.push_back(prefix + t.cFileName);
		}
		while (FindNextFileA(hdl, &t));
		FindClose(hdl);
	}

	return result;
}
#else
#include <sys/types.h>
#include <dirent.h>

static std::vector<std::string> get_sample_files()
{
	std::vector<std::string> result;
	std::string const prefix = TESTSDIR "/samples";

    DIR *dir = opendir (prefix.c_str());

	while (struct dirent* dp = readdir (dir))
	{
		std::string nm = dp->d_name;
		if (nm.size() > 5 && nm.substr(nm.size() - 5, 5) == ".sbdf")
		{
			result.push_back(prefix + "/" + nm);
		}
    }
    closedir (dir);

    return result;
}
#endif

static void compare(FILE* f1, FILE* f2)
{
	fseek(f1, 0, SEEK_END);
	int l1 = ftell(f1);
	fseek(f1, 0, SEEK_SET);
	fseek(f2, 0, SEEK_END);
	int l2 = ftell(f2);
	fseek(f2, 0, SEEK_SET);
	std::vector<unsigned char> v1, v2;
	v1.resize(l1);
	v2.resize(l2);
	fread(&v1[0], 1, l1, f1);
	fread(&v2[0], 1, l2, f2);
	
	for (int i = 0; i < (std::max)(l1, l2); ++i)
	{
		if (i >= l1 || i >= l2 || v1[i] != v2[i])
		{
			FILE* outf = fopen("error.sbdf", "wb");
			fwrite(&v2[0], 1, l2, outf);
			fclose(outf);
			char msgbuf[1024];
			sprintf(msgbuf, "%s(%d): files differ at position %d\n", __FILE__, __LINE__, i);
			throw std::runtime_error(msgbuf);
		}
	}
}

static void test_one(char const* fn)
{
	printf("Testing %s\n", fn);
	tmpf outf;
	int error = SBDF_OK;
	int major_v, minor_v;
	FILE* input = 0;
	FILE* output = outf.write();

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

	/* write file header */
	SBDF_CHECK(sbdf_fh_write_cur(output), SBDF_OK);

	/* read the table metadata */
	SBDF_CHECK(sbdf_tm_read(input, &table_meta), SBDF_OK);

	/* write the table metadata */
	SBDF_CHECK(sbdf_tm_write(output, table_meta), SBDF_OK);

	error = SBDF_OK;

	/* read the table slices */
	while (!error)
	{
		sbdf_tableslice* new_ts;
		error = sbdf_ts_read(input, table_meta, 0, &table_slice);
		if (error == SBDF_TABLEEND)
		{
			error = SBDF_OK;
			break;
		}

		sbdf_ts_create(table_meta, &new_ts);

		for (i = 0; i < table_slice->no_columns; ++i)
		{
			sbdf_columnslice* cs = table_slice->columns[i];
			sbdf_object* values = 0;

			/* unpacks the value array values */
			error = sbdf_va_get_values(cs->values, &values);

			sbdf_columnslice* new_slice = 0;
			sbdf_valuearray* new_values;

			sbdf_va_create_dflt(values, &new_values);
			sbdf_cs_create(&new_slice, new_values);
	
			/* internal use only. makes sure that all data referenced by new_slice gets automatically disposed. */
			new_slice->owned = true;

			// replaced, error, missing

			for (int prop = 0; prop < 3; ++prop)
			{
				static char const* props[] = { SBDF_ISINVALID_VALUEPROPERTY, SBDF_ERRORCODE_VALUEPROPERTY, SBDF_REPLACEDVALUE_VALUEPROPERTY };
				sbdf_valuearray* va;
				sbdf_valuearray* va2;
				sbdf_object* pobj;

				if (SBDF_OK == sbdf_cs_get_property(cs, props[prop], &va))
				{
					/* unpacks the value array values */
					error = sbdf_va_get_values(va, &pobj);

					// creates a new value array
					sbdf_va_create_dflt(pobj, &va2);

					// adds the property
					sbdf_cs_add_property(new_slice, props[prop], va2);	

					// destroys the values
					sbdf_obj_destroy(pobj);
				}
			}

			sbdf_ts_add(new_slice, new_ts);

			/* destroy the values object */
			sbdf_obj_destroy(values);
		}

		if (!error)
		{
			/* destroy the table slice, including the value arrays */
			sbdf_ts_destroy(table_slice);
		}

		/* write new table slice */
		SBDF_CHECK(sbdf_ts_write(output, new_ts), SBDF_OK);

		/* internal use only. makes sure that all data referenced by new_ts gets automatically disposed. */
		new_ts->owned = true;

		sbdf_ts_destroy(new_ts);
	}

	/* destroy the table metadata. this also destroys the column metadata*/
	sbdf_tm_destroy(table_meta);

	/* close the input file */
	fseek(input, 0, SEEK_SET);
	
	error = sbdf_ts_write_end(output);

	compare(input, outf.read());

	fclose(input);
}

SBDF_TEST(samplefile, test)
{
	std::vector<std::string> files = get_sample_files();

	for (std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
	{
		test_one(iter->c_str());
	}
}
