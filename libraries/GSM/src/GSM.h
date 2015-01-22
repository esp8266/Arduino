/*
This file is part of the GSM3 communications library for Arduino
-- Multi-transport communications platform
-- Fully asynchronous
-- Includes code for the Arduino-Telefonica GSM/GPRS Shield V1
-- Voice calls
-- SMS
-- TCP/IP connections
-- HTTP basic clients

This library has been developed by Telefónica Digital - PDI -
- Physical Internet Lab, as part as its collaboration with
Arduino and the Open Hardware Community. 

September-December 2012

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
https://github.com/BlueVia/Official-Arduino
*/
#ifndef _GSM3SIMPLIFIERFILE_
#define _GSM3SIMPLIFIERFILE_

// This file simplifies the use of the GSM3 library
// First we include everything. 

#include <GSM3CircularBuffer.h>
#include <GSM3MobileCellManagement.h>
#include <GSM3MobileClientService.h>
#include <GSM3MobileNetworkRegistry.h>
#include <GSM3MobileServerService.h>
#include <GSM3ShieldV1AccessProvider.h>
#include <GSM3ShieldV1BandManagement.h>
#include <GSM3ShieldV1ClientProvider.h>
#include <GSM3ShieldV1DataNetworkProvider.h>
#include <GSM3ShieldV1ModemVerification.h>
#include <GSM3ShieldV1CellManagement.h>
#include <GSM3ShieldV1PinManagement.h>
#include <GSM3ShieldV1ScanNetworks.h>
#include <GSM3SMSService.h>
#include <GSM3VoiceCallService.h>

#define GSM GSM3ShieldV1AccessProvider
#define GPRS GSM3ShieldV1DataNetworkProvider
#define GSMClient GSM3MobileClientService
#define GSMServer GSM3MobileServerService
#define GSMVoiceCall GSM3VoiceCallService
#define GSM_SMS GSM3SMSService

#define GSMPIN GSM3ShieldV1PinManagement
#define GSMModem GSM3ShieldV1ModemVerification
#define GSMCell GSM3ShieldV1CellManagement
#define GSMBand GSM3ShieldV1BandManagement
#define GSMScanner GSM3ShieldV1ScanNetworks

#endif