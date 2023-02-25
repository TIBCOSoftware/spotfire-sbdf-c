/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_4F2D4DC5_FD97_450e_A549_199729DDA4EA
#define SBDF_4F2D4DC5_FD97_450e_A549_199729DDA4EA

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the head of a metadata structure */
typedef struct sbdf_metadata_head
{
	struct sbdf_metadata* first; /* points to the first metadata entry */
	int modifiable;              /* determines if the metadata entries may be modified */
} sbdf_metadata_head;

/* a metadata element */
typedef struct sbdf_metadata
{
	struct sbdf_metadata* next;        /* points to the next metadata entry */
	char* name;                        /* the name of the metadata entry */
	struct sbdf_object* value;         /* the value of the metadata entry */
	struct sbdf_object* default_value; /* the default value of the metadata entry. may be null */
} sbdf_metadata;

/* creats an empty metadata structure */
SBDF_API int sbdf_md_create(sbdf_metadata_head** out);

/* adds a named string metadata value and default value to out */
SBDF_API int sbdf_md_add_str(char const* name, char const* value, char const* default_value, sbdf_metadata_head* out);

/* adds a named integer metadata value and default value to out */
SBDF_API int sbdf_md_add_int(char const* name, int value, int default_value, sbdf_metadata_head* out);

/* adds a named metadata value and default value to out */
SBDF_API int sbdf_md_add(char const* name, struct sbdf_object const* value, struct sbdf_object const* default_value, sbdf_metadata_head* out);

/* removes the named metadata value from out */
SBDF_API int sbdf_md_remove(char const* name, sbdf_metadata_head* out);

/* gets a copy of the named metadata value */
SBDF_API int sbdf_md_get(char const* name, sbdf_metadata_head const* meta, struct sbdf_object** out);

/* gets a copy of the named default metadata value */
SBDF_API int sbdf_md_get_dflt(char const* name, sbdf_metadata_head const* meta, struct sbdf_object** default_out);

/* destroys the metadata head and all its entries */
SBDF_API void sbdf_md_destroy(sbdf_metadata_head* out);

/* returns the number of metadata entries pointed to by head */
SBDF_API int sbdf_md_cnt(sbdf_metadata_head const* head);

/* returns a positive value if the named metadata exists. zero is returned if the metadata doesn't exist */
SBDF_API int sbdf_md_exists(char const* name, sbdf_metadata_head const* head);

/* copies metadata from head to out. */
SBDF_API int sbdf_md_copy(sbdf_metadata_head const* head, sbdf_metadata_head* out);

/* sets the metadata immutable so that it may not be modified by subsequent operations */
SBDF_API int sbdf_md_set_immutable(sbdf_metadata_head* metadata);

/* compares the metadata name in lhs with the metadata name in rhs */
/* returns negative value if lhs is smaller */
/* returns positive value if rhs is smaller */
/* return zero if lhs equals rhs */
SBDF_API int sbdf_md_compare_names(struct sbdf_metadata const** lhs, struct sbdf_metadata const** rhs);

#ifdef __cplusplus
}
#endif

#endif
