#include "Arduino.h"

#include "Netdump.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <FS.h>
#include <LittleFS.h>
#include <map>

using namespace NetCapture;

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

Netdump nd;

//FS* filesystem = &SPIFFS;
FS* filesystem = &LittleFS;

ESP8266WebServer webServer(80);    // Used for sending commands
WiFiServer       tcpServer(8000);  // Used to show netcat option.
File             tracefile;

std::map<PacketType, int> packetCount;

enum class SerialOption : uint8_t {
  AllFull,
  LocalNone,
  HTTPChar
};

void startSerial(SerialOption option) {
  switch (option) {
    case SerialOption::AllFull:  //All Packets, show packet summary.
      nd.printDump(Serial, Packet::PacketDetail::FULL);
      break;

    case SerialOption::LocalNone:  // Only local IP traffic, full details
      nd.printDump(Serial, Packet::PacketDetail::NONE,
                   [](Packet n) {
                     return (n.hasIP(WiFi.localIP()));
                   });
      break;
    case SerialOption::HTTPChar:  // Only HTTP traffic, show packet content as chars
      nd.printDump(Serial, Packet::PacketDetail::CHAR,
                   [](Packet n) {
                     return (n.isHTTP());
                   });
      break;
    default:
      Serial.printf("No valid SerialOption provided\r\n");
  };
}

void startTracefile() {
  // To file all traffic, format pcap file
  tracefile = filesystem->open("/tr.pcap", "w");
  nd.fileDump(tracefile);
}

void startTcpDump() {
  // To tcpserver, all traffic.
  tcpServer.begin();
  nd.tcpDump(tcpServer);
}

void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed, stopping sketch");
    while (1) {
      delay(1000);
    }
  }

  if (!MDNS.begin("netdumphost")) {
    Serial.println("Error setting up MDNS responder!");
  }

  filesystem->begin();

  webServer.on("/list",
               []() {
                 Dir    dir = filesystem->openDir("/");
                 String d   = "<h1>File list</h1>";
                 while (dir.next()) {
                   d.concat("<li>" + dir.fileName() + "</li>");
                 }
                 webServer.send(200, "text.html", d);
               });

  webServer.on("/req",
               []() {
                 static int rq = 0;
                 String     a  = "<h1>You are connected, Number of requests = " + String(rq++) + "</h1>";
                 webServer.send(200, "text/html", a);
               });

  webServer.on("/reset",
               []() {
                 nd.reset();
                 tracefile.close();
                 tcpServer.close();
                 webServer.send(200, "text.html", "<h1>Netdump session reset</h1>");
               });

  webServer.serveStatic("/", *filesystem, "/");
  webServer.begin();

  startSerial(SerialOption::AllFull);  // Serial output examples, use enum SerialOption for selection

  //  startTcpDump();     // tcpdump option
  //  startTracefile();  // output to SPIFFS or LittleFS

  // use a self provide callback, this count network packets
  /*
    nd.setCallback(
     [](Packet p)
     {
  	  Serial.printf("PKT : %s : ",p.sourceIP().toString().c_str());
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
  MDNS.update();
}
