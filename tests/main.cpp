/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include <cstring>
#include <iostream>

#include "test.h"
#include "runtest_tests.h"

int main(int argc, char **argv)
{
	if(argc < 1)
	{
		std::cout << "Must specify test to run.\n";
		return 2;
	}

	// Try C++ tests first
	for(test_list_cpp::iterator i = list1.begin(); i != list1.end(); i++)
	{
		if(strcmp(i->first, argv[1]) == 0)
		{
			try
			{
				i->second();
				return 0;
			}
			catch (const std::runtime_error &e)
			{
				std::cerr << e.what();
				return 1;
			}
		}
	}

	// Try C tests next
	for (test_list_c::iterator i = list2.begin(); i != list2.end(); i++)
	{
		if (strcmp(i->first, argv[1]) == 0)
		{
			char buf[4096];
			if (i->second(buf, 4096))
			{
				return 0;
			}
			else
			{
				std::cerr << buf;
				return 1;
			}
		}
	}

	// Fall through if test cannot be found.
	std::cout << "Cannot find test '" << argv[1] << "' to run.\n";
	return 2;
}
