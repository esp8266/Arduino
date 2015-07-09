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

#ifndef TEMBOOGLOBAL_H_
#define TEMBOOGLOBAL_H_

#include <Arduino.h>

//#define TEMBOO_VERBOSE

#ifdef TEMBOO_VERBOSE
    #define TEMBOO_TRACE(x) Serial.print(x)
    #define TEMBOO_TRACE_BYTES(x,c) Serial.write((const uint8_t*)x,c)
    #define TEMBOO_TRACELN(x) Serial.println(x)
#else
    #define TEMBOO_TRACE(x)
    #define TEMBOO_TRACE_BYTES(x,c)
    #define TEMBOO_TRACELN(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

char* uint16toa(uint16_t value, char* dest);
char* uint32toa(uint32_t value, char* dest);

#ifdef __cplusplus
}
#endif


#endif

