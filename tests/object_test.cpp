/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "cppsupp.h"

using namespace sbdf;

SBDF_TEST(object, test)
{
	obj o = "foo";
	obj x = "foo";
	SBDF_ASSERT(o == x);
	SBDF_ASSERT(o->count == 1);
	SBDF_ASSERT(strcmp((char const*)((void**)o->data)[0], "foo") == 0);
}
