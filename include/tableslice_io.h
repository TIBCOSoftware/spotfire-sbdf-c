/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_43A0C3BD_7495_4f8b_9CF6_AB8B6BA1F48B
#define SBDF_43A0C3BD_7495_4f8b_9CF6_AB8B6BA1F48B

#include <stdio.h>

#include "config.h"
#include "tableslice.h"

#ifdef __cplusplus
extern "C" {
#endif

/* reads a table slice from file. returns SBDF_ERROR_END_OF_TABLE when the end of the table is reached */
SBDF_API int sbdf_ts_read(FILE* f, struct sbdf_tablemetadata const* meta, char* subset, sbdf_tableslice** out);

/* writes a table slice to file */
SBDF_API int sbdf_ts_write(FILE* f, sbdf_tableslice const* in);

/* skips a table slice in file */
SBDF_API int sbdf_ts_skip(FILE* f, struct sbdf_tablemetadata const* meta);

/* writes the end-of-table marker to the file */
SBDF_API int sbdf_ts_write_end(FILE* f);

#ifdef __cplusplus
}
#endif

#endif
