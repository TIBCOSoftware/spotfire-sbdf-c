/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "bytearray.h"

SBDF_TEST(bytearray, test)
{
	unsigned char* s, *s2;
	unsigned char foo[] = { 3, 1, 4, 1, 5, 9, 2 };
	s = sbdf_ba_create(foo, 3);
	SBDF_ASSERT(memcmp(foo, s, 3) == 0);
	SBDF_ASSERT(sbdf_ba_get_len(s) == 3);
	SBDF_ASSERT(sbdf_ba_memcmp(s, s) == 0);
	
	s2 = sbdf_ba_create(foo, 7);
	SBDF_ASSERT(memcmp(foo, s2, 7) == 0);
	SBDF_ASSERT(sbdf_ba_get_len(s2) == 7);
	SBDF_ASSERT(sbdf_ba_memcmp(s, s2) < 0);

	sbdf_ba_destroy(s);
	sbdf_ba_destroy(s2);
}
