/**
 * base64.h
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

#ifndef CORE_BASE64_H_
#define CORE_BASE64_H_

#include <WString.h>

class base64
{
public:
    // NOTE: The default behaviour of backend (lib64)
    // is to add a newline every 72 (encoded) characters output.
    // This may 'break' longer uris and json variables
    static String encode(const uint8_t * data, size_t length, bool doNewLine);
    static inline String encode(const String& text, bool doNewLines)
    {
        return encode( (const uint8_t *) text.c_str(), text.length(), doNewLines );
    }

    // esp32 compat:

    static inline String encode(const uint8_t * data, size_t length)
    {
        return encode(data, length, false);
    }

    static inline String inline encode(const String& text)
    {
        return encode(test, false);
    }
private:
};


#endif /* CORE_BASE64_H_ */
