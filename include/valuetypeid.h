/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_D5A81383_1774_4bf9_8505_20E0591236A3
#define SBDF_D5A81383_1774_4bf9_8505_20E0591236A3

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SBDF_UNKNOWNTYPEID 0x00
#define SBDF_BOOLTYPEID 0x01      /* C type is char */
#define SBDF_INTTYPEID 0x02       /* C type is 32-bit int */
#define SBDF_LONGTYPEID 0x03      /* C type is 64-bit int */

#define SBDF_FLOATTYPEID 0x04     /* C type is float */
#define SBDF_DOUBLETYPEID 0x05    /* C type is double */
#define SBDF_DATETIMETYPEID 0x06  /* C representation is milliseconds since 01/01/01, 00:00:00, stored in a 64-bit int */
#define SBDF_DATETYPEID 0x07      /* C representation is milliseconds since 01/01/01, 00:00:00, stored in a 64-bit int */
#define SBDF_TIMETYPEID 0x08      /* C representation is milliseconds since 01/01/01, 00:00:00, stored in a 64-bit int */
#define SBDF_TIMESPANTYPEID 0x09  /* C representation is milliseconds since 01/01/01, 00:00:00, stored in a 64-bit int */

#define SBDF_STRINGTYPEID 0x0a    /* C representation is char-ptr */

#define SBDF_BINARYTYPEID 0x0c    /* C representation is void-ptr */

#define SBDF_DECIMALTYPEID 0xd    /* C representation is IEEE754 decimal128 Binary Integer Decimals */

#ifdef __cplusplus
}
#endif

#endif
