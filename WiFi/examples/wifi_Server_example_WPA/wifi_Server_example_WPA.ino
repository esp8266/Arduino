/*
 WiFi Server example

 A simple connection with WiFi AP with Wireless Security
 information try to access with WPA or WEP security keys
 A simple server is setup to exchange data.

 created 13 July 2010
 by dlf (Metodo2 srl)
 */
#include <WiFi.h>

#define _PRINT_

byte mac[6] = { 0 };
IPAddress ip;
IPAddress gateway;
IPAddress subnet;
byte dataBuf[80] = { 0 };
char ssid[32] = { 0 };
int status = WL_IDLE_STATUS;
#define MAX_NUM_SSID 10
char ssidList[MAX_NUM_SSID][32] = { {0} };
 

WiFiServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void printIpData()
{
  ip = WiFi.localIP();
       
  Serial.print("\nIP: ");
  Serial.print(ip[3],10);Serial.print(".");
  Serial.print(ip[2],10);Serial.print(".");
  Serial.print(ip[1],10);Serial.print(".");
  Serial.println(ip[0],10);
          
  subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print(subnet[3],10);Serial.print(".");
  Serial.print(subnet[2],10);Serial.print(".");
  Serial.print(subnet[1],10);Serial.print(".");
  Serial.println(subnet[0],10);
  
  gateway = WiFi.gatewayIP();
  Serial.print("GATEWAY: ");
  Serial.print(gateway[3],10);Serial.print(".");
  Serial.print(gateway[2],10);Serial.print(".");
  Serial.print(gateway[1],10);Serial.print(".");
  Serial.println(gateway[0],10);

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],16);Serial.print(":");
  Serial.print(mac[4],16);Serial.print(":");
  Serial.print(mac[3],16);Serial.print(":");
  Serial.print(mac[2],16);Serial.print(":");
  Serial.print(mac[1],16);Serial.print(":");
  Serial.println(mac[0],16);
}

void printCurrNet()
{
    char* ssid = WiFi.SSID();
    Serial.print("SSID: ");
    Serial.println(ssid);
    
    byte bssid[6];
    WiFi.BSSID(bssid);    
    Serial.print("BSSID: ");
    Serial.print(bssid[5],16);Serial.print(":");
    Serial.print(bssid[4],16);Serial.print(":");
    Serial.print(bssid[3],16);Serial.print(":");
    Serial.print(bssid[2],16);Serial.print(":");
    Serial.print(bssid[1],16);Serial.print(":");
    Serial.println(bssid[0],16);

    int32_t rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi,10);
    
    uint8_t enct = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(enct,16);
}

void scanNetworks()
{
    Serial.println("\n** Scan Networks **");
    byte numSsid = WiFi.scanNetworks();
    if (numSsid > MAX_NUM_SSID) numSsid = MAX_NUM_SSID;
    Serial.print("SSID List:");
    Serial.println(numSsid, 10);
    for (int i = 0; i<numSsid; ++i)
    {
        Serial.print(i,10);
        Serial.print(") Network: ");
        Serial.println(WiFi.SSID(i));
    }
}

int startWiFiWpa()
{
  Serial.println("\nSetup WiFi Wpa...");
  //strcpy(ssid, "AndroidAP9647");
  //strcpy(ssid, "AndroidAP3551");
  strcpy(ssid, "Cariddi");
  Serial.print("SSID: ");
  Serial.println(ssid);
  const char *pass = "1234567890";
  status = WiFi.begin(ssid, pass);
  //status = WiFi.begin(ssid);
  if ( status != WL_CONNECTED)
  {	  
    Serial.println("Connection Failed");
  }
  return status;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("*** Start WiFi example ***");
  
  WiFi.begin();
  
  // Wait for initialize WiFi
  delay(3000);
  
  scanNetworks();

  int _status = startWiFiWpa();

  if ( _status == WL_CONNECTED)
  {
    Serial.println("\nWifi Connected!");

    printIpData();      

    printCurrNet();
  
    Serial.println("\nStarting server...");
    server.begin();
    delay(1000);  
   
  }
}

void execCmd(char* buf)
{
  #ifdef _PRINT_
  Serial.print("\nExecuting command: ");
  Serial.println(buf);
  #endif
  server.write(buf);
}


void loop()
{
  if (status == WL_CONNECTED)
  {
      byte _status = 0;
      WiFiClient client = server.available(&_status);
      if (client) {
            if (!gotAMessage) {
              Serial.println("\nWe have a new client\n");
              client.println("Hello, client!"); 
              gotAMessage = true;
            }
    
      static byte idx = 0;
      int c = 0;

        do 
        {
          c = client.read();
          if (c!=-1)
          {
            dataBuf[idx] = c;
            #ifdef _PRINT_
            if (idx == 0) Serial.print("Client chatting...: ");
            Serial.print(dataBuf[idx]);
            #endif
            if ((dataBuf[idx] == 0xa)/*||(dataBuf[idx] == 0xd)*/)
            {
              dataBuf[idx+1]=0; 
              //Serial.println((char*)dataBuf);
              execCmd((char*)dataBuf);
              idx=0;
            }else{
              ++idx;
            }
          }
        }while (c!=-1);
      }
  }
}


