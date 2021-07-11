/**
   PostHttpsClient.ino
    Created on: 28.06.2021
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>

// Fingerprint for demo URL, expires on September 5, 2021, needs to be updated well before this date
const uint8_t fingerprint[20] = {0xbb, 0x88, 0x7f, 0x7c, 0x77, 0xc2, 0x59, 0x97, 0xb7, 0x00, 0x35, 0x74, 0x50, 0x47, 0x7e, 0x67, 0x42, 0x02, 0x2f, 0xf0};
const char* URL = "https://www.reseau-astuce.fr/fr/horaires-a-larret/28/StopTimeTable/NextDeparture";
const char* REQUEST = "destinations=%7B%221%22%3A%22Technop%C3%B4le+SAINT-ETIENNE-DU-ROUVRAY%22%7D&stopId=102154&lineId=175&sens=1";

const char* WIFI_SSID = "AirPort Extreme";
const char* WIFI_PWD = "3saih3x8";

ESP8266WiFiMulti WiFiMulti;



void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PWD);

  Serial.println("[WIFI] Connecting to WiFi ...");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.println();
  Serial.print("[WIFI] Connected with IP : ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

}



void loop() {

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.println("[HTTPS] begin...");

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();                  // Ignore SSL certificate
    //client->setFingerprint(fingerprint);  //Use SSL
    HTTPClient https;

    if (https.begin(*client, URL)) {
      https.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36");
      https.addHeader("Accept-Language", "fr-FR,fr;q=0.9,en;q=0.8");
      https.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
      int httpCode = https.POST(REQUEST);

      // If response code is positive => no error
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println("[HTTPS] POST... SUCCESS!");
          Serial.println(payload);
        }

        // Error (response code is negative)
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();

      // Unable to reach the server
    } else {
      Serial.println("[HTTPS] Unable to connect");
    }
  }
  delay(10000);
}
