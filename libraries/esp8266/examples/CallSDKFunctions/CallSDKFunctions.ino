
/*
   NativeSdk by Simon Peter
   Access functionality from the Espressif ESP8266 SDK
   This example code is in the public domain

   This is for advanced users.
   Note that this makes your code dependent on the ESP8266, which is generally
   a bad idea. So you should try to use esp8266/Arduino functionality
   where possible instead, in order to abstract away the hardware dependency.
*/

// Expose Espressif SDK functionality - wrapped in ifdef so that it still
// compiles on other platforms
#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Call Espressif SDK functionality - wrapped in ifdef so that it still
  // compiles on other platforms
#ifdef ESP8266
  Serial.print("wifi_station_get_hostname: ");
  Serial.println(wifi_station_get_hostname());
#endif
  delay(1000);
}
