/*
  DHCP-based IP printer

  This sketch uses the DHCP extensions to the Ethernet library
  to get an IP address via DHCP and print the address obtained.
  using an Arduino Wiznet Ethernet shield.

  Circuit:
  * Ethernet Wiznet5500/Wiznet5100/ENC28J60 on esp8266

  created 12 April 2011
  modified 9 Apr 2012
  by Tom Igoe
  modified 02 Sept 2015
  by Arturo Guadalupi

 */

// specific to esp8266 w/lwIP
#include <EthernetCompat.h>
ArduinoWiznet5500lwIP Ethernet(/*SS*/ 16);  // <== adapt to your hardware
// ArduinoWiznet5100lwIP Ethernet(/*SS*/ 16);  // <== adapt to your hardware
// ArduinoENC28J60lwIP Ethernet(/*SS*/ 16);  // <== adapt to your hardware


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte notNeededButAllowed_mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
byte* mac = nullptr;  // automatic mac

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  // Ethernet.init(10);  // Most Arduino shields
  // Ethernet.init(5);   // MKR ETH shield
  // Ethernet.init(0);   // Teensy 2.0
  // Ethernet.init(20);  // Teensy++ 2.0
  // Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  // Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  //                     // esp8266 w/lwIP: SS set in Ethernet constructor


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  switch (Ethernet.maintain()) {
    case 1:
      // renewed fail
      Serial.println("Error: renewed fail");
      break;

    case 2:
      // renewed success
      Serial.println("Renewed success");
      // print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    case 3:
      // rebind fail
      Serial.println("Error: rebind fail");
      break;

    case 4:
      // rebind success
      Serial.println("Rebind success");
      // print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    default:
      // nothing happened
      break;
  }
}
