
/* 
 *  Author: Pius Onyema Ndukwu
 *  License: MIT License
 *  GitHub:https://github.com/Pius171/esp8266-wifi-extender
 *  
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>

AsyncWebServer server(80);
DynamicJsonDocument Config(2048);


// variables
bool RepeaterIsWorking= true;
int ledState = LOW; 
unsigned long previousMillis = 0;
long delay_time=0; // interval between blinks
// blink every 200ms if connected to router
// blink every 1sec if web server is active
// led is off is there is an error with the repeater
//led is on when trying to connect to router.


/* Set these to your desired credentials. */

#if LWIP_FEATURES && !LWIP_IPV6

#define HAVE_NETDUMP 0


#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>

#define NAPT 1000
#define NAPT_PORT 10

#if HAVE_NETDUMP

#include <NetDump.h>


void dump(int netif_idx, const char* data, size_t len, int out, int success) {
  (void)success;
  Serial.print(out ? F("out ") : F(" in "));
  Serial.printf("%d ", netif_idx);

  // optional filter example: if (netDump_is_ARP(data))
  {
    netDump(Serial, data, len);
    //netDumpHex(Serial, data, len);
  }
}
#endif


class wifi {

  public:

    JsonObject obj = Config.as<JsonObject>();

    // just added this so i can see the files in the file system
    void listDir(const char * dirname) {
      Serial.printf("Listing directory: %s\n", dirname);

      Dir root = LittleFS.openDir(dirname);

      while (root.next()) {
        File file = root.openFile("r");
        Serial.print("  FILE: ");
        Serial.print(root.fileName());
        Serial.print("  SIZE: ");
        Serial.print(file.size());
        file.close();

      }
    }

    void create_server() {

      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {

        // scan for networks and get ssid
        String network_html = "";


        // link to code https://github.com/me-no-dev/ESPAsyncWebServer/issues/85#issuecomment-258603281
        // there is a problem with scanning whil using espasync that is why the code below is not just WiFi.scan()
        int n = WiFi.scanComplete();
        if (n == -2) {
          WiFi.scanNetworks(true);
        } else if (n) {
          for (int i = 0; i < n; ++i) {
            String router = WiFi.SSID(i);
            Serial.println(router);
            network_html += "<input type=\"radio\" id=\"#radiobuttonex\" name=\"ssid\" value=" + router + " required ><label for=\"html\">" + router + "</label><<br>";

          }
          WiFi.scanDelete();
          if (WiFi.scanComplete() == -2) {
            WiFi.scanNetworks(true);
          }
        }

        String html = "<!DOCTYPE html><html>";
        html+= "<head>";
        html+=" <link rel=\"stylesheet\" href=\"styles.css\">";
        html+= "</head>";
        html += "<body>";
        html+= "<div>";
        html += "<h1>Pius Electronics Extender Config page</h1>";
        html += "<button onclick=\"window.location.href='/';\">Scan </button>";
        html += "<p>networks found </p>";
        html += "<form action=\"/credentials\">";
        html += "<p>Please select a WiFi network:</p>" + network_html;
        html += "<input type=\"password\" id=\"pass\" name=\"pass\" value=\"\" required ><label for=\"pass\">password</label><br><br>";
        html += "<input type=\"text\" id=\"ap\" name=\"ap\" value=\"\" required ><label for=\"ap\">A.P name:</label><br>";
        html += "<input type=\"submit\" value=\"Submit\">";
        html += "</form></body></html>";
        html+= "</div>";

        request->send(200, "text/html", html);
      });

// css style from grepper
      server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        String style = "#radiobuttonex{\n"
"  border: 2px solid rgb(255,1,199);\n"
"  text-align: center;\n"
"  font-family: sans serif;\n"
"  width: 305px;\n"
"  background: rgb(50,50,100);\n"
"  border-radius: 40px;\n"
"}";
        
       request->send(200, "text/css", style);
        
        });

      // Send a GET request to <IP>/get?message=<message> 
      server.on("/credentials", HTTP_GET, [] (AsyncWebServerRequest * request) {
        String param = "ssid";

        if (request->hasParam(param)) {
          String ssid = request->getParam(param)->value();
          Config["ssid"] = ssid;
          Serial.println(ssid);
        } else {
          Serial.println("No " + param + " sent");
        }

        param = "pass";
        if (request->hasParam(param)) {
          String pass = request->getParam(param)->value();
          Config["pass"] = pass;
          Serial.println(pass);
        } else {
          Serial.println("No " + param + " sent");
        }

        param = "ap";
        if (request->hasParam(param)) {
          String ap = request->getParam(param)->value();
          Config["ap"] = ap;
          Serial.println(ap);
        } else {
          Serial.println("No " + param + " sent");
        }
        String output;
        serializeJson(Config, output);
        Serial.print(output);

        String path = "/config.json";

        Serial.printf("Writing file: %s\n", path);

        File file = LittleFS.open(path, "w");
        if (!file) {
          Serial.println("Failed to open file for writing");
          return "null";
        }
        if (file.print(output)) {
          Serial.println("File written");
        } else {
          Serial.println("Write failed");
        }
        file.close();

      });

    }



    String get_credentials(int a) {
      // a: 0=ssid, 1=pass; 2=ap name
      String path = "/config.json";
      String credentials = "";

      Serial.print("reading file ");
      Serial.println(path);

      File file = LittleFS.open(path, "r");
      if (!file) {
        Serial.println("Failed to open file for reading");
        Serial.println("this is probally first usage, so the file does not exist");
        return "null";
      }

      Serial.print("Read from file: ");
      while (file.available()) {
        credentials += file.readString();

      }
      Serial.println(credentials);
      deserializeJson(Config, credentials);
      file.close();
      String credential_array [3] = {Config["ssid"], Config["pass"], Config["ap"]};
      return credential_array[a];
    }

};



wifi my_wifi;


void setup() {
  delay(1000);
 pinMode(0,INPUT_PULLUP);
 pinMode(LED_BUILTIN,OUTPUT);
 digitalWrite(LED_BUILTIN,1); //active low
  Serial.begin(115200);

  Serial.println();

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  Serial.printf("\n\nNAPT Range extender\n");
  Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());

#if HAVE_NETDUMP
  phy_capture = dump;
#endif

  // first, connect to STA so we can get a proper local DNS server


  String ssid = my_wifi.get_credentials(0); // if the file does not exist the function will always return null
  String pass = my_wifi.get_credentials(1);
  String ap= my_wifi.get_credentials(2);

  if (ssid == "null") { // if the file does not exist ssid will be null
start_webserver:
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    WiFi.softAP("Pius_Electronics_extender0001");
    Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());
    my_wifi.create_server();
    server.begin();
    Serial.println("HTTP server started");
    delay_time=1000; // blink every sec if webserver is active
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass); // check function to understand
    int timeout_counter=0;
    while (WiFi.status() != WL_CONNECTED) {
      if(timeout_counter>=120){
        goto start_webserver; // if it fails to connect start_webserver
      }

      Serial.print('.');
      timeout_counter++;
      digitalWrite(LED_BUILTIN,0);// leave led on when trying to connect
      delay(500);
    }



    Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                  WiFi.localIP().toString().c_str(),
                  WiFi.dnsIP(0).toString().c_str(),
                  WiFi.dnsIP(1).toString().c_str());

    // give DNS servers to AP side
    dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
    dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));

    WiFi.softAPConfig(  // enable AP, with android-compatible google domain
      IPAddress(172, 217, 28, 254),
      IPAddress(172, 217, 28, 254),
      IPAddress(255, 255, 255, 0));
    WiFi.softAP(ap, pass);
    Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

    Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
    err_t ret = ip_napt_init(NAPT, NAPT_PORT);
    Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      ret = ip_napt_enable_no(SOFTAP_IF, 1);
      Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
      if (ret == ERR_OK) {
        Serial.printf("Successfully NATed to WiFi Network '%s' with the same password", ssid.c_str());
      }
    }
    Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
    if (ret != ERR_OK) {
      Serial.printf("NAPT initialization failed\n");
    }
    delay_time=200; // blink every half second if connection was succesfull
  }
  RepeaterIsWorking=true;
}

#else

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nNAPT not supported in this configuration\n");
  RepeaterIsWorking= false;
  digitalWrite(LED_BUILTIN,1); // led stays off
}

#endif

void loop() {
  if(digitalRead(0)==LOW){
    LittleFS.format();
    ESP.restart();
  }

  while(RepeaterIsWorking){
 unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= delay_time) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
  break;
}
}
