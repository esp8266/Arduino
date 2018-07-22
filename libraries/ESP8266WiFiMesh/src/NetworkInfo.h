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

#ifndef __NETWORKINFO_H__
#define __NETWORKINFO_H__

#include <ESP8266WiFi.h>

const int NETWORK_INFO_DEFAULT_INT = -1;

class NetworkInfo {

private:

  uint8_t bssid_array[6] {0};

public:

  String ssid = "";
  int wifi_channel = NETWORK_INFO_DEFAULT_INT;
  uint8_t *bssid = NULL;
  int network_index = NETWORK_INFO_DEFAULT_INT;

  /**
   * @param autofill Automatically fill in the rest of the network info using _network_index and the WiFi scan results.
   */
  NetworkInfo(int new_network_index, bool autofill = true);

  /**
   * Without giving channel and bssid, connection time is longer.
   */
  NetworkInfo(String new_ssid, int new_wifi_channel = NETWORK_INFO_DEFAULT_INT, uint8_t new_bssid[6] = NULL, int new_network_index = NETWORK_INFO_DEFAULT_INT);

  NetworkInfo(const NetworkInfo &other);

  NetworkInfo & operator=(const NetworkInfo &other);

  // No need for explicit destructor with current class design

  /**
   * Copy new_bssid into bssid. 
   * Prefer this method for changing NetworkInfo BSSID, unless you actually want to change the BSSID pointer.
   */
  void copyBSSID(uint8_t new_bssid[6]);
};

#endif
