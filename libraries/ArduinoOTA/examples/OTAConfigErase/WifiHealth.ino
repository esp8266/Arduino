
wifi_health_t wifiHealth = {
  0, 0, 0, 0,
  0, 0, 0, (time_t)LONG_MAX,
  0, 0, 0, (time_t)LONG_MAX,
  "", 0, INT8_MIN, INT8_MAX, 0};
WiFiDisconnectLog_t wifi_disconnect_log[MAX_CONNECTION_LOST_TIME_LOG] = {0, 0, 0, 0};
Ticker scheduleCheck;
bool WifiUp = false;


bool printLocalTime(Print& oPrint, time_t timeIn, const char *fmt) {
  time_t gtime = timeIn;
  if (0 == gtime) {
    time(&gtime);
  }

  char ephemeralBuffer[64];
  if (strftime(ephemeralBuffer, sizeof(ephemeralBuffer), fmt, localtime(&gtime)) > 0) {
    oPrint.print(ephemeralBuffer);
    return true;
  }
  return false;
}

bool printUpTime(Print& oPrint, time_t timeIn) {
  time_t gtime;
  if (timeIn) {
    gtime = timeIn;
  } else {
    gtime = (time_t)(micros64() / 1000000);
  }

  char ephemeralBuffer[64];
  struct tm *tv = gmtime(&gtime);
  if (strftime(ephemeralBuffer, sizeof(ephemeralBuffer), "%T", tv) > 0) {
    if (tv->tm_yday) {
      oPrint.print(String((tv->tm_yday)) + " day" + ((tv->tm_yday == 1) ? " " : "s "));
    }

    oPrint.print(ephemeralBuffer);
    return true;
  }
  return false;
}

#define StringF(a) String(F(a))
String getWiFiDisconnectReasonString(uint32_t reason) {
  const __FlashStringHelper *r;
  switch (reason) {
    case WIFI_DISCONNECT_REASON_UNSPECIFIED:
      r = F("UNSPECIFIED");
      break;

    case WIFI_DISCONNECT_REASON_AUTH_EXPIRE:
      r = F("AUTH_EXPIRE");
      break;

    case WIFI_DISCONNECT_REASON_AUTH_LEAVE:
      r = F("AUTH_LEAVE");
      break;

    case WIFI_DISCONNECT_REASON_ASSOC_EXPIRE:
      r = F("ASSOC_EXPIRE");
      break;

    case WIFI_DISCONNECT_REASON_ASSOC_TOOMANY:
      r = F("ASSOC_TOOMANY");
      break;

    case WIFI_DISCONNECT_REASON_NOT_AUTHED:
      r = F("NOT_AUTHED");
      break;

    case WIFI_DISCONNECT_REASON_NOT_ASSOCED:
      r = F("NOT_ASSOCED");
      break;

    case WIFI_DISCONNECT_REASON_ASSOC_LEAVE:
      r = F("ASSOC_LEAVE");
      break;

    case WIFI_DISCONNECT_REASON_ASSOC_NOT_AUTHED:
      r = F("ASSOC_NOT_AUTHED");
      break;

    case WIFI_DISCONNECT_REASON_DISASSOC_PWRCAP_BAD:
      r = F("DISASSOC_PWRCAP_BAD");
      break;

    case WIFI_DISCONNECT_REASON_DISASSOC_SUPCHAN_BAD:
      r = F("DISASSOC_SUPCHAN_BAD");
      break;

    case WIFI_DISCONNECT_REASON_IE_INVALID:
      r = F("IE_INVALID");
      break;

    case WIFI_DISCONNECT_REASON_MIC_FAILURE:
      r = F("MIC_FAILURE");
      break;

    case WIFI_DISCONNECT_REASON_4WAY_HANDSHAKE_TIMEOUT:
      r = F("4WAY_HANDSHAKE_TIMEOUT");
      break;

    case WIFI_DISCONNECT_REASON_GROUP_KEY_UPDATE_TIMEOUT:
      r = F("GROUP_KEY_UPDATE_TIMEOUT");
      break;

    case WIFI_DISCONNECT_REASON_IE_IN_4WAY_DIFFERS:
      r = F("IE_IN_4WAY_DIFFERS");
      break;

    case WIFI_DISCONNECT_REASON_GROUP_CIPHER_INVALID:
      r = F("GROUP_CIPHER_INVALID");
      break;

    case WIFI_DISCONNECT_REASON_PAIRWISE_CIPHER_INVALID:
      r = F("PAIRWISE_CIPHER_INVALID");
      break;

    case WIFI_DISCONNECT_REASON_AKMP_INVALID:
      r = F("AKMP_INVALID");
      break;

    case WIFI_DISCONNECT_REASON_UNSUPP_RSN_IE_VERSION:
      r = F("UNSUPP_RSN_IE_VERSION");
      break;

    case WIFI_DISCONNECT_REASON_INVALID_RSN_IE_CAP:
      r = F("INVALID_RSN_IE_CAP");
      break;

    case WIFI_DISCONNECT_REASON_802_1X_AUTH_FAILED:
      r = F("802_1X_AUTH_FAILED");
      break;

    case WIFI_DISCONNECT_REASON_CIPHER_SUITE_REJECTED:
      r = F("CIPHER_SUITE_REJECTED");
      break;

    case WIFI_DISCONNECT_REASON_BEACON_TIMEOUT:
      r = F("BEACON_TIMEOUT");
      break;

    case WIFI_DISCONNECT_REASON_NO_AP_FOUND:
      r = F("NO_AP_FOUND");
      break;

    case WIFI_DISCONNECT_REASON_AUTH_FAIL:
      r = F("AUTH_FAIL");
      break;

    case WIFI_DISCONNECT_REASON_ASSOC_FAIL:
      r = F("ASSOC_FAIL");
      break;

    case WIFI_DISCONNECT_REASON_HANDSHAKE_TIMEOUT:
      r = F("HANDSHAKE_TIMEOUT");
      break;

    default:
      return String(F("Unknown: ")) + String(reason, HEX);
  }
  return String(r);
}

char getPhyModeChar(WiFiPhyMode_t i) {
  switch (i) {
    case WIFI_PHY_MODE_11B:
      return 'b'; // = 1
    case WIFI_PHY_MODE_11G:
      return 'g'; // = 2,
    case WIFI_PHY_MODE_11N:
      return 'n'; // = 3,
    default:
      break;
  }
  return '?';
}

void updateWiFiStats(void) {
  int32_t rssi = WiFi.RSSI();
  if (rssi < 10) {
    wifiHealth.rssi_max = max((int8_t)rssi, wifiHealth.rssi_max);
    wifiHealth.rssi_min = min((int8_t)rssi, wifiHealth.rssi_min);
    wifiHealth.rssi = rssi;
    wifiHealth.rssi_sum += rssi;
    wifiHealth.rssi_count++;
  }
}

void onWiFiConnected(WiFiEventStationModeConnected data) {
  wifiHealth.channel = data.channel;
  memcpy(wifiHealth.bssid, data.bssid, sizeof(wifiHealth.bssid));
  wifiHealth.connected_count++;
  time_t stime = (time_t)(micros64() / 1000000);
  wifiHealth.connected_time = stime;
  wifiHealth.rssi = WiFi.RSSI();
  WifiUp = true;
  wifiLedOn();

  if (wifiHealth.disconnected_time) {
    time_t downtime = stime - wifiHealth.disconnected_time;
    wifiHealth.downtime_sum += downtime;
    wifiHealth.downtime_max = max(wifiHealth.downtime_max, downtime);
    wifiHealth.downtime_min = min(wifiHealth.downtime_min, downtime);
    wifiHealth.disconnected_time = (time_t)0;
  }
}

void onWiFiDisconnected(WiFiEventStationModeDisconnected data) {
  /* After a disconnect, an attempt is made about every 3 secs to reconnect.
     On each failed attempt, this function is called. */
  WifiUp = false;
  time_t gtime;
  time(&gtime);
  time_t stime = (time_t)(micros64() / 1000000);
  wifiHealth.gtime_adjust = gtime - stime;

  if (wifiHealth.connected_time) {
    wifiLedOff();
    wifiHealth.disconnected_time = stime;
    size_t last = wifiHealth.connected_count % MAX_CONNECTION_LOST_TIME_LOG;
    wifi_disconnect_log[last].time    = stime; // Note, 1st entry is at [1].
    wifi_disconnect_log[last].reason  = data.reason;
    wifi_disconnect_log[last].channel = wifiHealth.channel;
    wifi_disconnect_log[last].rssi    = wifiHealth.rssi;
    time_t uptime = stime - wifiHealth.connected_time;
    wifiHealth.uptime_sum += uptime;
    wifiHealth.uptime_max = max(wifiHealth.uptime_max, uptime);
    wifiHealth.uptime_min = min(wifiHealth.uptime_min, uptime);
    wifiHealth.connected_time = (time_t)0;
  }
}


bool printWiFiStats(Print& oStream) {
  bool bSuccess = false;

  if (WiFi.status() == WL_CONNECTED) {
    oStream.println(String_F("\nWiFi connected:   '") + WiFi.SSID() + "'");
    oStream.println(String_F("  SDK Version:       ") + String(ESP.getSdkVersion()));
    oStream.printf_P(PSTR("  BSSID:             %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
                     wifiHealth.bssid[0], wifiHealth.bssid[1], wifiHealth.bssid[2],
                     wifiHealth.bssid[3], wifiHealth.bssid[4], wifiHealth.bssid[5]);
    oStream.println(String_F("  PHY Mode:          802.11") + (getPhyModeChar(WiFi.getPhyMode())));
    oStream.println(String_F("  Channel:           ") + (WiFi.channel()));
    oStream.println(String_F("  RSSI:              ") + (WiFi.RSSI()));
    oStream.println(String_F("    MAX:             ") + (wifiHealth.rssi_max));
    oStream.println(String_F("    MIN:             ") + (wifiHealth.rssi_min));
    if (wifiHealth.rssi_count) {
      oStream.println(String_F("    AVG:             ") + (wifiHealth.rssi_sum / (int32_t)wifiHealth.rssi_count));
    }

    oStream.println(String_F("    sample count:    ") + (wifiHealth.rssi_count));
    if (0 != wifiHealth.connected_time) {
      oStream.print(String_F("  Connection Uptime: "));
      time_t uptime = (time_t)(micros64() / 1000000);
      uptime -= wifiHealth.connected_time;
      printUpTime(oStream, uptime);
      oStream.println();

      uptime = wifiHealth.uptime_sum;
      if (uptime) {
        oStream.print(String_F("  Total Uptime:      "));
        printUpTime(oStream, uptime);
        oStream.println();

        if (2 < wifiHealth.connected_count) {
          if (wifiHealth.uptime_max) {
            oStream.print(String_F("    MAX:             "));
            printUpTime(oStream, wifiHealth.uptime_max);
            oStream.println();
          }
          if ((time_t)LONG_MAX != wifiHealth.uptime_min) {
            oStream.print(String_F("    MIN:             "));
            printUpTime(oStream, wifiHealth.uptime_min);
            oStream.println();
          }
          oStream.print(String_F("    AVG:             "));
          uptime /= (decltype(uptime))(wifiHealth.connected_count - 1);
          printUpTime(oStream, uptime);
          oStream.println();
        }
      }

      time_t downtime = wifiHealth.downtime_sum;
      if (downtime) {
        oStream.print(String_F("  Total Downtime:    "));
        printUpTime(oStream, downtime);
        oStream.println();

        if (2 < wifiHealth.connected_count) {
          if (wifiHealth.downtime_max) {
            oStream.print(String_F("    MAX:             "));
            printUpTime(oStream, wifiHealth.downtime_max);
            oStream.println();
          }
          if ((time_t)LONG_MAX != wifiHealth.downtime_min) {
            oStream.print(String_F("    MIN:             "));
            printUpTime(oStream, wifiHealth.downtime_min);
            oStream.println();
          }
          oStream.print(String_F("    AVG:             "));
          downtime /= (decltype(uptime))(wifiHealth.connected_count - 1);
          printUpTime(oStream, downtime);
          oStream.println();
        }
      }
      oStream.println(String_F("  Reconnects:        ") + (wifiHealth.connected_count - 1));

      if (wifiHealth.connected_count > 1) {
        oStream.println(String_F("  Recent Disconnect times:"));
        ssize_t back_count = wifiHealth.connected_count;
        for (ssize_t i = 1; i <= MAX_CONNECTION_LOST_TIME_LOG; i++) {
          back_count -= 1;
          if (back_count < 1) {
            break;
          }

          ssize_t iLog = back_count % MAX_CONNECTION_LOST_TIME_LOG;
          oStream.print(String_F("    "));
          printLocalTime(oStream, wifi_disconnect_log[iLog].time + wifiHealth.gtime_adjust);
          oStream.println();
          oStream.println(String_F("      Reason:  ") + getWiFiDisconnectReasonString(wifi_disconnect_log[iLog].reason));
          oStream.println(String_F("      Channel: ") + (wifi_disconnect_log[iLog].channel));
          oStream.println(String_F("      RSSI:    ") + (wifi_disconnect_log[iLog].rssi));
        }
      }
    }

    oStream.println(String_F("  IP Address:        ") + (WiFi.localIP().toString()));
    oStream.println(String_F("  Network Mask:      ") + (WiFi.subnetMask().toString()));
    oStream.println(String_F("  Gateway:           ") + (WiFi.gatewayIP().toString()));
    oStream.println(String_F("  DNS1:              ") + (WiFi.dnsIP(0).toString()));
    oStream.println(String_F("  DNS2:              ") + (WiFi.dnsIP(1).toString()));


    bSuccess = true;

  } else {
    oStream.println(String_F("WiFi not connected."));
  }

  return bSuccess;
}
