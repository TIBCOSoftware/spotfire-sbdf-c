/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "cppsupp.h"

#include "valuearray.h"
#include "valuearray_io.h"

using namespace sbdf;

SBDF_TEST(valuearray, rle)
{
	int vals[] = {  1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 999, 999, -1 };
	unsigned char ser[] = { 2, 2, 18, 0, 0, 0, 7, 0, 0, 0, 4, 3, 2, 1, 0, 1, 0, 7, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 231, 3, 0, 0, 255, 255, 255, 255 };

	tmpf tmp;
	sbdf_valuearray* va;

	// serialization
	{
		obj o = vals;
		sbdf_va_create_rle(o, &va);
		sbdf_va_write(va, tmp.write());
		sbdf_va_destroy(va);
		SBDF_ASSERT(check_arrays(ser, tmp.read_vec()));
	}

	// deserialization
	{
		tmp.set(ser);
		sbdf_va_read(tmp.read(), &va);
		obj oo;
		sbdf_va_get_values(va, oo.obj_ptr());
		SBDF_ASSERT(check_arrays(vals, oo.as_vec<int>()));
		sbdf_va_destroy(va);
	}
}

SBDF_TEST(valuearray, bit)
{
    bool a1[] = { false, false, true, false, false, true, true, false };
    bool a2[] = { true, false, false, true, false, false, true, true, false, true };
	unsigned char ser1[] = { 3, 1, 8, 0, 0, 0, 38 };
	unsigned char ser2[] =  { 3, 1, 10, 0, 0, 0, 147, 64 };

	sbdf_valuearray* va;
	tmpf tmp;

	// serialization 1
	{
		obj o = a1;
		sbdf_va_create_bit(o, &va);
		sbdf_va_write(va, tmp.write());
		sbdf_va_destroy(va);
		SBDF_ASSERT(check_arrays(ser1, tmp.read_vec()));
	}

	// deserialization 1
	{
		tmp.set(ser1);
		sbdf_va_read(tmp.read(), &va);
		obj o;
		sbdf_va_get_values(va, o.obj_ptr());
		SBDF_ASSERT(check_arrays(a1, o.as_vec<bool>()));
		sbdf_va_destroy(va);
	}

	// serialization 2
	{
		obj o = a2;
		sbdf_va_create_bit(o, &va);
		sbdf_va_write(va, tmp.write());
		sbdf_va_destroy(va);
		SBDF_ASSERT(check_arrays(ser2, tmp.read_vec()));
	}

	// deserialization 2
	{
		tmp.set(ser2);
		sbdf_va_read(tmp.read(), &va);
		obj o;
		sbdf_va_get_values(va, o.obj_ptr());
		SBDF_ASSERT(check_arrays(a2, o.as_vec<bool>()));
		sbdf_va_destroy(va);
	}
}

SBDF_TEST(valuearray, single)
{
	int vals[] = {  1, 2, 3, 4, 5 };
	unsigned char ser[] = { 1, 2, 5, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0 };

	sbdf_valuearray* va;
	tmpf tmp;

	// serialization 1
	{
		obj o = vals;
		sbdf_va_create_plain(o, &va);
		sbdf_va_write(va, tmp.write());
		sbdf_va_destroy(va);
		SBDF_ASSERT(check_arrays(ser, tmp.read_vec()));
	}

	// deserialization 1
	{
		tmp.set(ser);
		sbdf_va_read(tmp.read(), &va);
		obj o;
		sbdf_va_get_values(va, o.obj_ptr());
		SBDF_ASSERT(check_arrays(vals, o.as_vec<int>()));
		sbdf_va_destroy(va);
	}
}
