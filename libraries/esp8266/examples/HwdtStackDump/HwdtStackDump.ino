/*

   There is a tool to print a stack dump at boot after a Hardware WDT
   reset. To use the Hardware WDT Reset stack dump tool, you can select HWDT or
   HWDT_NOEXTRA4K from the Arduino IDE menu "Tools->Debug Level" before
   building your sketch. Note, 'Tools->Debug port' selection is not needed or
   referenced for printing the HWDT stack dump.

   When the ESP8266 restarts because of a Hardware WDT reset, the serial port
   speed defaults to 115200 bps. The HWDT stack dump will always print on port
   'Serial'.

   To demonstrate this tool, this Sketch offers a few options for crashing the
   ESP8266 with and without a HWDT reset.

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <user_interface.h>
#include <coredecls.h>  // g_pcont - only needed for this debug demo
#include <StackThunk.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

////////////////////////////////////////////////////////////////////
// This block is just for putting something on the BearSSL stack
// to show that it has not been zeroed out before HWDT stack dump
// gets to runs.
extern "C"
{
#if CORE_MOCK
#define thunk_ets_uart_printf ets_uart_printf

#else
  int thunk_ets_uart_printf(const char* format, ...) __attribute__((format(printf, 1, 2)));
  // Second stack thunked helper - this macro creates the global function thunk_ets_uart_printf
  make_stack_thunk(ets_uart_printf);
#endif
};
////////////////////////////////////////////////////////////////////

void setup(void)
{
  WiFi.persistent(false);  // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(20);  // This delay helps when using the 'Modified Serial monitor' otherwise it is not needed.
  Serial.println();
  Serial.println();
  Serial.println(F("The Hardware Watchdog Timer Demo is starting ..."));
  Serial.println();

  // This allows us to test dumping a BearSSL stack after HWDT.
  stack_thunk_add_ref();
  thunk_ets_uart_printf("Using Thunk Stack to print this line.\n\n");

  // We don't need this for this example; however, starting WiFi uses a little
  // more of the SYS stack.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println(F("A WiFi connection attempt has been started."));
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

void loop(void)
{
  if (Serial.available() > 0)
  {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}
