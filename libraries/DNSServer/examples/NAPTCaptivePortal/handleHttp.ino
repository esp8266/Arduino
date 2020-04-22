#include "WifiHttp.h"
String& sendIfOver(String & str, size_t threshold = 512);

size_t maxPage = 0;

void addNoCacheHeader() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
}


String& sendIfOver(String & str, size_t threshold) {
  size_t len = str.length();
  if (len > threshold) {
      maxPage = std::max(maxPage, len);
      server.sendContent(str);
      str = "";
  }
  return str;
}


/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) {
    // If caprive portal is needed, redirect instead of displaying the page.
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
  server.sendContent_P(configHead);

  String page;

  page = FPSTR(configHead2);
  /*
    Set previously used/entered credentials as a default entries.
    This allows an opportunity to correct them and try again.
  */
  page.replace("{s}", String(ssid));
  page.replace("{p}", String(password));
  sendIfOver(page, 0);

  page = FPSTR(configConnection);
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

  page += FPSTR(configInfo);
  {
    uint8_t sta_cnt = wifi_softap_get_station_num();
    page.replace("{s}", String(softAP_ssid));
    page.replace("{b}", String(WiFi.softAPmacAddress()));
    // page.replace("{c}", String(WiFi.softAPchannel())); //WiFi.softAPIP().channel()));
    page.replace("{i}", WiFi.softAPIP().toString());
    page.replace("{a}", String(sta_cnt));
    if (sta_cnt) {
      page += String(F("\r\n<PRE>\r\n"));
      struct station_info *info = wifi_softap_get_station_info();
      IPAddress addr;
      while (info != NULL) {
        addr = info->ip;
        page += macToString(info->bssid) + F("  ") + addr.toString() + F("\r\n");
        info = STAILQ_NEXT(info, next);
      }
      page += F("</PRE>\r\n");
    }
  }

  /*
    Before we prepare a large block for sending, we call 'sendIfOver' with a
    threshold of 0 to force the sending of the current 'page' content.
  */
  sendIfOver(page, 0);

  if (WiFi.localIP().isSet()) {
    page += FPSTR(configInfo2);
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
    sendIfOver(page, 0);
  } else {
    page += F("<br /><h2>WLAN - offline</h2>");
  }

  page += FPSTR(configList);
  sendIfOver(page, 0);

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
      sendIfOver(page);
  }

  page += FPSTR(configEnd);
  page.replace("<p></p>", String(F("<p>MAX String memory used: ")) + (maxPage) + F("</p>"));
  server.sendContent(page);
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
