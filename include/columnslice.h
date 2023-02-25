/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_1A3C7707_7626_4ceb_A6B2_FC6A3FD09DE9
#define SBDF_1A3C7707_7626_4ceb_A6B2_FC6A3FD09DE9

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the name of the standard valueproperty IsInvalid. the type of the property is bool (char) */
#define SBDF_ISINVALID_VALUEPROPERTY "IsInvalid"

/* the name of the standard valueproperty errorcode. the type of the property is string */
#define SBDF_ERRORCODE_VALUEPROPERTY "ErrorCode"

/* the name of the standard valueproperty replacedvalue. the type of the property is string */
#define SBDF_REPLACEDVALUE_VALUEPROPERTY "HasReplacedValue" /* type boolean (char) */

/* defines the type used to represent a sbdf column slice */
typedef struct sbdf_columnslice
{
	struct sbdf_valuearray* values;      /* the actual values of this column */
	int prop_cnt;                        /* the number of value properties */
	char** property_names;               /* the names of the properties */
	struct sbdf_valuearray** properties; /* the properties */
	int owned;                           /* internal use, determines if this structure owns the valuearrays */
} sbdf_columnslice;

/* creates a column slice and stores a reference to the valuearray in it */
SBDF_API int sbdf_cs_create(sbdf_columnslice** out, struct sbdf_valuearray* values);

/* stores a named value property reference in the given column slice */
SBDF_API int sbdf_cs_add_property(sbdf_columnslice* out, char const* name, struct sbdf_valuearray* values);

/* gets a value property reference with the given name */
SBDF_API int sbdf_cs_get_property(sbdf_columnslice* in, char const* name, struct sbdf_valuearray** out);

/* gets the number of rows of the values in the column slice */
SBDF_API int sbdf_cs_row_cnt(sbdf_columnslice* in);

/* destroy the column slice cs and the owned value properties */
SBDF_API void sbdf_cs_destroy(sbdf_columnslice* cs);

/* destroys the column slice cs and the value arrays too. internal use only */
SBDF_API void sbdf_cs_destroy_all(sbdf_columnslice* cs);

#ifdef __cplusplus
}
#endif

#endif
