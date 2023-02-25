/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "bytearray.h"

#include <stdlib.h>
#include <string.h>

#include "internals.h"

unsigned char* sbdf_ba_create(unsigned char const* str, int length)
{
	unsigned char* ptr = sbdf_allocate_array(length);
	if (ptr && str)
	{
		memcpy(ptr, str, length);
	}
	return ptr;
}

void sbdf_ba_destroy(unsigned char* str)
{
	sbdf_dispose_array(str);
}

int sbdf_ba_get_len(unsigned char const* str)
{
	return sbdf_get_array_length(str);
}

int sbdf_ba_memcmp(unsigned char const* lhs, unsigned char const* rhs)
{
	int ll;
	int rl;
	int min;
	int r;

	ll = sbdf_ba_get_len(lhs);
	rl = sbdf_ba_get_len(rhs);
	min = ll < rl ? ll : rl;
	r = memcmp(lhs, rhs, min);
	if (r) return r;
	return ll - rl;
}
