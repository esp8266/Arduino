/*
    This sketch demonstrates how to use Smart Config.
    It calls a user callback on recieving credentials from ESPTouch mobile app. 
    We can send wifi password encrypted with a pre shared key from Mobile app,
    and decrypt the password in decryptSmartConfigPassword function. 
    Note: to keep this example free of dependency on encryption library the decryptSmartConfigPassword is kept empty
    but one can fill it with a simple decryption block.  
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "smartconfig.h"

void decryptSmartConfigPassword(char * buffer, const char* encString){
  // currently returning same string
  // but we can throw in simple aes encryption. 
  memcpy(buffer,encString, strlen(encString));
}

void smartConfigCallback(uint32_t st, void *result) {
  station_config *sta_conf = reinterpret_cast<station_config *>(result);
  char recievedPassword[65] = "";
  memcpy(recievedPassword, sta_conf->password, 64);

  struct station_config conf;
  memcpy(&conf, sta_conf, sizeof(conf));

  // Decrypt password. 
  char passwordBuffer[64] = "";
  decryptSmartConfigPassword(passwordBuffer,(const char *)recievedPassword);
  memcpy(conf.password, passwordBuffer, 64);

  wifi_station_set_config_current(&conf);
  wifi_station_disconnect();
  wifi_station_connect();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Starting smart config...");

  WiFi.softAPdisconnect();
  WiFi.enableAP(false);
  WiFi.enableSTA(true);
  WiFi.mode(WIFI_STA);
  delay(1000);

  if(!WiFi.beginSmartConfig(reinterpret_cast<sc_callback_t>(&smartConfigCallback))) { 
    Serial.println("Could not begin smart config");
    return;
  }

  while(!WiFi.smartConfigDone()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Smart config done");
  
  // print network info. 
  Serial.print("WiFi SSID ");
  Serial.println(WiFi.SSID());
  
  Serial.print("WiFi SSID ");
  Serial.println(WiFi.psk());
  
  Serial.println("Waiting for connection");
  while (!WiFi.isConnected()){
    Serial.print(".");
    delay(100);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  
}