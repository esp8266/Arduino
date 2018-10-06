/**
   httpUpdate.ino

    Created on: 27.11.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const char pubkey[] PROGMEM = R"EOF(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyW5a4OO7xd6pRDTETO7h
vEMBOr/wCqcTi/gi2/99rPnVvT7IH/qGSiYMxpGFKCXVqS4rU5k2XspALEquyGse
Uav5hqsgHO6CQFFALqXzUVNCsJA9V6raUFhBaIqqCKmWzmeAkV+avM/zDQR9Wj1Q
TCmi997sJJ5ICQc8cGSdvrhisUSbfPpKI9Ql4FApOZRABBBuZKhN9ujIzTv3OIAa
rpQVfACKKuv7a2N2qU0uxRDojeO6odT1c6AZv6BlcF76GQGTo+/oBhqPdbAQuaBy
cuWNgTnDQd6KUzV0E4it2fNG+cHN4kEvofN6gHx8IbOrXwFttlpAH/o7bcfCnUVh
TQIDAQAB
-----END PUBLIC KEY-----
)EOF";
BearSSLPublicKey *signPubKey = nullptr;
BearSSLHashSHA256 *hash;
BearSSLVerifier *sign;

void setup() {

  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SSID", "PASS");

  signPubKey = new BearSSLPublicKey(pubkey);
  hash = new BearSSLHashSHA256();
  sign = new BearSSLVerifier(signPubKey);
}


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;


    Update.installSignature(hash, sign);

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://192.168.1.8/esp8266.bin");
    // Or:
    //t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
    }
  }
  delay(10000);
}

