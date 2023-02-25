/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "test.h"

#include "cppsupp.h"

#include "all_io.h"

using namespace sbdf;

class test_table
{
	char const** column_names_;
	sbdf_valuetype* value_types_;
	obj* data_columns_;
	bool** empty_values_;
	bool** replaced_values_;
	char const*** error_values_;
	int cols_;
	int rows_;

	static char const* _3x3_names_[];
	static sbdf_valuetype _3x3_types_[];
	
	static int _3x3_col1_[];
	static char const* _3x3_col2_[];
	static double _3x3_col3_[];
	static obj _3x3_values_[];

	static bool _3x3_empties1_[];
	static bool* _3x3_empties_[];

	static char const* _2x4_names_[];
	static sbdf_valuetype _2x4_types_[];
	
	static int _2x4_col1_[];
	static char const* _2x4_col2_[];
	static double _2x4_col3_[];
	static int64_t _2x4_col4_[];
	static obj _2x4_values_[];
	
	static bool _2x4_replaced3_[];
	static bool* _2x4_replaced_[];

	static char const* _2x4_errors4_[];
	static char const** _2x4_errors_[];

public:
	test_table(char const** column_names,
		sbdf_valuetype* value_types,
		obj* data_columns,
		bool** empty_values,
		bool** replaced_values,
		char const*** error_values,
		int cols,
		int rows) : column_names_(column_names),
                    value_types_(value_types),
				    data_columns_(data_columns),
				    empty_values_(empty_values),
				    replaced_values_(replaced_values),
				    error_values_(error_values),
					cols_(cols),
					rows_(rows)
	{
	}

	static const test_table _3x3_; // 3 rows, 4 columns
	
	static const test_table _2x4_; // 2 columns, 4 rows

	int cols() const
	{
		return cols_;
	}

	int rows() const
	{
		return rows_;
	}

	char const* col_name(int col) const
	{
		return column_names_[col];
	}

	sbdf_valuetype col_type(int col) const
	{
		return data_columns_[col]->type;
	}

	obj const& values(int col) const
	{
		return data_columns_[col];
	}

	char const** errors(int col) const
	{
		return error_values_?error_values_[col]:0;
	}

	bool* empties(int col) const
	{
		return empty_values_?empty_values_[col]:0;
	}

	bool* replaced(int col) const
	{
		return replaced_values_?replaced_values_[col]:0;
	}
};

template <int N>
static obj make_dates(int64_t const (&val)[N])
{
	obj o(val);
	o->type.id = SBDF_DATETYPEID;
	return o;
}

#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MISPEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc__) 
static unsigned char nan_value[8]={0xff, 0xf8, 0, 0, 0, 0, 0, 0 };
#else
static unsigned char nan_value[8]={0, 0, 0, 0, 0, 0, 0xf8, 0xff};
#endif


char const* test_table::_3x3_names_[] = { "A", "B", "C" };
sbdf_valuetype test_table::_3x3_types_[] = { { SBDF_INTTYPEID },  { SBDF_STRINGTYPEID }, { SBDF_DOUBLETYPEID } };
int test_table::_3x3_col1_[] = { 1, 2, 0 };
char const* test_table::_3x3_col2_[] = { "Foo", "Fie", "Fum" };
double test_table::_3x3_col3_[] = { 3.5, 100., *( double* )nan_value };
obj test_table::_3x3_values_[] = { obj(test_table::_3x3_col1_), obj(test_table::_3x3_col2_), obj(test_table::_3x3_col3_) };
bool test_table::_3x3_empties1_[] = { false, false, true };
bool* test_table::_3x3_empties_[] = { test_table::_3x3_empties1_, 0, 0 };

char const* test_table::_2x4_names_[] = { "A", "B", "C", "D" };
sbdf_valuetype test_table::_2x4_types_[] = { { SBDF_INTTYPEID },  { SBDF_STRINGTYPEID }, { SBDF_DOUBLETYPEID }, { SBDF_DATETYPEID } };
int test_table::_2x4_col1_[] = { 1, 2  };
char const* test_table::_2x4_col2_[] = { "Foo", "Fie" };
double test_table::_2x4_col3_[] = { 3.5, 100. };
int64_t test_table::_2x4_col4_[] = { 12345, 0 };
obj test_table::_2x4_values_[] = { obj(test_table::_2x4_col1_), obj(test_table::_2x4_col2_), obj(test_table::_2x4_col3_), make_dates(test_table::_2x4_col4_) };
bool test_table::_2x4_replaced3_[] = { true, false };
bool* test_table::_2x4_replaced_[] = { 0, 0, _2x4_replaced3_, 0 };
char const* test_table::_2x4_errors4_[] = { "", "illegal value" };
char const** test_table::_2x4_errors_[] = { 0, 0, 0, test_table::_2x4_errors4_ };

const test_table test_table::_3x3_(test_table::_3x3_names_, test_table::_3x3_types_, test_table::_3x3_values_, test_table::_3x3_empties_, 0, 0, 3, 3);
const test_table test_table::_2x4_(test_table::_2x4_names_, test_table::_2x4_types_, test_table::_2x4_values_, 0, test_table::_2x4_replaced_, test_table::_2x4_errors_, 4, 2);

sbdf_tablemetadata* create_meta(test_table const& tbl)
{
	int error = SBDF_OK;
	sbdf_metadata_head* table_meta_head = 0;
	sbdf_tablemetadata* table_meta = 0;

	do
	{
		// create the table metadata head (empty in this example)
		if (error = sbdf_md_create(&table_meta_head))
		{
			break;
		}

		// create the table metadata, storing a copy of table_meta_head
		if (error = sbdf_tm_create(table_meta_head, &table_meta))
		{
			break;
		}

		// destroys the table specific metadata
		sbdf_md_destroy(table_meta_head);

		// build and add the column metadata
		for (int i = 0; i < tbl.cols(); ++i)
		{
			sbdf_metadata_head* column_metadata;

			// create an empty metadata structure
			if (error = sbdf_md_create(&column_metadata))
			{
				break;
			}

			// sets the name and datatype column properties
			if (error = sbdf_cm_set_values(tbl.col_name(i), tbl.col_type(i), column_metadata))
			{
				sbdf_md_destroy(column_metadata);
				break;
			}

			// adds a copy of the column metadat to the table metadata
			if (error = sbdf_tm_add(column_metadata, table_meta))
			{
				sbdf_md_destroy(column_metadata);
				break;
			}

			// destroy the original column metadata
			sbdf_md_destroy(column_metadata);
		}

		if (error)
		{
			break;
		}
	} while (0);

	if (error)
	{
		if (table_meta)
		{
			sbdf_tm_destroy(table_meta);
		}

		if (table_meta_head)
		{
			sbdf_md_destroy(table_meta_head);
		}

		return 0;
	}

	return table_meta;
}

bool compare_meta(sbdf_metadata_head const* lhs, sbdf_metadata_head const* rhs)
{
	sbdf_metadata const* l = lhs->first;
	sbdf_metadata const* r = rhs->first;
	int r_cnt = 0, l_cnt = 0;

	while (r)
	{
		++r_cnt;
		r = r->next;
	}

	while (l)
	{
		++l_cnt;
		l = l->next;
	}

	if (l_cnt != r_cnt)
	{
		return false;
	}

	l = lhs->first;
	while (l)
	{
		for (r = rhs->first; r; r = r->next)
		{
			if (!strcmp(l->name, r->name))
			{
				if (!sbdf_obj_eq(l->value, r->value))
				{
					return false;
				}
				else
				{
					break;
				}
			}
		}

		if (!r)
		{
			// l was not found in r
			return false;
		}

		l = l->next;
	}

	return true;
}

int write_table(FILE* out, test_table const& tbl)
{
	int error = SBDF_OK;
	int i;
	int rows = tbl.rows();

	// create the table metadata
	sbdf_tablemetadata* table_meta = create_meta(tbl);

	sbdf_tableslice* slice = 0;

	do
	{
		// write the file header
		if (error = sbdf_fh_write_cur(out))
		{
			break;
		}

		// write the table metadata
		if (error = sbdf_tm_write(out, table_meta))
		{
			break;
		}

		// create the table slice
		if (error = sbdf_ts_create(table_meta, &slice))
		{
			break;
		}

		// iterate and write column slices
		for (i = 0; i < table_meta->no_columns; ++i)
		{
			sbdf_valuearray* values = 0;
			sbdf_columnslice* cs = 0;

			// create a valuearray for the column values
			if (error = sbdf_va_create_dflt(tbl.values(i), &values))
			{
				break;
			}

			// create a column slice with the valeus
			if (error = sbdf_cs_create(&cs, values))
			{
				sbdf_va_destroy(values);
				break;
			}

			// add the error codes as a value property
			if (tbl.errors(i))
			{
				sbdf_object* obj;
				sbdf_valuearray* errors;
				sbdf_valuetype vt = { SBDF_STRINGTYPEID };

				// create an sbdf object array containing the error codes
				if (error = sbdf_obj_create_arr(vt, rows, tbl.errors(i), 0, &obj))
				{
					break;
				}

				// creates a value array from the sbdf object array
				if (error = sbdf_va_create_dflt(obj, &errors))
				{
					sbdf_obj_destroy(obj);
					break;
				}

				// destroy the original object arrray
				sbdf_obj_destroy(obj);

				// adds the named error code value property to the column slice
				if (error = sbdf_cs_add_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, errors))
				{
					sbdf_va_destroy(errors);
					break;
				}
			}

			// add the 'is empty' property as a value property
			if (tbl.empties(i))
			{
				sbdf_object* obj;
				sbdf_valuearray* isempty;
				sbdf_valuetype vt = { SBDF_BOOLTYPEID };

				// create an sbdf object array containing the 'is empty' property
				if (error = sbdf_obj_create_arr(vt, rows, tbl.empties(i), 0, &obj))
				{
					break;
				}

				// creates a value array from the sbdf object array
				if (error = sbdf_va_create_dflt(obj, &isempty))
				{
					sbdf_obj_destroy(obj);
					break;
				}

				// destroy the original object arrray
				sbdf_obj_destroy(obj);

				// adds the named 'is empty' value property to the column slice
				if (error = sbdf_cs_add_property(cs, SBDF_ISINVALID_VALUEPROPERTY, isempty))
				{
					sbdf_va_destroy(isempty);
					break;
				}
			}

			// add the 'is replaced' property as a value property
			if (tbl.replaced(i))
			{
				sbdf_object* obj;
				sbdf_valuearray* isreplaced;
				sbdf_valuetype vt = { SBDF_BOOLTYPEID };

				// create an sbdf object array containing the 'is replaced' property
				if (error = sbdf_obj_create_arr(vt, rows, tbl.replaced(i), 0, &obj))
				{
					break;
				}

				// creates a value array from the sbdf object array
				if (error = sbdf_va_create_dflt(obj, &isreplaced))
				{
					sbdf_obj_destroy(obj);
					break;
				}

				// destroy the original object arrray
				sbdf_obj_destroy(obj);

				// adds the named 'is replaced' value property to the column slice
				if (error = sbdf_cs_add_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, isreplaced))
				{
					sbdf_va_destroy(isreplaced);
					break;
				}
			}

			// adds the column slice to the table slice
			if (error = sbdf_ts_add(cs, slice))
			{
				sbdf_cs_destroy_all(cs);
				break;
			}
		}

		// writes the table slice to the output file
		if (error = sbdf_ts_write(out, slice))
		{
			break;
		}

		// writes the end-of-table marker to the output file
		if (error = sbdf_ts_write_end(out))
		{
			break;
		}
	}
	while (0);

	if (slice)
	{
		// completely destroy all column slices and values
		for (i = 0; i < slice->no_columns; ++i)
		{
			sbdf_cs_destroy_all(slice->columns[i]);
		}
		sbdf_ts_destroy(slice);
		slice = 0;
	}

	if (table_meta)
	{
		sbdf_tm_destroy(table_meta);
		table_meta = 0;
	}

	return error;
}

int read_table(FILE* in, test_table const& orig)
{
	int error, major, minor, i;
	sbdf_tablemetadata* table_meta = 0;

	// reads the sbdf file header
	if (error = sbdf_fh_read(in, &major, &minor))
	{
		return error;
	}

	// determine if this is a known and supported version
	if (major != 1 || minor != 0)
	{
		return SBDF_ERROR_UNKNOWN_VERSION;
	}

	// read the table metadata from file
	if (error = sbdf_tm_read(in, &table_meta))
	{
		return error;
	}

	// verify that the read metadata equals the original metadata
	sbdf_tablemetadata* orig_meta = create_meta(orig);
	SBDF_ASSERT(compare_meta(table_meta->table_metadata, orig_meta->table_metadata));
	SBDF_ASSERT(orig.cols() == table_meta->no_columns);

	for (i = 0; i < table_meta->no_columns; ++i)
	{
		SBDF_ASSERT(compare_meta(table_meta->column_metadata[i], orig_meta->column_metadata[i]));
	}

	sbdf_tm_destroy(orig_meta);

	// read the one and only table slice from file
	sbdf_tableslice* ts = 0;
	error = sbdf_ts_read(in, table_meta, 0, &ts);
	if (error)
	{
		sbdf_tm_destroy(table_meta);
		return error;
	}

	// verify table contents
	for (i = 0; i < ts->no_columns; ++i)
	{
		sbdf_valuearray* va;
		sbdf_columnslice* cs = ts->columns[i];
		obj o;

		// check that the values math
		SBDF_CHECK(sbdf_va_get_values(cs->values, o.obj_ptr()), SBDF_OK);
		SBDF_ASSERT(!!sbdf_obj_eq(orig.values(i), o));

		// check that the error codes match
		if (orig.errors(i))
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, &va), SBDF_OK);
			SBDF_CHECK(sbdf_va_get_values(va, o.obj_ptr()), SBDF_OK);
			char const** vals = static_cast<char const**>(o->data);
			for (int j = 0; j < orig.rows(); ++j)
			{
				SBDF_ASSERT(!strcmp(vals[j], orig.errors(i)[j]));
			}
		}
		else
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ERRORCODE_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);
		}

		// check that the 'is empty' properties match
		if (orig.empties(i))
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ISINVALID_VALUEPROPERTY, &va), SBDF_OK);
			SBDF_CHECK(sbdf_va_get_values(va, o.obj_ptr()), SBDF_OK);
			obj o2(orig.empties(i), orig.rows());
			SBDF_ASSERT(!!sbdf_obj_eq(o, o2));
		}
		else
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_ISINVALID_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);
		}

		// check that the 'is replaced' properties match
		if (orig.replaced(i))
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, &va), SBDF_OK);
			SBDF_CHECK(sbdf_va_get_values(va, o.obj_ptr()), SBDF_OK);
			obj o2(orig.replaced(i), orig.rows());
			SBDF_ASSERT(!!sbdf_obj_eq(o, o2));
		}
		else
		{
			SBDF_CHECK(sbdf_cs_get_property(cs, SBDF_REPLACEDVALUE_VALUEPROPERTY, &va), SBDF_ERROR_PROPERTY_NOT_FOUND);
		}
	}

	// release the table slice resources
	sbdf_ts_destroy(ts);

	// check that there are no more table slices to read
	SBDF_CHECK(sbdf_ts_read(in, table_meta, 0, &ts), SBDF_TABLEEND);

	// release the table metadata information
	if (table_meta)
	{
		sbdf_tm_destroy(table_meta);
		table_meta = 0;
	}

	return SBDF_OK;
}

SBDF_TEST(advanced_api, test)
{
	tmpf f;
	SBDF_CHECK(write_table(f.write(), test_table::_3x3_), SBDF_OK);
	SBDF_CHECK(read_table(f.read(), test_table::_3x3_), SBDF_OK);
	FILE* fi = fopen(TESTSDIR "/3x3_cs.sbdf", "rb");
	f.write_vec(read_vec(fi));
	fclose(fi);
	SBDF_CHECK(read_table(f.read(), test_table::_3x3_), SBDF_OK);

	SBDF_CHECK(write_table(f.write(), test_table::_2x4_), SBDF_OK);
	SBDF_CHECK(read_table(f.read(), test_table::_2x4_), SBDF_OK);
	fi = fopen(TESTSDIR "/2x4_cs.sbdf", "rb");
	f.write_vec(read_vec(fi));
	fclose(fi);
	SBDF_CHECK(read_table(f.read(), test_table::_2x4_), SBDF_OK);
}
