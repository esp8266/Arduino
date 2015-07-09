/*
###############################################################################
#
# Temboo Arduino library
#
# Copyright 2015, Temboo Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the License.
#
###############################################################################
*/

#include "TembooGlobal.h"

char* uint16toa(uint16_t value, char* dest) {
    return uint32toa(value, dest);
}

char* uint32toa(uint32_t value, char* dest) {
    char* end = dest;
    do {
        *end++ = (value % 10) + '0';
    } while (value /= 10);
    *end = '\0';
    end--;

    char c;
    char* begin = dest;
    while(end > begin) {
        c = *end;
        *end = *begin;
        *begin = c;
        end--;
        begin++;
    }

    return dest;
}
