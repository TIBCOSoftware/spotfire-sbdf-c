/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_F6961445_CD3D_4314_A009_49667B8F9BE4
#define SBDF_F6961445_CD3D_4314_A009_49667B8F9BE4

#define SBDF_TEST_C(category, name) int test_c_##category##_##name(char *buf, int buf_len)

#define SBDF_ASSERT_C(cond) \
	do { \
		if(!(cond)) { \
			snprintf(buf, buf_len, "%s(%d): Test '%s' failed\n", __FILE__, __LINE__, #cond); \
			return 0; \
		} \
	} while(0)

#define SBDF_CHECK_C(operation, expected_result) \
	do { \
		int r = (operation); \
		if(r != (expected_result)) { \
			snprintf(buf, buf_len, "%s(%d): Operation '%s' returned '%s', expected '%s'\n", __FILE__, __LINE__, #operation, sbdf_err_get_str(r), sbdf_err_get_str(expected_result)); \
			return 0; \
		} \
	} while(0)

#endif
