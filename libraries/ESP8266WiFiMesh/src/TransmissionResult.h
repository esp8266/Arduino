/*
 * TransmissionResult
 * Copyright (C) 2018 Anders Löfgren
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







/********************************************************************************************
* NOTE!
*
* This class is deprecated and will be removed in core version 3.0.0.
* If you are still using this class, please consider migrating to the new API shown in 
* the EspnowNetworkInfo.h or TcpIpNetworkInfo.h source files.
*
* TODO: delete this file.
********************************************************************************************/









#ifndef __TRANSMISSIONRESULT_H__
#define __TRANSMISSIONRESULT_H__

#include <ESP8266WiFi.h>
#include "NetworkInfo.h"
#include "TransmissionOutcome.h"

typedef enum 
{
    TS_CONNECTION_FAILED    = -1,
    TS_TRANSMISSION_FAILED    = 0,
    TS_TRANSMISSION_COMPLETE   = 1
} transmission_status_t;

class TransmissionResult : public NetworkInfo {

public:

  transmission_status_t transmissionStatus;

  /**
   * @param autofill Automatically fill in the rest of the network info using newNetworkIndex and the WiFi scan results.
   */
  TransmissionResult(int newNetworkIndex, transmission_status_t newTransmissionStatus, bool autofill = true) __attribute__((deprecated));

  TransmissionResult(const String &newSSID, int newWiFiChannel, uint8_t newBSSID[6], transmission_status_t newTransmissionStatus) __attribute__((deprecated));

  TransmissionResult(const String &newSSID, int newWiFiChannel, uint8_t newBSSID[6], int newNetworkIndex, transmission_status_t newTransmissionStatus) __attribute__((deprecated));

  TransmissionResult(const NetworkInfo& origin, transmission_status_t newTransmissionStatus);
};

#endif
