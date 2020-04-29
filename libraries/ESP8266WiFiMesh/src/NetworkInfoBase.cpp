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

#include "NetworkInfoBase.h"

uint8_t * const NetworkInfoBase::defaultBSSID = nullptr;
const String NetworkInfoBase::defaultSSID;
const int32_t NetworkInfoBase::defaultWifiChannel = NETWORK_INFO_DEFAULT_INT;
const uint8_t NetworkInfoBase::defaultEncryptionType = 0;
const int32_t NetworkInfoBase::defaultRSSI = ~0;
const bool NetworkInfoBase::defaultIsHidden = false;

void NetworkInfoBase::storeBSSID(const uint8_t newBSSID[6])
{
  if(newBSSID != nullptr)
  {
    if(_BSSID == nullptr)
    {
      _BSSID = _bssidArray;
    }
    
    for(int i = 0; i < 6; ++i)
    {
      _BSSID[i] = newBSSID[i];
    }
  }
  else
  {
    _BSSID = nullptr;
  }
}

NetworkInfoBase::NetworkInfoBase() {};

NetworkInfoBase::NetworkInfoBase(const uint8_t networkIndex)
{
  uint8_t *bssidPtr = nullptr;
  WiFi.getNetworkInfo(networkIndex, _SSID, _encryptionType, _RSSI, bssidPtr, _wifiChannel, _isHidden);
  storeBSSID(bssidPtr);
}

NetworkInfoBase::NetworkInfoBase(const String &SSID, const int32_t wifiChannel, const uint8_t BSSID[6], const uint8_t encryptionType, const int32_t RSSI, const bool isHidden) : 
  _SSID(SSID), _wifiChannel(wifiChannel), _encryptionType(encryptionType), _RSSI(RSSI), _isHidden(isHidden)
{
  storeBSSID(BSSID);
}

NetworkInfoBase::NetworkInfoBase(const NetworkInfoBase &other) : _SSID(other.SSID()), _wifiChannel(other.wifiChannel()), _encryptionType(other.encryptionType()), 
                                                                 _RSSI(other.RSSI()), _isHidden(other.isHidden())
{
  storeBSSID(other.BSSID());
}

NetworkInfoBase & NetworkInfoBase::operator=(const NetworkInfoBase &other)
{
  if(this != &other)
  {
    storeBSSID(other.BSSID());
    _SSID = other.SSID();
    _wifiChannel = other.wifiChannel();
    _encryptionType = other.encryptionType();
    _RSSI = other.RSSI();
    _isHidden = other.isHidden();
  }
  
  return *this;
}

NetworkInfoBase::~NetworkInfoBase() { };

void NetworkInfoBase::setBSSID(const uint8_t BSSID[6]) { storeBSSID(BSSID); }
const uint8_t *NetworkInfoBase::BSSID() const { return _BSSID; }
uint8_t *NetworkInfoBase::getBSSID(uint8_t resultArray[6]) const 
{ 
  if(BSSID())
  {
    std::copy_n(_bssidArray, 6, resultArray);
    return resultArray;
  }
  else
  {
    return nullptr;
  }
}

void NetworkInfoBase::setSSID(const String &SSID) { _SSID = SSID; }
String NetworkInfoBase::SSID() const { return _SSID; }

void NetworkInfoBase::setWifiChannel(const int32_t wifiChannel) { _wifiChannel = wifiChannel; }
int32_t NetworkInfoBase::wifiChannel() const { return _wifiChannel; }

void NetworkInfoBase::setEncryptionType(const uint8_t encryptionType) { _encryptionType = encryptionType; }
uint8_t NetworkInfoBase::encryptionType() const { return _encryptionType; }

void NetworkInfoBase::setRSSI(const int32_t RSSI) { _RSSI = RSSI; }
int32_t NetworkInfoBase::RSSI() const { return _RSSI; }

void NetworkInfoBase::setIsHidden(const bool isHidden) { _isHidden = isHidden; }
bool NetworkInfoBase::isHidden() const { return _isHidden; }
