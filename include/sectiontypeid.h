/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_25A65D91_1F96_4fee_808E_BFB1BBFB32AB
#define SBDF_25A65D91_1F96_4fee_808E_BFB1BBFB32AB

#include "config.h"

/* An unknown section type. */
#define SBDF_UNKNOWN_SECTIONID 0x0

/* A file header section. */
#define SBDF_FILEHEADER_SECTIONID 0x1

/* A table metadata section, marking the beginning of a complete table. */
#define SBDF_TABLEMETADATA_SECTIONID 0x2

/* A table slice section. */
#define SBDF_TABLESLICE_SECTIONID 0x3

/* A column slice section. */
#define SBDF_COLUMNSLICE_SECTIONID 0x4

/* Marks the end of a complete data table. */
#define SBDF_TABLEEND_SECTIONID 0x5

#endif
