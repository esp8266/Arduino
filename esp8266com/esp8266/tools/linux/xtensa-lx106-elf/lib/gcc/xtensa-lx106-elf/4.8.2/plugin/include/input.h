/* Declarations for variables relating to reading the source file.
   Used by parsers, lexical analyzers, and error message routines.
   Copyright (C) 1993-2013 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_INPUT_H
#define GCC_INPUT_H

#include "line-map.h"

extern GTY(()) struct line_maps *line_table;

/* A value which will never be used to represent a real location.  */
#define UNKNOWN_LOCATION ((source_location) 0)

/* The location for declarations in "<built-in>" */
#define BUILTINS_LOCATION ((source_location) 1)

/* line-map.c reserves RESERVED_LOCATION_COUNT to the user.  Ensure
   both UNKNOWN_LOCATION and BUILTINS_LOCATION fit into that.  */
extern char builtins_location_check[(BUILTINS_LOCATION
				     < RESERVED_LOCATION_COUNT) ? 1 : -1];

extern expanded_location expand_location (source_location);
extern const char * location_get_source_line(expanded_location xloc);
extern expanded_location expand_location_to_spelling_point (source_location);
extern source_location expansion_point_location_if_in_system_header (source_location);

/* Historically GCC used location_t, while cpp used source_location.
   This could be removed but it hardly seems worth the effort.  */
typedef source_location location_t;

extern location_t input_location;

#define LOCATION_FILE(LOC) ((expand_location (LOC)).file)
#define LOCATION_LINE(LOC) ((expand_location (LOC)).line)
#define LOCATION_COLUMN(LOC)((expand_location (LOC)).column)
#define LOCATION_LOCUS(LOC) \
  ((IS_ADHOC_LOC(LOC)) ? get_location_from_adhoc_loc (line_table, LOC) : (LOC))
#define LOCATION_BLOCK(LOC) \
  ((tree) ((IS_ADHOC_LOC (LOC)) ? get_data_from_adhoc_loc (line_table, (LOC)) \
  : NULL))

#define input_line LOCATION_LINE (input_location)
#define input_filename LOCATION_FILE (input_location)
#define in_system_header_at(LOC) \
  ((linemap_location_in_system_header_p (line_table, LOC)))
#define in_system_header (in_system_header_at (input_location))

void dump_line_table_statistics (void);

#endif
