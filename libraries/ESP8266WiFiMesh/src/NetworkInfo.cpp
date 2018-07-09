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

void NetworkInfo::copyBSSID(uint8_t new_bssid[6])
{
  if(new_bssid != NULL)
  {
    if(bssid == NULL)
    {
      bssid = bssid_array;
    }
    
    for(int i = 0; i < 6; i++)
    {
      bssid[i] = new_bssid[i];
    }
  }
  else
  {
    bssid = NULL;
  }
}

NetworkInfo::NetworkInfo(int new_network_index, bool autofill) : network_index(new_network_index)
{ 
  if(autofill)
  {
    ssid = WiFi.SSID(new_network_index);
    wifi_channel = WiFi.channel(new_network_index);
    copyBSSID(WiFi.BSSID(new_network_index)); 
  }
}

NetworkInfo::NetworkInfo(String new_ssid, int new_wifi_channel, uint8_t new_bssid[6], int new_network_index) : 
  ssid(new_ssid), wifi_channel(new_wifi_channel), network_index(new_network_index)
{
  copyBSSID(new_bssid);
}

NetworkInfo::NetworkInfo(const NetworkInfo &other) : ssid(other.ssid), wifi_channel(other.wifi_channel), network_index(other.network_index)
{
  copyBSSID(other.bssid);
}

NetworkInfo & NetworkInfo::operator=(const NetworkInfo &other)
{
  ssid = other.ssid;
  wifi_channel = other.wifi_channel;
  copyBSSID(other.bssid);
  network_index = other.network_index;
  return *this;
}

