/*
  itoa.h - Integer to ASCII conversion
  Copyright (c) 2016 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

// Standard C functions required in Arduino API
// If these functions are not provided by the standard library, the
// core should supply an implementation of them.

#ifdef __cplusplus
extern "C" {
#endif

extern char* itoa(int value, char *string, int radix);
extern char* ltoa(long value, char *string, int radix);
extern char* utoa(unsigned value, char *string, int radix);
extern char* ultoa(unsigned long value, char *string, int radix);

#ifdef __cplusplus
} // extern "C"
#endif

