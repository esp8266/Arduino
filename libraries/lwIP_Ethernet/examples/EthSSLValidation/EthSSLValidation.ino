// Example of the different modes of the X.509 validation options
// in the WiFiClientBearSSL object
//
// Mar 2018 by Earle F. Philhower, III
// Released to the public domain
//
// This is Ethernet version of:
// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_Validation/BearSSL_Validation.ino

#include <LwipEthernet.h>

Wiznet5500lwIP eth(/*SS*/ 16);  // <== adapt to your hardware

#define setup forgetMe
#include <../../libraries/ESP8266WiFi/examples/BearSSL_Validation/BearSSL_Validation.ino>
#undef setup

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  Serial.println("\nEthernet\n");

  // 1. Currently when no default route is set, esp8266-Arduino uses the first
  //    DHCP client interface receiving a valid address and gateway to
  //    become the new lwIP default interface.
  // 2. Otherwise - when using static addresses - lwIP for every packets by
  //    defaults selects automatically the best suited output interface
  //    matching the destination address.  If several interfaces match,
  //    the first one is picked.  On esp8266/Arduno: WiFi interfaces are
  //    checked first.
  // 3. Or, use `::setDefault()` to force routing through this interface.
  // eth.setDefault(true); // default route set through this interface

  if (!ethInitDHCP(eth)) {
    Serial.printf("no hardware found\n");
    while (1)
      delay(1000);
  }

  while (!eth.connected()) {
    Serial.printf(".");
    delay(1000);
  }

  Serial.printf("Ethernet: IP Address: %s\n",
                eth.localIP().toString().c_str());

  fetchNoConfig();
  fetchInsecure();
  fetchFingerprint();
  fetchSelfSigned();
  fetchKnownKey();
  fetchCertAuthority();
  fetchFaster();
}
