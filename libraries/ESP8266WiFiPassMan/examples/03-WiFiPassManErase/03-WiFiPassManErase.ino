/*
    This sketch demonstrates weakness of the Espressif ESP8266 chip design to
    store WiFi passwords plain text in flash.

    Use the ESP8266WiFiPassMan to encode WiFi credentials.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiPassMan.h>

bool dumpWiFiPasswords() {
  // Fixed number of WiFi configurations
  const uint8_t numConfigs = 5;
  struct station_config config[numConfigs];
  bool retval;

  // Clear config
  memset(config, 0, sizeof(config));

  // Print configs
  retval = wifi_station_get_ap_info(config);
  for (uint8_t i = 0; i < numConfigs; i++) {
    Serial.printf(PSTR("Config %d:\n"), i);
    Serial.printf(PSTR("  ssid:      %s\n"), config[i].ssid);
    Serial.printf(PSTR("  password:  %s\n"), config[i].password);
    Serial.printf(PSTR("  bssid_set: %d\n"), config[i].bssid_set);
    Serial.printf(PSTR("  bssid:     %02x:%02x:%02x:%02x:%02x\n"),
                  config[i].bssid[0], config[i].bssid[1], config[i].bssid[2],
                  config[i].bssid[3], config[i].bssid[4], config[i].bssid[5]);
    Serial.printf(PSTR("  threshold.rssi: %d\n"), config[i].threshold.rssi);
    Serial.printf(PSTR("  threshold.authmode: %d\n"), (uint8_t)config[i].threshold.authmode);
  }

  return retval;
}

void serialReadAnswer() {
  static char readBuffer[5];
  static uint8_t readIndex;

  while (1) {
    if (Serial.available()) {
      char c = Serial.read();
      Serial.print(c);
      if (c == '\n') {
        if (strncmp(readBuffer, "YES", 3) == 0) {
          Serial.println(F("Erasing WiFi credentials..."));

          // The erase includes system_restore() and system_restart()
          ESP8266WiFiPassMan wpm;
          wpm.eraseWiFiPasswords();
        } else {
          Serial.println(F("Aborted."));
          break;
        }
        readIndex = 0;
      } else {
        readBuffer[readIndex] = c;
        readIndex++;
        if (readIndex > sizeof(readBuffer)) {
          readIndex = 0;
        }
      }
    }
  }
}

void setup() {
  // Initialize Serial
  delay(500);
  Serial.begin(115200);
  Serial.println(F("\nESP8266 erase WiFi credentials example"));

  // Print all plain text WiFi credentials stored in flash
  if (dumpWiFiPasswords()) {
    Serial.println(F("Would you like to erase all plain text WiFi passwords?"));
    Serial.println(F("Type YES + ENTER to erase"));
    serialReadAnswer();
  } else {
    Serial.println(F("No plain text WiFi passwords found"));
  }
}

void loop() {
}
