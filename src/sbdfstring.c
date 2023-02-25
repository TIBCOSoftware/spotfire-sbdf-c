/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "sbdfstring.h"

#include <stdlib.h>
#include <string.h>

#include "internals.h"

char* sbdf_str_create(char const* str)
{
	return sbdf_str_create_len(str, (int)strlen(str));
}

char* sbdf_str_create_len(char const* str, int length)
{
	char* ptr = sbdf_allocate_array(1 + length);
	if (ptr)
	{
		ptr[length] = 0;
		if (str)
		{
			memcpy(ptr, str, length);
		}
	}

	return ptr;
}

void sbdf_str_destroy(char* str)
{
	sbdf_dispose_array(str);
}

int sbdf_str_len(char const* str)
{
	return sbdf_get_array_length(str) - 1;
}

int sbdf_str_cmp(char const* lhs, char const* rhs)
{
	int ll;
	int rl;
	int min;
	int r;

	ll = sbdf_str_len(lhs);
	rl = sbdf_str_len(rhs);
	min = ll < rl ? ll : rl;
	r = memcmp(lhs, rhs, min);
	if (r) return r;
	return ll - rl;
}

char* sbdf_str_copy(char const* inp)
{
	return sbdf_str_create_len(inp, sbdf_str_len(inp));
}

#define REPLACEMENT_CHAR 0x1a

int sbdf_convert_utf8_to_iso88591(char const* inp, char* out)
{
	int result = 0;
	unsigned char ch;

	while (ch = *inp++)
	{
		if (ch <= 0x7f)
		{
			if (out)
			{
				*out++ = ch;
			}

			++result;
		}
		else if (ch >= 0xc0 && ch < 0xdf)
		{
			int uch = (ch & 0x1f) << 6;
			ch = *inp++;
			uch += ch & 0x3f;
			if ((ch & 0xc0) != 0x80 || uch >= 0x100)
			{
				if (out)
				{
					*out++ = REPLACEMENT_CHAR;
				}
			}
			else
			{
				if (out)
				{
					*out++ = uch;
				}
			}

			++result;
		}
		else
		{
			while ((*inp & 0xc0) == 0x80)
			{
				++inp;
			}

			if (out)
			{
				*out++ = REPLACEMENT_CHAR;
			}

			++result;
		}
	}

	if (out)
	{
		*out++ = 0;
	}

	++result;

	return result;
}

int sbdf_convert_iso88591_to_utf8(char const* inp, char* out)
{
	int result = 0;

	unsigned char ch;

	while (ch = *inp++)
	{
		if (ch <= 0x7f)
		{
			if (out)
			{
				*out++ = ch;
			}

			++result;
		}
		else
		{
			if (out)
			{
				*out++ = 0xc0 | (ch >> 6);
				*out++ = 0x80 | (ch & 0x3f);
			}

			result += 2;
		}
	}

	if (out)
	{
		*out++ = 0;
	}

	++result;

	return result;
}
