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

#include "TransmissionResult.h"

TransmissionResult::TransmissionResult(int new_network_index, transmission_status_t new_transmission_status, bool autofill) : 
  NetworkInfo(new_network_index, autofill), transmission_status(new_transmission_status)
{ }

TransmissionResult::TransmissionResult(const String &new_ssid, int new_wifi_channel, uint8_t new_bssid[6], transmission_status_t new_transmission_status) : 
  NetworkInfo(new_ssid, new_wifi_channel, new_bssid), transmission_status(new_transmission_status)
{ }

TransmissionResult::TransmissionResult(const String &new_ssid, int new_wifi_channel, uint8_t new_bssid[6], int new_network_index, transmission_status_t new_transmission_status) :
  NetworkInfo(new_ssid, new_wifi_channel, new_bssid, new_network_index), transmission_status(new_transmission_status)
{ }

TransmissionResult::TransmissionResult(const NetworkInfo& origin, transmission_status_t new_transmission_status) : 
  NetworkInfo(origin), transmission_status(new_transmission_status)
{ }
