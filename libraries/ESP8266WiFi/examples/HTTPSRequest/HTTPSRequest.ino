/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "SKY";
const char* password = "wellcometrust";

//const char* host = "api.github.com";
//const char* host = "raw.githubusercontent.com";
//const char * host = "www.google.co.uk";
const char * host = "test2.tls-o-matic.com"; 

const int httpsPort = 402;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
// const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C"; // git api
//const char * fingerprint = "E2 11 20 48 85 00 92 5B F9 56 EE 20 41 AF FC 52 D4 CC 39 1E"; // google
//const char * fingerprint = "B0 74 BB EF 10 C2 DD 70 89 C8 EA 58 A2 F9 E1 41 00 D3 38 82"; //git raw
  const char * fingerprint = "B9 1D 9E A7 57 8A 43 BF 8E 15 26 93 09 09 8F E9 3A 5D EE 52";


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  //String url = "/repos/esp8266/Arduino/commits/esp8266/status";
  //String url = "/sticilface/ESPmanager/fixcrashing/examples/Settingsmanager-example/data/jquery.mobile-1.4.5.min.js.gz";
    String url = ""; 
  
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");

  
//  while (client.connected()) {
//    String line = client.readStringUntil('\n');
//    if (line == "\r") {
//      Serial.println("headers received");
//      break;
//    }
//  }

    size_t buf_size = 1024; 
    uint8_t buf[buf_size];
    
  Serial.println("reply was:");
  Serial.println("==========");
  
  
  
     String line = client.readStringUntil('\r\n\r\n');

     Serial.print(line);
     
//            while (client.available()) {
//              Serial.print(client.read()); 
//    
//            }

//  String line = client.readStringUntil('\n');
//  if (line.startsWith("{\"state\":\"success\"")) {
//    Serial.println("esp8266/Arduino CI successfull!");
//  } else {
//    Serial.println("esp8266/Arduino CI has failed");
//  }

  //Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

void loop() {
}
