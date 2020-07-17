/*

   There is a tool to print a stack dump at boot after a Hardware WDT
   reset. To use the Hardware WDT Reset stack dump tool, you can select HWDT or
   HWDT_NO4KEXTRA from the Arduino IDE menu "Tools->Debug Level" before
   building your sketch. Note, 'Tools->Debug port' selection is not needed or
   referenced for printing the HWDT stack dump.

   When the ESP8266 restarts because of a Hardware WDT reset, the serial port
   speed defaults to 115200 bps. The HWDT stack dump will always print on port
   'Serial'.

   To demonstrates this tool, this Sketch offers a few options for crashing the
   ESP8266 with and without a HWDT reset.

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>

void setup(void) {
  WiFi.persistent(false); // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(20);    // This delay helps when using the 'Modified Serial monitor' otherwise it is not needed.
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
