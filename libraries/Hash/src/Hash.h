/**
 * @file Hash.h
 * @date 20.05.2015
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the esp8266 core for Arduino environment.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef HASH_H_
#define HASH_H_

//#define DEBUG_SHA1

void sha1(uint8_t * data, uint32_t size, uint8_t hash[20]);
void sha1(char * data, uint32_t size, uint8_t hash[20]);
void sha1(const uint8_t * data, uint32_t size, uint8_t hash[20]);
void sha1(const char * data, uint32_t size, uint8_t hash[20]);
void sha1(const String& data, uint8_t hash[20]);

String sha1(uint8_t* data, uint32_t size);
String sha1(char* data, uint32_t size);
String sha1(const uint8_t* data, uint32_t size);
String sha1(const char* data, uint32_t size);
String sha1(const String& data);

#endif /* HASH_H_ */
