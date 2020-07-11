/*
   A Hardware WDT Reset stack dump tool by Michael Hightower

   This Sketch demonstrates the use of a tool that prints a stack dump after
   a Hardware WDT reset. After a Hardware Watchdog Timer reset, the module
   hwdt_app_entry.cpp writes a stack dump to the serial interface.

   The goal was to have a file (hwdt_app_entry.cpp) that could be dropped into
   a sketch directory, then open the sketch project, build, upload, and debug.

   When the ESP8266 restarts because of a Hardware WDT reset, the serial port
   speed defaults to 115200 bps. For support of other speeds, review the
   comments on option HWDT_UART_SPEED in wdt_app_entry.cpp.

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>

#include "AddOn.h"
/*
  If you need to run your sketch w/o this tool or you want to see the
  difference with and without this tool. Comment out the #include below. And
  comment out the "#define DEBUG_HWDT" line in hwdt_app_entry.cpp.
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
