/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_E6433394_E292_4f24_B860_D0DA429E7A00
#define SBDF_E6433394_E292_4f24_B860_D0DA429E7A00

#include <stdio.h>

#include "config.h"
#include "valuearray.h"

#ifdef __cplusplus
extern "C" {
#endif

/* reads the value array from the current file position */
SBDF_API int sbdf_va_read(FILE* file, sbdf_valuearray** handle);

/* writes the value array to the current file position */
SBDF_API int sbdf_va_write(sbdf_valuearray* handle, FILE* file);

/* skips the value array at the current file position */
SBDF_API int sbdf_va_skip(FILE* file);

#ifdef __cplusplus
}
#endif

#endif
