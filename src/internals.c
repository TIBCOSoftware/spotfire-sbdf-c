/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "internals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "valuetypeid.h"
#include "errors.h"
#include "object.h"
#include "sbdfstring.h"
#include "bytearray.h"
#include "bswap.h"

static int get_packed_digits_size(int digits)
{
	if (digits < 3)
	{
		return 1;
	}
	if (digits < 5)
	{
		return 2;
	}
	if (digits < 10)
	{
		return 4;
	}
	return 8;
}

int sbdf_get_unpacked_size(sbdf_valuetype t)
{
	switch (t.id)
	{
	case SBDF_BYTETYPEID:
		return 1;
	case SBDF_FLOATTYPEID:
		return 4;
	case SBDF_DOUBLETYPEID:
		return 8;
	case SBDF_DATETIMETYPEID:
		return 8;
	case SBDF_DATETYPEID:
		return 8;
	case SBDF_TIMETYPEID:
		return 8;
	case SBDF_TIMESPANTYPEID:
		return 8;

	case SBDF_STRINGTYPEID:
		return 0; /* size is dynamic */
	case SBDF_BINARYTYPEID:
		return 0; /* size is dynamic */

	case SBDF_DECIMALTYPEID:
		return 8 * 2;

	case SBDF_BOOLTYPEID:
		return 1;
	case SBDF_INTTYPEID:
		return 4;
	case SBDF_LONGTYPEID:
		return 8;
	}

	return SBDF_ERROR_UNKNOWN_TYPEID;
}

int sbdf_get_packed_size(sbdf_valuetype t)
{
	return sbdf_get_unpacked_size(t);
}

int sbdf_read_int32(FILE* f, int* v)
{
	if (!f || !v)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (fread(v, sizeof(int), 1, f) != 1)
	{
		return SBDF_ERROR_IO;
	}

	sbdf_swap(v, sizeof(int), 1);

	return SBDF_OK;
}

int sbdf_write_int32(FILE* f, int v)
{
	if (!f)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	sbdf_swap(&v, sizeof(int), 1);

	if (fwrite(&v, sizeof(int), 1, f) != 1)
	{
		return SBDF_ERROR_IO;
	}

	return SBDF_OK;
}

int sbdf_read_int8(FILE* f, int* v)
{
	unsigned char c;

	if (!f || !v)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (fread(&c, sizeof(char), 1, f) != 1)
	{
		return SBDF_ERROR_IO;
	}

	*v = c;

	return SBDF_OK;
}

int sbdf_write_int8(FILE* f, int v)
{
	unsigned char c = v;

	if (!f)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (fwrite(&c, sizeof(char), 1, f) != 1)
	{
		return SBDF_ERROR_IO;
	}

	return SBDF_OK;
}

int sbdf_write_string(FILE* f, char const* s)
{
	int error;
	int l;
	
	if (!s)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	l = sbdf_str_len(s);
	if (error = sbdf_write_int32(f, l))
	{
		return error;
	}
	if (fwrite(s, 1, (size_t)l, f) != (size_t)l)
	{
		return SBDF_ERROR_IO;
	}

	return SBDF_OK;
}

int sbdf_read_string(FILE* f, char** s)
{
	int error;
	int l;
	char* t;
	
	if (!s)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_read_int32(f, &l))
	{
		return error;
	}

	if (l < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	if (!(t = sbdf_str_create_len(0, l)))
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	if (fread(t, 1, (size_t)l, f) != (size_t)l)
	{
		sbdf_str_destroy(t);
		return SBDF_ERROR_IO;
	}

	t[l] = 0;

	*s = t;

	return SBDF_OK;
}

int sbdf_skip_string(FILE* f)
{
	int l, error;

	if (error = sbdf_read_int32(f, &l))
	{
		return error;
	}

	if (l < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	if (fseek(f, l, SEEK_CUR))
	{
		return SBDF_ERROR_IO;
	}

	return SBDF_OK;
}

int sbdf_valuetype_to_object(sbdf_valuetype vt, sbdf_object** out)
{
	sbdf_object* t;
	int len;
	unsigned char* outptr;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(sizeof(sbdf_object), 1);
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	len = 1;

	t->data = malloc(sizeof(void*));
	if (!t->data)
	{
		free(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}
	t->count = 1;
	t->type.id = SBDF_BINARYTYPEID;
	outptr = *(void**)t->data = sbdf_ba_create(0, len);
	if (!outptr)
	{
		free(t->data);
		free(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	*outptr++ = vt.id;

	*out = t;

	return SBDF_OK;
}

int sbdf_calculate_array_capacity(int size)
{
	int cap = 0;
	while (cap < size)
	{
		cap = 1 + cap * 3 / 2;
	}

	return cap;
}

int sbdf_get_array_length(void const* array)
{
	return ((int*)array)[-1];
}

void sbdf_dispose_array(void* array)
{
	if (array)
	{
		free ((int*)array - 1);
	}
}

void* sbdf_allocate_array(int length)
{
	int* alloc = malloc(length + sizeof(int));

	if (alloc)
	{
		*alloc++ = length;
	}

	return alloc;
}

void* sbdf_copy_array(void const* src)
{
	void* dst;
	int l;
	
	l = sbdf_get_array_length(src);
	dst = sbdf_allocate_array(l);

	if (dst)
	{
		memcpy(dst, src, l);
	}

	return dst;
}

int sbdf_alloc(void** inout, int sz)
{
	void* t;

	if (sz <= 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	if (!inout)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (*inout)
	{
		t = realloc(*inout, sz);
	}
	else
	{
		t = malloc(sz);
	}

	if (t)
	{
		*inout = t;
		return SBDF_OK;
	}
	else
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}
}

int sbdf_ti_is_arr(int id)
{
	switch (id)
	{
	case SBDF_STRINGTYPEID:
	case SBDF_BINARYTYPEID:
		return 1;
	}

	return 0;
}


int sbdf_read_7bitpacked_int32(FILE* f, int* v)
{
	unsigned int result = 0;
	int shl = 0;

	for (;;)
	{
		unsigned char uch;

		if (fread(&uch, sizeof(uch), 1, f) != 1)
		{
			return SBDF_ERROR_IO;
		}

		result |= ((unsigned int)(uch & 0x7f)) << shl;
		if ((uch & 0x80) == 0x80)
		{
			shl += 7;
		}
		else
		{
			break;
		}
	}

	*v = (int)result;

	return SBDF_OK;
}

int sbdf_write_7bitpacked_int32(FILE* f, int v)
{
	unsigned int val = (unsigned int)v;
	for (;;)
	{
		unsigned char uch = (unsigned char)(val & 0x7f);

		if (val > 0x7f)
		{
			uch |= 0x80;
		}

		if (fwrite(&uch, sizeof(uch), 1, f) != 1)
		{
			return SBDF_ERROR_IO;
		}

		if (val > 0x7f)
		{
			val >>= 7;
		}
		else
		{
			break;
		}
	}

	return SBDF_OK;
}

int sbdf_get_7bitpacked_len(int val)
{
	if (val < (1 << 7))
	{
		return 1;
	}
	else if (val < (1 << 14))
	{
		return 2;
	}
	else if (val < (1 << 21))
	{
		return 3;
	}
	else if (val < (1 << 28))
	{
		return 4;
	}
	else
	{
		return 5;
	}
}

