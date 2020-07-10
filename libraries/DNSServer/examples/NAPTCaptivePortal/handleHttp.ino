#if LWIP_FEATURES && !LWIP_IPV6

#include "WifiHttpv.h"

#ifndef TCP_MSS
#define TCP_MSS 1460
#endif
/*
  Use kMaxChunkSize to limit size of chuncks
*/
constexpr size_t kMaxChunkSize = TCP_MSS;
String& sendIfOver(String & str, size_t threshold = kMaxChunkSize / 2);
size_t sendAsChunks_P(PGM_P content, size_t chunkSize = kMaxChunkSize);

size_t maxPage = 0;

void addNoCacheHeader() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
}


String& sendIfOver(String & str, size_t threshold) {
  size_t len = str.length();
  if (len > threshold) {
    // Use later to determine if we reserved enough room in page to avoid realloc
    maxPage = std::max(maxPage, len);
    server.sendContent(str);
    str = "";
  }
  return str;
}

/*
  The idea here is to avoid a large allocation by sendContent_P to copy a
  big PROGMEM string. Slice PROGMEM string into chuncks and send.
*/
size_t sendAsChunks_P(PGM_P content, size_t chunkSize) {
  size_t len = strlen_P(content);
  for (size_t pos = 0; pos < len; pos += chunkSize) {
    server.sendContent_P(&content[pos], ((len - pos) >= chunkSize) ? chunkSize : len - pos);
  }
  return len;
}

/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) {
    // If captive portal is needed, redirect instead of displaying the page.
    return;
  }
  addNoCacheHeader();

  String Page;
  Page += F(
            "<!DOCTYPE html>"
            "<html lang='en'><head><meta name='viewport' content='width=device-width'>"
            "<title>ADV CAP Portal Example</title>"
            "</head><body>"
            "<h1>HELLO WORLD!!</h1>");
  if (server.client().localIP() == apIP) {
    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  } else {
    Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  }
  Page += F(
            "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
            "</body></html>");

  server.send(200, F("text/html"), Page);
}

/*
  Redirect to the captive portal if we got a request for another domain.
  Return true in that case, so the page handler does not try to handle
  the request again.
*/
boolean captivePortal() {
  IPAddress hAddr, cAddr;

  cAddr = server.client().localIP();
  if (!cAddr.isSet()) {
    // The connection closed prematurely on us.
    // Return true, so no further action is taken.
    return true;
  }

  if (hAddr.fromString(server.hostHeader()) && hAddr == cAddr) {
    return false;
  }

  if (hAddr.isSet() ||
      (server.hostHeader() != (String(myHostname) + ".local") && // arrived here by mDNS
       server.hostHeader() != String(myHostname))) { // arrived here by local router DNS
    String whereTo = String("http://") + server.client().localIP().toString();
    sendPortalRedirect(whereTo, F("Captive Portal Example"));
    return true;
  }

  return false;
}


/** Wifi Details and Config page handler */
void handleWifi() {
  addNoCacheHeader();

  // use HTTP/1.1 Chunked response to avoid building a huge temporary string
  if (!server.chunkedResponseModeStart(200, F("text/html"))) {
    server.send(505, F("text/plain"), F("HTTP1.1 required"));
    return;
  }

  // Send a few chunks of the HTML that don't need to change.
  sendAsChunks_P(configHead);

  String page;

  CONSOLE_PRINTLN2("sizeof(configHead): ", (sizeof(configHead)));
  CONSOLE_PRINTLN2("sizeof(configWLANInfo): ", (sizeof(configWLANInfo)));
  CONSOLE_PRINTLN2("sizeof(configList): ", (sizeof(configList)));
  CONSOLE_PRINTLN2("sizeof(configEnd): ", (sizeof(configEnd)));
  // Just do max on some of the visually larger HTML chunks that will be loaded
  // into page and add a little for growth when substituting values in.
  size_t thisMany = std::max(sizeof(configWLANInfo), sizeof(configList)) + 200;
  CONSOLE_PRINTLN2("Estimate Minimum page reserve size: ", (thisMany));
  page.reserve(std::max(kMaxChunkSize, thisMany));

  page = FPSTR(configPresetInput);
  /*
    Set previously used/entered credentials as a default entries.
    This allows an opportunity to correct them and try again.
  */
  page.replace("{s}", String(ssid));
  page.replace("{p}", String(password));
  sendIfOver(page);

  page += FPSTR(configConnection);
  if (server.client().localIP() == apIP) {
    page.replace("{w}", String(F("SoftAP: ")) + softAP_ssid);
  } else {
    page.replace("{w}", String(F("WiFi Network: ")) + ssid);
  }

  /*
    To avoid sending lots of small packets. We call this function frequently,
    to check if the 'page' has gone over 512 bytes and if so send.
  */
  sendIfOver(page);

  page += FPSTR(configAPInfo);
  {
    uint8_t sta_cnt = wifi_softap_get_station_num();
    page.replace("{s}", String(softAP_ssid));
    page.replace("{b}", String(WiFi.softAPmacAddress()));
    page.replace("{i}", WiFi.softAPIP().toString());
    page.replace("{a}", String(sta_cnt));
    sendIfOver(page);
    if (sta_cnt) {
      page += String(F("\r\n<PRE>\r\n"));
      struct station_info *info = wifi_softap_get_station_info();
      IPAddress addr;
      while (info != NULL) {
        addr = info->ip;
        page += macToString(info->bssid) + F("  ") + addr.toString() + F("\r\n");
        info = STAILQ_NEXT(info, next);
        sendIfOver(page);
      }
      page += F("</PRE>\r\n");
    }
  }

  /*
    Before we prepare a large block for sending, we call 'sendIfOver' with a
    threshold of 0 to force the sending of the current 'page' content.
  */

  if (WiFi.localIP().isSet()) {
    sendIfOver(page, 0);
    page += FPSTR(configWLANInfo);
    page.replace("{s}", String(ssid));
    page.replace("{b}", macToString(bssid));
    page.replace("{c}", String(WiFi.channel()));
    page.replace("{p}", String(F("802.11")) + (getPhyModeChar(WiFi.getPhyMode())));
    page.replace("{r}", String(WiFi.RSSI()));
    page.replace("{i}", WiFi.localIP().toString());
    page.replace("{g}", WiFi.gatewayIP().toString());
    page.replace("{m}", WiFi.subnetMask().toString());
    page.replace("{1}", WiFi.dnsIP(0).toString());
    page.replace("{2}", WiFi.dnsIP(1).toString());
  } else {
    page += FPSTR(configWLANOffline);
  }

  sendIfOver(page, 0);
  sendAsChunks_P(configList);

  CONSOLE_PRINTLN("scan start");
  int n = WiFi.scanNetworks();
  CONSOLE_PRINTLN("scan done");

  if (n > 0) {
    for (size_t i = 0; i < (size_t)n; i++) {
      page += FPSTR(configItem);
      page.replace("{s}", WiFi.SSID(i));
      page.replace("{t}", WiFi.BSSIDstr(i));
      page.replace("{c}", String(WiFi.channel(i)));
      page.replace("{l}", (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? F("") : F("&#x1f512;"));
      page.replace("{r}", String(WiFi.RSSI(i)));
      sendIfOver(page);
    }
  } else {
    page += FPSTR(configNoAPs);
  }
  sendIfOver(page, 0); // send what we have buffered before next direct send.
  sendAsChunks_P(configEnd);

  CONSOLE_PRINTLN2("MAX String memory used: ", (maxPage));
  server.chunkedResponseFinalize();
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  CONSOLE_PRINTLN("wifi save");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  sendPortalRedirect(F("wifi"), F("Wifi Config"));
  saveCredentials();
  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}

void handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found\r\n\r\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\r\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\r\nArguments: ");
  message += server.args();
  message += F("\r\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\r\n");
  }
  addNoCacheHeader();
  server.send(404, F("text/plain"), message);
}

#endif // LWIP_FEATURES && !LWIP_IPV6
