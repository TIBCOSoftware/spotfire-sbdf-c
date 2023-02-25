/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_524C0E74_A0A2_4a6b_956C_B58584C155E1
#define SBDF_524C0E74_A0A2_4a6b_956C_B58584C155E1

#include <stdio.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SBDF_MAJOR_VERSION 1
#define SBDF_MINOR_VERSION 0

#define SBDF_VERSION_STRING "1.0"

/* writes the current sbdf fileheader to file */
SBDF_API int sbdf_fh_write_cur(FILE*);

/* reads the sbdf fileheader from file */
SBDF_API int sbdf_fh_read(FILE*, int* major, int* minor);

#ifdef __cplusplus
}
#endif

#endif
