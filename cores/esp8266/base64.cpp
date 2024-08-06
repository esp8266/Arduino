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
#include "libb64/cencode.h"
}
#include "Base64.h"
#include <algorithm>

String Base64::encode(const uint8_t* data, size_t length, bool doNewLines)
{
    String base64;
    // base64 needs more size than the source data, use cencode.h macros
    size_t size = ((doNewLines ? base64_encode_expected_len(length)
                    : base64_encode_expected_len_nonewlines(length)) + 1);
    if (base64.reserve(size))
    {
        base64_encodestate _state;
        if (doNewLines)
        {
            base64_init_encodestate(&_state);
        }
        else
        {
            base64_init_encodestate_nonewlines(&_state);
        }
        constexpr size_t BUFSIZE = 48;
        char buf[BUFSIZE + 1 /* newline */ + 1 /* NUL */];
        for (size_t len = 0; len < length; len += BUFSIZE * 3 / 4)
        {
            size_t blocklen = base64_encode_block(
                reinterpret_cast<const char*>(data + len),
                std::min(BUFSIZE * 3 / 4, length - len),
                buf,
                &_state
            );
            buf[blocklen] = '\0';
            base64 += buf;
        }
        if (base64_encode_blockend(buf, &_state))
            base64 += buf;
    }
    else
    {
        base64 = F("-FAIL-");
    }
    return base64;
}

String Base64::encode(const String& text, bool doNewLines)
{
    return encode(reinterpret_cast<const uint8_t*>(text.c_str()), text.length(), doNewLines);
}
