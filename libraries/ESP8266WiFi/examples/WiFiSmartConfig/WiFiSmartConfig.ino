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

void decryptSmartConfigPasswordTrivial(char * decrypted, const char* encString){
  // No encryption. Password is sent as plan text
  // If password is encrypted with AES(CBC/ECB). Decrypt here
  // Note: The format of encrypted string and choice of encryption algoright is upto the user.
  // It has to be chosen with constraint that maxium length of encrypted string should not be above 64. 
    
  strcpy(decrypted, encString);
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

  // If sending encrypted password. Pass an optional decrypt password function
  WiFi.setSmartConfigDecryptFn(decryptSmartConfigPasswordTrivial);

  if(!WiFi.beginSmartConfig() { 
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