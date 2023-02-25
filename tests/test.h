/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_A6B836CA_9B1D_4745_9716_595D6694E80E
#define SBDF_A6B836CA_9B1D_4745_9716_595D6694E80E

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>

#include "cppsupp.h"

class tmpf
{
	std::string fn_;
	FILE* open_;

	tmpf(tmpf const&);
	tmpf& operator=(tmpf const&);
public:
	tmpf();
	~tmpf();

	FILE* read();
	FILE* write();

	std::vector<unsigned char> read_vec();
	void write_vec(std::vector<unsigned char> const&);

	void set(unsigned char* t, size_t n);

	template <int N>
	void set(unsigned char (&t)[N])
	{
		set(t, N);
	}

	int pos() const;
};

std::vector<unsigned char> read_vec(FILE* f);

template <int N>
std::vector<unsigned char> vec(unsigned char const (&t)[N])
{
	return std::vector<unsigned char>(t, t + N);
}

template <typename T, int N>
int array_len(T const (&a)[N])
{
	return N;
}


template <typename T, int N>
bool check_arrays(T const (&a)[N], std::vector<T> const& vec)
{
	if (vec.size() != N)
	{
		return false;
	}

	for (int i = 0; i < N; ++i)
	{
		if (vec[i] != a[i])
		{
			return false;
		}
	}

	return true;
}

template <typename T, int N>
bool check_arrays(T const (&a)[N], sbdf::obj const& obj)
{
	return check_arrays(a, obj.as_vec<T>());
}


template <int N>
bool check_arrays(char const* const (&a)[N], sbdf::obj const& obj)
{
	// check char arrays...
	if (obj->type.id != SBDF_STRINGTYPEID)
	{
		return false;
	}

	if (obj->count != N)
	{
		return false;
	}

	for (int i = 0; i < N; ++i)
	{
		if (strcmp(a[i], static_cast<char const**>(obj->data)[i]))
		{
			return false;
		}
	}
	return true;
}

void print_object(FILE* dest, sbdf_object const* obj);

#define SBDF_TEST(category, name) void test_##category##_##name()

#define SBDF_ASSERT(cond) \
	do { \
		if(!(cond)) { \
			char buf[4096]; \
			snprintf(buf, 4096, "%s(%d): Test '%s' failed\n", __FILE__, __LINE__, #cond); \
			throw std::runtime_error(buf); \
		} \
	} while(0)

#define SBDF_CHECK(operation, expected_result) \
	do { \
		int r = (operation); \
		if(r != (expected_result)) { \
			char buf[4096]; \
			snprintf(buf, 4096, "%s(%d): Operation '%s' returned '%s', expected '%s'\n", __FILE__, __LINE__, #operation, sbdf_err_get_str(r), sbdf_err_get_str(expected_result)); \
			throw std::runtime_error(buf); \
		} \
	} while(0)

#endif
