/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_23BEAEA3_6554_4fe0_A696_4999A8BBC159
#define SBDF_23BEAEA3_6554_4fe0_A696_4999A8BBC159

#include <stdio.h>

#include "config.h"
#include "columnslice.h"

#ifdef __cplusplus
extern "C" {
#endif

/* reads a value array from file */
SBDF_API int sbdf_cs_read(FILE* file, sbdf_columnslice** out);

/* writes a value array to file */
SBDF_API int sbdf_cs_write(FILE* file, sbdf_columnslice const* in);

/* skips a value array in the file */
SBDF_API int sbdf_cs_skip(FILE*);

#ifdef __cplusplus
}
#endif

#endif
