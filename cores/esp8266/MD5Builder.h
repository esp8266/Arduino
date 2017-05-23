/*
  MD5Builder.h - exposed md5 ROM functions for esp8266

  Copyright (c) 2015 Hristo Gochkov, All rights reserved.
  Refactored by Sven Eliasson.
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

#ifndef __ESP8266_MD5_BUILDER__
#define __ESP8266_MD5_BUILDER__

#include <WString.h>
#include <Stream.h>
#include "md5.h"
#include "HashBuilder.h"

class MD5Builder : public HashBuilder {
  private:
    const int HEX_STR_LEN = 16;
    md5_context_t _ctx;
    void _init(void) override;
    void _update(uint8_t * data, uint16_t len) override;
    void _final() override;
    std::vector<uint8_t> &_result(){ return _resultVector;}
    std::vector<uint8_t> _resultVector = {};
};



#endif
