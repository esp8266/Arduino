/*
   I2S stereo microphone (input) UDP transmitter
   Needs a UDP listener (netcat/etc.) on port 8266 on the PC

   Under Linux:
    nc -u -p 8266 -l | play -t raw -r 11025 -b 16 -c 2 -e signed-integer -

   Released to the Public Domain by Earle F. Philhower, III
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <I2S.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

// Set your network here
const char *SSID = STASSID;
const char *PASS = STAPSK;

WiFiUDP udp;
// Set your listener PC's IP here:
const IPAddress listener = { 192, 168, 1, 2 };
const int port = 8266;

int16_t buffer[100][2]; // Temp staging for samples

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("My IP: ");
  Serial.println(WiFi.localIP());

  i2s_rxtx_begin(true, false); // Enable I2S RX
  i2s_set_rate(11025);

  Serial.print("\nStart the listener on ");
  Serial.print(listener);
  Serial.print(":");
  Serial.println(port);
  Serial.println("ex:  nc -u -p 8266 -l | play -t raw -r 11025 -b 16 -c 2 -e signed-integer -");

  udp.beginPacket(listener, port);
  udp.write("I2S Receiver\r\n");
  udp.endPacket();

}

void loop() {
  static uint32_t cnt = 0;
  // Each loop will send 100 raw samples (400 bytes)
  // UDP needs to be < TCP_MSS which can be 500 bytes in LWIP2
  for (int i = 0; i < 100; i++) {
    i2s_read_sample(&buffer[i][0], &buffer[i][1], true);
  }
  udp.beginPacket(listener, port);
  udp.write((uint8_t*)buffer, sizeof(buffer));
  udp.endPacket();
  cnt++;
  if ((cnt % 100) == 0) {
    Serial.printf("%" PRIu32 "\n", cnt);
  }
}
