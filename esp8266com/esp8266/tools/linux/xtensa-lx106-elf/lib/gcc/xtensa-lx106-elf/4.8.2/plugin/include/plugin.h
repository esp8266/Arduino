/* Header file for internal GCC plugin mechanism.
   Copyright (C) 2009-2013 Free Software Foundation, Inc.

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
struct scoped_attributes;

extern void add_new_plugin (const char *);
extern void parse_plugin_arg_opt (const char *);
extern int invoke_plugin_callbacks_full (int, void *);
extern void initialize_plugins (void);
extern bool plugins_active_p (void);
extern void dump_active_plugins (FILE *);
extern void debug_active_plugins (void);
extern void warn_if_plugins (void);
extern void plugins_internal_error_function (diagnostic_context *,
					     const char *, va_list *);
extern void print_plugins_versions (FILE *file, const char *indent);
extern void print_plugins_help (FILE *file, const char *indent);
extern void finalize_plugins (void);

extern bool flag_plugin_added;

/* Called from inside GCC.  Invoke all plugin callbacks registered with
   the specified event.
   Return PLUGEVT_SUCCESS if at least one callback was called,
   PLUGEVT_NO_CALLBACK if there was no callback.

   EVENT    - the event identifier
   GCC_DATA - event-specific data provided by the compiler  */

static inline int
invoke_plugin_callbacks (int event ATTRIBUTE_UNUSED,
			 void *gcc_data ATTRIBUTE_UNUSED)
{
#ifdef ENABLE_PLUGIN
  /* True iff at least one plugin has been added.  */
  if (flag_plugin_added)
    return invoke_plugin_callbacks_full (event, gcc_data);
#endif

  return PLUGEVT_NO_CALLBACK;
}

/* In attribs.c.  */

extern void register_attribute (const struct attribute_spec *attr);
extern struct scoped_attributes* register_scoped_attributes (const struct attribute_spec *,
							     const char *);

#endif /* PLUGIN_H */
