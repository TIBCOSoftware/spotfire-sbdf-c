/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "test.h"

#include "tablemetadata.h"
#include "tablemetadata_io.h"
#include "columnmetadata.h"
#include "metadata.h"
#include "errors.h"

#include "cppsupp.h"

using namespace sbdf;

SBDF_TEST(tablemetadata, test)
{
	sbdf_valuetype vt_s = { SBDF_STRINGTYPEID };
	sbdf_valuetype vt_i = { SBDF_INTTYPEID };

	tmpf f;
	{
		sbdf_metadata_head* h, *c1, *c2;
		sbdf_tablemetadata* table;
		SBDF_CHECK(sbdf_md_create(&h), SBDF_OK);
		SBDF_CHECK(sbdf_md_add_str("foo", "bar", "baz", h), SBDF_OK);

		SBDF_CHECK(sbdf_tm_create(h, &table), SBDF_OK);

		SBDF_CHECK(sbdf_md_create(&c1), SBDF_OK);
		SBDF_CHECK(sbdf_cm_set_values("col1", vt_s, c1), SBDF_OK);
		
		SBDF_CHECK(sbdf_md_create(&c2), SBDF_OK);
		SBDF_CHECK(sbdf_cm_set_values("col2", vt_i, c2), SBDF_OK);
		SBDF_CHECK(sbdf_md_add("pi", obj(3.14), obj(3.), c2), SBDF_OK);

		SBDF_CHECK(sbdf_tm_add(c1, table), SBDF_OK);
		SBDF_CHECK(sbdf_tm_add(c2, table), SBDF_OK);

		sbdf_md_destroy(h);
		sbdf_md_destroy(c1);
		sbdf_md_destroy(c2);

		SBDF_CHECK(sbdf_tm_write(f.write(), table), SBDF_OK);

		sbdf_tm_destroy(table);
	}
	{
		sbdf_tablemetadata* table = 0;

		SBDF_CHECK(sbdf_tm_read(f.read(), &table), SBDF_OK);

		sbdf_tm_destroy(table);
	}
}
