/*
     These functions may exist in other projects
*/

#if LWIP_FEATURES && !LWIP_IPV6

/*
  Returns a descriptive string for WiFi.status() value
*/
String getWiFiStatusString(uint32_t status) {
  const __FlashStringHelper *r;
  switch (status) {
    case WL_IDLE_STATUS:
      r = F("WL_IDLE_STATUS");
      break;

    case WL_NO_SSID_AVAIL:
      r = F("WL_NO_SSID_AVAIL");
      break;

    case WL_SCAN_COMPLETED:
      r = F("WL_SCAN_COMPLETED");
      break;

    case WL_CONNECTED:
      r = F("WL_CONNECTED");
      break;

    case WL_CONNECT_FAILED:
      r = F("WL_CONNECT_FAILED");
      break;

    case WL_CONNECTION_LOST:
      r = F("WL_CONNECTION_LOST");
      break;

    case WL_DISCONNECTED:
      r = F("WL_DISCONNECTED");
      break;

    case WL_NO_SHIELD:
      r = F("WL_NO_SHIELD");
      break;

    default:
      return String(F("Unknown: 0x")) + String(status, HEX);
  }
  return String(r);
}

/*
  Returns a single charcter to append to a "802.11" string to describe the PHY
  mode of a WiFi device. Can be used with the value returned by WiFi.getPhyMode().
*/
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

/*
  Return a String of 6 colon separated hex bytes.
  This format is commonly used when printing 6 byte MAC addresses.
*/
String macToString(const unsigned char* mac) {
  char buf[20];
  int rc = snprintf(buf, sizeof(buf), PSTR("%02X:%02X:%02X:%02X:%02X:%02X"),
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  if (rc < 0 || rc >= (int)sizeof(buf)) {
    return emptyString;
  }
  return String(buf);
}

#endif // LWIP_FEATURES && !LWIP_IPV6
