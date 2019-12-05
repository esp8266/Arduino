#include "Arduino.h"

#include "Netdump.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "map"

using namespace NetCapture;

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

Netdump nd;

FS* filesystem = &SPIFFS;
//FS* filesystem = &LittleFS;

ESP8266WebServer webServer(80);    // Used for sending commands
WiFiServer       tcpServer(8000);  // Used to show netcat option.
File             tracefile;

std::map<PacketType,int> packetCount;

enum SerialOption
{
	AllFull,
	LocalNone,
	HTTPChar

};


void startSerial(int option)
{
	switch (option)
	{
	case AllFull : //All Packets, show packet summary.
			 nd.printDump(Serial, Packet::PacketDetail::FULL);
	         break;

	case LocalNone : // Only local IP traffic, full details
		     nd.printDump(Serial, Packet::PacketDetail::NONE,
				[](Packet n)
				{
				   return (n.hasIP(WiFi.localIP()));
				}
		     );
		     break;
	case HTTPChar : // Only HTTP traffic, show packet content as chars
	         nd.printDump(Serial, Packet::PacketDetail::CHAR,
			    [](Packet n)
			    {
			       return (n.isHTTP());
			    }
	         );
	         break;
	default :
			Serial.printf("No valid SerialOption provided\r\n");
	};
}

void startTracefile()
{
	// To file all traffic, format pcap file
	tracefile = filesystem->open("/tr.pcap", "w");
	nd.fileDump(tracefile);
}

void startTcpDump()
{
	// To tcpserver, all traffic.
    tcpServer.begin();
	nd.tcpDump(tcpServer);
}

void capturePacket(Packet np)
{

}

void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  filesystem->begin();

  webServer.on("/list",
    []()
	{
	  Dir dir = filesystem->openDir("/");
	  String d = "<h1>File list</h1>";
	  while (dir.next())
	  {
		d.concat("<li>" + dir.fileName() + "</li>");
	  }
	  webServer.send(200, "text.html", d);
	}
  );

  webServer.on("/req",
	[]()
	{
	  static int rq = 0;
	  String a = "<h1>You are connected, Number of requests = " + String(rq++) + "</h1>";
	  webServer.send(200, "text/html", a);
	}
  );

  webServer.on("/reset" ,
    []()
	{
	  nd.reset();
	  tracefile.close();
	  tcpServer.close();
	  webServer.send(200, "text.html", "<h1>Netdump session reset</h1>");
	}
  );

  webServer.serveStatic("/", *filesystem, "/");
  webServer.begin();

  startSerial(AllFull); // Serial output examples, use enum SerialOption for selection

//  startTcpDump();     // tcpdump option
 //  startTracefile();  // output to SPIFFS or LittleFS
/*
  // use a self provide callback, this count network packets
  nd.setCallback(
   [](Packet p)
   {
	  Serial.printf("PKT : ");
	  for ( auto pp : p.allPacketTypes())
		  {
		     Serial.printf("%s ",pp.toString().c_str());
			 packetCount[pp]++;
		  }
	  Serial.printf("\r\n CNT ");
	  for (auto pc : packetCount)
		  {
		  	  Serial.printf("%s %d ", pc.first.toString().c_str(),pc.second);
		  }
	  Serial.printf("\r\n");
   }
  );
*/
}

void loop(void) {
	  webServer.handleClient();
}

