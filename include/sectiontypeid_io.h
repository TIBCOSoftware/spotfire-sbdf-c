/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_B4948A33_FBBC_4da7_A7A1_0603FA504390
#define SBDF_B4948A33_FBBC_4da7_A7A1_0603FA504390

#include <stdio.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* writes the given section type id to file */
SBDF_API int sbdf_sec_write(FILE* f, int id);

/* reads the given section type id from file */
SBDF_API int sbdf_sec_read(FILE* f, int* id);

/* reads the section type id from file. returns error if different from passed id */
SBDF_API int sbdf_sec_expect(FILE* f, int expected_id);

#ifdef __cplusplus
}
#endif

#endif
