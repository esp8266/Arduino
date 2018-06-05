/*
  ESP8266WiFiMesh.cpp - Mesh network node
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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WiFiServer.h>

#include "ESP8266WiFiMesh.h"

#define SSID_PREFIX          "Mesh_Node"
#define SERVER_IP_ADDR      "192.168.4.1"
#define SERVER_PORT        4011

const IPAddress empty_IP = IPAddress();

ESP8266WiFiMesh::ESP8266WiFiMesh(uint32_t chip_id, std::function<String(String)> requestHandler, std::function<void(String)> responseHandler, String mesh_password, bool verbose_mode)
: _server(SERVER_PORT), last_ssid(""), static_IP(empty_IP), gateway(192,168,4,1), subnet_mask(255,255,255,0) // IP needs to be at the same subnet as server gateway (192.168.4 in this case). Station gateway ip must match ip for server.
{
  _chip_id = chip_id;
  _ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );
  _ssid_prefix = String( SSID_PREFIX );
  _requestHandler = requestHandler;
  _responseHandler = responseHandler;
  _mesh_password = mesh_password;
  _verbose_mode = verbose_mode;
}

void ESP8266WiFiMesh::setStaticIP(IPAddress new_IP)
{
  // Comment out the line below to remove static IP and use DHCP instead. 
  // DHCP makes WiFi connection happen slower, but there is no need to care about manually giving different IPs to the nodes and less need to worry about used IPs giving "Server unavailable" issues. 
  // Static IP is faster and will make sending of data to a node that is already transmitting data happen more reliably. 
  // Note that after WiFi.config(static_IP, gateway, subnet_mask) is used, static IP will always be active, even for new connections, unless WiFi.config(0u,0u,0u); is called.
  WiFi.config(new_IP, gateway, subnet_mask); 
  
  static_IP = new_IP;
}

IPAddress ESP8266WiFiMesh::getStaticIP()
{
  return static_IP;
}

void ESP8266WiFiMesh::begin()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP( _ssid.c_str(), _mesh_password.c_str() ); // Note that a maximum of 5 stations can be connected at a time to each AP
  _server.begin();
}

/**
 * Disconnect completely from a network.
 */
void ESP8266WiFiMesh::fullStop(WiFiClient curr_client)
{
  curr_client.stop();
  yield();
  WiFi.disconnect();
  yield();
}

/**
 * Wait for a WiFiClient to transmit
 *
 * @returns: True if the client is ready, false otherwise.
 * 
 */
bool ESP8266WiFiMesh::waitForClientTransmission(WiFiClient curr_client, int max_wait)
{
  int wait = max_wait;
  while(curr_client.connected() && !curr_client.available() && wait--)
    delay(3);

  /* Return false if the client isn't ready to communicate */
  if (WiFi.status() == WL_DISCONNECTED && !curr_client.available())
  {
    if(_verbose_mode)
      Serial.println("Disconnected!"); 
    return false;
  }
  
  return true;
}

/**
 * Send the supplied message then read back the other node's response
 * and pass that to the user-supplied responseHandler.
 *
 * @target_ssid The name of the AP the other node has set up.
 * @message The string to send to the node.
 * @returns: True if the exchange was a succes, false otherwise.
 * 
 */
bool ESP8266WiFiMesh::exchangeInfo(String message, WiFiClient curr_client)
{
  if(_verbose_mode)
    Serial.println("Transmitting");
    
  curr_client.print(message + "\r");
  yield();

  if (!waitForClientTransmission(curr_client, 1000))
  {
    fullStop(curr_client);
    return false;
  }

  if (!curr_client.available()) 
  {
    if(_verbose_mode)
      Serial.println("No response!");
    return false; // WiFi.status() != WL_DISCONNECTED so we do not want to use fullStop(curr_client) here since that would force the node to scan for WiFi networks.
  }

  String response = curr_client.readStringUntil('\r');
  yield();
  curr_client.flush();

  /* Pass data to user callback */
  _responseHandler(response);
  return true;
}

/**
 * Handle data transfer process with a connected AP.
 *
 * @message The string to send to the AP.
 * @returns: True if data transfer process successful, false otherwise.
 */
bool ESP8266WiFiMesh::attemptDataTransfer(String message)
{
  // Unlike WiFi.mode(WIFI_AP);, WiFi.mode(WIFI_AP_STA); allows us to stay connected to the AP we connected to in STA mode, at the same time as we can receive connections from other stations. 
  // We cannot send data to the AP in STA_AP mode though, that requires STA mode. 
  // Switching to STA mode will disconnect all stations connected to the node AP (though they can request a reconnect even while we are in STA mode).
  WiFi.mode(WIFI_STA);
  delay(100);
  bool result = attemptDataTransferKernel(message);
  WiFi.mode(WIFI_AP_STA); 
  delay(100);
  
  return result;
}

/**
 * Helper function that contains the core functionality for the data transfer process with a connected AP.
 *
 * @message The string to send to the AP.
 * @returns: True if data transfer process successful, false otherwise.
 */
bool ESP8266WiFiMesh::attemptDataTransferKernel(String message)
{
  WiFiClient curr_client;
  
  /* Connect to the node's server */
  if (!curr_client.connect(SERVER_IP_ADDR, SERVER_PORT)) 
  {
    fullStop(curr_client);
    if(_verbose_mode)
      Serial.println("Server unavailable");
    return false;
  }  
  
  if (!exchangeInfo(message, curr_client))
  {
    if(_verbose_mode)
      Serial.println("Transmission failed during exchangeInfo.");
    return false;
  }
  
  curr_client.stop();
  yield();

  return true;
}

/**
 * Connect to the AP at ssid and send them a message.
 *
 * @target_ssid The name of the AP the other node has set up.
 * @message The string to send to the node.
 * @target_channel The WiFI channel of the AP the other node has set up.
 * @target_bssid The mac address of the AP the other node has set up.
 * 
 */
void ESP8266WiFiMesh::connectToNode(String target_ssid, String message, int target_channel, uint8_t *target_bssid)
{
  if(static_IP != empty_IP && last_ssid != "" && last_ssid != target_ssid)
  {
    WiFi.config(0u,0u,0u); // Deactivate static IP so that we can connect to other servers via DHCP (DHCP is slower but required for connecting to more than one server, it seems (possible bug?)).
    yield();
    static_IP = empty_IP; // So we only do this once, in case there is a performance impact.
    if(_verbose_mode)
      Serial.println("\nConnecting to a second network. Static IP deactivated to make this possible.");
  }
  last_ssid = target_ssid;

  if(_verbose_mode)
    Serial.print("Connecting... ");
  WiFi.begin( target_ssid.c_str(), _mesh_password.c_str(), target_channel, target_bssid ); // Without giving channel and bssid, connection time is longer.

  int connection_start_time = millis();
  int attempt_number = 1;

  int waiting_time = millis() - connection_start_time;
  while((WiFi.status() == WL_DISCONNECTED) && waiting_time <= 10000)
  {
    if(waiting_time > attempt_number * 10000) // 10000 can be lowered if you want to limit the time allowed for each connection attempt.
    {
      if(_verbose_mode)
        Serial.print("... ");
      WiFi.disconnect();
      yield();
      WiFi.begin( target_ssid.c_str(), _mesh_password.c_str(), target_channel, target_bssid ); // Without giving channel and bssid, connection time is longer.
      attempt_number++;
    }
    delay(1);
    waiting_time = millis() - connection_start_time;
  }

  if(_verbose_mode)
    Serial.println(waiting_time);
  
  /* If the connection timed out */
  if (WiFi.status() != WL_CONNECTED)
  {
    if(_verbose_mode)
      Serial.println("Timeout");
    return;
  }

  attemptDataTransfer(message);
}

void ESP8266WiFiMesh::attemptTransmission(String message)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    attemptDataTransfer(message);
  }
  else
  {
    if(_verbose_mode)
      Serial.print("Scanning... ");
    
    /* Scan for APs */
    int n = WiFi.scanNetworks(); // Scanning causes the WiFi radio to cycle through all WiFi channels which means existing WiFi connections are likely to break or work poorly if done frequently.
  
    for (int i = 0; i < n; ++i) 
    {
      String current_ssid = WiFi.SSID(i);
      int index = current_ssid.indexOf( _ssid_prefix );
      uint32_t target_chip_id = (current_ssid.substring(index + _ssid_prefix.length())).toInt();
  
      /* Connect to any _suitable_ APs which contain _ssid_prefix */
      if (index >= 0 && (target_chip_id < _chip_id)) 
      {
        if(_verbose_mode)
          Serial.printf("First match: %s, Ch:%d (%ddBm) %s... ", WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");

        connectToNode(current_ssid, message, WiFi.channel(i), WiFi.BSSID(i));
       
        break;
      }
    }
  }
}

void ESP8266WiFiMesh::acceptRequest()
{
  while (true) {
    _client = _server.available();
    if (!_client)
      break;

    if (!waitForClientTransmission(_client, 1500) || !_client.available()) {
      continue;
    }

    /* Read in request and pass it to the supplied requestHandler */
    String request = _client.readStringUntil('\r');
    yield();
    _client.flush();

    String response = _requestHandler(request);

    /* Send the response back to the client */
    if (_client.connected())
    {
      if(_verbose_mode)
        Serial.println("Responding");
      _client.print(response + "\r");
      _client.flush();
      yield();
    }
  }
}
