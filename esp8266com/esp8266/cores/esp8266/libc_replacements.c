/* 
  libc_replacements.c - replaces libc functions with functions
                        from Espressif SDK

  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
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

#include <stddef.h>
#include <stdarg.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"


void* malloc(size_t size) {
  return os_malloc(size);
}

void free(void* ptr) {
  os_free(ptr);
}

void* realloc(void* ptr, size_t size) {
  return os_realloc(ptr, size);
}

int printf(const char* format, ...) {
  va_list arglist;
  va_start(arglist, format);
  ets_vprintf(format, arglist);
  va_end(arglist);
}

int sprintf(char* buffer, const char* format, ...) {
  va_list arglist;
  va_start(arglist, format);
  ets_vsprintf(buffer, format, arglist);
  va_end(arglist);
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
  va_list arglist;
  va_start(arglist, format);
  ets_vsnprintf(buffer, size, format, arglist);
  va_end(arglist);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  return ets_memcmp(s1, s2, n);
}

void* memcpy(void *dest, const void *src, size_t n) {
  return ets_memcpy(dest, src, n);
}

void* memset(void *s, int c, size_t n) {
  return ets_memset(s, c, n);
}

int strcmp(const char *s1, const char *s2) {
  return ets_strcmp(s1, s2);
}

char* strcpy(char *dest, const char *src) {
  return ets_strcpy(dest, src);
}

size_t strlen(const char *s) {
  return ets_strlen(s);
}

int strncmp(const char *s1, const char *s2, size_t len) {
  return ets_strncmp(s1, s2, len);
}

char *strncpy(char *dest, const char *src, size_t n) {
  return ets_strncpy(dest, src, n);
}

char *ets_strstr(const char *haystack, const char *needle) {
  return strstr(haystack, needle);
}

