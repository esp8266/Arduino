/*
  Old version of ESP8266WiFiMesh.cpp - Mesh network node
  Sets up a Mesh Node which acts as a router, creating a Mesh Network with other nodes. All information
  is passed in both directions, but it is up to the user what the data sent is and how it is dealt with.
 
  Copyright (c) 2015 Julian Fell. All rights reserved.
  Updated 2018 by Anders Löfgren.
 
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






/********************************************************************************************
* NOTE!
*
* All method signatures in this file are deprecated and will be removed in core version 2.5.0.
* If you are still using these methods, please consider migrating to the new API shown in 
* the ESP8266WiFiMesh.h source file.
*
* TODO: delete this file.
********************************************************************************************/





#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WiFiServer.h>

#include "ESP8266WiFiMesh.h"

#define SSID_PREFIX      		"Mesh_Node"
#define SERVER_IP_ADDR			"192.168.4.1"
#define SERVER_PORT				4011

// DEPRECATED!
ESP8266WiFiMesh::ESP8266WiFiMesh(uint32_t chip_id, ESP8266WiFiMesh::compatibilityLayerHandlerType handler)
: _server(SERVER_PORT)
{
	_chip_id = chip_id;
	_ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );
	_ssid_prefix = String( SSID_PREFIX );
	_handler = handler;
}

/**
 * Wait for a WiFiClient to connect
 *
 * @returns: True if the client is ready, false otherwise.
 * 
 */
// DEPRECATED!
bool ESP8266WiFiMesh::waitForClient(WiFiClient &curr_client, int max_wait)
{
	int wait = max_wait;
	while(curr_client.connected() && !curr_client.available() && wait--)
		delay(3);

	/* Return false if the client isn't ready to communicate */
	if (WiFi.status() == WL_DISCONNECTED || !curr_client.connected())
		return false;
	
	return true;
}

/**
 * Send the supplied message then read back the other node's response
 * and pass that to the user-supplied handler.
 *
 * @target_ssid The name of the AP the other node has set up.
 * @message The string to send to the node.
 * @returns: True if the exchange was a succes, false otherwise.
 * 
 */
// DEPRECATED!
bool ESP8266WiFiMesh::exchangeInfo(const char *message, WiFiClient &curr_client)
{
	curr_client.println( message );

	if (!waitForClient(curr_client, 1000))
		return false;

	String response = curr_client.readStringUntil('\r');
	curr_client.readStringUntil('\n');

	if (response.length() <= 2) 
		return false;

	/* Pass data to user callback */
	_handler(response);
	return true;
}

/**
 * Connect to the AP at ssid, send them a message then disconnect.
 *
 * @target_ssid The name of the AP the other node has set up.
 * @message The string to send to the node.
 * 
 */
// DEPRECATED!
void ESP8266WiFiMesh::connectToNode(const String &target_ssid, const char *message)
{
	WiFiClient curr_client;
	WiFi.begin( target_ssid.c_str() );

	int wait = 1500;
	while((WiFi.status() == WL_DISCONNECTED) && wait--)
		delay(3);

	/* If the connection timed out */
	if (WiFi.status() != 3)
		return;

	/* Connect to the node's server */
	if (!curr_client.connect(SERVER_IP_ADDR, SERVER_PORT)) 
		return;

	if (!exchangeInfo(message, curr_client))
		return;

	curr_client.stop();
	WiFi.disconnect();
}

// DEPRECATED!
void ESP8266WiFiMesh::attemptScanKernel(const char *message)
{
	/* Scan for APs */
	int n = WiFi.scanNetworks();

	for (int i = 0; i < n; ++i) {
		String current_ssid = WiFi.SSID(i);
		int index = current_ssid.indexOf( _ssid_prefix );
		uint32_t target_chip_id = (current_ssid.substring(index + _ssid_prefix.length())).toInt();

		/* Connect to any _suitable_ APs which contain _ssid_prefix */
		if (index >= 0 && (target_chip_id < _chip_id)) {

			WiFi.mode(WIFI_STA);
			delay(100);
			connectToNode(current_ssid, message);
			WiFi.mode(WIFI_AP_STA);
			delay(100);
		}
	}
}

// DEPRECATED!
void ESP8266WiFiMesh::attemptScan(const String &message)
{
  attemptScanKernel(message.c_str());
}

// DEPRECATED!
void ESP8266WiFiMesh::attemptScan(char *message)
{
  attemptScanKernel(message);
}

// DEPRECATED!
template<size_t Size>
void ESP8266WiFiMesh::attemptScan(char (&message)[Size])
{
  attemptScanKernel(message);
}