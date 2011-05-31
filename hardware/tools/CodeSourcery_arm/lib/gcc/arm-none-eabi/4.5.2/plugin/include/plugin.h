/* Header file for internal GCC plugin mechanism.
   Copyright (C) 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef PLUGIN_H
#define PLUGIN_H

#include "gcc-plugin.h"

struct attribute_spec;

extern void add_new_plugin (const char *);
extern void parse_plugin_arg_opt (const char *);
extern int invoke_plugin_callbacks (int, void *);
extern void initialize_plugins (void);
extern bool plugins_active_p (void);
extern void dump_active_plugins (FILE *);
extern void debug_active_plugins (void);
extern void print_plugins_versions (FILE *file, const char *indent);
extern void print_plugins_help (FILE *file, const char *indent);
extern void finalize_plugins (void);

/* In attribs.c.  */

extern void register_attribute (const struct attribute_spec *attr);

#endif /* PLUGIN_H */
