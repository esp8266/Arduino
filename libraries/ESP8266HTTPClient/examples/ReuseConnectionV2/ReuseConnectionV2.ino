/**
   reuseConnectionV2.ino

    Created on: 22.11.2015

   This example reuses the http connection and also restores the connection if the connection is lost
*/


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

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
  WiFiMulti.addAP("SSID", "PASSWORD");

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

void loop() {
  for(int i = 0; i < 10; i++) {
    Serial.printf("Reuse connection example, GET url for the %d time\n", i + 1);
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

    Serial.println("\n\n\nWait 5 second...\n");
    delay(5000);
  }

  http.end();

  Serial.println("Done testing, now wait forever");
  for(;;) delay(100); // Wait forever
}
