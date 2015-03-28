/* Various declarations for language-independent diagnostics subroutines.
   Copyright (C) 2000-2013 Free Software Foundation, Inc.
   Contributed by Gabriel Dos Reis <gdr@codesourcery.com>

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

#ifndef GCC_DIAGNOSTIC_H
#define GCC_DIAGNOSTIC_H

#include "pretty-print.h"
#include "diagnostic-core.h"

/* A diagnostic is described by the MESSAGE to send, the FILE and LINE of
   its context and its KIND (ice, error, warning, note, ...)  See complete
   list in diagnostic.def.  */
typedef struct diagnostic_info
{
  text_info message;
  location_t location;
  unsigned int override_column;
  /* Auxiliary data for client.  */
  void *x_data;
  /* The kind of diagnostic it is about.  */
  diagnostic_t kind;
  /* Which OPT_* directly controls this diagnostic.  */
  int option_index;
} diagnostic_info;

/* Each time a diagnostic's classification is changed with a pragma,
   we record the change and the location of the change in an array of
   these structs.  */
typedef struct diagnostic_classification_change_t
{
  location_t location;
  int option;
  diagnostic_t kind;
} diagnostic_classification_change_t;

/*  Forward declarations.  */
typedef void (*diagnostic_starter_fn) (diagnostic_context *,
				       diagnostic_info *);
typedef diagnostic_starter_fn diagnostic_finalizer_fn;

/* This data structure bundles altogether any information relevant to
   the context of a diagnostic message.  */
struct diagnostic_context
{
  /* Where most of the diagnostic formatting work is done.  */
  pretty_printer *printer;

  /* The number of times we have issued diagnostics.  */
  int diagnostic_count[DK_LAST_DIAGNOSTIC_KIND];

  /* True if we should display the "warnings are being tread as error"
     message, usually displayed once per compiler run.  */
  bool some_warnings_are_errors;

  /* True if it has been requested that warnings be treated as errors.  */
  bool warning_as_error_requested;

  /* The number of option indexes that can be passed to warning() et
     al.  */
  int n_opts;

  /* For each option index that can be passed to warning() et al
     (OPT_* from options.h when using this code with the core GCC
     options), this array may contain a new kind that the diagnostic
     should be changed to before reporting, or DK_UNSPECIFIED to leave
     it as the reported kind, or DK_IGNORED to not report it at
     all.  */
  diagnostic_t *classify_diagnostic;

  /* History of all changes to the classifications above.  This list
     is stored in location-order, so we can search it, either
     binary-wise or end-to-front, to find the most recent
     classification for a given diagnostic, given the location of the
     diagnostic.  */
  diagnostic_classification_change_t *classification_history;

  /* The size of the above array.  */
  int n_classification_history;

  /* For pragma push/pop.  */
  int *push_list;
  int n_push;

  /* True if we should print the source line with a caret indicating
     the location.  */
  bool show_caret;

  /* Maximum width of the source line printed.  */
  int caret_max_width;

  /* True if we should print the command line option which controls
     each diagnostic, if known.  */
  bool show_option_requested;

  /* True if we should raise a SIGABRT on errors.  */
  bool abort_on_error;

  /* True if we should show the column number on diagnostics.  */
  bool show_column;

  /* True if pedwarns are errors.  */
  bool pedantic_errors;

  /* True if permerrors are warnings.  */
  bool permissive;

  /* The index of the option to associate with turning permerrors into
     warnings.  */
  int opt_permissive;

  /* True if errors are fatal.  */
  bool fatal_errors;

  /* True if all warnings should be disabled.  */
  bool dc_inhibit_warnings;

  /* True if warnings should be given in system headers.  */
  bool dc_warn_system_headers;

  /* Maximum number of errors to report.  */
  unsigned int max_errors;

  /* This function is called before any message is printed out.  It is
     responsible for preparing message prefix and such.  For example, it
     might say:
     In file included from "/usr/local/include/curses.h:5:
                      from "/home/gdr/src/nifty_printer.h:56:
                      ...
  */
  diagnostic_starter_fn begin_diagnostic;

  /* This function is called after the diagnostic message is printed.  */
  diagnostic_finalizer_fn end_diagnostic;

  /* Client hook to report an internal error.  */
  void (*internal_error) (diagnostic_context *, const char *, va_list *);

  /* Client hook to say whether the option controlling a diagnostic is
     enabled.  Returns nonzero if enabled, zero if disabled.  */
  int (*option_enabled) (int, void *);

  /* Client information to pass as second argument to
     option_enabled.  */
  void *option_state;

  /* Client hook to return the name of an option that controls a
     diagnostic.  Returns malloced memory.  The first diagnostic_t
     argument is the kind of diagnostic before any reclassification
     (of warnings as errors, etc.); the second is the kind after any
     reclassification.  May return NULL if no name is to be printed.
     May be passed 0 as well as the index of a particular option.  */
  char *(*option_name) (diagnostic_context *, int, diagnostic_t, diagnostic_t);

  /* Auxiliary data for client.  */
  void *x_data;

  /* Used to detect that the last caret was printed at the same location.  */
  location_t last_location;

  /* Used to detect when the input file stack has changed since last
     described.  */
  const struct line_map *last_module;

  int lock;

  bool inhibit_notes_p;
};

static inline void
diagnostic_inhibit_notes (diagnostic_context * context)
{
  context->inhibit_notes_p = true;
}


/* Client supplied function to announce a diagnostic.  */
#define diagnostic_starter(DC) (DC)->begin_diagnostic

/* Client supplied function called after a diagnostic message is
   displayed.  */
#define diagnostic_finalizer(DC) (DC)->end_diagnostic

/* Extension hooks for client.  */
#define diagnostic_context_auxiliary_data(DC) (DC)->x_data
#define diagnostic_info_auxiliary_data(DI) (DI)->x_data

/* Same as pp_format_decoder.  Works on 'diagnostic_context *'.  */
#define diagnostic_format_decoder(DC) ((DC)->printer->format_decoder)

/* Same as output_prefixing_rule.  Works on 'diagnostic_context *'.  */
#define diagnostic_prefixing_rule(DC) ((DC)->printer->wrapping.rule)

/* Maximum characters per line in automatic line wrapping mode.
   Zero means don't wrap lines.  */
#define diagnostic_line_cutoff(DC) ((DC)->printer->wrapping.line_cutoff)

#define diagnostic_flush_buffer(DC) pp_base_flush ((DC)->printer)

/* True if the last module or file in which a diagnostic was reported is
   different from the current one.  */
#define diagnostic_last_module_changed(DC, MAP)	\
  ((DC)->last_module != MAP)

/* Remember the current module or file as being the last one in which we
   report a diagnostic.  */
#define diagnostic_set_last_module(DC, MAP)	\
  (DC)->last_module = MAP

/* Raise SIGABRT on any diagnostic of severity DK_ERROR or higher.  */
#define diagnostic_abort_on_error(DC) \
  (DC)->abort_on_error = true

/* This diagnostic_context is used by front-ends that directly output
   diagnostic messages without going through `error', `warning',
   and similar functions.  */
extern diagnostic_context *global_dc;

/* The total count of a KIND of diagnostics emitted so far.  */
#define diagnostic_kind_count(DC, DK) (DC)->diagnostic_count[(int) (DK)]

/* The number of errors that have been issued so far.  Ideally, these
   would take a diagnostic_context as an argument.  */
#define errorcount diagnostic_kind_count (global_dc, DK_ERROR)
/* Similarly, but for warnings.  */
#define warningcount diagnostic_kind_count (global_dc, DK_WARNING)
/* Similarly, but for sorrys.  */
#define sorrycount diagnostic_kind_count (global_dc, DK_SORRY)

/* Returns nonzero if warnings should be emitted.  */
#define diagnostic_report_warnings_p(DC, LOC)				\
  (!(DC)->dc_inhibit_warnings						\
   && !(in_system_header_at (LOC) && !(DC)->dc_warn_system_headers))

#define report_diagnostic(D) diagnostic_report_diagnostic (global_dc, D)

/* Override the column number to be used for reporting a
   diagnostic.  */
#define diagnostic_override_column(DI, COL) (DI)->override_column = (COL)

/* Override the option index to be used for reporting a
   diagnostic.  */
#define diagnostic_override_option_index(DI, OPTIDX) \
    ((DI)->option_index = (OPTIDX))

/* Diagnostic related functions.  */
extern void diagnostic_initialize (diagnostic_context *, int);
extern void diagnostic_finish (diagnostic_context *);
extern void diagnostic_report_current_module (diagnostic_context *, location_t);
extern void diagnostic_show_locus (diagnostic_context *, const diagnostic_info *);

/* Force diagnostics controlled by OPTIDX to be kind KIND.  */
extern diagnostic_t diagnostic_classify_diagnostic (diagnostic_context *,
						    int /* optidx */,
						    diagnostic_t /* kind */,
						    location_t);
extern void diagnostic_push_diagnostics (diagnostic_context *, location_t);
extern void diagnostic_pop_diagnostics (diagnostic_context *, location_t);
extern bool diagnostic_report_diagnostic (diagnostic_context *,
					  diagnostic_info *);
#ifdef ATTRIBUTE_GCC_DIAG
extern void diagnostic_set_info (diagnostic_info *, const char *, va_list *,
				 location_t, diagnostic_t) ATTRIBUTE_GCC_DIAG(2,0);
extern void diagnostic_set_info_translated (diagnostic_info *, const char *,
					    va_list *, location_t,
					    diagnostic_t)
     ATTRIBUTE_GCC_DIAG(2,0);
extern void diagnostic_append_note (diagnostic_context *, location_t,
                                    const char *, ...) ATTRIBUTE_GCC_DIAG(3,4);
#endif
extern char *diagnostic_build_prefix (diagnostic_context *, const diagnostic_info *);
void default_diagnostic_starter (diagnostic_context *, diagnostic_info *);
void default_diagnostic_finalizer (diagnostic_context *, diagnostic_info *);
void diagnostic_set_caret_max_width (diagnostic_context *context, int value);


/* Pure text formatting support functions.  */
extern char *file_name_as_prefix (const char *);

#endif /* ! GCC_DIAGNOSTIC_H */
