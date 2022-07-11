/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "test.h"

#include "errors.h"
#include "bytearray.h"
#include "sbdfstring.h"

#include <stdio.h>
#include <string.h>
#if _MSC_VER
#include <windows.h>
#endif

tmpf::tmpf() : fn_(_tempnam("./", 0)), open_(0)
{
}

tmpf::~tmpf()
{
	if (open_)
	{
		fclose(open_);
	}
	open_ = 0;

	remove(fn_.c_str());
}

FILE* tmpf::read()
{
	if (open_)
	{
		fclose(open_);
		open_ = 0;
	}

	open_ = fopen(fn_.c_str(), "rb");
	return open_;
}

FILE* tmpf::write()
{
	if (open_)
	{
		fclose(open_);
		open_ = 0;
	}

	open_ = fopen(fn_.c_str(), "wb");
	return open_;
}

std::vector<unsigned char> tmpf::read_vec()
{
	return ::read_vec(read());
}

std::vector<unsigned char> read_vec(FILE* f)
{
	std::vector<unsigned char> result;
	if (f)
	{
		fseek(f, 0, SEEK_END);
		long l = ftell(f);
		fseek(f, 0, SEEK_SET);
		result.resize(l);
		fread(&*result.begin(), 1, l, f);
	}

	return result;
}


void tmpf::write_vec(std::vector<unsigned char> const& v)
{
	FILE* f = write();
	if (f && !v.empty())
	{
		fwrite(&*v.begin(), 1, v.size(), f);
	}
}

void tmpf::set(unsigned char *t, size_t n)
{
	FILE* f = write();
	if (f && n)
	{
		fwrite(t, 1, n, f);
	}
}

int tmpf::pos() const
{
	return open_?ftell(open_) : 0;
}

bool check_char_arrays(char const** a, int N, sbdf::obj const& obj)
{
	// check char arrays...
	if (obj->type.id != SBDF_STRINGTYPEID)
	{
		return false;
	}

	if (obj->count != N)
	{
		return false;
	}

	for (int i = 0; i < N; ++i)
	{
		if (strcmp(a[i], static_cast<char const**>(obj->data)[i]))
		{
			return false;
		}
	}
	return true;
}

static int sbdf_get_unpacked_size(sbdf_valuetype t)
{
	switch (t.id)
	{
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

static void print_bool(FILE* dest, void const* ptr)
{
	fprintf(dest, "%d", *(char*)ptr);
}

static void print_int(FILE* dest, void const* ptr)
{
	fprintf(dest, "%d", *(int*)ptr);
}

static void print_long(FILE* dest, void const* ptr)
{
	fprintf(dest, "%lld", *(int64_t*)ptr);
}

static void print_float(FILE* dest, void const* ptr)
{
	fprintf(dest, "%f", *(float*)ptr);
}

static void print_double(FILE* dest, void const* ptr)
{
	fprintf(dest, "%lf", *(double*)ptr);
}

static void print_string(FILE* dest, void const* ptr)
{
	// cvt to iso 8859-1
	char const* utf8 = *(char const**)ptr;
	int l = sbdf_convert_utf8_to_iso88591(utf8, 0);
	std::vector<char> iso(l);
	sbdf_convert_utf8_to_iso88591(utf8, &iso[0]);
	fprintf(dest, "%s", &iso[0]);
}

static void print_binary(FILE* dest, void const* ptr, int sz)
{
	fprintf(dest, "%d bytes: ", sz);
	while (sz--)
	{
		fprintf(dest, "%.2x", *((unsigned char*)ptr));
		ptr = (char*)ptr + 1;
	}
}

void print_object(FILE* dest, sbdf_object const* obj)
{
	void (*printer)(FILE* dest, void const*) = 0;
	char const* desc = 0;

	switch (obj->type.id)
	{
	case SBDF_UNKNOWNTYPEID:
		desc = "unknown";
		break;
	case SBDF_BOOLTYPEID:
		desc = "bool";
		printer = print_bool;
		break;
	case SBDF_INTTYPEID:
		desc = "int";
		printer = print_int;
		break;
	case SBDF_LONGTYPEID:
		desc = "long";
		printer = print_long;
		break;
	case SBDF_FLOATTYPEID:
		desc = "float";
		printer = print_float;
		break;
	case SBDF_DOUBLETYPEID:
		desc = "double";
		printer = print_double;
		break;
	case SBDF_DATETIMETYPEID:
		desc = "datetime";
		printer = print_long;
		break;
	case SBDF_DATETYPEID:
		desc = "date";
		printer = print_long;
		break;
	case SBDF_TIMETYPEID:
		desc = "time";
		printer = print_long;
		break;
	case SBDF_TIMESPANTYPEID:
		desc = "timespan";
		printer = print_long;
		break;
	case SBDF_STRINGTYPEID:
		desc = "string";
		printer = print_string;
		break;
	case SBDF_BINARYTYPEID:
		desc = "binary";
		break;
	case SBDF_DECIMALTYPEID:
		desc = "decimal";
		break;
	default:
		break;
	}
	
	fprintf(dest, "Type %s count %d\n", desc, obj->count);
	int size = sbdf_get_unpacked_size(obj->type);
	int is_array = size == 0;
	if (size == 0)
	{
		size = sizeof(void*);
	}
	void const* data = obj->data;
	for (int i = 0; i < obj->count; ++i)
	{
		if (i)
		{
			fprintf(dest, ", ");
		}
		if (printer)
		{
			printer(dest, data);
			fprintf(dest, " (");
		}

		if (is_array)
		{
			void* array = *(void**)data;
			print_binary(dest, array, sbdf_ba_get_len((static_cast<unsigned char const*>(array))));
		}
		else
		{
			print_binary(dest, data, size);
		}

		if (printer)
		{
			fprintf(dest, ")");
		}

		data = ((char const*)data) + size;
	}

	fprintf(dest, "\n");
}
