/*
   httpUpdateSigned.ino - Earle F. Philhower, III
   Released into the Public Domain

   For use while building under Linux or Mac.

   Automatic code signing is not supported on Windows, so this example
   DOES NOT WORK UNDER WINDOWS.

   Shows how to use a public key extracted from your private certificate to
   only allow updates that you have signed to be applied over HTTP.  Remote
   updates will require your private key to sign them, but of course
   **ANYONE WITH PHYSICAL ACCESS CAN UPDATE THE 8266 VIA THE SERIAL PORT**.
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

ESP8266WiFiMulti WiFiMulti;

#define MANUAL_SIGNING 0

// This example is now configured to use the automated signing support
// present in the Arduino IDE by having a "private.key" and "public.key"
// in the sketch folder.  You can also programmatically enable signing
// using the method shown here.

// This key is taken from the server public certificate in BearSSL examples
// You should make your own private/public key pair and guard the private
// key (never upload it to the 8266).
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
#if MANUAL_SIGNING
BearSSL::PublicKey *signPubKey = nullptr;
BearSSL::HashSHA256 *hash;
BearSSL::SigningVerifier *sign;
#endif

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(STASSID, STAPSK);

  #if MANUAL_SIGNING
  signPubKey = new BearSSL::PublicKey(pubkey);
  hash = new BearSSL::HashSHA256();
  sign = new BearSSL::SigningVerifier(signPubKey);
  #endif
}


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    #if MANUAL_SIGNING
    // Ensure all updates are signed appropriately.  W/o this call, all will be accepted.
    Update.installSignature(hash, sign);
    #endif
    // If the key files are present in the build directory, signing will be
    // enabled using them automatically

    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://192.168.1.8/esp8266.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
  delay(10000);
}

