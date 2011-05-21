/*
 Wifi Chat  Server
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
  
  This example obtains an IP address from the Wifi router and
  reports it.
 
  Circuit:
 * WiFi shield attached
 
 created 21 May 2011
 by Tom Igoe
 
 */

#include <SPI.h>
#include <WiFi.h>
#include <IPAddress.h>

char ssid[] = "yourWifiNetwork";
char pass[] = "seekrit-password";
int status = WL_IDLE_STATUS;
// telnet defaults to port 23
Server server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status == WL_CONNECTED) {
    // start listening for clients
    server.begin();
     Serial.println("Wifi shield is idle");
    // report the IP address:
    IPAddress myIPAddress = WiFi.localIp();
    Serial.print("My IP Address: ");
    Serial.print(WiFi.localIp()[3]);
    Serial.print(".");
    Serial.print(WiFi.localIp()[2]);
    Serial.print(".");
    Serial.print(WiFi.localIp()[1]);
    Serial.print(".");
    Serial.println(WiFi.localIp()[0]); 
  } else {
    // if you didn't get a wifi connection, report what happened:
   switch (status) {
      case WL_IDLE_STATUS:
      Serial.println("Wifi connection succeeded");
      break;
      case WL_NO_SSID_AVAIL:
       Serial.println("No Wifi network available");
        break;
      case WL_CONNECT_FAILED:
       Serial.println("Wifi connection failed");
        break;
      case WL_CONNECTION_LOST:
       Serial.println("Wifi connection lost");
        break;
      case WL_DISCONNECTED:
       Serial.println("Wifi disconnected");
     break;
   }
  }
}

void loop() {
  // wait for a new client:
  Client client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.println("I have a new client");
      client.println("Hello, client!"); 
      gotAMessage = true;
    }

    // read the bytes incoming from the client:
    char thisChar = client.read();
    // echo the bytes back to the client:
    server.write(thisChar);
    // echo the bytes to the server as well:
    Serial.print(thisChar);
  }
}







