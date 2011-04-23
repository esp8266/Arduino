/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.
 
 Circuit:
 * WiFi shield attached
 * Analog inputs attached to pins A0 through A5 (optional)

 created 13 July 2010
 by Domenico La Fauci
 */


#include <WiFi.h>
#include <IPAddress.h>

char ssid[32] = { 0 };
int status = WL_IDLE_STATUS;
IPAddress server(74,125,232,115);  // Google
//byte server[] = { 173,194,33,104 }; // Google

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
Client client(server, 80);

int startWiFiWpa()
{
  Serial.println("\nSetup WiFi Wpa...");
  //strcpy(ssid, "AndroidAP9647");
  strcpy(ssid, "Cariddi");
  Serial.print("SSID: ");
  Serial.println(ssid);
  const char *pass = "1234567890";
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED)
  {
    Serial.println("Connection Failed");
  }
  return status;
}


void setup() {
	// start the WiFi connection and the server:
	Serial.begin(9600);
	Serial.println("*** Start WebClient WiFi example ***");

	int _status = startWiFiWpa();
	if ( _status == WL_CONNECTED)
	{
		Serial.println("\nStarting connection...");
		// if you get a connection, report back via serial:
		if (client.connect()) {
			Serial.println("connected");
			// Make a HTTP request:
			client.println("GET /search?q=arduino HTTP/1.0");
			client.println();
		}
		else {
			// kf you didn't get a connection to the server:
			Serial.println("connection failed");
		}
	}
}

void loop()
{
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

