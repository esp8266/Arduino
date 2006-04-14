/*
  buffer.h - Buffer library for Wiring & Arduino
  Based on Hernando Barragan's original C implementation
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

#ifndef buffer_h
#define buffer_h

  #include <inttypes.h>
  
  typedef struct {
    char* in;
    char* out;
    char* buf;
    uint16_t len;
    uint16_t cnt;
  } buffer_t;
  
  void buffer_init(buffer_t*, char*, uint16_t);
  void buffer_put(buffer_t*, char);
  uint16_t buffer_get(buffer_t*);
  uint8_t buffer_look(buffer_t*);
  uint8_t buffer_available(buffer_t*);

#endif

