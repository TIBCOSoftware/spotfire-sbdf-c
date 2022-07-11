/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "cppsupp.h"

#include "columnslice.h"
#include "columnslice_io.h"
#include "valuearray.h"
#include "errors.h"

#include <iterator>

using namespace sbdf;

static int array[] = { 4, 8, 15, 16, 23, 42 };
static bool emptyValues[] = { true, false, false, false, true, false };
static bool replacedValues[] = { false, true, true, true, false, true };
static char const* errorCodes[] =  { "an error", "", "", "", "", "" };

static scoped_ptr<sbdf_columnslice> create_owned()
{
	sbdf_columnslice* result;
	sbdf_valuearray* va;
	SBDF_CHECK(sbdf_va_create_dflt(obj(array), &va), SBDF_OK);
	SBDF_CHECK(sbdf_cs_create(&result, va), SBDF_OK);

	return make_scoped_ptr(result, sbdf_cs_destroy_all);
}

static sbdf_valuearray* empty()
{
	sbdf_valuearray* va;
	SBDF_CHECK(sbdf_va_create_dflt(obj(emptyValues), &va), SBDF_OK);
	return va;
}

static sbdf_valuearray* replaced()
{
	sbdf_valuearray* va;
	SBDF_CHECK(sbdf_va_create_dflt(obj(replacedValues), &va), SBDF_OK);
	return va;
}

static sbdf_valuearray* error()
{
	sbdf_valuearray* va;
	SBDF_CHECK(sbdf_va_create_dflt(obj(errorCodes), &va), SBDF_OK);
	return va;
}

template <typename T>
static void check_cs_array(sbdf_columnslice* cs, char const* nm, T const& arr)
{
	sbdf_valuearray* va = 0;
	if (nm)
	{
		SBDF_CHECK(sbdf_cs_get_property(cs, nm, &va), SBDF_OK);
	}
	else
	{
		va = cs->values;
	}
	obj o;
	SBDF_CHECK(sbdf_va_get_values(va, o.obj_ptr()), SBDF_OK);
	SBDF_ASSERT(check_arrays(arr, o));
}

SBDF_TEST(columnslice, basic_properties)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	SBDF_ASSERT(array_len(array) == sbdf_cs_row_cnt(cs));
	check_cs_array(cs, 0 , array);
}

SBDF_TEST(columnslice, empty_values)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	sbdf_valuearray* va = 0;
	SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ISINVALID_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);

	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ISINVALID_VALUEPROPERTY, empty()), SBDF_OK);
	va = empty();
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ISINVALID_VALUEPROPERTY, va), SBDF_ERROR_PROPERTY_ALREADY_EXISTS);
	sbdf_va_destroy(va);
	va = 0;

	check_cs_array(cs, SBDF_ISINVALID_VALUEPROPERTY, emptyValues);
}

SBDF_TEST(columnslice, replaced_values)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	sbdf_valuearray* va = 0;
	SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);

	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, replaced()), SBDF_OK);
	va = replaced();
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, va), SBDF_ERROR_PROPERTY_ALREADY_EXISTS);
	sbdf_va_destroy(va);
	va = 0;

	check_cs_array(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, replacedValues);
}

SBDF_TEST(columnslice, error_codes)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	sbdf_valuearray* va = 0;
	SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);

	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, error()), SBDF_OK);
	va = error();
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, va), SBDF_ERROR_PROPERTY_ALREADY_EXISTS);
	sbdf_va_destroy(va);
	va = 0;

	check_cs_array(cs, SBDF_ERRORCODE_VALUEPROPERTY, errorCodes);
}

SBDF_TEST(columnslice, custom_property)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	static int vals[] = { 1, 2, 3, 4, 5, 6 };
	sbdf_valuearray* va = 0;
	SBDF_CHECK(sbdf_cs_get_property(cs, "foo", &va), SBDF_ERROR_PROPERTY_NOT_FOUND);

	sbdf_va_create_dflt(obj(vals), &va);
	SBDF_CHECK(sbdf_cs_add_property(cs, "foo", va), SBDF_OK);
	sbdf_va_create_dflt(obj(vals), &va);
	SBDF_CHECK(sbdf_cs_add_property(cs, "foo", va), SBDF_ERROR_PROPERTY_ALREADY_EXISTS);
	sbdf_va_destroy(va);
	va = 0;

	check_cs_array(cs, "foo", vals);
}

SBDF_TEST(columnslice, serialization)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ISINVALID_VALUEPROPERTY, empty()), SBDF_OK);
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, replaced()), SBDF_OK);
	SBDF_CHECK(sbdf_cs_add_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, error()), SBDF_OK);

	tmpf f;
	SBDF_CHECK(sbdf_cs_write(f.write(), cs), SBDF_OK);
	scoped_ptr<sbdf_columnslice> cs2 = make_scoped_ptr(sbdf_cs_destroy);
	SBDF_CHECK(sbdf_cs_read(f.read(), cs2.obj_ptr()), SBDF_OK);

	check_cs_array(cs2, 0, array);
	check_cs_array(cs2, SBDF_ISINVALID_VALUEPROPERTY, emptyValues);
	check_cs_array(cs2, SBDF_REPLACEDVALUE_VALUEPROPERTY, replacedValues);
	check_cs_array(cs2, SBDF_ERRORCODE_VALUEPROPERTY, errorCodes);

	std::vector<unsigned char> rep = f.read_vec(), rep2;
	std::copy(rep.begin(), rep.end(), std::back_inserter(rep2));
	std::copy(rep.begin(), rep.end(), std::back_inserter(rep2));
	f.write_vec(rep2);
	SBDF_CHECK(sbdf_cs_skip(f.read()), SBDF_OK);
	SBDF_ASSERT(f.pos() == static_cast<int>(rep.size()));
}

SBDF_TEST(columnslice, robustness)
{
	scoped_ptr<sbdf_columnslice> cs = create_owned();
	sbdf_valuearray* va;
	static int ttt[] = { 1, 2, 3 };
	SBDF_CHECK(sbdf_va_create_dflt(obj(ttt), &va), SBDF_OK);
	SBDF_CHECK(sbdf_cs_add_property(cs, "bar", va), SBDF_ERROR_ROW_COUNT_MISMATCH);
	sbdf_va_destroy(va);
}
