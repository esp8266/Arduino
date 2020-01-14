/*
   A Hardware WDT Stack Dump by Michael Hightower

   This Sketch demonstrates the use of a tool to print a stack dump
   at reboot after a Hardware WDT event.

   The module hwdt_app_entry.cpp writes a stack dump to the serial interface
   after a Hardware Watchdog Timer has struck and a new boot cycle has begun.
   Note, at restart the UART speed is set by the ROM to 115200 bps. This is the
   speed your serial device needs to be set to for displaying the dump.
   See hwdt_app_entry.cpp for more details.

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>
// void enable_debug_hwdt_at_link_time(void);

#include "AddOn.h"
/*
  To see the difference with and without this tool. Comment out the #include
  below. And comment out the "#define DEBUG_HWDT" line in hwdt_app_entry.cpp.
*/
#include "hwdt_app_entry.h"


void setup(void) {
#ifdef DEBUG_HWDT
  enable_debug_hwdt_at_link_time();
#endif
  WiFi.persistent(false); // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(20);
  Serial.println();
  Serial.println();
  Serial.println(F("The Hardware Watchdog Timer Demo is now available for crashing ..."));
  Serial.println();
  processKey(Serial, '?');
}


void loop(void) {
  if (Serial.available() > 0) {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}
