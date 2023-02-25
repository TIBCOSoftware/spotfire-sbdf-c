/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_5D89C681_7201_4e40_B296_9E785E86F8B3
#define SBDF_5D89C681_7201_4e40_B296_9E785E86F8B3

#include "config.h"
#include "valuetype.h"

/* the opaque type used to represent a collection of sbdf metadata */
struct sbdf_metadata_head;

#ifdef __cplusplus
extern "C" {
#endif

/* sets the column metadata values name and data type for the previously allocated metadata head */
SBDF_API int sbdf_cm_set_values(char const* column_name, sbdf_valuetype data_type, struct sbdf_metadata_head* out);

/* gets the value type of the column metadata */
SBDF_API int sbdf_cm_get_type(struct sbdf_metadata_head* inp, struct sbdf_valuetype* out);

/* gets the name of the column metadata */
/* out must be disposed by calling sbdf_str_destroy */
SBDF_API int sbdf_cm_get_name(struct sbdf_metadata_head* inp, char** out);

/* the name of the standard column metadata property name */
#define SBDF_COLUMNMETADATA_NAME "Name"

/* the name of the standard column metadata property datatype */
#define SBDF_COLUMNMETADATA_DATATYPE "DataType"

#ifdef __cplusplus
}
#endif

#endif
