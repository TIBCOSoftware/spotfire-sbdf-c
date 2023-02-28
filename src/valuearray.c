/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "valuearray.h"

#include <stdlib.h>
#include <string.h>

#include "valuearray_io.h"
#include "errors.h"
#include "valuetypeid.h"
#include "object_io.h"
#include "valuetype_io.h"
#include "sbdfstring.h"
#include "bytearray.h"

#include "internals.h"

struct sbdf_valuearray
{
	sbdf_valuetype valuetype;
	int encoding;
	int value1;
	sbdf_object* object1;
	sbdf_object* object2;
};

void sbdf_va_destroy(sbdf_valuearray* handle)
{
	if (handle)
	{
		if (handle->object1)
		{
			sbdf_obj_destroy(handle->object1);
		}

		if (handle->object2)
		{
			sbdf_obj_destroy(handle->object2);
		}

		free(handle);
	}
}

/* creates a plain value array encoding */
int sbdf_va_create_plain(sbdf_object const* array, sbdf_valuearray** handle)
{
	int err;

	if (!array || !handle)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	*handle = calloc(sizeof(sbdf_valuearray), 1);
	if (!*handle)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	(*handle)->valuetype = array->type;
	(*handle)->encoding = SBDF_PLAINARRAYENCODINGTYPEID;

	err = sbdf_obj_copy(array, &(*handle)->object1);
	if (err)
	{
		return err;
	}

	return SBDF_OK;
}

static sbdf_valuetype byte_vt = { SBDF_BYTETYPEID };

int sbdf_va_create_rle(sbdf_object const* array, sbdf_valuearray** handle)
{
	int err;

	if (!array || !handle)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	*handle = calloc(sizeof(sbdf_valuearray), 1);
	if (!*handle)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	(*handle)->valuetype = array->type;
	(*handle)->encoding = SBDF_RUNLENGTHENCODINGTYPEID;
	(*handle)->value1 = array->count;

	{
		char const* cur_inp;
		char* out_base;
		char* out;
		unsigned char* run_out_base;
		unsigned char* run_out;
		int out_capacity;
		int out_size;
		int i;
		int cur_sz;
		int prev_sz;
		int is_array;
		void const* prev_data;
		void const* cur_data;
		int run;
		int elem_size;
		int is_string;

		run = 0;
		cur_inp = array->data;
		out_size = out_capacity = 0;
		out = out_base = 0;
		run_out = run_out_base = 0;
		cur_sz = prev_sz = 0;
		prev_data = cur_data = 0;
		is_array = sbdf_ti_is_arr(array->type.id);

		if (is_array)
		{
			elem_size = sizeof(void*);
			is_string = array->type.id == SBDF_STRINGTYPEID;
		}
		else
		{
			elem_size = cur_sz = sbdf_get_unpacked_size(array->type);
			if (elem_size < 0)
			{
				free(*handle);
				return elem_size;
			}
			else if (elem_size == 0)
			{
				free(*handle);
				return SBDF_ERROR_UNKNOWN_TYPEID;
			}
			is_string = 0;
		}

		for (i = 0;;++i)
		{
			int end = i == array->count;
			if (!end)
			{
				if (is_array)
				{
					cur_data = *(void**)cur_inp;
					cur_sz = sbdf_get_array_length(cur_data);
				}
				else
				{
					cur_data = cur_inp;
				}
				cur_inp += elem_size;
			}

			if (run == 256 || i == array->count || (prev_data && (prev_sz != cur_sz || memcmp(prev_data, cur_data, cur_sz))))
			{
				if (out_size == out_capacity)
				{
					out_capacity = 1 + out_capacity * 3 / 2;

					if (out_base)
					{
						out_base = realloc(out_base, elem_size * out_capacity);
						run_out_base = realloc(run_out_base, out_capacity);
					}
					else
					{
						out_base = malloc(elem_size * out_capacity);
						run_out_base = malloc(out_capacity);
					}

					if (!out_base || !run_out_base)
					{
						free(*handle);
						return SBDF_ERROR_OUT_OF_MEMORY;
					}

					out = out_base + elem_size * out_size;
					run_out = run_out_base + out_size;
				}

				*run_out++ = run - 1;
				if (is_array)
				{
					void* copy = sbdf_copy_array(prev_data);
					if (!copy)
					{
						free(run_out_base);
						free(out_base);
						free(*handle);
						return SBDF_ERROR_OUT_OF_MEMORY;
					}
					*(void**)out = copy;
				}
				else
				{
					memcpy(out, prev_data, prev_sz);
				}
				++out_size;
				out += elem_size;

				if (end)
				{
					break;
				}

				run = 1;
			}
			else
			{
				++run;
			}

			prev_data = cur_data;
			prev_sz = cur_sz;
		}

		err = sbdf_obj_create_arr(byte_vt, out_size, run_out_base, 0, &(*handle)->object1);

		if (err == SBDF_OK)
		{
			/* out_base already contains cloned copies */
			err = sbdf_init_array_dontclone(array->type, out_size, out_base, &(*handle)->object2);
		}

		if (err)
		{
			sbdf_va_destroy(*handle);
		}

		free(run_out_base);
		free(out_base);
	}

	return err;
}

static char nulls[16] = { 0 };

static sbdf_valuetype bool_vt = { SBDF_BOOLTYPEID };
static sbdf_valuetype bytearray_vt = { SBDF_BINARYTYPEID };

int sbdf_va_create_bit(sbdf_object const* array, sbdf_valuearray** handle)
{
	int len, packed_len;
	unsigned char* out;
	unsigned char* in;
	int i;
	int elem_size;
	int remaining_bits;
	sbdf_valuetype vt;
	sbdf_object* t;

	if (!array || !handle)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	vt = array->type;

	*handle = calloc(sizeof(sbdf_valuearray), 1);
	if (!*handle)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	(*handle)->valuetype = bool_vt;
	(*handle)->encoding = SBDF_BITARRAYENCODINGTYPEID;

	if (sbdf_ti_is_arr(vt.id))
	{
		elem_size = sizeof(void*);
	}
	else
	{
		elem_size = sbdf_get_unpacked_size(vt);
		if (elem_size < 0)
		{
			return elem_size;
		}
		else if (elem_size == 0)
		{
			return SBDF_ERROR_UNKNOWN_TYPEID;
		}
	}

	len = (*handle)->value1 = array->count;
	remaining_bits = len % 8;
	packed_len = len / 8 + !!remaining_bits;
	out = malloc(packed_len);
	if (!out)
	{
		free(*handle);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	in = array->data;

	for (i = len / 8; i > 0; --i)
	{
		int inner;
		int value = 0;
		for (inner = 0; inner < 8; ++inner)
		{
			value = value << 1;

			if (memcmp(nulls, in, elem_size))
			{
				++value;
			}

			in += elem_size;
		}

		*out++ = value;
	}

	if (remaining_bits)
	{
		int inner;
		int value = 0;
		for (inner = 0; inner < remaining_bits; ++inner)
		{
			value = value << 1;

			if (memcmp(nulls, in, elem_size))
			{
				++value;
			}

			in += elem_size;
		}

		value = value << (8 - remaining_bits);

		*out++ = value;
	}

	t = calloc(1, sizeof(sbdf_object));
	if (!t)
	{
		free(*handle);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}
	t->type = bytearray_vt;

	t->data = malloc(sizeof(void*));
	if (!t->data)
	{
		free(*handle);
		free(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	*(void**)t->data = sbdf_ba_create(out - packed_len, packed_len);
	if (!t->data)
	{
		free(*handle);
		free(t);
		free(t->data);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	free(out - packed_len);
	t->count = 1;

	(*handle)->object1 = t;

	return SBDF_OK;
}

int sbdf_va_create(int arrayencoding, sbdf_object const* array, sbdf_valuearray** handle)
{
	*handle = 0;
	switch (arrayencoding)
	{
	case SBDF_PLAINARRAYENCODINGTYPEID:
		return sbdf_va_create_plain(array, handle);
	case SBDF_RUNLENGTHENCODINGTYPEID:
		return sbdf_va_create_rle(array, handle);
	case SBDF_BITARRAYENCODINGTYPEID:
		return sbdf_va_create_bit(array, handle);
	}
	return SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING;
}

static int sbdf_get_rle_values(sbdf_valuearray* handle, sbdf_object** result)
{
	int i;
	int is_string;
	int elem_size;
	int is_array;
	sbdf_object* t;
	int elem_cnt = 0;
	char* data_out;
	char const* data_in;

	if (!handle || !result)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (sbdf_ti_is_arr(handle->valuetype.id))
	{
		elem_size = sizeof(void*);
		is_array = 1;
		is_string = handle->valuetype.id == SBDF_STRINGTYPEID;
	}
	else
	{
		elem_size = sbdf_get_unpacked_size(handle->valuetype);
		is_array = 0;
		is_string = 0;
	}

	if (elem_size < 0)
	{
		return elem_size;
	}
	else if (elem_size == 0)
	{
		return SBDF_ERROR_UNKNOWN_TYPEID;
	}

	if (!(t = calloc(1, sizeof(sbdf_object))))
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}
	t->type = handle->object2->type;
	t->count = elem_cnt = handle->value1;

	if (!(t->data = malloc(elem_size * elem_cnt)))
	{
		sbdf_obj_destroy(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	data_out = t->data;

	data_in = handle->object2->data;

	for (i = 0; i < handle->object1->count; ++i)
	{
		int run;

		for (run = ((unsigned char *)handle->object1->data)[i]; run >= 0; --run)
		{
			if (is_array)
			{
				void* copy = *(void**)data_out = sbdf_copy_array(*(void**)data_in);
				if (!copy)
				{
					sbdf_obj_destroy(t);
					return SBDF_ERROR_OUT_OF_MEMORY;
				}
			}
			else
			{
				memcpy(data_out, data_in, elem_size);
			}
			data_out += elem_size;
		}
		data_in += elem_size;
	}

	*result = t;

	return SBDF_OK;
}

static int sbdf_get_bitarray_values(sbdf_valuearray* handle, sbdf_object** result)
{
	int i, ofs, value;
	unsigned char* inp;
	unsigned char* outp;
	sbdf_object* t;

	if (!handle || !result)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(1, sizeof(sbdf_object));
	if (!t)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	t->type = bool_vt;

	outp = t->data = malloc(handle->value1);
	if (!outp)
	{
		free(t);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	inp = *(unsigned char **)handle->object1->data;

	for (i = handle->value1, ofs = 0; i > 0 ; --i)
	{
		if (ofs == 0)
		{
			ofs = 7;
			value = *inp++;
		}
		else
		{
			--ofs;
			value = value << 1; 
		}

		*outp++ = !!(value & 128);
	}

	t->count = handle->value1;
	*result = t;

	return SBDF_OK;
}

int sbdf_va_get_values(sbdf_valuearray* handle, sbdf_object** result)
{
	if (!handle || !result)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	switch (handle->encoding)
	{
	case SBDF_PLAINARRAYENCODINGTYPEID:
		return sbdf_obj_copy(handle->object1, result);
	case SBDF_RUNLENGTHENCODINGTYPEID:
		return sbdf_get_rle_values(handle, result);
	case SBDF_BITARRAYENCODINGTYPEID:
		return sbdf_get_bitarray_values(handle, result);
	}

	return SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING;
}

static int sbdf_read_valuearray_int(FILE* file, sbdf_valuearray** handle)
{
	int e;
	sbdf_valuetype vt;
	int err;

	if (err = sbdf_read_int8(file, &e))
	{
		return err;
	}

	if (err = sbdf_vt_read(file, &vt))
	{
		return err;
	}

	if (handle)
	{
		*handle = calloc(sizeof(sbdf_valuearray), 1);
		if (!*handle)
		{
			return SBDF_ERROR_OUT_OF_MEMORY;
		}
		(*handle)->encoding = e;
		(*handle)->valuetype = vt;
	}

	switch (e)
	{
	case SBDF_PLAINARRAYENCODINGTYPEID:
		if (handle)
		{
			err = sbdf_obj_read_arr(file, vt, &(*handle)->object1);
		}
		else
		{
			err = sbdf_obj_skip_arr(file, vt);
		}
		if (err)
		{
			if (handle)
			{
				sbdf_va_destroy(*handle);
			}
			return err;
		}
		break;
	case SBDF_RUNLENGTHENCODINGTYPEID:
		{
			if (handle)
			{
				err = sbdf_read_int32(file, &(*handle)->value1);
				if (err)
				{
					sbdf_va_destroy(*handle);
					return err;
				}

				err = sbdf_obj_read_arr(file, byte_vt, &(*handle)->object1);
			}
			else
			{
				err = sbdf_obj_skip_arr(file, byte_vt);
			}

			if (err)
			{
				if (handle)
				{
					sbdf_va_destroy(*handle);
				}
				return err;
			}

			if (handle)
			{
				err = sbdf_obj_read_arr(file, vt, &(*handle)->object2);
			}
			else
			{
				err = sbdf_obj_skip_arr(file, vt);
			}

			if (err)
			{
				if (handle)
				{
					sbdf_va_destroy(*handle);
				}
				return err;
			}
		}
		break;
	case SBDF_BITARRAYENCODINGTYPEID:
		{
			int v;
			int packed_size = 0;
			err = sbdf_read_int32(file, &v);

			if (err)
			{
				if (handle)
				{
					sbdf_va_destroy(*handle);
				}
				return err;
			}

			if (handle)
			{
				(*handle)->value1 = v;
			}

			/* just read the raw byte stream */
			packed_size = v / 8 + !!(v%8);
			if (handle)
			{
				unsigned char* buf = malloc((size_t)packed_size);
				if (!buf)
				{
					sbdf_va_destroy(*handle);
					return SBDF_ERROR_OUT_OF_MEMORY;
				}

				if (fread(buf, 1, (size_t)packed_size, file) != (size_t)packed_size)
				{
					free(buf);
					sbdf_va_destroy(*handle);
					return SBDF_ERROR_IO;
				}
				err = sbdf_obj_create(bytearray_vt, &buf, &packed_size, &(*handle)->object1);
				free(buf);
			}
			else if (fseek(file, (long)packed_size, SEEK_CUR))
			{
				return SBDF_ERROR_IO;
			}

			if (err)
			{
				if (handle)
				{
					sbdf_va_destroy(*handle);
				}
				return err;
			}
		}
		break;
	default:
		return SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING;
	}

	return SBDF_OK;
}

int sbdf_va_write(sbdf_valuearray* handle, FILE* file)
{
	int err;

	if (!handle)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (err = sbdf_write_int8(file, handle->encoding))
	{
		return err;
	}
	
	if (err = sbdf_vt_write(file, handle->valuetype))
	{
		return err;
	}

	switch (handle->encoding)
	{
	case SBDF_PLAINARRAYENCODINGTYPEID:
		if (err = sbdf_obj_write_arr(handle->object1, file))
		{
			return err;
		}
		break;
	case SBDF_RUNLENGTHENCODINGTYPEID:
		if (err = sbdf_write_int32(file, handle->value1))
		{
			return err;
		}
		if (err = sbdf_obj_write_arr(handle->object1, file))
		{
			return err;
		}
		if (err = sbdf_obj_write_arr(handle->object2, file))
		{
			return err;
		}
		break;
	case SBDF_BITARRAYENCODINGTYPEID:
		if (err = sbdf_write_int32(file, handle->value1))
		{
			return err;
		}

		/* just write the byte stream */
		{
			size_t len = sbdf_ba_get_len(*(void**)handle->object1->data);
			if (fwrite(*(void**)handle->object1->data, 1, len, file) != len)
			{
				return SBDF_ERROR_IO;
			}
		}
		break;
	default:
		return SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING;
	}

	return SBDF_OK;
}

int sbdf_va_read(FILE* file, sbdf_valuearray** handle)
{
	if (!handle)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	return sbdf_read_valuearray_int(file, handle);
}

int sbdf_va_skip(FILE* file)
{
	return sbdf_read_valuearray_int(file, 0);
}

int sbdf_va_create_dflt(sbdf_object const* array, sbdf_valuearray** handle)
{
	if (!array)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (array->type.id == SBDF_BOOLTYPEID)
	{
		return sbdf_va_create_bit(array, handle);
	}
	else
	{
		return sbdf_va_create_plain(array, handle);
	}
}

int sbdf_va_row_cnt(sbdf_valuearray* in)
{
	if (!in)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	switch (in->encoding)
	{
	case SBDF_PLAINARRAYENCODINGTYPEID:
		return in->object1->count;
	case SBDF_RUNLENGTHENCODINGTYPEID:
		return in->value1;
	case SBDF_BITARRAYENCODINGTYPEID:
		return in->value1;
	}

	return SBDF_ERROR_UNKNOWN_VALUEARRAY_ENCODING;
}
