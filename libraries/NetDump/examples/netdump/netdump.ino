
/*
  dump network packets on serial console
  released to the public domain
*/

#include <NetDump.h>
#include <lwipopts.h> // get global handler phy_capture

void dump (int netif_idx, const char* data, size_t len, int out, int success) {
  (void)success;
  Serial.print(out ? F("out ") : F(" in "));
  Serial.printf("%d ", netif_idx);

  // optional filter example: if (netDump_is_ARP(data))
  {
    netDump(Serial, data, len);
    //netDumpHex(Serial, data, len);
  }
}

void setup(void) {
  Serial.begin(115200);
  phy_capture = dump;

  // put your setup code here, to run once:
}

void loop(void) {
  // put your main code here, to run repeatedly:
}
