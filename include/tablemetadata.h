/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_B60A991E_2A0F_456e_8C4D_9C6646DD036D
#define SBDF_B60A991E_2A0F_456e_8C4D_9C6646DD036D

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines the opaque metadata head element */
struct sbdf_metadata_head;

/* defines a table metadata element */
typedef struct sbdf_tablemetadata
{
	struct sbdf_metadata_head* table_metadata;   /* contains the table specific information */
	int no_columns;                              /* the number of columns */
	struct sbdf_metadata_head** column_metadata; /* contains the column specific information */
} sbdf_tablemetadata;

/* creates the table metadata, storing a copy the given table information */
SBDF_API int sbdf_tm_create(struct sbdf_metadata_head* table_metadata, sbdf_tablemetadata** out);

/* destroys the table metadata */
SBDF_API void sbdf_tm_destroy(sbdf_tablemetadata*);

/* adds column metadata to the table metadata */
SBDF_API int sbdf_tm_add(struct sbdf_metadata_head* columndata, sbdf_tablemetadata* tabledata);

#ifdef __cplusplus
}
#endif

#endif
