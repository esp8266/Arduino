/* Command line option handling.
   Copyright (C) 2002, 2003, 2004, 2005, 2007, 2008
   Free Software Foundation, Inc.

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

#ifndef GCC_OPTS_H
#define GCC_OPTS_H

/* Specifies how a switch's VAR_VALUE relates to its FLAG_VAR.  */
enum cl_var_type {
  /* The switch is enabled when FLAG_VAR is nonzero.  */
  CLVC_BOOLEAN,

  /* The switch is enabled when FLAG_VAR == VAR_VALUE.  */
  CLVC_EQUAL,

  /* The switch is enabled when VAR_VALUE is not set in FLAG_VAR.  */
  CLVC_BIT_CLEAR,

  /* The switch is enabled when VAR_VALUE is set in FLAG_VAR.  */
  CLVC_BIT_SET,

  /* The switch takes a string argument and FLAG_VAR points to that
     argument.  */
  CLVC_STRING
};

struct cl_option
{
  const char *opt_text;
  const char *help;
  unsigned short back_chain;
  unsigned char opt_len;
  int neg_index;
  unsigned int flags;
  void *flag_var;
  enum cl_var_type var_type;
  int var_value;
};

/* Records that the state of an option consists of SIZE bytes starting
   at DATA.  DATA might point to CH in some cases.  */
struct cl_option_state {
  const void *data;
  size_t size;
  char ch;
};

extern const struct cl_option cl_options[];
extern const unsigned int cl_options_count;
extern const char *const lang_names[];
extern const unsigned int cl_lang_count;

#define CL_PARAMS               (1 << 17) /* Fake entry.  Used to display --param info with --help.  */
#define CL_WARNING		(1 << 18) /* Enables an (optional) warning message.  */
#define CL_OPTIMIZATION		(1 << 19) /* Enables an (optional) optimization.  */
#define CL_TARGET		(1 << 20) /* Target-specific option.  */
#define CL_COMMON		(1 << 21) /* Language-independent.  */

#define CL_MIN_OPTION_CLASS	CL_PARAMS
#define CL_MAX_OPTION_CLASS	CL_COMMON

/* From here on the bits describe attributes of the options.
   Before this point the bits have described the class of the option.
   This distinction is important because --help will not list options
   which only have these higher bits set.  */

#define CL_SAVE			(1 << 22) /* Target-specific option for attribute.  */
#define CL_DISABLED		(1 << 23) /* Disabled in this configuration.  */
#define CL_REPORT		(1 << 24) /* Report argument with -fverbose-asm  */
#define CL_JOINED		(1 << 25) /* If takes joined argument.  */
#define CL_SEPARATE		(1 << 26) /* If takes a separate argument.  */
#define CL_REJECT_NEGATIVE	(1 << 27) /* Reject no- form.  */
#define CL_MISSING_OK		(1 << 28) /* Missing argument OK (joined).  */
#define CL_UINTEGER		(1 << 29) /* Argument is an integer >=0.  */
#define CL_UNDOCUMENTED		(1 << 30) /* Do not output with --help.  */

/* Input file names.  */

extern const char **in_fnames;

/* The count of input filenames.  */

extern unsigned num_in_fnames;

size_t find_opt (const char *input, int lang_mask);
extern void prune_options (int *argcp, char ***argvp);
extern void decode_options (unsigned int argc, const char **argv);
extern int option_enabled (int opt_idx);
extern bool get_option_state (int, struct cl_option_state *);
extern void set_option (const struct cl_option *, int, const char *);

extern void enable_warning_as_error (const char *arg, int value,
				     unsigned int lang_mask);
extern void print_ignored_options (void);
#endif
