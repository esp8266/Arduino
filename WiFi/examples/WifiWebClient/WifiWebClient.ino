#include <SPI.h>

/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.
 
 Circuit:
 * WiFi shield attached
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 13 July 2010
 by Domenico La Fauci
 modified 21 May 2011
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi.h>
#include <IPAddress.h>

char ssid[] = "tigoenet";
char pass[] = "m30w-m30w";
int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
Client client;

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
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}


