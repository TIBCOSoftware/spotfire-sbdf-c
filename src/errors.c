/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "errors.h"

char const* sbdf_err_get_str(int error)
{
	switch(error)
	{
	case SBDF_OK:
		return "no error";
	case SBDF_ERROR_ARGUMENT_NULL:
		return "an argument is null";
	case SBDF_ERROR_OUT_OF_MEMORY:
		return "memory exhausted";
	case SBDF_ERROR_UNKNOWN_TYPEID:
		return "unknown typeid";
	case SBDF_ERROR_IO:
		return "i/o error";
	case SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING:
		return "unknown valuearray encoding";
	case SBDF_ERROR_ARRAY_LENGTH_MUST_BE_1:
		return "the array length must be one item";
	case SBDF_ERROR_METADATA_NOT_FOUND:
		return "the metadata with the given name was not found";
	case SBDF_ERROR_METADATA_ALREADY_EXISTS:
		return "the metadata with the given name already exists";
	case SBDF_ERROR_INCORRECT_METADATA:
		return "the metadata is incorrect";
	case SBDF_ERROR_METADATA_READONLY:
		return "the metadata is readonly and may not be modified";
	case SBDF_ERROR_VALUETYPES_MUST_BE_EQUAL:
		return "the valuetypes of the arguments must be equal";
	case SBDF_ERROR_UNEXPECTED_SECTION_ID:
		return "unexpected section id";
	case SBDF_ERROR_PROPERTY_ALREADY_EXISTS:
		return "the property with the given name already exists";
	case SBDF_ERROR_PROPERTY_NOT_FOUND:
		return "the property with the given name wasn't found";
	case SBDF_ERROR_INCORRECT_PROPERTY_TYPE:
		return "the property type is incorrect";
	case SBDF_ERROR_ROW_COUNT_MISMATCH:
		return "the row count of the property doesn't match the row count of the values";
	case SBDF_ERROR_UNKNOWN_VERSION:
		return "unknown SBDF version";
	case SBDF_ERROR_COLUMN_COUNT_MISMATCH:
		return "the number of the columnslice  doesn't match the number of the columns of the metadata";
	case SBDF_ERROR_MAGIC_NUMBER_MISSING:
		return "the SBDF magic number wasn't found";
	case SBDF_ERROR_INVALID_SIZE:
		return "the number of elements is incorrect";
	case SBDF_TABLEEND:
		return "the end of the table was reached";
	}

	return "unknown error";
}
