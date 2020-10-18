/*
    This sketch decodes WiFi SSID/password and connects to the access point.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiPassMan.h>

//----------------------------------------------
// *** DON'T LOOSE YOUR SECRET KEY! ***

// WiFi credentials can be copied to any ESP8266
#define ENCODE_AP_MAC false

// Secret key for WiFi password manager
const uint32_t SECRET_KEY[4] PROGMEM = {
  0x8DD84B5F, 0x642460D9, 0xAB1FA984, 0x677D2D1A
};

// Encoded WiFi SSID
const uint32_t WIFI_ENC_SSID[16] PROGMEM = {
  0x1DD7222B, 0x95059384, 0x4DD5DDBA, 0x90421435,
  0x1E337631, 0x84A2BE61, 0xE2BD2CDF, 0x5A8D26F4,
  0xE61D203E, 0x05FAF1B4, 0x2498B611, 0xA61EC580,
  0xE54CCC21, 0xD16632FF, 0x1B4C0C64, 0x4439A5CB,
};

// Encoded WiFi password
const uint32_t WIFI_ENC_PASS[16] PROGMEM = {
  0xFE06668A, 0xF23554A9, 0xD3A974C9, 0xE60768A6,
  0x39743699, 0x5FBB5DC9, 0xB6C64C7B, 0xCF4E4D66,
  0x02AC6FB0, 0x6875CED0, 0x6498BA68, 0xF8D7244D,
  0xC89461D7, 0xD93F8A3C, 0x465F39D6, 0xD9EC5AC3,
};
//----------------------------------------------

void setup() {
  char wifi_ssid[64];
  char wifi_pass[64];
  uint32_t tStart;

  // Never save WiFi configuration in flash when using WiFi password manager
  WiFi.persistent(false);

  // Initialize Serial
  delay(500);
  Serial.begin(115200);
  Serial.println(F("\nESP8266 WiFi password manager connect"));

  // Create WiFi password manager object.
  ESP8266WiFiPassMan wpm(ENCODE_AP_MAC);

  // Check if unsecure WiFi password is saved in on-chip flash
  if (wpm.isWiFiCredentialSaved()) {
    Serial.println(F("Erasing unsecure WiFi credentials..."));
    wpm.eraseWiFiPasswords();
    // ESP8266 reset is generated and does not return here
  }

  // Start time to measure decoding duration
  tStart = micros();

  // Decode WiFi SSID
  if (!wpm.decode_P(SECRET_KEY, WIFI_ENC_SSID, wifi_ssid)) {
    Serial.println(F("Decode SSID failed"));
    return;
  }

  // Decode WiFi password
  if (!wpm.decode_P(SECRET_KEY, WIFI_ENC_PASS, wifi_pass)) {
    Serial.println(F("Decode PASS failed"));
    return;
  }

  // Print decoding duration
  Serial.printf(PSTR("Decoding: %dus\n"), micros() - tStart);

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Connect to base station
  Serial.print(F("Connecting to "));
  Serial.println(wifi_ssid);
  // Serial.print(F("Password: "));
  // Serial.println(wifi_pass);

  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi connected, IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
}
