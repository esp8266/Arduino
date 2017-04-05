/*
 *  Smart config example sketch
 *
 */

#include <ESP8266WiFi.h>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

void setup()
{
    Serial.begin(115200);

    // clear all ssid
    wifi_station_clear_username();
    delay(500);

    Serial.println();
    Serial.println();
    Serial.println();

    if (WiFi.beginSmartConfig())
    {
        Serial.println("begin SmartConfig success!");

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            if(WiFi.smartConfigDone())
            {
                WiFi.stopSmartConfig()
            }
        }

        Serial.println("");
        Serial.println("WiFi connected");
        WiFi.printDiag(Serial);
    }
    else
    {
        Serial.println("begin SmartConfig fail!");
    }
}


void loop()
{
}
