/*
  Web  Server
 
 A simple web server that shows the value of the analog input pins.
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

Server server(80);

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

void setup()
{
	// start the WiFi connection and the server:
	Serial.begin(9600);
	Serial.println("*** Start WebServer WiFi example ***");

	int _status = startWiFiWpa();
	if ( _status == WL_CONNECTED)
	{
		Serial.println("\nStarting server...");
		server.begin();
	}
}

void loop()
{
  // listen for incoming clients
  Client client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(analogRead(analogChannel));
            client.println("<br />");
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
