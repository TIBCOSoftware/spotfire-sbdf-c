/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_4567D4E6_7E8A_455a_AA12_6CAD53663198
#define SBDF_4567D4E6_7E8A_455a_AA12_6CAD53663198

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sbdf_valuetype
{
	int id;        /* the valuetypeid */
} sbdf_valuetype;

/* compares to valuetypes. returns negative value ifs lhs < rhs, positive value if lhs > rhs, zero if equal */
SBDF_API int sbdf_vt_cmp(sbdf_valuetype lhs, sbdf_valuetype rhs);

/* returns a bool valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_bool();

/* returns an int valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_int();

/* returns a long valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_long();

/* returns a float valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_float();

/* returns a double valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_double();

/* returns a datetime valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_datetime();

/* returns a date valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_date();

/* returns a time valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_time();

/* returns a timespan valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_timespan();

/* returns a string valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_string();

/* returns a binary valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_binary();

/* returns a decimal valuetype struct */
SBDF_API sbdf_valuetype sbdf_vt_decimal();

#ifdef __cplusplus
}
#endif

#endif
