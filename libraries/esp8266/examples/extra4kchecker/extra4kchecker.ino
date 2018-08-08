
/*
   This is a debugging / validation test only

   It demonstrates the effect of calling
       disable_extra4k_at_link_time()

   Currently only WPS is using that call

   released to public domain
*/

#include <ESP8266WiFi.h>
#include <cont.h>

#define USE_WPS 0 // try me with 0 or 1

extern cont_t* g_pcont;

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_STA);
  bool sysstack = (((unsigned long)g_pcont) >> 16) == 0x3fff;
  Serial.printf("\nUsing sys stack: %s\ncont: %p (USER:0x3ffe SYS:0x3fff)\n"
                "check https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map)\n",
                sysstack ? "YES" : "NO",
                g_pcont);

  Serial.printf("FreeHeap: %d (~52112 without WPS, ~46832 with)\n", ESP.getFreeHeap());

#if USE_WPS
  Serial.printf("starting WPS...\n");
  Serial.printf("wps: %d\n", WiFi.beginWPSConfig());
#else
  Serial.printf("done\n");
#endif
}

void loop() {
}
