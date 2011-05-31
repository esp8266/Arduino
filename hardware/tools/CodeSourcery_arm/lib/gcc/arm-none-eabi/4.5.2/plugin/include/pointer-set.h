/* Set operations on pointers
   Copyright (C) 2004, 2007 Free Software Foundation, Inc.

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

#ifndef POINTER_SET_H
#define POINTER_SET_H

struct pointer_set_t;
struct pointer_set_t *pointer_set_create (void);
void pointer_set_destroy (struct pointer_set_t *pset);

int pointer_set_contains (const struct pointer_set_t *pset, const void *p);
int pointer_set_insert (struct pointer_set_t *pset, const void *p);
void pointer_set_traverse (const struct pointer_set_t *,
			   bool (*) (const void *, void *),
			   void *);
size_t pointer_set_n_elements (struct pointer_set_t *);
void pointer_set_clear (struct pointer_set_t *);

struct pointer_map_t;
struct pointer_map_t *pointer_map_create (void);
void pointer_map_destroy (struct pointer_map_t *pmap);

void **pointer_map_contains (const struct pointer_map_t *pmap, const void *p);
void **pointer_map_insert (struct pointer_map_t *pmap, const void *p);
void pointer_map_traverse (const struct pointer_map_t *,
			   bool (*) (const void *, void **, void *), void *);
size_t pointer_map_n_elements (struct pointer_map_t *);
void pointer_map_clear (struct pointer_map_t *);

#endif  /* POINTER_SET_H  */
