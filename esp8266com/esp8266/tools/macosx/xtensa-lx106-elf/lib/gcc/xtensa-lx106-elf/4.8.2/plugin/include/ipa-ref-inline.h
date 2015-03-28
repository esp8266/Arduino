/* IPA reference lists.
   Copyright (C) 2010-2013 Free Software Foundation, Inc.
   Contributed by Jan Hubicka

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

/* Return callgraph node REF is referring.  */
static inline struct cgraph_node *
ipa_ref_node (struct ipa_ref *ref)
{
  return cgraph (ref->referred);
}

/* Return varpool node REF is referring.  */

static inline struct varpool_node *
ipa_ref_varpool_node (struct ipa_ref *ref)
{
  return varpool (ref->referred);
}

/* Return cgraph node REF is in.  */

static inline struct cgraph_node *
ipa_ref_referring_node (struct ipa_ref *ref)
{
  return cgraph (ref->referring);
}

/* Return varpool node REF is in.  */

static inline struct varpool_node *
ipa_ref_referring_varpool_node (struct ipa_ref *ref)
{
  return varpool (ref->referring);
}

/* Return reference list REF is in.  */

static inline struct ipa_ref_list *
ipa_ref_referring_ref_list (struct ipa_ref *ref)
{
  return &ref->referring->symbol.ref_list;
}

/* Return reference list REF is in.  */

static inline struct ipa_ref_list *
ipa_ref_referred_ref_list (struct ipa_ref *ref)
{
  return &ref->referred->symbol.ref_list;
}

/* Return first reference in LIST or NULL if empty.  */

static inline struct ipa_ref *
ipa_ref_list_first_reference (struct ipa_ref_list *list)
{
  if (!vec_safe_length (list->references))
    return NULL;
  return &(*list->references)[0];
}

/* Return first referring ref in LIST or NULL if empty.  */

static inline struct ipa_ref *
ipa_ref_list_first_referring (struct ipa_ref_list *list)
{
  if (!list->referring.length ())
    return NULL;
  return list->referring[0];
}

/* Clear reference list.  */

static inline void
ipa_empty_ref_list (struct ipa_ref_list *list)
{
  list->referring.create (0);
  list->references = NULL;
}

/* Clear reference list.  */

static inline unsigned int
ipa_ref_list_nreferences (struct ipa_ref_list *list)
{
  return vec_safe_length (list->references);
}

#define ipa_ref_list_reference_iterate(L,I,P) \
   vec_safe_iterate ((L)->references, (I), &(P))
#define ipa_ref_list_referring_iterate(L,I,P) \
   (L)->referring.iterate ((I), &(P))
