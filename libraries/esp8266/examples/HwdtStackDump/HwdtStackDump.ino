/*
   A Hardware WDT Reset stack dump tool by Michael Hightower

   This Sketch demonstrates the use of a tool that prints a stack dump after
   a Hardware WDT reset. After a Hardware Watchdog Timer reset, the module
   hwdt_app_entry.cpp writes a stack dump to the serial interface.

   The goal was to have a file (hwdt_app_entry.cpp) that could be dropped into
   a sketch directory, then open the sketch project, build, upload, and debug.

   Note, at the restart, the UART speed is set by the ROM to 115200 bps. This
   is the speed your serial device needs to be for displaying the dump.
   See hwdt_app_entry.cpp for more options and details.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>

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
