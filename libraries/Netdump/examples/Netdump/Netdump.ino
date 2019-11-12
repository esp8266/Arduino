#include "Arduino.h"

#include "Netdump.h"
#include <ESP8266WiFi.h>
#include "LocalDefines.h"
#include <ESP8266WebServer.h>
#include <FS.h>
#include "Schedule.h"

Netdump nd;

FS* filesystem = &SPIFFS;
//FS* filesystem = &LittleFS;

ESP8266WebServer webServer(80);    // Used for sending commands
WiFiServer       tcpServer(8000);  // Used to show netcat option.
File             tracefile;

void startSerial(int option)
{
	switch (option)
	{
	case 1 : //All Packets, show packet summary.
			 nd.printDump(Serial, NetdumpPacket::PacketDetail::NONE);
	         break;

	case 2 : // Only local IP traffic, full details
		     nd.printDump(Serial, NetdumpPacket::PacketDetail::FULL,
				[](NetdumpPacket n)
				{
				   return (n.hasIP(WiFi.localIP()));
				}
		     );
	case 3 : // Only HTTP traffic, show packet content as chars
	         nd.printDump(Serial, NetdumpPacket::PacketDetail::CHARS,
			    [](NetdumpPacket n)
			    {
			       return (n.isHTTP());
			    }
	         );
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

  //startSerial(3);
  // startTcpDump();
   startTracefile();

}

void loop(void) {
	  webServer.handleClient();
}

