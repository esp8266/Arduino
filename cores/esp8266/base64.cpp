/**
 * base64.cpp
 *
 * Created on: 09.12.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266 core for Arduino.
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

#include "Arduino.h"
extern "C" {
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
}
#include "base64.h"

/**
 * convert input data to base64
 * @param data uint8_t *
 * @param length size_t
 * @return String
 */
String base64::encode(uint8_t * data, size_t length) {
    // base64 needs more size then the source data
    size_t size = ((length * 1.6f) + 1);
    char * buffer = (char *) malloc(size);
    if(buffer) {
        base64_encodestate _state;
        base64_init_encodestate(&_state);
        int len = base64_encode_block((const char *) &data[0], length, &buffer[0], &_state);
        len = base64_encode_blockend((buffer + len), &_state);

        String base64 = String(buffer);
        free(buffer);
        return base64;
    }
    return String("-FAIL-");
}

/**
 * convert input data to base64
 * @param text String
 * @return String
 */
String base64::encode(String text) {
    return base64::encode((uint8_t *) text.c_str(), text.length());
}

/**
 * convert input data from base64
 * @param text String base64 encoded zero-terminated string
 * @param data uint8_t * byte array to hold resulting decoded data
 * @param maxlength size_t maximum number of bytes we can copy to array
 * @return number of bytes copied, including zero terminating char
 */
size_t base64::decode(char* text, uint8_t * data, size_t maxlength) {
  // allocate big enough array to hold result
  char* plaintext = new char[strlen(text)];
  int len = base64_decode_chars(text, strlen(text), plaintext);
  if (len <= maxlength) {
    // in case output array can contain extra zero character, it will be copied.
    size_t bytes_to_copy = min(maxlength, len+1);
    memcpy(data, plaintext, bytes_to_copy);
    delete(plaintext);
    return bytes_to_copy;
  } else {
    delete(plaintext);
    return 0;
  }
}
