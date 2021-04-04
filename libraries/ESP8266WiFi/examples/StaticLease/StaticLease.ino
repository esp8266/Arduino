
/* Create a WiFi access point and provide static lease */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LwipDhcpServer.h>

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *password = "thereisnospoon";

ESP8266WebServer server(80);

/* Set the IP Address you want for your AP */
IPAddress apIP(192, 168, 0, 1);

/* Go to http://192.168.0.1 in a web browser to see current lease */
void handleRoot() {
  String result;
  char wifiClientMac[18];
  unsigned char number_client;
  struct station_info *stat_info;

  int i = 1;

  number_client = wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();

  result = "<html><body><h1>Total Connected Clients : ";
  result += String(number_client);
  result += "</h1></br>";
  while (stat_info != NULL) {

    result += "Client ";
    result += String(i);
    result += " = ";
    result += IPAddress(stat_info->ip).toString();
    result += " - ";
    sprintf(wifiClientMac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(stat_info->bssid));
    result += wifiClientMac;
    result += "</br>";

    stat_info = STAILQ_NEXT(stat_info, next);
    i++;
  }
  result = result + "</body></html>";

  server.send(200, "text/html", result);
}

void setup() {
  /* List of mac address for static lease */
  uint8 mac_CAM[6] = { 0x00, 0x0C, 0x43, 0x01, 0x60, 0x15 };
  uint8 mac_PC[6] = { 0xb4, 0x52, 0x7e, 0x9a, 0x19, 0xa5 };

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  /* Disable the WiFi persistence to avoid any re-configuration that may erase static lease when starting softAP */
  WiFi.persistent(false);

  WiFi.mode(WIFI_AP);
  /* Configure AP with IP = 192.168.0.1 / Gateway = 192.168.0.1 / Subnet = 255.255.255.0
     if you specify the ESP8266's IP-address with 192.168.0.1, the function softAPConfig() sets the DHCP-range as 192.168.0.100 - 192.168.0.200
  */
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  /* Setup your static leases.

     As it depend from your first address, and need to be done BEFORE any request from client,
     this need to be specified after WiFi.softAPConfig() and before WiFi.softAP().

     first call to wifi_softap_add_dhcps_lease() will setup first IP address of the range
     second call to wifi_softap_add_dhcps_lease() will setup second IP address of the range
     ...
     any client not listed will use next IP address available from the range (here 192.168.0.102 and more)
  */
  dhcpSoftAP.add_dhcps_lease(mac_CAM);  // always 192.168.0.100
  dhcpSoftAP.add_dhcps_lease(mac_PC);   // always 192.168.0.101
  /* Start Access Point. You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  /* Setup HTTP Server */
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
