/*
  UDPSendReceive.pde:
  This sketch receives UDP message strings, prints them to the serial port
  and sends an "acknowledge" string back to the sender

  A Processing sketch is included at the end of file that can be used to send
  and received messages for testing with a computer.

  created 21 Aug 2010
  by Michael Margolis

  This code is in the public domain.

  adapted from Ethernet library examples
*/


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

#define MAX_UDP2_RETRIES 5          // kept low for the example

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\r\n";       // a string to send back

WiFiUDP Udp;
WiFiUDP Udp2;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
  Serial.printf("Second UDP server on port %d\n", localPort + 1);
  Udp2.begin(localPort + 1);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }


  // same example, resending data in case of transmit error

  // if there's data available, read a packet
  packetSize = Udp2.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp2.remoteIP().toString().c_str(), Udp2.remotePort(),
                  Udp2.destinationIP().toString().c_str(), Udp2.localPort(),
                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp2.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    String reply = String(packetBuffer).substring(0, 5);
    if (reply.length()) {
      int strip = reply.length() - 1;
      while (strip > 0 && reply[strip] <= 32) {
        strip--;
      }
      reply.remove(strip + 1);
    }
    // Send a reply, to the IP address and port that sent us the packet we received,
    // sending it 20 times, trying to stuff the ouput queue, and trying to resend it
    // until success.
    for (int i = 0; i < 20; i++) {
      Serial.printf("%d ", i);

      Udp2.beginPacket(Udp2.remoteIP(), Udp2.remotePort());
      Udp2.printf("Reply '%s' #%d heap=%u\r\n", reply.c_str(), i, ESP.getFreeHeap());
      // now resending it until we win
      int retries = 0;
      while (!Udp2.endPacketMayRetry() && ++retries < MAX_UDP2_RETRIES) {
        Serial.print("! ");
      }

      if (retries >= MAX_UDP2_RETRIES) {
        Serial.print("...cancelled :(");
        Udp2.cancelPacket();
      }

      // MAX_UDP2_RETRIES loop can be replaced with a timeout

      Serial.printf("\n");
    }
  }
}

/*
  test (shell/netcat):
  --------------------
	  nc -u 192.168.esp.address 8888
	  nc -u 192.168.esp.address 8889
*/
