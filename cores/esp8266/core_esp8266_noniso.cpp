/*
 core_esp8266_noniso.c - nonstandard (but usefull) conversion functions

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

 Modified 03 April 2015 by Markus Sattler

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "stdlib_noniso.h"

extern "C" {

char* ltoa(long value, char* result, int base) {
    return itoa((int)value, result, base);
}

char* ultoa(unsigned long value, char* result, int base) {
    return utoa((unsigned int)value, result, base);
}

char * dtostrf(double number, signed char width, unsigned char prec, char *s) {
    char fmt[32];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(s, fmt, number);
    return s;
}

};
