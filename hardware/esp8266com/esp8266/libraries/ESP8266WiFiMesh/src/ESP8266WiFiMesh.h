/*
  ESP8266WiFiMesh.h - Mesh network node
  Sets up a Mesh Node which acts as a router, creating a Mesh Network with other nodes. All information
  is passed in both directions, but it is up to the user what the data sent is and how it is dealt with.
 
  Copyright (c) 2015 Julian Fell. All rights reserved.
 
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

#ifndef __WIFIMESH_H__
#define __WIFIMESH_H__

#include <WiFiClient.h> 
#include <WiFiServer.h>
#include <functional>

class ESP8266WiFiMesh {

private:
	String _ssid;
	String _ssid_prefix;
	uint32_t _chip_id;

	std::function<String(String)> _handler;
	
	WiFiServer  _server;
	WiFiClient  _client;

	void connectToNode(String target_ssid, String message);
	bool exchangeInfo(String message, WiFiClient curr_client);
	bool waitForClient(WiFiClient curr_client, int max_wait);

public:

	/**
	 * WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
	 *
	 * @chip_id A unique identifier number for the node.
	 * @handler The callback handler for dealing with received messages. Takes a string as an argument which
	 *          is the string received from another node and returns the string to send back.
	 * 
	 */
	ESP8266WiFiMesh(uint32_t chip_id, std::function<String(String)> handler);

	/**
	 * Initialises the node.
	 */
	void begin();

	/**
	 * Scan for other nodes, and exchange the chosen message with any that are found.
	 *
	 * @message The message to send to all other nodes.
	 * 
	 */
	void attemptScan(String message);

	/**
	 * If any clients are connected, accept their requests and call the hander function for each one.
	 */
	void acceptRequest();
};

#endif
