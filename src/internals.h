/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_D0C64852_66B8_4831_9B1F_ACDDF7C3A696
#define SBDF_D0C64852_66B8_4831_9B1F_ACDDF7C3A696

#include <stdio.h>

#include "valuetype.h"
#include "config.h"

#define SBDF_BYTETYPEID 0xfe /* only used internally */

#ifdef __cplusplus
extern "C" {
#endif

struct sbdf_metadata;
struct sbdf_columnmetadata;
struct sbdf_tablemetadata;
struct sbdf_object;

/* returns the unpacked byte size (in memory) of an sbdf valuetype */
extern int sbdf_get_unpacked_size(sbdf_valuetype t);

/* returns the packed byte size (on disk) of an sbdf valuetype */
extern int sbdf_get_packed_size(sbdf_valuetype t);

/* reads a little endian integer from f */
extern int sbdf_read_int32(FILE* f, int* v);

/* writes a little endian integer to f */
extern int sbdf_write_int32(FILE* f, int v);

/* reads a packed little endian integer from f */
/* the lower 7 bits of a byte are used for values */
/* if the high bit is set, the previous value is shifted left and the next byte is read too */
extern int sbdf_read_7bitpacked_int32(FILE* f, int* v);

/* writes a packed little endian integer to f */
extern int sbdf_write_7bitpacked_int32(FILE* f, int v);

/* calculates the byte length of a packed int */
extern int sbdf_get_7bitpacked_len(int v);

/* reads an 8-bit int from f */
extern int sbdf_read_int8(FILE* f, int* v);

/* writes an 8-bit int to f */
extern int sbdf_write_int8(FILE* f, int v);

/* writes a string to f */
extern int sbdf_write_string(FILE* f, char const* s);

/* reads a string from f */
extern int sbdf_read_string(FILE* f, char** s);

/* skips the next string in f */
extern int sbdf_skip_string(FILE* f);

/* convers a valuetype to an object */
extern int sbdf_valuetype_to_object(struct sbdf_valuetype vt, struct sbdf_object** out);

/* determines the necessary array capacaity given the array size */
extern int sbdf_calculate_array_capacity(int size);

/* determines the length of an array in bytes */
extern int sbdf_get_array_length(void const* array);

/* disposes the array */
extern void sbdf_dispose_array(void* array);

/* allocates an array of the given length */
extern void* sbdf_allocate_array(int length);

/* copies the src array */
extern void* sbdf_copy_array(void const* src);

/* creates an array, storing references to the data */
extern int sbdf_init_array_dontclone(struct sbdf_valuetype type, int count, void const* data, struct sbdf_object** out);

/* allocates / reallocates the given amount of memory */
extern int sbdf_alloc(void** inout, int sz);

/* determines if the type index is an array type index */
extern int sbdf_ti_is_arr(int);

#ifdef __cplusplus
}
#endif

#endif
