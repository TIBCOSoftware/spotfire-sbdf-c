/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_BAB4E975_39D1_460e_9E7A_A67E996EA3B9
#define SBDF_BAB4E975_39D1_460e_9E7A_A67E996EA3B9

#include <stdio.h>

#include "config.h"
#include "valuetype.h"

#ifdef __cplusplus
extern "C" {
#endif

/* writes a valuetype to the current file position */
SBDF_API int sbdf_vt_write(FILE*, sbdf_valuetype);

/* reads a valuetype from the current file position */
SBDF_API int sbdf_vt_read(FILE*, sbdf_valuetype*);

#ifdef __cplusplus
}
#endif

#endif
