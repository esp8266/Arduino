/*

   There is a tool to print a stack dump at boot after a Hardware WDT
   reset. To use the Hardware WDT Reset stack dump tool, you can select HWDT or
   HWDT_NOEXTRA4K from the Arduino IDE menu "Tools->Debug Level" before
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
#include <coredecls.h> // g_pcont - only needed for this debug demo

void setup(void) {
  WiFi.persistent(false); // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(20);    // This delay helps when using the 'Modified Serial monitor' otherwise it is not needed.
  Serial.println();
  Serial.println();

  // #define DEMO_NOEXTRA4K
#ifdef DEMO_NOEXTRA4K
  /*
    When a call to disable_extra4k_at_link_time() is made, building with HWDT
    selected on the Arduino IDE menu "Tools->Debug Level", will have the same
    result as if built with HWDT_NOEXTRA4K selected.
  */
  disable_extra4k_at_link_time();
#endif

  Serial.printf_P(PSTR("This example was built with%s an extra 4K of heap space (g_pcont == 0x%08lX)\r\n"),
                  ((uintptr_t)0x3FFFC000 < (uintptr_t)g_pcont) ? "" : "out",
                  (uintptr_t)g_pcont);
#if defined(DEBUG_ESP_HWDT) || defined(DEBUG_ESP_HWDT_NOEXTRA4K)
  Serial.print(F("and with the HWDT"));
#if defined(DEBUG_ESP_HWDT_NOEXTRA4K)
  Serial.print(F("_NOEXTRA4K"));
#endif
  Serial.println(F(" option specified."));
#endif

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
