
/*
  provide a tcp-server for tcpdump of locally sent/received packets
  under unix with NetCat, open a terminal, run:
     nc esp-ip-address 2 | tcpdump -r - [<options>] [<pcap-filter>]
  
  released to the public domain
*/

#include <NetDump.h>

void setup() {
  // put your setup code here, to run once:
  // setup WiFi
  // now tcpdump server can be initialized
  tcpdump_setup();
}

void loop() {
  tcpdump_loop();
  // put your main code here, to run repeatedly:
}
