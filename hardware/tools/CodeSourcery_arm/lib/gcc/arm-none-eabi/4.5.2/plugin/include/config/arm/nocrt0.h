/* Definitions for generic libgloss based cofigs where crt0 is supplied by
   the linker script.
   Copyright (C) 2006 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC	" crti%O%s crtbegin%O%s"

#undef LIB_SPEC
#define LIB_SPEC "-lc"
