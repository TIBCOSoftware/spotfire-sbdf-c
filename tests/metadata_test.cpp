/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "metadata.h"
#include "errors.h"

#include "cppsupp.h"

using namespace sbdf;

SBDF_TEST(metadata, add)
{
	sbdf_metadata_head* t = 0;
	obj pi = 3.14;

	SBDF_CHECK(sbdf_md_create(&t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("hello", pi, 0, t), SBDF_OK);
	SBDF_ASSERT(!!t);
	SBDF_CHECK(sbdf_md_add("hello", pi, 0, t), SBDF_ERROR_METADATA_ALREADY_EXISTS);
	SBDF_CHECK(sbdf_md_add("goodbye", pi, 0, t), SBDF_OK);
	sbdf_md_destroy(t);
}

SBDF_TEST(metadata, remove)
{
	sbdf_metadata_head* t = 0;
	obj pi = 3.14;

	SBDF_CHECK(sbdf_md_create(&t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("hello", pi, 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("goodbye", pi,0,  t), SBDF_OK);
	SBDF_ASSERT(sbdf_md_exists("hello", t) > 0);
	SBDF_ASSERT(sbdf_md_exists("goodbye", t) > 0);
	SBDF_CHECK(sbdf_md_remove("hello", t), SBDF_OK);
	SBDF_ASSERT(sbdf_md_exists("hello", t) == 0);
	SBDF_ASSERT(sbdf_md_exists("goodbye", t) > 0);
	SBDF_CHECK(sbdf_md_remove("hello", t), SBDF_OK);
	SBDF_CHECK(sbdf_md_remove("goodbye", t), SBDF_OK);
	SBDF_ASSERT(sbdf_md_exists("goodbye", t) == 0);

	sbdf_md_destroy(t);
}

SBDF_TEST(metadata, get)
{
	sbdf_metadata_head* t = 0;
	obj foo = "bar";
	obj bar = "foo";
	obj tt;

	SBDF_CHECK(sbdf_md_create(&t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("apa", foo, 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("bepa", bar, 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_get("apa", t, tt.obj_ptr()), SBDF_OK);
	SBDF_ASSERT(!!sbdf_obj_eq(foo, tt));
	SBDF_CHECK(sbdf_md_get("bepa", t, tt.obj_ptr()), SBDF_OK);
	SBDF_ASSERT(!!sbdf_obj_eq(bar, tt));
	SBDF_CHECK(sbdf_md_get("qwe", t, tt.obj_ptr()), SBDF_ERROR_METADATA_NOT_FOUND);
	sbdf_md_destroy(t);

}

SBDF_TEST(metadata, copy)
{
	sbdf_metadata_head* t = 0, *u = 0;
	SBDF_CHECK(sbdf_md_create(&t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("123", obj(123), 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("456", obj(456), 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("789", obj(789), 0, t), SBDF_OK);

	SBDF_CHECK(sbdf_md_create(&u), SBDF_OK);
	SBDF_CHECK(sbdf_md_copy(t, u), SBDF_OK);
	sbdf_md_destroy(t);

	SBDF_ASSERT(sbdf_md_exists("123", u) == 1);
	SBDF_ASSERT(sbdf_md_exists("456", u) == 1);
	SBDF_ASSERT(sbdf_md_exists("789", u) == 1);
	SBDF_ASSERT(sbdf_md_cnt(u) == 3);

	sbdf_md_destroy(u);
}

SBDF_TEST(metadata, copy2)
{
	sbdf_metadata_head* t = 0, *u = 0;
	SBDF_CHECK(sbdf_md_create(&t), SBDF_OK);
	SBDF_CHECK(sbdf_md_create(&u), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("123", obj(123), 0, t), SBDF_OK);
	SBDF_CHECK(sbdf_md_add("456", obj(456), 0, u),  SBDF_OK);
	SBDF_ASSERT(sbdf_md_copy(t, t) != SBDF_OK);
	SBDF_CHECK(sbdf_md_copy(t, u), SBDF_OK);
	SBDF_ASSERT(sbdf_md_exists("123", u) == 1);
	SBDF_ASSERT(sbdf_md_exists("456", u) == 1);
	sbdf_md_destroy(t);
	sbdf_md_destroy(u);
}
