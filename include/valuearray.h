/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_B868ACF4_9859_4598_AF45_1E763B9019D3
#define SBDF_B868ACF4_9859_4598_AF45_1E763B9019D3

#include "config.h"
#include "valuetype.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

/* encodes the value as a plain array, i.e. no encoding */
#define SBDF_PLAINARRAYENCODINGTYPEID 0x1

/* stores the values run-length encoded */
#define SBDF_RUNLENGTHENCODINGTYPEID 0x2

/* packs bool values into a bit array. no other value types are supported */
#define SBDF_BITARRAYENCODINGTYPEID 0x3

typedef struct sbdf_valuearray sbdf_valuearray;

/* creates a value array from the specified values */
SBDF_API int sbdf_va_create(int arrayencoding, sbdf_object const* array, sbdf_valuearray** result);

/* creates a plain encoded value array */
SBDF_API int sbdf_va_create_plain(sbdf_object const* array, sbdf_valuearray** result);

/* cretes a rle encoded value array */
SBDF_API int sbdf_va_create_rle(sbdf_object const* array, sbdf_valuearray** result);

/* creates a packed bit encoded value array */
SBDF_API int sbdf_va_create_bit(sbdf_object const* array, sbdf_valuearray** result);

/* destroys a valuearray and releases all its resources */
SBDF_API void sbdf_va_destroy(sbdf_valuearray*);

/* creates a default value array encoding */
SBDF_API int sbdf_va_create_dflt(sbdf_object const* array, sbdf_valuearray** result);

/* extracts the values from the array */
SBDF_API int sbdf_va_get_values(sbdf_valuearray* input, sbdf_object** result);

/* returns the number of rows stored in the value array */
SBDF_API int sbdf_va_row_cnt(sbdf_valuearray* in);

#ifdef __cplusplus
}
#endif

#endif
