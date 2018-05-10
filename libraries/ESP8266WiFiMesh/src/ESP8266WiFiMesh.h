/*
  ESP8266WiFiMesh.h - Mesh network node
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
  bool _verbose_mode;

  bool DHCP_activated;

  IPAddress static_IP;
  IPAddress gateway;
  IPAddress subnet_mask;

  String last_ssid;

  std::function<String(String)> _requestHandler;
  std::function<void(String)> _responseHandler;
  
  WiFiServer  _server;
  WiFiClient  _client;

  void fullStop(WiFiClient curr_client);
  void connectToNode(String target_ssid, String message, int target_channel, uint8_t *target_bssid);
  bool exchangeInfo(String message, WiFiClient curr_client);
  bool waitForClient(WiFiClient curr_client, int max_wait);
  bool attemptDataTransfer(String message);
  bool attemptDataTransferKernel(String message);

public:

  /**
   * WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
   *
   * @chip_id A unique identifier number for the node.
   * @requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
   *          is the request string received from another node and returns the string to send back.
   * @responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
   *          is the response string received from another node.
   * @verbose_mode Determines if we should print the events occurring in the library to Serial. Off by default.
   * 
   */
  ESP8266WiFiMesh(uint32_t chip_id, std::function<String(String)> requestHandler, std::function<void(String)> responseHandler, bool verbose_mode = false);

  /**
   * Initialises the node.
   */
  void begin();

  /**
   * If AP connection exists, send message to AP.
   * Otherwise, scan for other nodes and exchange the chosen message with any that are found.
   *
   * @message The message to send to other nodes.
   * 
   */
  void attemptTransmission(String message);

  /**
   * If any clients are connected, accept their requests and call the requestHandler function for each one.
   */
  void acceptRequest();

  // IP needs to be at the same subnet as server gateway
  void setStaticIP(IPAddress new_ip);
  IPAddress getStaticIP();
};

#endif
