/*
 * Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "EspnowNetworkInfo.h"
#include <assert.h>

EspnowNetworkInfo::EspnowNetworkInfo(int networkIndex) : NetworkInfoBase(networkIndex) { };

EspnowNetworkInfo::EspnowNetworkInfo(const NetworkInfoBase &originalNetworkInfo) : NetworkInfoBase(originalNetworkInfo) 
{
  assert(BSSID() != defaultBSSID); // We need at least BSSID to be able to connect. 
};

EspnowNetworkInfo::EspnowNetworkInfo(const uint8_t BSSID[6], const String &SSID, int32_t wifiChannel, uint8_t encryptionType, int32_t RSSI , bool isHidden)
                   : NetworkInfoBase(SSID, wifiChannel, BSSID, encryptionType, RSSI, isHidden)
{ }
                  
