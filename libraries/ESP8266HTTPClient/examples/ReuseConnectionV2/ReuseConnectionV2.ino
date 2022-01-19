/**
   reuseConnectionV2.ino

    Created on: 22.11.2015

   This example reuses the http connection and also restores the connection if the connection is lost
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

ESP8266WiFiMulti WiFiMulti;

HTTPClient http;
WiFiClient client;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println("Connecting to WiFi...");

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);

  // wait for WiFi connection
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.write('.');
    delay(500);
  }
  Serial.println(" connected to WiFi");

  // allow reuse (if server supports it)
  http.setReuse(true);

  http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html");
  //http.begin(client, "jigsaw.w3.org", 80, "/HTTP/connection.html");
}

int pass = 0;

void loop() {
  // First 10 loop()s, retrieve the URL
  if (pass < 10) {
    pass++;
    Serial.printf("Reuse connection example, GET url for the %d time\n", pass);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        http.writeToStream(&Serial);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      // Something went wrong with the connection, try to reconnect
      http.end();
      http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html");
      //http.begin(client, "jigsaw.w3.org", 80, "/HTTP/connection.html");
    }

    if (pass == 10) {
      http.end();
      Serial.println("Done testing");
    } else {
      Serial.println("\n\n\nWait 5 second...\n");
      delay(5000);
    }
  }
}
