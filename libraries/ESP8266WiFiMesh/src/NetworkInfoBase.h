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

#ifndef __NETWORKINFOBASE_H__
#define __NETWORKINFOBASE_H__

#include <ESP8266WiFi.h>

const int NETWORK_INFO_DEFAULT_INT = -1;

class NetworkInfoBase {

public:

  /**
   * Automatically fill in the rest of the network info using networkIndex and the WiFi scan results.
   */
  NetworkInfoBase(uint8_t networkIndex);

  /**
   * Without giving channel and BSSID, connection time is longer.
   */
  NetworkInfoBase(const String &SSID, int32_t wifiChannel, const uint8_t BSSID[6], uint8_t encryptionType, int32_t RSSI, bool isHidden);

  NetworkInfoBase(const NetworkInfoBase &other);

  NetworkInfoBase & operator=(const NetworkInfoBase &other);
  
  void setBSSID(const uint8_t BSSID[6]);
  const uint8_t *BSSID() const;
  /**
   * @return If BSSID is set, a pointer to resultArray which will contain a copy of BSSID. nullptr otherwise.
   */
  uint8_t *getBSSID(uint8_t resultArray[6]) const;

  void setSSID(String &SSID);
  String SSID() const;

  void setWifiChannel(int32_t wifiChannel);
  int32_t wifiChannel() const;

  void setEncryptionType(uint8_t encryptionType);
  uint8_t encryptionType() const;  

  void setRSSI(int32_t RSSI);
  int32_t RSSI() const;

  void setIsHidden(bool isHidden);
  bool isHidden() const;

  static const String defaultSSID;
  static const int32_t defaultWifiChannel;
  static const uint8_t defaultEncryptionType;
  static const int32_t defaultRSSI;
  static const bool defaultIsHidden;

protected:

  ~NetworkInfoBase();

  NetworkInfoBase();

  /**
   * Copy newBSSID into _BSSID. 
   * Prefer this method for changing NetworkInfo BSSID, unless you actually want to change the _BSSID pointer.
   */
  void storeBSSID(const uint8_t newBSSID[6]);

private:

  uint8_t _bssidArray[6] {0};
  uint8_t *_BSSID = nullptr;
  String _SSID = defaultSSID;
  int32_t _wifiChannel = defaultWifiChannel;
  uint8_t _encryptionType = defaultEncryptionType; // see enum wl_enc_type for values
  int32_t _RSSI = defaultRSSI;
  bool _isHidden = defaultIsHidden;
};

#endif
