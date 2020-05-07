
#include <Arduino.h>
#include <PolledTimeout.h>

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>


clsMDNSHost            mDNSHost_AP;
clsMDNSHost            mDNSHost_STA;
ESP8266WebServer    server(80);

void connectToWiFi(const char* p_pcSSID,
                   const char* p_pcPWD,
                   uint32_t p_u32Timeout = 20)
{
    WiFi.begin(p_pcSSID, p_pcPWD);
    Serial.println("");

    // Wait for connection
    uint8   u8Tries = p_u32Timeout;
    while ((WiFi.status() != WL_CONNECTED) &&
           (u8Tries--)) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(p_pcSSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.printf("FAILED to connect to '%s'!\n", p_pcSSID);
    }
}

void setup(void)
{
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    delay(2000);
    Serial.printf("\nStart\n");

    // Setup WiFi and AP
    WiFi.setAutoConnect(false);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP8266", "12345678");
    Serial.print("Created AP ");
    Serial.println("ESP8266");
    Serial.print("AP-IP address: ");
    Serial.println(WiFi.softAPIP());

    if (mDNSHost_AP.begin("ESP8266", WIFI_AP, [](clsMDNSHost& p_rMDNSHost,
                                                 const char* p_pcDomainName,
                                                 bool p_bProbeResult)->void
        {
            Serial.printf("mDNSHost_AP::ProbeResultCallback: '%s' is %s\n", p_pcDomainName, (p_bProbeResult ? "FREE" : "USED!"));

            // Unattended added service
            p_rMDNSHost.addService(0, "http", "tcp", 80);
        }))
    {
        Serial.println("mDNS-AP started");
    }
    else
    {
        Serial.println("FAILED to start mDNS-AP");
    }

    // Connect to WiFi network, with WRONG password
    connectToWiFi("AP8", "WRONG_PW", 5);

    if (mDNSHost_STA.begin("esp8266", WIFI_STA, [](clsMDNSHost& p_rMDNSHost,
                                                   const char* p_pcDomainName,
                                                   bool p_bProbeResult)->void
        {
            Serial.printf("mDNSHost_STA::ProbeResultCallback: '%s' is %s\n", p_pcDomainName, (p_bProbeResult ? "FREE" : "USED!"));
            if (p_bProbeResult)
            {
                p_rMDNSHost.addService("LEA_Weather", "http", "tcp", 80, [](clsMDNSHost::clsService& p_rService,
                                                                const char* p_pcInstanceName,
                                                                bool p_bProbeResult)->void
                {
                    Serial.printf("mDNSHost_STA::HTTP-Service::ProbeResultCallback: '%s' is %s\n", p_pcInstanceName, (p_bProbeResult ? "FREE" : "USED!"));
                    if (p_bProbeResult)
                    {
                        if (!p_rService.addServiceTxt("path", "/"))
                        {
                            Serial.println("FAILED to add service TXT item!");
                        }
                        p_rService.setDynamicServiceTxtCallback([](clsMDNSHost::clsService& p_rService)->void
                        {
                            Serial.printf("mDNSHost_STA::HTTP-Service::DynamicTXTCallback\n");

                            p_rService.addDynamicServiceTxt("user", "admin");
                            static uint32_t u32Counter = 0;
                            p_rService.addDynamicServiceTxt("cnt", ++u32Counter);
                        });
                    }
                    else
                    {
                        if (p_rService.indexInstanceName())
                        {
                            Serial.printf("Changed service instance name to '%s'\n", p_rService.instanceName());
                        }
                        else
                        {
                            Serial.println("FAILED to index service instance name!");
                        }
                    }
                });

                // Unattended added service
                p_rMDNSHost.addService("MQTTInstance", "mqtt", "tcp", 1883);
            }
            else
            {
                p_rMDNSHost.indexHostName();
            }
        }))
    {
        Serial.println("mDNS-STA started");
    }
    else
    {
        Serial.println("FAILED to start mDNS-STA");
    }

    // Non-synchronized added service
    mDNSHost_STA.addService(0, "test", "tcp", 999);

    // Setup HTTP server
    server.on("/", [](void)
    {
        Serial.println("server.on");
        server.send(200, "text/plain", "test");
    });
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
    mDNSHost_AP.update();
    mDNSHost_STA.update();

    static esp8266::polledTimeout::oneShotMs    timer2(esp8266::polledTimeout::oneShotMs::alwaysExpired);
    if (timer2)
    {
        Serial.println("FIX PASSWORD");
        connectToWiFi("AP8", "_______");

        timer2.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
    }
}





