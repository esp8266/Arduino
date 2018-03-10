// Example use of ESP_EEPROM library for ESP8266
//
// Normally writing to the 'emulated' EEPROM on ESP8266 requires an erase of the flash page used to hold
// the EEPROM data followed by a re-write of the changed data.
// The erasure takes a significant amount of time (10s of ms) and during this time
// interrupts must be blocked.
// In some cases this interferes with the sketch operation (e.g. it produces a noticeable
// blackout/flash for any PWM controlled lights as ESP8266 PWM relies on interrupts)
//
// The ESP_EEPROM library writes each new version of the EEPROM data to a new area until flash
// is full and so avoids wiping flash until necessary
//
// It's best for use when there are only a few things to save in EEPROM
// (i.e. total the size of the saved info is much smaller than the available flash size)
//

#include <ESP_EEPROM.h>

int eepromVar1 = 0;
long eepromVar2 = 1234;

void setup() {
  // Remember to set your serial monitor to 74880 baud
  // This odd speed will show ESP8266 boot diagnostics too
  Serial.begin(74880);
  Serial.println();

  // All the library functions are accessed via the EEPROM object created when
  // you include the library header ESP_EEPROM.h

  // The library needs to know what size you need for your EEPROM variables
  // The minimum size is 16

  // The begin() call is required to initialise the EEPROM library
  EEPROM.begin(16);

  // put some data into eeprom
  EEPROM.put(0, eepromVar1);  // int - so 4 bytes (next address is '4')
  EEPROM.put(4, eepromVar2);  // long - so 8 bytes (next address would be '12')

  // write the data to EEPROM
  boolean ok1 = EEPROM.commit();
  Serial.println((ok1) ? "First commit OK" : "Commit failed");

  // The eeprom data gets changed
  eepromVar1 = 1;   // Change some data

  // set the EEPROM data ready for writing
  EEPROM.put(0, eepromVar1);

  // commit (write) the data to EEPROM - only actually writes if there has been a change
  boolean ok2 = EEPROM.commit();
  Serial.println((ok2) ? "Second commit OK" : "Commit failed");

  // How to read stuff back into variables
  // variables should be same size as originally written
  int aNewVar1;
  long aNewVar2;
  EEPROM.get(0, aNewVar1);
  EEPROM.get(4, aNewVar2);
  Serial.print("Read back a variable 1 from EEPROM: ");
  Serial.println(aNewVar1);
  Serial.print("Read back a variable 2 from EEPROM: ");
  Serial.println(aNewVar2);

}


void loop() {
  delay(1000);
}
