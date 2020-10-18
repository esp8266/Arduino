/*
    This sketch generates encoded WiFi SSID/password.
*/

#include <ESP8266WiFiPassMan.h>

// WiFi credentials to be encoded
#define WIFI_SSID   "my-ssid"
#define WIFI_PASS   "my-secret-password-123456789"

// Use predefined secret key for encoding. Keep 0 to generate new secret key.
const uint32_t secretKey[4] PROGMEM = {
  0x00000000, 0x00000000, 0x00000000, 0x00000000
};

// true:  WiFi credentials can be decoded on this ESP8266 only (recommended).
// false: WiFi credentials can be decoded on any ESP8266 (less secure).
bool encodeAPMAC = true;


void setup() {
  // Initialize Serial
  delay(500);
  Serial.begin(115200);
  Serial.println(F("\nESP8266 WiFi password manager setup\n"));

  // Create WiFi password manager object.
  ESP8266WiFiPassMan wpm(encodeAPMAC);

  // Optional: Run selftest encoding/decoding string up to 63 characters
  if (!wpm.selftest("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!")) {
    return;
  }
  if (!wpm.selftest(WIFI_SSID)) {
    return;
  }
  if (!wpm.selftest(WIFI_PASS)) {
    return;
  }

  // Encode and print WiFi credentials
  wpm.encodeWiFiCredentials(WIFI_SSID, WIFI_PASS, secretKey);
}

void loop() {
}