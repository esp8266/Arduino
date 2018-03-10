// Example use of ESP_EEPROM library for ESP8266
//
// This simple sketch wipes (erases) the flash storage area used
// to hold the EEPROM data.
//
// Useful when testing to ensure your code deals with a 'new' device
// as well as one hilding previous EEPROM data.
//

#include <ESP_EEPROM.h>

const int BLUE_LED_PIN = 2;

void setup() {
  // Remember to set your serial monitor to 74880 baud
  // This odd speed will show ESP8266 boot diagnostics too
  Serial.begin(74880);
  Serial.println();

  // It is still necessary to call begin
  // or no wipe will be performed
  EEPROM.begin(EEPROM_MIN_SIZE);


  boolean result = EEPROM.wipe();
  if (result) {
    Serial.println("All EEPROM data wiped");
  } else {
    Serial.println("EEPROM data could not be wiped from flash store");
  }
}


void loop() {
  // do nothing
}
