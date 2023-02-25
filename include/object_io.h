/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_3F38DBAA_F1F7_4aa7_84B7_442B67D53183
#define SBDF_3F38DBAA_F1F7_4aa7_84B7_442B67D53183

#include <stdio.h>

#include "config.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

/* reads an array object with the given value type from the specified file */
SBDF_API int sbdf_obj_read_arr(FILE*, sbdf_valuetype, sbdf_object**);

/* writes the array information to the specified file. valuetype information is not written */
SBDF_API int sbdf_obj_write_arr(sbdf_object const*, FILE*);

/* skips an array with the given valuetype*/
SBDF_API int sbdf_obj_skip_arr(FILE*, sbdf_valuetype);

/* reads an object with the given valuetype from the file. */
SBDF_API int sbdf_obj_read(FILE*, sbdf_valuetype, sbdf_object**);

/* writes the object to the specified file. valuetype information is not written */
SBDF_API int sbdf_obj_write(sbdf_object const*, FILE*);

/* skips an object with the given valuetype */
SBDF_API int sbdf_obj_skip(FILE*, sbdf_valuetype);

#ifdef __cplusplus
}
#endif

#endif
