/*
 WiFi Server example

 A simple connection with WiFi AP with Wireless Security
 information try to access with WPA or WEP security keys
 A simple server is setup to exchange data.

 created 13 July 2010
 by dlf (Metodo2 srl)
 */
#include <WiFi.h>

byte mac[6] = { 0 };
IPAddress ip;
IPAddress gateway;
IPAddress subnet;
byte dataBuf[80] = { 0 };
char ssid[32] = { 0 };
int status = WL_IDLE_STATUS;

WiFiServer server(23);

void printIpData()
{
  ip = WiFi.localIP();
       
  Serial.print("IP: ");
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
    //WiFi.getCurrSSID(&ssid[0]);
    //Serial.print("SSID:");
    //Serial.println(ssid);
    byte bssid[6];
    WiFi.BSSID(bssid);
    //delay(200);
    Serial.print("BSSID:");
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
       
    char ssid[20][32] = { {0} };
    byte numSsid = WiFi.scanNetworks();
    Serial.print("SSID List:");
    Serial.println(numSsid, 10);
    for (int i = 0; i<numSsid; ++i)
    {
        Serial.println(WiFi.SSID(i));
    }

}

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup WiFi...");
  strcpy(ssid, "Vodafone-10289870");
  Serial.println(ssid);
  int status = WiFi.begin(ssid);
  if ( status != WL_CONNECTED)
  {
    // Using WPA
    Serial.println("Trying with Passphrase...");
    const char *pass = "12345678";
    status = WiFi.begin(ssid, pass);
    if ( status != WL_CONNECTED)
    {	  
      // using WEP     
      Serial.println("Trying with Key...");
      uint8_t key_idx = 0;
      const char *key = "12345678";
      status = WiFi.begin(ssid,key_idx, key);
      if ( status != WL_CONNECTED)
      {	  
        Serial.println("Wifi Connection failed!");  
        return;
      }      
    }
  }
  Serial.println("Wifi Connected!");

  printIpData();      

  printCurrNet();

  Serial.println("Starting server...");
  server.begin();
  delay(1000);  
}

void loop()
{
  static uint8_t count = 0;
  Serial.println("Retry connect...");
  status = WiFi.begin(ssid);
  if (status == WL_CONNECTED)
  {
      byte status = 0;
      WiFiClient client = server.available(&status);
      if (client) {
        //Serial.print("Status: ");
        //Serial.println(status, 16);
        byte idx = 0;
        while (client.available())
        {
            dataBuf[idx++] = client.read();
        }

        if (idx>0) 
        {
          dataBuf[idx]=0; 
          //Serial.println((char*)&dataBuf[0]);
          server.write((char*)&dataBuf[0]);
        }
        return;
      }
  }
}

