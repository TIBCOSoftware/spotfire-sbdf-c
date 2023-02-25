/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_A196DF75_2B6A_4b53_BA72_28485E541C74
#define SBDF_A196DF75_2B6A_4b53_BA72_28485E541C74

#include "config.h"
#include "valuetype.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines a sbdf object, similiar to a .net or java boxed object */
typedef struct sbdf_object
{
	sbdf_valuetype type; /* the type of items in data */
	int count;           /* the number of items in data */
	void* data;          /* the data itself */
} sbdf_object;

/* creates an object based on an array */
/* the data and length arrays are copied and stored internally */
/* lengths are only required for string, text and binary data */
SBDF_API int sbdf_obj_create_arr(sbdf_valuetype type, int count, void const* data, int const* lengths, sbdf_object**);

/* destroy the object and releases all its resources */
SBDF_API void sbdf_obj_destroy(sbdf_object*);

/* initializes an sbdf_object with the supplied information */
/* data and length are copied and stored internally */
SBDF_API int sbdf_obj_create(sbdf_valuetype type, void const* data, int const* length, sbdf_object**);

/* copies and sbdf object */
SBDF_API int sbdf_obj_copy(sbdf_object const* src, sbdf_object** dst);

/* returns 1 if two objects are equal, 0 otherwise */
SBDF_API int sbdf_obj_eq(sbdf_object const* lhs, sbdf_object const* rhs);

#ifdef __cplusplus
}
#endif

#endif
