/**
 * @file OTA-mDNS-SPIFFS.ino
 * 
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 * @data 2015-09-18
 * 
 */


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <FS.h>


/**
 * @brief mDNS and OTA Constants
 * @{
 */
#define HOSTNAME "ESP8266-OTA-" ///< Hostename. The setup function adds the Chip ID at the end.
#define APORT 8266 ///< Port for OTA update
/// @}

/**
 * @brief Default WiFi connection information.
 * @{
 */
const char* ap_default_ssid = "esp8266"; ///< Default SSID.
const char* ap_default_psk = "esp8266esp8266"; ///< Default PSK.
/// @}

/// OTA Update UDP server handle.
WiFiUDP OTA;


/**
 * @brief Read WiFi connection information from file system.
 * @param ssid String pointer for storing SSID.
 * @param pass String pointer for storing PSK.
 * @return True or False.
 * 
 * The config file have to containt the WiFi SSID in the first line
 * and the WiFi PSK in the second line.
 * Line seperator have to be \r\n (CR LF).
 */
bool loadConfig(String *ssid, String *pass)
{
  // open file for reading.
  File configFile = SPIFFS.open("/cl_conf.txt", "r");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();
  
  content.trim();

  // Check if ther is a second line available.
  uint8_t pos = content.indexOf("\r\n");
  if (pos == 0)
  {
    Serial.println("Infvalid content.");
    Serial.println(content);

    return false;
  }

  // Store SSID and PSK into string vars.
  *ssid = content.substring(0, pos);
  *pass = content.substring(pos + 2);

  return true;
} // loadConfig


/**
 * @brief Save WiFi SSID and PSK to configuration file.
 * @param ssid SSID as string pointer.
 * @param pass PSK as string pointer,
 * @return True or False.
 */
bool saveConfig(String *ssid, String *pass)
{
  // Open config file for writing.
  File configFile = SPIFFS.open("/cl_conf.txt", "w");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt for writing");

    return false;
  }

  // Save SSID and PSK.
  configFile.println(*ssid);
  configFile.println(*pass);

  configFile.close();
  
  return true;
} // saveConfig


/**
 * @brief Handle OTA update stuff.
 * 
 * This function comes from ESP8266 Arduino example:
 * https://github.com/esp8266/Arduino/blob/esp8266/hardware/esp8266com/esp8266/libraries/ESP8266mDNS/examples/DNS_SD_Arduino_OTA/DNS_SD_Arduino_OTA.ino
 * 
 * Modification for uploading SPIFFS images from Pascal Gollor.
 *
 */
static inline void ota_handle(void)
{
  bool spiffs = false;
  
  if (! OTA.parsePacket())
  {
    return;
  }

  // Get remote IP
  IPAddress remote = OTA.remoteIP();

  // Get command
  int cmd = OTA.parseInt();
  Serial.print("command: ");
  Serial.println(cmd);
  if (cmd == U_SPIFFS)
  {
    spiffs = true;
    Serial.println("Get SPIFFS image.");
  }

  // Get remote port
  int port = OTA.parseInt();

  // Get sketch size.
  int sketch_size = OTA.parseInt();

  // Output stuff
  Serial.print("Update Start: ip:");
  Serial.print(remote);
  Serial.printf(", port:%d, size:%d\r\n", port, sketch_size);

  // Stop all UDP connections.
  WiFiUDP::stopAll();
  
  // OTA start Time
  uint32_t startTime = millis();

  // Start Updateing.
  if(!Update.begin(sketch_size, cmd))
  {
    Serial.println("Update Begin Error");
    return;
  }

  WiFiClient client;
  if (client.connect(remote, port))
  {
    uint32_t written;
    while(!Update.isFinished())
    {
      written = Update.write(client);
      if(written > 0) client.print(written, DEC);
    }
    Serial.setDebugOutput(false);

    if(Update.end())
    {
      client.println("OK");
      Serial.printf("Update Success: %u\nRebooting...\n", (unsigned int)(millis() - startTime));
      ESP.restart();
    }
    else
    {
      Update.printError(client);
      Update.printError(Serial);
    }
  }
  else
  {
    Serial.printf("Connect Failed: %u\n", (unsigned int)(millis() - startTime));
  }
} // ota_handle


/**
 * @brief Arduino setup function.
 */
void setup()
{
  String station_ssid = "";
  String station_psk = "";

  Serial.begin(115200);
  
  delay(100);

  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

  // Set Hostname.
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);

  // Print hostname.
  Serial.print("hostname: ");
  Serial.println(WiFi.hostname());


  // Initialize file system.
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  // Load wifi connection information.
  if (! loadConfig(&station_ssid, &station_psk))
  {
    station_ssid = "";
    station_psk = "";

    Serial.println("No WiFi connection information available.");
  }

  // Check WiFi connection
  // ... check mode
  if (WiFi.getMode() != WIFI_STA)
  {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  // ... Load sdk config.
  String ssid(WiFi.SSID());
  String psk(WiFi.psk());

  // ... Compare fiel config with sdk config.
  if (ssid != station_ssid || psk != station_psk)
  {
    Serial.println("WiFi config changed.");

    // ... Try to connect to WiFi station.
    WiFi.begin(station_ssid.c_str(), station_psk.c_str());

    // ... Pritn new SSID
    Serial.print("new SSID: ");
    Serial.println(WiFi.SSID());

    // ... Uncomment this for debugging output.
    //WiFi.printDiag(Serial);
  }
  else
  {
    // ... Begin with sdk config.
    WiFi.begin();
  }

  Serial.println("Wait for WiFi connection.");

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
  {
    Serial.write('.');
    //Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println();

  // Check connection
  if(WiFi.status() == WL_CONNECTED)
  {
    // ... print IP Address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");
    
    // Go into software AP mode.
    WiFi.mode(WIFI_AP);

    delay(10);

    WiFi.softAP(ap_default_ssid, ap_default_psk);

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
  }

  // Initialize mDNS service.
  MDNS.begin(hostname.c_str());

  // ... Add OTA service.
  MDNS.addService("arduino", "tcp", APORT);

  // Open OTA Server.
  OTA.begin(APORT);
}


/**
 * @brief Arduino loop function.
 */
void loop()
{
  // Handle OTA update.
  ota_handle();
}

