/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_6165FA69_0812_4c2b_B34B_E231F10A9624
#define SBDF_6165FA69_0812_4c2b_B34B_E231F10A9624

#include <stdio.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sbdf_tablemetadata;

/* reads table metadata from file */
SBDF_API int sbdf_tm_read(FILE* in, struct sbdf_tablemetadata** out);

/* writes table metadata to file */
SBDF_API int sbdf_tm_write(FILE* out, struct sbdf_tablemetadata const* in);

#ifdef __cplusplus
}
#endif

#endif
