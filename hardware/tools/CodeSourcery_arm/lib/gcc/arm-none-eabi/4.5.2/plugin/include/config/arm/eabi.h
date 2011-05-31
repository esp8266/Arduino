/* Configuration file for ARM EABI targets.
   Copyright (C) 2008
   Free Software Foundation, Inc.
   Contributed by Doug Kwan (dougkwan@google.com)

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

/* This file contains macro overrides for EABI targets.  */

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()			\
  do							\
    {							\
      TARGET_BPABI_CPP_BUILTINS ();			\
      if (TARGET_ANDROID)				\
	builtin_define ("__ANDROID__");			\
    }							\
  while (false)

#undef SUBSUBTARGET_EXTRA_SPECS
#define SUBSUBTARGET_EXTRA_SPECS \
  { "link_android",		ANDROID_LINK_SPEC },		\
  { "link_default",		BPABI_LINK_SPEC },		\
  { "cc1_android",		ANDROID_CC1_SPEC },		\
  { "cc1_default",		CC1_DEFAULT_SPEC },		\
  { "cc1plus_android",		ANDROID_CC1PLUS_SPEC },		\
  { "cc1plus_default",		CC1PLUS_DEFAULT_SPEC },		\
  { "lib_android",		ANDROID_LIB_SPEC },		\
  { "lib_default",		LIB_DEFAULT_SPEC },		\
  { "startfile_android",	ANDROID_STARTFILE_SPEC },	\
  { "startfile_default",	UNKNOWN_ELF_STARTFILE_SPEC },	\
  { "endfile_android",		ANDROID_ENDFILE_SPEC },		\
  { "endfile_default",		UNKNOWN_ELF_ENDFILE_SPEC },	\

#undef ANDROID_LINK_SPEC
#define ANDROID_LINK_SPEC \
"%{mbig-endian:-EB} %{mlittle-endian:-EL} "		\
"%{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic} "	\
"%{!static:" \
   "%{shared: -Bsymbolic} " \
   "%{!shared:" \
      "%{rdynamic:-export-dynamic} " \
      "%{!dynamic-linker:-dynamic-linker /system/bin/linker}}} " \
"-X" SUBTARGET_EXTRA_LINK_SPEC

/* Override LINK_SPEC in bpabi.h. */
#undef LINK_SPEC
#define LINK_SPEC \
"%{mandroid: %(link_android) ;" \
"          : %(link_default)}"

/* Android uses -fno-exceptions by default. */
#undef ANDROID_CC1_SPEC
#define ANDROID_CC1_SPEC "%{!fexceptions:-fno-exceptions}"

/* Default CC1_SPEC as in arm.h. */
#undef CC1_DEFAULT_SPEC
#define CC1_DEFAULT_SPEC ""

#undef CC1_SPEC
#define CC1_SPEC \
"%{mandroid: %(cc1_android) ;" \
"          : %(cc1_default)}"

/* Android uses -fno-rtti by default. */
#undef ANDROID_CC1PLUS_SPEC
#define ANDROID_CC1PLUS_SPEC "%{!frtti:-fno-rtti}"

/* Default CC1PLUS_SPEC as in gcc.c. */
#undef CC1PLUS_DEFAULT_SPEC
#define CC1PLUS_DEFAULT_SPEC ""

#undef CC1PLUS_SPEC
#define CC1PLUS_SPEC \
"%{mandroid: %(cc1plus_android) ;" \
"          : %(cc1plus_default)}"

#undef ANDROID_LIB_SPEC
#define ANDROID_LIB_SPEC	"-lc %{!static:-ldl}"

/* Default LIB_SPEC as in gcc.c. */
#undef LIB_DEFAULT_SPEC
#define LIB_DEFAULT_SPEC \
"%{!shared:%{g*:-lg} %{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}}"

#undef LIB_SPEC
#define LIB_SPEC \
"%{mandroid: %(lib_android) ;" \
"          : %(lib_default)}"

#undef ANDROID_STARTFILE_SPEC
#define ANDROID_STARTFILE_SPEC \
"%{!shared:" \
   "%{static: crtbegin_static%O%s ;" \
   "        : crtbegin_dynamic%O%s}}"

/* Override STARTFILE_SPEC in unknown-elf.h. */
#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
"%{mandroid: %(startfile_android) ;" \
"          : %(startfile_default)}"

#undef ANDROID_ENDFILE_SPEC
#define ANDROID_ENDFILE_SPEC	"%{!shared:crtend_android%O%s}"

/* Override ENDFILE_SPEC in unknown-elf.h. */
#undef ENDFILE_SPEC
#define ENDFILE_SPEC \
"%{mandroid: %(endfile_android) ;" \
"          : %(endfile_default)}"

