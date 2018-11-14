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

#define SSID "ssid"
#define PSK  "psk"

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\r\n";       // a string to send back

WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  delay(10);
}


/*
  Processing sketch to run with this example
  =====================================================

  // Processing UDP example to send and receive string data from Arduino
  // press any key to send the "Hello Arduino" message


  import hypermedia.net.*;

  UDP udp;  // define the UDP object


  void setup() {
  udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
  //udp.log( true ); 		// <-- printout the connection activity
  udp.listen( true );           // and wait for incoming message
  }

  void draw()
  {
  }

  void keyPressed() {
  String ip       = "192.168.1.177";	// the remote IP address
  int port        = 8888;		// the destination port

  udp.send("Hello World", ip, port );   // the message to send

  }

  void receive( byte[] data ) { 			// <-- default handler
  //void receive( byte[] data, String ip, int port ) {	// <-- extended handler

  for(int i=0; i < data.length; i++)
  print(char(data[i]));
  println();
  }
*/


