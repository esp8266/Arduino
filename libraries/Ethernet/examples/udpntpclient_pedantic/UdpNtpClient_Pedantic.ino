/*

  Udp NTP Client

  Get the time from a Network Time Protocol (NTP) time server
  Demonstrates use of UDP sendPacket and ReceivePacket
  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol

  created 4 Sep 2010
  by Michael Margolis
  modified 9 Apr 2012
  by Tom Igoe

  This code is in the public domain.

  Modified by David Henry to show where all the 'magic numbers' come from.
  You need to read the RFC-1305 spec to understand https://tools.ietf.org/html/rfc1305
  mgadriver@gmail.com

*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "RFC1305.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

unsigned int localPort = 8888;       // local port to listen for UDP packets

char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

#define NTP_PACKET_SIZE sizeof(struct sRFC1305)

struct sRFC1305 packetBuffer; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  //Serial.println(NTP_PACKET_SIZE); // just for debugging
  //Serial.println(ENDIAN_SWAP_32(0x11223344),HEX);
  //Serial.println(ENDIAN_SWAP_16(0xAABB),HEX);
  // start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  Udp.begin(localPort);
}

void loop() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    Udp.read((byte *)&packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
#if 0 // just for debugging
    Serial.println(ENDIAN_SWAP_16(packetBuffer.rootdelay_main), HEX);
    Serial.println(ENDIAN_SWAP_16(packetBuffer.rootdelay_fraction), HEX);
    Serial.println(ENDIAN_SWAP_16(packetBuffer.rootdispersion_main), HEX);
    Serial.println(ENDIAN_SWAP_16(packetBuffer.rootdispersion_fraction), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.referencetimestamp_main), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.referencetimestamp_fraction), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.origintimestamp_main), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.origintimestamp_fraction), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.receivetimestamp_main), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.receivetimestamp_fraction), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.transmittimestamp_main), HEX);
    Serial.println(ENDIAN_SWAP_32(packetBuffer.transmittimestamp_fraction), HEX);
#endif
    Serial.print("Delay ");
    Serial.print(ENDIAN_SWAP_16(packetBuffer.rootdelay_main)); Serial.print("."); Serial.println(ENDIAN_SWAP_16(packetBuffer.rootdelay_fraction));
    Serial.print("Seconds since Jan 1 1900 = ");
    unsigned long secsSince1900 = ENDIAN_SWAP_32(packetBuffer.transmittimestamp_main);
    Serial.print(secsSince1900); Serial.print("."); Serial.println(ENDIAN_SWAP_32(packetBuffer.transmittimestamp_fraction));

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

#define SECS_PER_MINUTE 60
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY  86400L

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % SECS_PER_DAY) / SECS_PER_HOUR);
    Serial.print(':');
    if (((epoch % SECS_PER_HOUR) / SECS_PER_MINUTE) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % SECS_PER_HOUR) / SECS_PER_MINUTE);
    Serial.print(':');
    if ((epoch % SECS_PER_MINUTE) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % SECS_PER_MINUTE); // print the second
  }
  // wait ten seconds before asking for the time again
  delay(10000);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset((char *)&packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer.LI = LI_ALARM;
  packetBuffer.VN = VERN;
  packetBuffer.MODE = MODE_CLIENT;
  packetBuffer.stratum = 0;
  packetBuffer.poll = 6;
  packetBuffer.precision = -20;  // ? copied from original UdnNtpClient code
  packetBuffer.identifier[0] = '1';  // I've no idea where this ID comes from
  packetBuffer.identifier[1] = 'N';
  packetBuffer.identifier[2] = '1';
  packetBuffer.identifier[3] = '4';
  //  Serial.println(*(uint8_t *)&packetBuffer,HEX);
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write((byte *)&packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}










