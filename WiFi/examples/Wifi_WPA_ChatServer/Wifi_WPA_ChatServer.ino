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

char ssid[] = "yourNetwork";     // the name of your network
char pass[] = "secretPassword";  // the WPA2 password for your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// telnet defaults to port 23
Server server(23);

boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  else {
    server.begin();
    Serial.print("Connected to wifi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.print(myAddress[0]);
    Serial.print(".");
    Serial.print(myAddress[1]);
    Serial.print(".");
    Serial.print(myAddress[2]);
    Serial.print(".");
    Serial.println(myAddress[3]);
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
    client.write(thisChar);
    // echo the bytes to the server as well:
    Serial.print(thisChar);
  }
}




