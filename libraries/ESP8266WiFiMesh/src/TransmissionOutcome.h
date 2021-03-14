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

#ifndef __TRANSMISSIONOUTCOME_H__
#define __TRANSMISSIONOUTCOME_H__

#include <ESP8266WiFi.h>
#include "NetworkInfoBase.h"

enum class TransmissionStatusType
{
  CONNECTION_FAILED      = -1,
  TRANSMISSION_FAILED    = 0,
  TRANSMISSION_COMPLETE  = 1
};

class TransmissionOutcome : public NetworkInfoBase {

public:

  TransmissionOutcome(const NetworkInfoBase &origin, const TransmissionStatusType transmissionStatus);

  TransmissionOutcome(const String &SSID, const int32_t wifiChannel, const uint8_t BSSID[6], const uint8_t encryptionType, 
                      const int32_t RSSI, const bool isHidden, const TransmissionStatusType transmissionStatus);

  void setTransmissionStatus(const TransmissionStatusType transmissionStatus);
  TransmissionStatusType transmissionStatus() const;

private:

  TransmissionStatusType _transmissionStatus;
};

#endif
