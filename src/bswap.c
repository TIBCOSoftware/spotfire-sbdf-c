/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "bswap.h"

#include <string.h>

#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MISPEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc) 
#define BIG_ENDIAN
#undef LITTLE_ENDIAN
#else
#define LITTLE_ENDIAN
#undef BIG_ENDIAN
#endif

void sbdf_swap(void* inout, int sz, int count)
{
#ifdef LITTLE_ENDIAN
	/* do nothing */
#else
	while (count-- > 0)
	{
		char* front = (char*)inout;
		char* back = (char*)inout + sz;
		int i;
		for (i = sz / 2; i > 0; --i)
		{
			char tmp = *front;
			*front++ = *--back;
			*back = tmp;
		}
		inout = (char*)inout + sz;
	}
#endif
}
