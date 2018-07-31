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

#include "NetworkInfo.h"

void NetworkInfo::copyBSSID(uint8_t newBSSID[6])
{
  if(newBSSID != NULL)
  {
    if(BSSID == NULL)
    {
      BSSID = _bssidArray;
    }
    
    for(int i = 0; i < 6; i++)
    {
      BSSID[i] = newBSSID[i];
    }
  }
  else
  {
    BSSID = NULL;
  }
}

NetworkInfo::NetworkInfo(int newNetworkIndex, bool autofill) : networkIndex(newNetworkIndex)
{ 
  if(autofill)
  {
    SSID = WiFi.SSID(newNetworkIndex);
    wifiChannel = WiFi.channel(newNetworkIndex);
    copyBSSID(WiFi.BSSID(newNetworkIndex)); 
  }
}

NetworkInfo::NetworkInfo(const String &newSSID, int newWiFiChannel, uint8_t newBSSID[6], int newNetworkIndex) : 
  SSID(newSSID), wifiChannel(newWiFiChannel), networkIndex(newNetworkIndex)
{
  copyBSSID(newBSSID);
}

NetworkInfo::NetworkInfo(const NetworkInfo &other) : SSID(other.SSID), wifiChannel(other.wifiChannel), networkIndex(other.networkIndex)
{
  copyBSSID(other.BSSID);
}

NetworkInfo & NetworkInfo::operator=(const NetworkInfo &other)
{
  SSID = other.SSID;
  wifiChannel = other.wifiChannel;
  copyBSSID(other.BSSID);
  networkIndex = other.networkIndex;
  return *this;
}

