/*
 * NetworkInfo
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








#ifndef __NETWORKINFO_H__
#define __NETWORKINFO_H__

#include <ESP8266WiFi.h>
#include "NetworkInfoBase.h"

class NetworkInfo {

private:

  uint8_t _bssidArray[6] {0};

public:

  String SSID = "";
  int wifiChannel = NETWORK_INFO_DEFAULT_INT;
  uint8_t *BSSID = NULL;
  int networkIndex = NETWORK_INFO_DEFAULT_INT;

  /**
   * @param autofill Automatically fill in the rest of the network info using newNetworkIndex and the WiFi scan results.
   */
  NetworkInfo(int newNetworkIndex, bool autofill = true);

  /**
   * Without giving channel and BSSID, connection time is longer.
   */
  NetworkInfo(const String &newSSID, int newWiFiChannel = NETWORK_INFO_DEFAULT_INT, uint8_t newBSSID[6] = NULL, int newNetworkIndex = NETWORK_INFO_DEFAULT_INT);

  NetworkInfo(const NetworkInfo &other);

  NetworkInfo & operator=(const NetworkInfo &other);

  // No need for explicit destructor with current class design

  /**
   * Copy newBSSID into BSSID. 
   * Prefer this method for changing NetworkInfo BSSID, unless you actually want to change the BSSID pointer.
   */
  void copyBSSID(uint8_t newBSSID[6]);
};

#endif
