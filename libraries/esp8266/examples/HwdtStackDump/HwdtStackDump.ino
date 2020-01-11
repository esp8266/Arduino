/*
 * A Hardware WDT Stack Dump by Michael Hightower
 *
 * This Sketch demonstrates the use of a tool to print a stack dump
 * at reboot after a Hardware WDT event.
 *
 * The module hwdt_app_entry.cpp writes a stack dump to the serial interface
 * after a Hardware Watchdog Timer has struck and a new boot cycle has begun.
 * Be sure your sketch has properly initialized the Serial port before the crash.
 * See hwdt_app_entry.cpp for more details.
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>
// #include <coredecls.h> // for disable_extra4k_at_link_time();
void enable_debug_hwdt_at_link_time(void);

#include "AddOn.h"
#include "hwdt_app_entry.h"


void setup(void)
{
  enable_debug_hwdt_at_link_time();
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
