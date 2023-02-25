/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "fileheader.h"
#include "errors.h"
#include "sectiontypeid.h"
#include "sectiontypeid_io.h"
#include "internals.h"

int sbdf_fh_write_cur(FILE* f)
{
	int error;

	if (error = sbdf_sec_write(f, SBDF_FILEHEADER_SECTIONID))
	{
		return error;
	}

	if (error = sbdf_write_int8(f, SBDF_MAJOR_VERSION))
	{
		return error;
	}

	if (error = sbdf_write_int8(f, SBDF_MINOR_VERSION))
	{
		return error;
	}

	return SBDF_OK;
}

int sbdf_fh_read(FILE* f, int* major, int* minor)
{
	int error, imajor, iminor;

	if (!major || !minor)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_sec_expect(f, SBDF_FILEHEADER_SECTIONID))
	{
		return error;
	}

	if (error = sbdf_read_int8(f, &imajor))
	{
		return error;
	}

	if (error = sbdf_read_int8(f, &iminor))
	{
		return error;
	}

	*major = imajor;
	*minor = iminor;

	return SBDF_OK;
}

int sbdf_sec_write(FILE* f, int id)
{
	int error;

	if (error = sbdf_write_int8(f, 0xdf))
	{
		return error;
	}

	if (error = sbdf_write_int8(f, 0x5b))
	{
		return error;
	}

	return sbdf_write_int8(f, id);
}

int sbdf_sec_read(FILE* f, int* id)
{
	int v, error;

	if (error = sbdf_read_int8(f, &v))
	{
		return error;
	}
	if (v != 0xdf)
	{
		return SBDF_ERROR_MAGIC_NUMBER_MISSING;
	}

	if (error = sbdf_read_int8(f, &v))
	{
		return error;
	}
	if (v != 0x5b)
	{
		return SBDF_ERROR_MAGIC_NUMBER_MISSING;
	}

	return sbdf_read_int8(f, id);
}

int sbdf_sec_expect(FILE* f, int id)
{
	int v, error;

	if (error = sbdf_sec_read(f, &v))
	{
		return error;
	}

	if (v != id)
	{
		return SBDF_ERROR_UNEXPECTED_SECTION_ID;
	}

	return SBDF_OK;
}
