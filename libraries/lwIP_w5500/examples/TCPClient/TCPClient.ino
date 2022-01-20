/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <SPI.h>
#include <W5500lwIP.h>
//or #include <W5100lwIP.h>
//or #include <ENC28J60lwIP.h>

#include <WiFiClient.h> // WiFiClient (-> TCPClient)

const char* host = "djxmmx.net";
const uint16_t port = 17;

using TCPClient = WiFiClient;

#define CSPIN 16 // wemos/lolin/nodemcu D0
Wiznet5500lwIP eth(CSPIN);

void setup() {
  Serial.begin(115200);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  eth.setDefault(); // use ethernet for default route
  if (!eth.begin()) {
    Serial.println("ethernet hardware not found ... sleeping");
    while (1) {
      delay(1000);
    }
  } else {
    Serial.print("connecting ethernet");
    while (!eth.connected()) {
      Serial.print(".");
      delay(1000);
    }
  }
  Serial.println();
  Serial.print("ethernet IP address: ");
  Serial.println(eth.localIP());
}

void loop() {
  static bool wait = false;

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  TCPClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
    client.println("hello from ESP8266");
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  if (wait) {
    delay(300000); // execute once every 5 minutes, don't flood remote service
  }
  wait = true;
}
