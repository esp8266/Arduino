// Do not remove the include below
#include "NetDumpTest.h"
#include <ESP8266WiFi.h>
#include "LocalDefines.h"
#include <ESP8266WebServer.h>
#include <Netdump.h>
#include <FS.h>

/*
  dump network packets on serial console
  released to the public domain
*/


Netdump nd;
ESP8266WebServer server(80);
WiFiServer ws(8000);


void handleRoot() {
  static int rq = 0;
  String a = "<h1>You are connected, rq = "+String(rq++)+"</h1>";
  server.send(200, "text/html", a);
}


File outfile = SPIFFS.open("test", "w");

void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  SPIFFS.begin();

  server.on("/", handleRoot);
  server.begin();

/*
// To serial, include hex print only localIP traffic
  nd.printDump(Serial, true,
		  	  	  [](NetdumpPacket n)
				  {
	  	  	  	  	  return ((n.ethType() != 0x8912) &&
	  	  	  	  			  ((n.sourceIP()==WiFi.localIP()) || n.destIP()==WiFi.localIP()) );
				  });
*/

/*
// To serial, include hex print only localIP traffic
  nd.printDump(Serial, false); // To serial all traffic, no hex dump
*/

/*
    nd.printDump(outfile, false); // To file all traffic, no hex dump, format ascii
*/

/*
    nd.fileDump(outfile, false); // To file all traffic, no hex dump, format pcap file
*/

/*
// To telnet, all traffic, use `n 10.0.0.212 8000 | tcpdump -r -`
  ws.begin();
  nd.tcpDump(ws);

*/


}

void loop(void) {
	  server.handleClient();

}
