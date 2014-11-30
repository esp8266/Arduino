/*
  WiFi.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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
*/

#include "ESP8266WiFi.h"
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}



WiFiClass::WiFiClass()
{
	// Driver initialization
	init();
}

void WiFiClass::init()
{

}


// int WiFiClass::begin(char* ssid)
// {
// 	uint8_t status = WL_IDLE_STATUS;
// 	uint8_t attempts = WL_MAX_ATTEMPT_CONNECTION;

//    if (WiFiDrv::wifiSetNetwork(ssid, strlen(ssid)) != WL_FAILURE)
//    {
// 	   do
// 	   {
// 		   delay(WL_DELAY_START_CONNECTION);
// 		   status = WiFiDrv::getConnectionStatus();
// 	   }
// 	   while ((( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED))&&(--attempts>0));
//    }else
//    {
// 	   status = WL_CONNECT_FAILED;
//    }
//    return status;
// }

// int WiFiClass::begin(char* ssid, uint8_t key_idx, const char *key)
// {

// }

int WiFiClass::begin(const char* ssid, const char *passphrase)
{
	struct station_config conf;
	strcpy((char*) conf.ssid, ssid);
	strcpy((char*) conf.password, passphrase);
	wifi_station_set_config(&conf);
	return 0;
}

// void WiFiClass::config(IPAddress local_ip)
// {
// 	WiFiDrv::config(1, (uint32_t)local_ip, 0, 0);
// }

// void WiFiClass::config(IPAddress local_ip, IPAddress dns_server)
// {
// 	WiFiDrv::config(1, (uint32_t)local_ip, 0, 0);
// 	WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);
// }

// void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
// {
// 	WiFiDrv::config(2, (uint32_t)local_ip, (uint32_t)gateway, 0);
// 	WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);
// }

// void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
// {
// 	WiFiDrv::config(3, (uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
// 	WiFiDrv::setDNS(1, (uint32_t)dns_server, 0);
// }

// void WiFiClass::setDNS(IPAddress dns_server1)
// {
// 	WiFiDrv::setDNS(1, (uint32_t)dns_server1, 0);
// }

// void WiFiClass::setDNS(IPAddress dns_server1, IPAddress dns_server2)
// {
// 	WiFiDrv::setDNS(2, (uint32_t)dns_server1, (uint32_t)dns_server2);
// }

// int WiFiClass::disconnect()
// {
//     return WiFiDrv::disconnect();
// }

// uint8_t* WiFiClass::macAddress(uint8_t* mac)
// {
// 	uint8_t* _mac = WiFiDrv::getMacAddress();
// 	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
//     return mac;
// }
   
// IPAddress WiFiClass::localIP()
// {
// 	IPAddress ret;
// 	WiFiDrv::getIpAddress(ret);
// 	return ret;
// }

// IPAddress WiFiClass::subnetMask()
// {
// 	IPAddress ret;
// 	WiFiDrv::getSubnetMask(ret);
// 	return ret;
// }

// IPAddress WiFiClass::gatewayIP()
// {
// 	IPAddress ret;
// 	WiFiDrv::getGatewayIP(ret);
// 	return ret;
// }

// char* WiFiClass::SSID()
// {
//     return WiFiDrv::getCurrentSSID();
// }

// uint8_t* WiFiClass::BSSID(uint8_t* bssid)
// {
// 	uint8_t* _bssid = WiFiDrv::getCurrentBSSID();
// 	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
//     return bssid;
// }

// int32_t WiFiClass::RSSI()
// {
//     return WiFiDrv::getCurrentRSSI();
// }

// uint8_t WiFiClass::encryptionType()
// {
//     return WiFiDrv::getCurrentEncryptionType();
// }


// int8_t WiFiClass::scanNetworks()
// {
// 	uint8_t attempts = 10;
// 	uint8_t numOfNetworks = 0;

// 	if (WiFiDrv::startScanNetworks() == WL_FAILURE)
// 		return WL_FAILURE;
//  	do
//  	{
//  		delay(2000);
//  		numOfNetworks = WiFiDrv::getScanNetworks();
//  	}
// 	while (( numOfNetworks == 0)&&(--attempts>0));
// 	return numOfNetworks;
// }

// char* WiFiClass::SSID(uint8_t networkItem)
// {
// 	return WiFiDrv::getSSIDNetoworks(networkItem);
// }

// int32_t WiFiClass::RSSI(uint8_t networkItem)
// {
// 	return WiFiDrv::getRSSINetoworks(networkItem);
// }

// uint8_t WiFiClass::encryptionType(uint8_t networkItem)
// {
//     return WiFiDrv::getEncTypeNetowrks(networkItem);
// }

// uint8_t WiFiClass::status()
// {
//     return WiFiDrv::getConnectionStatus();
// }

// int WiFiClass::hostByName(const char* aHostname, IPAddress& aResult)
// {
// 	return WiFiDrv::getHostByName(aHostname, aResult);
// }

WiFiClass WiFi;
