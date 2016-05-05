#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// Put your own Wifi Router SSID/KEY here
// remember that password len should be >7 to get it working
// If you leave this wrong default values, ESP will try to connect
// to last SSID/PASS that worked on this device (if any of course)
const char* ssid = "******";
const char* password = "******";

const char* host = "OTA-LEDS";

int led_pin = 13;
#define N_DIMMERS 3
int dimmer_pin[] = {14, 5, 15};

void setup() {
   Serial.begin(115200);

   /* switch on led */
   pinMode(led_pin, OUTPUT);
   digitalWrite(led_pin, LOW);

   Serial.println("\r\nBooting");
   WiFi.mode(WIFI_STA);

  // If sketch as no default SSID and bad PSK (len <8)
  // it should try to connect to SDK saved one (if any)
  Serial.print(F("Connecting with SSID of "));
  if ( strlen(password)<8 ) {
    Serial.printf( "SDK '%s'\r\n", WiFi.SSID().c_str() );
    // If autoconnect is disabled force connection
    if (!WiFi.getAutoConnect()) 
      WiFi.begin();
  } else {
    Serial.printf( "Sketch '%s'\r\n", ssid ); 
    WiFi.begin(ssid, password);
  }
  // Will be blocked in this while loop until connected
  while (WiFi.waitForConnectResult() != WL_CONNECTED){
    delay(1000);
    Serial.printf("%4ld sec : ...\n", millis()/1000);
  }
  
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin,990);

  for (int i=0; i<N_DIMMERS; i++)
  {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i],50);
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
                        for(int i=0; i<N_DIMMERS;i++)
                          analogWrite(dimmer_pin[i], 0);
                          analogWrite(led_pin,0);
                    });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
                          for (int i=0;i<30;i++)
                          {
                            analogWrite(led_pin,(i*100) % 1001);
                            delay(50);
                          }
                        });

   ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });

   /* setup the OTA server */
   ArduinoOTA.begin();
   Serial.println("Ready");

}

void loop() {
  ArduinoOTA.handle();
}
