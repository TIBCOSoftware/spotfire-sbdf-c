/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_2EA19C3D_A2F6_444d_9CDF_BDFD13A27EDE
#define SBDF_2EA19C3D_A2F6_444d_9CDF_BDFD13A27EDE

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* returns textua description of the error */
SBDF_API char const* sbdf_err_get_str(int error);

#define SBDF_OK 0

#define SBDF_ERROR_ARGUMENT_NULL -1
#define SBDF_ERROR_OUT_OF_MEMORY -2
#define SBDF_ERROR_UNKNOWN_TYPEID -3
#define SBDF_ERROR_IO -4
#define SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING -5
#define SBDF_ERROR_ARRAY_LENGTH_MUST_BE_1 -6
#define SBDF_ERROR_METADATA_NOT_FOUND -7
#define SBDF_ERROR_METADATA_ALREADY_EXISTS -8
#define SBDF_ERROR_INCORRECT_METADATA -9
#define SBDF_ERROR_METADATA_READONLY -10
#define SBDF_ERROR_INCORRECT_COLUMNMETADATA -11
#define SBDF_ERROR_VALUETYPES_MUST_BE_EQUAL -12
#define SBDF_ERROR_UNEXPECTED_SECTION_ID -13
#define SBDF_ERROR_PROPERTY_ALREADY_EXISTS -14
#define SBDF_ERROR_PROPERTY_NOT_FOUND -15
#define SBDF_ERROR_INCORRECT_PROPERTY_TYPE -16
#define SBDF_ERROR_ROW_COUNT_MISMATCH -17
#define SBDF_ERROR_UNKNOWN_VERSION -18
#define SBDF_ERROR_COLUMN_COUNT_MISMATCH -19
#define SBDF_ERROR_MAGIC_NUMBER_MISSING -20
#define SBDF_ERROR_INVALID_SIZE -21

#define SBDF_TABLEEND -1000 /* marks the end of a table and is in some cases not an error */

#define SBDF_ERROR_UNKNOWN_ERROR -32767

#ifdef __cplusplus
}
#endif

#endif
