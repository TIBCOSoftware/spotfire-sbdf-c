/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_8A29F33B_0E09_4e25_AF6C_79D1C4EEA9E2
#define SBDF_8A29F33B_0E09_4e25_AF6C_79D1C4EEA9E2

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines a table slice structure */
typedef struct sbdf_tableslice
{
	struct sbdf_tablemetadata* table_metadata; /* a reference to the table metadata */
	int no_columns;                            /* the number of columns */
	struct sbdf_columnslice** columns;         /* the column slices */
	int owned;                                 /* internal use, determines if this structure owns the columnslices or not. */
} sbdf_tableslice;

/* creates a table slice, storing a reference to the table metadata */
SBDF_API int sbdf_ts_create(struct sbdf_tablemetadata* head, struct sbdf_tableslice** out);

/* adds a column slice reference to the table slice */
SBDF_API int sbdf_ts_add(struct sbdf_columnslice* col, struct sbdf_tableslice* table);

/* destroys the table slice */
SBDF_API void sbdf_ts_destroy(struct sbdf_tableslice* slice);

#ifdef __cplusplus
}
#endif

#endif
