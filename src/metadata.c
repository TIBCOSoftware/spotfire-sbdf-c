/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#include "metadata.h"

#include <string.h>
#include <stdlib.h>

#include "object.h"
#include "errors.h"
#include "valuetypeid.h"
#include "valuetype.h"
#include "sbdfstring.h"

int sbdf_md_create(sbdf_metadata_head** out)
{
	sbdf_metadata_head* t = 0;

	if (!out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t = calloc(sizeof(sbdf_metadata_head), 1);

	if (!t)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	t->modifiable = 1;

	*out = t;
	return SBDF_OK;
}

int sbdf_md_add_str(char const* name, char const* value, char const* default_value, sbdf_metadata_head* out)
{
	sbdf_object* obj, *default_obj;
	int error;
	sbdf_valuetype vt = { SBDF_STRINGTYPEID };

	if (!value)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (error = sbdf_obj_create(vt, &value, 0, &obj))
	{
		return error;
	}
	if (default_value)
	{
		if (error = sbdf_obj_create(vt, &default_value, 0, &default_obj))
		{
			sbdf_obj_destroy(obj);
			return error;
		}
	}
	else
	{
		default_obj = 0;
	}
	error = sbdf_md_add(name, obj, default_obj, out);
	sbdf_obj_destroy(obj);
	if (default_obj)
	{
		sbdf_obj_destroy(default_obj);
	}
	return error;
}

int sbdf_md_add_int(char const* name, int value, int default_value, sbdf_metadata_head* out)
{
	sbdf_object* obj, *default_obj;
	int error;
	sbdf_valuetype vt = { SBDF_INTTYPEID };

	if (error = sbdf_obj_create(vt, &value, 0, &obj))
	{
		return error;
	}
	if (error = sbdf_obj_create(vt, &default_value, 0, &default_obj))
	{
		sbdf_obj_destroy(obj);
		return error;
	}
	error = sbdf_md_add(name, obj, default_obj, out);
	sbdf_obj_destroy(obj);
	sbdf_obj_destroy(default_obj);
	return error;
}

int sbdf_md_add(char const* name, sbdf_object const* value, sbdf_object const* default_value, sbdf_metadata_head* out)
{
	sbdf_metadata* item, *prev;
	int result;

	if (!out || !name || !value)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (!out->modifiable)
	{
		return SBDF_ERROR_METADATA_READONLY;
	}

	if (default_value && sbdf_vt_cmp(value->type, default_value->type))
	{
		return SBDF_ERROR_VALUETYPES_MUST_BE_EQUAL;
	}

    if (value->count != 1 || (default_value && default_value->count != 1))
    {
        return SBDF_ERROR_ARRAY_LENGTH_MUST_BE_1;
    }

	item = out->first;
	prev = 0;

	while (item)
	{
		if (!strcmp(name, item->name))
		{
			return SBDF_ERROR_METADATA_ALREADY_EXISTS;
		}

		prev = item;
		item = item->next;
	}

	item = calloc(1, sizeof(sbdf_metadata));
	if (!item)
	{
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	item->name = sbdf_str_create(name);
	if (!item->name)
	{
		free(item);
		return SBDF_ERROR_OUT_OF_MEMORY;
	}

	result = sbdf_obj_copy(value, &item->value);
	if (result < 0)
	{
		sbdf_str_destroy(item->name);
		free(item);
		return result;
	}

	if (default_value)
	{
		result = sbdf_obj_copy(default_value, &item->default_value);
		if (result < 0)
		{
			sbdf_obj_destroy(item->value);
			sbdf_str_destroy(item->name);
			free(item);
			return result;
		}
	}
	else
	{
		item->default_value = 0;
	}

	if (prev)
	{
		prev->next = item;
	}
	else
	{
		out->first = item;
	}

	return SBDF_OK;
}

int sbdf_md_remove(char const* name, sbdf_metadata_head* out)
{
	sbdf_metadata* item, *prev;

	if (!out || !name)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (!out->modifiable)
	{
		return SBDF_ERROR_METADATA_READONLY;
	}

	item = out->first;
	prev = 0;

	while (item)
	{
		if (!strcmp(name, item->name))
		{
			if (prev)
			{
				prev->next = item->next;
			}
			else
			{
				out ->first= item->next;
			}

			sbdf_obj_destroy(item->value);
			sbdf_obj_destroy(item->default_value);
			sbdf_str_destroy(item->name);
			free(item);

			return SBDF_OK;
		}

		prev = item;
		item = item->next;
	}

	return SBDF_OK;
}

int sbdf_md_get(char const* name, sbdf_metadata_head const* meta, sbdf_object** out)
{
	sbdf_metadata* t;

	if (!out || !name || !meta)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	for (t = meta->first; t; t = t->next)
	{
		if (!strcmp(name, t->name))
		{
			sbdf_obj_copy(t->value, out);
			return SBDF_OK;
		}
	}

	return SBDF_ERROR_METADATA_NOT_FOUND;
}


int sbdf_md_get_dflt(char const* name, sbdf_metadata_head const* meta, sbdf_object** default_out)
{
	sbdf_metadata* t;

	if (!default_out || !name || !meta)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	for (t = meta->first; t; t = t->next)
	{
		if (!strcmp(name, t->name))
		{
			if (t->default_value)
			{
				sbdf_obj_copy(t->default_value, default_out);
			}
			else
			{
				*default_out = 0;
			}

			return SBDF_OK;
		}
	}

	return SBDF_ERROR_METADATA_NOT_FOUND;
}

void sbdf_md_destroy(sbdf_metadata_head* out)
{
	sbdf_metadata const* head;

	if (out)
	{
		head = out->first;

		while (head)
		{
			sbdf_metadata const* next = head->next;
			sbdf_obj_destroy(head->value);
			sbdf_obj_destroy(head->default_value);
			sbdf_str_destroy(head->name);
			free((void*)head);
			head = next;
		}

		free(out);
	}
}

int sbdf_md_exists(char const* name, sbdf_metadata_head const* head)
{
	sbdf_metadata* t;

	if (!name || !head)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	for (t = head->first; t; t = t->next)
	{
		if (!strcmp(name, t->name))
		{
			return 1;
		}
	}

	return 0;
}

int sbdf_md_cnt(sbdf_metadata_head const* head)
{
	sbdf_metadata* t;
	int result = 0;

	if (!head)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	for (t = head->first; t; t = t->next)
	{
		++result;
	}

	return result;
}

int sbdf_md_copy(sbdf_metadata_head const* head, sbdf_metadata_head* out)
{
	int error;
	sbdf_metadata* prev, *first, *iter;

	if (!head || !out)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	if (!out->modifiable)
	{
		return SBDF_ERROR_METADATA_READONLY;
	}

	/* check that no items in head are in out */
	for (first = head->first; first; first = first->next)
	{
		for (prev = out->first; prev; prev = prev->next)
		{
			if (!strcmp(first->name, prev->name))
			{
				return SBDF_ERROR_METADATA_ALREADY_EXISTS;
			}
		}
	}

	first = out->first;
	prev = first;
	if (prev)
	{
		while (prev->next)
		{
			prev = prev->next;
		}
	}

	for (iter = head->first; iter; iter = iter->next)
	{
		sbdf_metadata* t = calloc(sizeof(sbdf_metadata), 1);
		if (!t)
		{
			return SBDF_ERROR_OUT_OF_MEMORY;
		}

		t->name = sbdf_str_copy(iter->name);
		if (!t->name)
		{
			free(t);
			return SBDF_ERROR_OUT_OF_MEMORY;
		}

		error = sbdf_obj_copy(iter->value, &t->value);
		if (error)
		{
			sbdf_str_destroy(t->name);
			free(t);
			return error;
		}

		if (iter->default_value)
		{
			error = sbdf_obj_copy(iter->default_value, &t->default_value);
			if (error)
			{
				sbdf_obj_destroy(t->value);
				sbdf_str_destroy(t->name);
				free(t);
				return error;
			}
		}
		else
		{
			t->default_value = 0;
		}

		if (prev)
		{
			prev->next = t;
		}
		else
		{
			first = t;
		}

		prev = t;
	}

	out->first = first;
	return SBDF_OK;
}

int sbdf_md_set_immutable(sbdf_metadata_head* metadata)
{
	if (!metadata)
	{
		return SBDF_ERROR_ARGUMENT_NULL;
	}

	metadata->modifiable = 0;

	return SBDF_OK;
}

int sbdf_md_compare_names(sbdf_metadata const** lhs, sbdf_metadata const** rhs)
{
	return strcmp((*lhs)->name, (*rhs)->name);
}
