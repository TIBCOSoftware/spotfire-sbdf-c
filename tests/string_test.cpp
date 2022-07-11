/* Copyright (c) 2022 TIBCO Software Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "sbdfstring.h"

#if _MSC_VER
#include <windows.h>
#endif

SBDF_TEST(string, test)
{
	char* s, *s2;
	s = sbdf_str_create("foo\0bar");
	SBDF_ASSERT(strcmp("foo", s) == 0);
	SBDF_ASSERT(sbdf_str_len(s) == 3);
	SBDF_ASSERT(sbdf_str_cmp(s, s) == 0);
	
	s2 = sbdf_str_create_len("foo\0bar", 7);
	SBDF_ASSERT(s2[3] == 0);
	SBDF_ASSERT(s2[7] == 0);
	SBDF_ASSERT(strcmp("foo", s2) == 0);
	SBDF_ASSERT(sbdf_str_len(s2) == 7);
	SBDF_ASSERT(sbdf_str_cmp(s, s2) < 0);

	sbdf_str_destroy(s);
	sbdf_str_destroy(s2);

	char iso_alphabet[256];
	iso_alphabet[255] = 0;
	for (int i = 0; i < 255; ++i)
	{
		iso_alphabet[i] = static_cast<char>(i + 1);
	}

	int len = sbdf_convert_iso88591_to_utf8(iso_alphabet, 0);

	std::vector<char> utf8(len);

	sbdf_convert_iso88591_to_utf8(iso_alphabet, &utf8[0]);

	std::vector<char> utf82;

#if _MSC_VER
	// convert iso 8859-1 to utf 8 by using windows functions.
	int wle = MultiByteToWideChar(28591, 0, iso_alphabet, -1, 0, 0); // 28591 is CP_28591, iso-8859-1
	++wle;
	std::vector<wchar_t> wide(wle);
	MultiByteToWideChar(28591, 0, iso_alphabet, -1, &wide[0], wle);

	int utf8l = WideCharToMultiByte(CP_UTF8, 0, &wide[0], -1, 0, 0, 0, 0);
	utf82.resize(utf8l);
	WideCharToMultiByte(CP_UTF8, 0, &wide[0], -1, &utf82[0], utf8l, 0, 0);

	SBDF_ASSERT(utf8.size() == utf82.size());
	if (utf8.size() == utf82.size())
	{
		bool utf8_match = true;
		for (unsigned int i = 0; i < utf8.size(); ++i)
		{
			if (utf8[i] != utf82[i])
			{
				utf8_match = false;
				break;
			}
		}

		SBDF_ASSERT(utf8_match);
	}

	len = sbdf_convert_utf8_to_iso88591(&utf8[0], 0);

	std::vector<char> iso8859(len);

	sbdf_convert_utf8_to_iso88591(&utf8[0], &iso8859[0]);

	SBDF_ASSERT(iso8859.size() == 256);
	if (iso8859.size() == 256)
	{
		bool iso88591_match = true;
		for (unsigned int i = 0; i < iso8859.size(); ++i)
		{
			if (iso8859[i] != iso_alphabet[i])
			{
				iso88591_match = false;
				break;
			}
		}

		SBDF_ASSERT(iso88591_match);
	}
#endif
}
