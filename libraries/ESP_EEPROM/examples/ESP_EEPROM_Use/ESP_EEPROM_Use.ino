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

const int BLUE_LED_PIN = 2;

// The neatest way to access variables stored in EEPROM is using a structure
struct MyEEPROMStruct {
  int     anInteger;
  float   aFloating;
  int     anotherInteger;
  byte    someBytes[12];
  boolean state;
} eepromVar1, eepromVar2;

void setup() {
  // Remember to set your serial monitor to 74880 baud
  // This odd speed will show ESP8266 boot diagnostics too
  Serial.begin(74880);
  Serial.println();
  
  // Set up the initial (default) values for what is to be stored in EEPROM
  eepromVar1.anInteger = 99;
  eepromVar1.aFloating = 99.99;
  eepromVar1.anotherInteger = 42;
  eepromVar1.state = true;

  // All the library functions are accessed via the EEPROM object created when
  // you include the library header ESP_EEPROM.h

  // The library needs to know what size you need for your EEPROM variables
  // Using a structure makes this easy.

  // The begin() call will find the data previously saved in EEPROM if the same size
  // as was previously committed. If the size is different then the EEEPROM data is cleared. 
  // Note that this is not made permanent until you call commit();
  EEPROM.begin(sizeof(MyEEPROMStruct));

  // Check if the EEPROM contains valid data from another run
  // If so, overwrite the 'default' values set up in our struct
  if(EEPROM.percentUsed()!=0) {
    EEPROM.get(0, eepromVar1);
    eepromVar1.anInteger++;     // make a change to our copy of the EEPROM data
    Serial.println("EEPROM has data from a previous run.");
    Serial.print(EEPROM.percentUsed());
    Serial.println("% of ESP flash space currently used");
  } else {
    Serial.println("EEPROM size changed - EEPROM data zeroed - commit() to make permanent");    
  }

  //
  // (some code that might change the EEPROM data)
  //

  // set the EEPROM data ready for writing
  EEPROM.put(0, eepromVar1);

  // write the data to EEPROM
  boolean ok = EEPROM.commit();
  Serial.println((ok) ? "Commit OK" : "Commit failed");

  // Get EEPROM data into our local copy
  // For this example, a different struct variable is used 
  EEPROM.get(0, eepromVar2);

  Serial.print("EEPROM data read, anInteger=");
  Serial.println(eepromVar2.anInteger);
}


void loop() {
  // do nothing
}
