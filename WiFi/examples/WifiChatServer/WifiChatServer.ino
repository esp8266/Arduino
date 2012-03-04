/*
 Chat  Server
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 
 Circuit:
 * WiFi shield attached
 
 created 18 Dec 2009
 by David A. Mellis
 modified 4 Mar 2012
 by Tom Igoe
 
 */


#include <SPI.h>
#include <WiFi.h>
char ssid[] = "tigoenet"; //  your network SSID (name) 
char pass[] = "m30w-m30w";    // your network password (use for WPA, or use as key for WEP)


//char ssid[] = "YourNetwork"; //  your network SSID (name) 
//char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(23);

boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to Wifi network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  else {
    server.begin();
    Serial.print("Connected to wifi.");
    printWifiStatus();
  }
}
void loop() {
  // wait for a new client:
  WiFiClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.println("We have a new client");
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


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

