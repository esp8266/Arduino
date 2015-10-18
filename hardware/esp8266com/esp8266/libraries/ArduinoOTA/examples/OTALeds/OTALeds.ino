#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "...";
const char* password = "...";
const char* host_prefix = "OTA-LEDS-";

ArduinoOTA ota_server(host_prefix, 8266, /* debug_serial= */ true);

int led_pin = 13;
#define N_DIMMERS 3
int dimmer_pin[] = {14, 5, 15};

void setup() {
   Serial.begin(115200);

   /* switch on led */
   pinMode(led_pin, OUTPUT);
   digitalWrite(led_pin, LOW);

   Serial.println("Booting");
   WiFi.mode(WIFI_STA);

   /* try the flash stored password first */
   WiFi.begin();

   while (WiFi.waitForConnectResult() != WL_CONNECTED){
     WiFi.begin(ssid, password);
     Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* setup the OTA server */
  ota_server.setup();
  Serial.println("Ready");

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin,990);

  for (int i=0; i<N_DIMMERS; i++)
  {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i],50);
  }

  ota_server.onStart([]() { // switch off all the PWMs during upgrade
                        for(int i=0; i<N_DIMMERS;i++)
                          analogWrite(dimmer_pin[i], 0);
                          analogWrite(led_pin,0);
                    });

  ota_server.onEnd([]() { // do a fancy thing with our board led at end
                          for (int i=0;i<30;i++)
                          {
                            analogWrite(led_pin,(i*100) % 1001);
                            delay(50);
                          }
                        });

   ota_server.onError([]() { ESP.restart(); });

}

void loop() {
  ota_server.handle();
  yield();
}
