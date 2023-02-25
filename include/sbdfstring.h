/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_AB39855C_228E_400a_8546_C0F159676559
#define SBDF_AB39855C_228E_400a_8546_C0F159676559

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sbdf strings are normal null-terminated strings, preceeded by a length int, so that embedded nulls are possible */

/* creates an sbdf string from the input */
SBDF_API char* sbdf_str_create(char const* str);

/* creates an sbdf string with the given length */
SBDF_API char* sbdf_str_create_len(char const* str, int len);

/* destroys the given sbdf string, releasing all its resources */
SBDF_API void sbdf_str_destroy(char* str);

/* returns the length of the sbdf input string */
SBDF_API int sbdf_str_len(char const* str);

/* compares the two strings. returns negative number if lhs < rhs, positive number if lhs > rhs and 0 if lhs equals rhs */
SBDF_API int sbdf_str_cmp(char const* lhs, char const* rhs);

/* copies the input string */
SBDF_API char* sbdf_str_copy(char const* inp);

/* converts an utf8 string to iso8859-1 */
/* pass null as out to calculate the required length of the output buffer */
/* returns the number of characters needed in or written to the output buffer */
SBDF_API int sbdf_convert_utf8_to_iso88591(char const* inp, char* out);

/* converts an iso8859-1 string to utf8 */
/* pass null as out to calculate the required length of the output buffer */
/* returns the number of characters needed in or written to the output buffer */
SBDF_API int sbdf_convert_iso88591_to_utf8(char const* inp, char* out);

#ifdef __cplusplus
}
#endif

#endif
