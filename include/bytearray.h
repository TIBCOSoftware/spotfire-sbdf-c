/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_8D935E9F_C8A9_4176_8359_5DFB8AEDEBC7
#define SBDF_8D935E9F_C8A9_4176_8359_5DFB8AEDEBC7

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sbdf arrays are normal arrays of unsigned char, preceeded by a length int */

/* create a byte array with the data at ba, length len */
SBDF_API unsigned char* sbdf_ba_create(unsigned char const* ba, int len);

/* releases the memory previously allocatd by sbdf_ba_create */
SBDF_API void sbdf_ba_destroy(unsigned char* str);

/* gets the length of the byte array passed as parameter */
SBDF_API int sbdf_ba_get_len(unsigned char const* ba);

/* compares two byte arrays. returns a negative number if lhs < rhs, 
  a positive number if rhs < lhs and 0 if lhs is equal to rhs */
SBDF_API int sbdf_ba_memcmp(unsigned char const* lhs, unsigned char const* rhs);

#ifdef __cplusplus
}
#endif


#endif
