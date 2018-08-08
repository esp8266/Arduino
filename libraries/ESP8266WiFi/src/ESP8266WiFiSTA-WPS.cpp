
#include "ESP8266WiFi.h"
#include "ESP8266WiFiGeneric.h"
#include "ESP8266WiFiSTA.h"

void wifi_wps_status_cb(wps_cb_status status);

/**
 * WPS config
 * so far only WPS_TYPE_PBC is supported (SDK 1.2.0)
 * @return ok
 */
bool beginWPSConfig(void) {

    if(!WiFi.enableSTA(true)) {
        // enable STA failed
        return false;
    }

    WiFi.disconnect();

    DEBUGV("wps begin\n");

    if(!wifi_wps_disable()) {
        DEBUGV("wps disable failed\n");
        return false;
    }

    // so far only WPS_TYPE_PBC is supported (SDK 1.2.0)
    if(!wifi_wps_enable(WPS_TYPE_PBC)) {
        DEBUGV("wps enable failed\n");
        return false;
    }

    if(!wifi_set_wps_cb((wps_st_cb_t) &wifi_wps_status_cb)) {
        DEBUGV("wps cb failed\n");
        return false;
    }

    if(!wifi_wps_start()) {
        DEBUGV("wps start failed\n");
        return false;
    }

    esp_yield();
    // will return here when wifi_wps_status_cb fires

    return true;
}

/**
 * WPS callback
 * @param status wps_cb_status
 */
void wifi_wps_status_cb(wps_cb_status status) {
    DEBUGV("wps cb status: %d\r\n", status);
    switch(status) {
        case WPS_CB_ST_SUCCESS:
            if(!wifi_wps_disable()) {
                DEBUGV("wps disable failed\n");
            }
            wifi_station_connect();
            break;
        case WPS_CB_ST_FAILED:
            DEBUGV("wps FAILED\n");
            break;
        case WPS_CB_ST_TIMEOUT:
            DEBUGV("wps TIMEOUT\n");
            break;
        case WPS_CB_ST_WEP:
            DEBUGV("wps WEP\n");
            break;
        case WPS_CB_ST_UNK:
            DEBUGV("wps UNKNOWN\n");
            if(!wifi_wps_disable()) {
                DEBUGV("wps disable failed\n");
            }
            break;
    }
    // TODO user function to get status

    esp_schedule(); // resume the beginWPSConfig function
}

