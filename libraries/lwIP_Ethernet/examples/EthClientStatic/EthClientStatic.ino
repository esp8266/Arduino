/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.

    This is Ethernet version of:
    https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient.ino
*/

#include <LwipEthernet.h>

#define LOCAL_IP   IPAddress(192,168,0,233)
#define LOCAL_GW   IPAddress(192,168,0,254)   // <== adapt to your network
#define LOCAL_MASK IPAddress(255,255,255,0)
#define DNS        IPAddress(8,8,8,8)

Wiznet5500lwIP eth(/*SS*/16);                 // <== adapt to your hardware

const char* host = "djxmmx.net";
const uint16_t port = 17;

void setup() {
  Serial.begin(115200);

  Serial.println("\nEthernet\n");

  if (!ethInitStatic(eth, LOCAL_IP, LOCAL_GW, LOCAL_MASK, DNS)) {
    // enabling debug message will show the real cause
    Serial.printf("no hardware found or bad network configuration\n");
    while (1) {
      delay(1000);
    }
  }

  Serial.printf("Ethernet: IP Address: %s\n",
                eth.localIP().toString().c_str());
}

void loop() {

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  // (this class could have been named TCPClient)
  WiFiClient client;
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
  client.sendAll(Serial); // this peer closes once all data are sent

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  delay(600000); // execute once every 10 minutes, don't flood remote service
}
