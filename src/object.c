/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "object_io.h"
#include "valuetypeid.h"
#include "errors.h"
#include "sbdfstring.h"
#include "bytearray.h"

#if _MSC_VER
typedef __int64 int64_t;
#else
#if defined(FLATSOURCETREE) && defined(SUNOS)
#include <inttypes.h>
#else
#include <stdint.h>
#endif
#endif

#include "internals.h"
#include "bswap.h"

int sbdf_init_array_int(sbdf_valuetype type, int count, void const* data, int const* lengths, sbdf_object** out, int clone_array);

int sbdf_obj_create_arr(sbdf_valuetype type, int count, void const* data, int const* lengths, sbdf_object** out)
{
	return sbdf_init_array_int(type, count, data, lengths, out, 1);
}

int sbdf_init_array_dontclone(sbdf_valuetype type, int count, void const* data, sbdf_object** out)
{
	return sbdf_init_array_int(type, count, data, 0, out, 0);
}

int sbdf_init_array_int(sbdf_valuetype type, int count, void const* data, int const* lengths, sbdf_object** out, int clone_array)
{
	sbdf_object* t;

	if (!out || !data)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	*out = 0;

	t = calloc(1, sizeof(sbdf_object));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->type = type;
	t->count = count;

	if (sbdf_ti_is_arr(type.id))
	{
		int i;
		int is_string;
		char** dst;
		char const** src;
		int calculate_lengths = lengths == 0;

		is_string = type.id == SBDF_STRINGTYPEID;
		if (clone_array && calculate_lengths && type.id == SBDF_BINARYTYPEID)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_ARGUMENT_NULL;
		}

		t->data = calloc(count, sizeof(void*));
		if (!t->data)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_OUT_OF_MEMORY;
		}

		if (clone_array)
		{
			dst = t->data;
			src = (char const**)data;

			for (i = 0; i < count; ++i)
			{
				int l = 0;
				
				if (!src[i])
				{
					/* TODO is this really OK or is it an error? */
					continue;
				}

				if (calculate_lengths)
				{
					if (src[i])
					{
						l = (int)strlen(src[i]);
					}
				}
				else
				{
					l = lengths[i];
				}

				if (is_string)
				{
					if (calculate_lengths)
					{
						dst[i] = sbdf_str_create(src[i]);
					}
					else
					{
						dst[i] = sbdf_str_create_len(src[i], l);
					}
				}
				else
				{
					dst[i] = (char*) sbdf_ba_create((unsigned char*) src[i], l);
				}
				if (!dst[i])
				{
					sbdf_obj_destroy(t);
					return SBDF_ERROR_OUT_OF_MEMORY;
				}
			}
		}
		else
		{
			memcpy(t->data, data, count * sizeof(void*));
		}
	}
	else
	{
		int elem_size = sbdf_get_unpacked_size(type);

		if (elem_size < 0)
		{
			sbdf_obj_destroy(t);
			return elem_size;
		}
		else if (elem_size == 0)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_UNKNOWN_TYPEID;
		}

		t->data = calloc(count, elem_size);
		if (!t->data)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_OUT_OF_MEMORY;
		}

		memcpy(t->data, data, count * elem_size);
	}

	*out = t;

	return SBDF_OK;
}

int sbdf_obj_create(sbdf_valuetype type, void const* data, int const* length, sbdf_object** o)
{
	return sbdf_obj_create_arr(type, 1, data, length, o);
}

void sbdf_obj_destroy(sbdf_object* object)
{
	if (object)
	{
		if (object->data)
		{
			if (sbdf_ti_is_arr(object->type.id))
			{
				/* free array data */
				char** ptr = (char**)object->data;
				int i;
				for (i = object->count - 1; i >= 0; --i)
				{
					if (*ptr)
					{
						sbdf_dispose_array(*ptr++);
					}
				}
			}
			free(object->data);
			object->data = 0;
		}

		free(object);
	}
}

static int sbdf_read_objects(FILE* f, sbdf_valuetype v, int count, sbdf_object** object, int packed_array)
{
	sbdf_object* t;
	*object = 0;

	if (!f || !object)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (count < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	t = calloc(1, sizeof(sbdf_object));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->type = v;
	t->count = count;

	if (sbdf_ti_is_arr(t->type.id))
	{
		int i;
		int is_string;
		int err;
		void** dest;
		dest = t->data = calloc(count, sizeof(void*));
		if (!dest)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_OUT_OF_MEMORY;
		}

		is_string = t->type.id == SBDF_STRINGTYPEID;

		/* read byte size and ignore it */
		if (packed_array && (err = sbdf_read_int32(f, &i)))
		{
			sbdf_obj_destroy(t);
			return err;
		}

		for (i = 0; i < count; ++i)
		{
			int length;
			if (packed_array)
			{
				err = sbdf_read_7bitpacked_int32(f, &length);
			}
			else
			{
				err = sbdf_read_int32(f, &length);
			}

			if (err)
			{
				sbdf_obj_destroy(t);
				return err;
			}

			if (length < 0)
			{
				sbdf_obj_destroy(t);
				return SBDF_ERROR_INVALID_SIZE;
			}

			if (is_string)
			{
				*dest = sbdf_str_create_len(0, length);
			}
			else
			{
				*dest = sbdf_ba_create(0, length);
			}

			if (*dest == 0)
			{
				sbdf_obj_destroy(t);
				return SBDF_ERROR_OUT_OF_MEMORY;
			}

			if (fread(*dest, 1, (size_t)length, f) != (size_t)length)
			{
				sbdf_obj_destroy(t);
				return SBDF_ERROR_IO;
			}

			++dest;
		}
	}
	else
	{
		int sz = sbdf_get_packed_size(v);
		if (sz < 0)
		{
			sbdf_obj_destroy(t);
			return sz;
		}
		else if (sz == 0)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_UNKNOWN_TYPEID;
		}
		t->data = malloc(sz * count);
		if (!t->data)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_OUT_OF_MEMORY;
		}
		if (fread(t->data, sz, (size_t)count, f) != (size_t)count)
		{
			sbdf_obj_destroy(t);
			return SBDF_ERROR_IO;
		}
		sbdf_swap(t->data, sz, count);
	}
	*object = t;

	return SBDF_OK;
}

int sbdf_obj_read_arr(FILE* f, sbdf_valuetype v, sbdf_object** array)
{
	int count, err;

	if (err = sbdf_read_int32(f, &count))
	{
		return err;
	}

	return sbdf_read_objects(f, v, count, array, 1);
}

int sbdf_obj_read(FILE* f, sbdf_valuetype v, sbdf_object** o)
{
	return sbdf_read_objects(f, v, 1, o, 0);
}

static int sbdf_write_objects(sbdf_object const* o, FILE* f, int packed_array)
{
	if (!o)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (sbdf_ti_is_arr(o->type.id))
	{
		int i;
		int pass;
		int err;
		int byte_size = 0;
		int is_string = o->type.id == SBDF_STRINGTYPEID;

		for (pass = 0; pass < 2; ++pass)
		{
			if (pass == 1)
			{
				if (!packed_array)
				{
					break;
				}

				if (err = sbdf_write_int32(f, byte_size))
				{
					return err;
				}
			}

			for (i = 0; i < o->count; ++i)
			{
				int length;
				char const** data = ((char const**)o->data) + i;
				length = is_string?sbdf_str_len(*data):sbdf_ba_get_len((unsigned char*) *data);

				if (pass == 0 && packed_array)
				{
					byte_size += sbdf_get_7bitpacked_len(length) + length;
				}
				else
				{
					if (packed_array)
					{
						if (err = sbdf_write_7bitpacked_int32(f, length))
						{
							return err;
						}
					}
					else
					{
						if (err = sbdf_write_int32(f, length))
						{
							return err;
						}
					}

					if (length)
					{
						if (fwrite(*data, 1, length, f) != length)
						{
							return SBDF_ERROR_OUT_OF_MEMORY;
						}
					}
				}
			}
		}
	}
	else
	{
		void* data;
		int elem_size = sbdf_get_unpacked_size(o->type);

		if (elem_size < 0)
		{
			return elem_size;
		}
		else if (elem_size == 0)
		{
			return SBDF_ERROR_UNKNOWN_TYPEID;
		}

		data = malloc(o->count * elem_size);
		if (!data)
		{
			return SBDF_ERROR_OUT_OF_MEMORY;
		}
		memcpy(data, o->data, o->count * elem_size);
		sbdf_swap(data, elem_size, o->count);

		if (fwrite(data, elem_size, (size_t)(o->count), f) != (size_t)(o->count))
		{
			free(data);
			return SBDF_ERROR_IO;
		}

		free(data);
	}

	return SBDF_OK;
}

int sbdf_obj_write_arr(sbdf_object const* o, FILE* f)
{
	int error;

	if (!o)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	error = sbdf_write_int32(f, o->count);
	if (error)
	{
		return error;
	}

	return sbdf_write_objects(o, f, 1);
}

int sbdf_obj_write(sbdf_object const* o, FILE* f)
{
	return sbdf_write_objects(o, f, 0);
}

int sbdf_obj_copy(sbdf_object const* src, sbdf_object** dst)
{
	int is_array;
	sbdf_object* t;
	int elem_size;

	if (!src || !dst)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(1, sizeof(sbdf_object));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->type = src->type;
	t->count = src->count;
	is_array = sbdf_ti_is_arr(src->type.id);

	elem_size = is_array?sizeof(void*):sbdf_get_unpacked_size(t->type);

	if (elem_size < 0)
	{
		sbdf_obj_destroy(t);
		return elem_size;
	}
	else if (elem_size == 0)
	{
		sbdf_obj_destroy(t);
		return SBDF_ERROR_UNKNOWN_TYPEID;
	}

	t->data = calloc(t->count, elem_size);
	if (!t->data)
	{
		sbdf_obj_destroy(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	if (is_array)
	{
		int i;
		for (i = 0; i < t->count; ++i)
		{
			if (!(((void**)t->data)[i] = sbdf_copy_array(((void**)src->data)[i])))
			{
				sbdf_obj_destroy(t);
				return SBDF_ERROR_OUT_OF_MEMORY;
			}
		}
	}
	else
	{
		memcpy(t->data, src->data, t->count * elem_size);
	}

	*dst = t;

	return SBDF_OK;
}

static int sbdf_skip_objects(FILE* f, sbdf_valuetype v, int c, int packed_array)
{
	if (!f)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (c < 0)
	{
		return SBDF_ERROR_INVALID_SIZE;
	}

	if (sbdf_ti_is_arr(v.id))
	{
		int skip;
		int err;

		if (packed_array)
		{
			if (err = sbdf_read_int32(f, &skip))
			{
				return err;
			}

			if (fseek(f, skip, SEEK_CUR))
			{
				return SBDF_ERROR_IO;
			}
		}
		else
		{
			while (c--)
			{
				if (err = sbdf_read_int32(f, &skip))
				{
					return err;
				}

				if (fseek(f, skip, SEEK_CUR))
				{
					return SBDF_ERROR_IO;
				}
			}
		}
	}
	else
	{
		int sz = sbdf_get_packed_size(v);
		if (sz < 0)
		{
			return sz;
		}
		else if (sz == 0)
		{
			return SBDF_ERROR_UNKNOWN_TYPEID;
		}
		if (fseek(f, c * sz, SEEK_CUR))
		{
			return SBDF_ERROR_IO;
		}
	}

	return SBDF_OK;
}

int sbdf_obj_skip_arr(FILE* f, sbdf_valuetype vt)
{
	int count, err = sbdf_read_int32(f, &count);
	if (err)
	{
		return err;
	}

	return sbdf_skip_objects(f, vt, count, 1);
}

int sbdf_obj_skip(FILE* f, sbdf_valuetype vt)
{
	return sbdf_skip_objects(f, vt, 1, 0);
}

int sbdf_obj_eq(sbdf_object const* lhs, sbdf_object const* rhs)
{
	if (lhs == rhs)
	{
		return 1;
	}

	if (!lhs || !rhs)
	{
		return 0;
	}

	if (lhs->type.id != rhs->type.id) return 0;
	
	if (lhs->count != rhs->count)
	{
		return 0;
	}

	if (sbdf_ti_is_arr(lhs->type.id))
	{
		int is_string = lhs->type.id == SBDF_STRINGTYPEID;
		int i;
		for (i = 0; i < lhs->count; ++i)
		{
			int cmp = 0;
			if (is_string)
			{
				cmp = sbdf_str_cmp(((void const**)lhs->data)[i], ((void const**)rhs->data)[i]);
			}
			else
			{
				cmp = sbdf_ba_memcmp(((void const**)lhs->data)[i], ((void const**)rhs->data)[i]);
			}

			if (cmp)
			{
				return cmp;
			}
		}
	}
	else
	{
	    int sz = sbdf_get_unpacked_size(lhs->type);
	    if (sz < 0)
	    {
	    	/* TODO what should we report here? */
	    	return -1;
	    }
	    /* TODO What about dynamic types? */
	    return !memcmp(lhs->data, rhs->data, sz * lhs->count);
	}

	return 1;
}
