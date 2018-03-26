/*
    This sketch shows how to handle HTTP Digest Authorization.

    Written by Parham Alvani and Sajjad Rahnama, 2018-01-07.

   This example is released into public domain,
   or, at your option, CC0 licensed.
*/

#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>

const char* ssid = "........";
const char* ssidPassword = "........";

const char *username = "admin";
const char *password = "admin";

const char *server = "http://httpbin.org";
const char *uri = "/digest-auth/auth/admin/admin/MD5";

String exractParam(String& authReq, const String& param, const char delimit) {
  int _begin = authReq.indexOf(param);
  if (_begin == -1) {
    return "";
  }
  return authReq.substring(_begin + param.length(), authReq.indexOf(delimit, _begin + param.length()));
}

String getCNonce(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  String s = "";

  for (int i = 0; i < len; ++i) {
    s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return s;
}

String getDigestAuth(String& authReq, const String& username, const String& password, const String& uri, unsigned int counter) {
  // extracting required parameters for RFC 2069 simpler Digest
  String realm = exractParam(authReq, "realm=\"", '"');
  String nonce = exractParam(authReq, "nonce=\"", '"');
  String cNonce = getCNonce(8);

  char nc[9];
  snprintf(nc, sizeof(nc), "%08x", counter);

  // parameters for the RFC 2617 newer Digest
  MD5Builder md5;
  md5.begin();
  md5.add(username + ":" + realm + ":" + password);  // md5 of the user:realm:user
  md5.calculate();
  String h1 = md5.toString();

  md5.begin();
  md5.add(String("GET:") + uri);
  md5.calculate();
  String h2 = md5.toString();

  md5.begin();
  md5.add(h1 + ":" + nonce + ":" + String(nc) + ":" + cNonce + ":" + "auth" + ":" + h2);
  md5.calculate();
  String response = md5.toString();

  String authorization = "Digest username=\"" + username + "\", realm=\"" + realm + "\", nonce=\"" + nonce +
                         "\", uri=\"" + uri + "\", algorithm=\"MD5\", qop=auth, nc=" + String(nc) + ", cnonce=\"" + cNonce + "\", response=\"" + response + "\"";
  Serial.println(authorization);

  return authorization;
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, ssidPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  // configure traged server and url
  http.begin(String(server) + String(uri));


  const char *keys[] = {"WWW-Authenticate"};
  http.collectHeaders(keys, 1);

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  if (httpCode > 0) {
    String authReq = http.header("WWW-Authenticate");
    Serial.println(authReq);

    String authorization = getDigestAuth(authReq, String(username), String(password), String(uri), 1);

    http.end();
    http.begin(String(server) + String(uri));

    http.addHeader("Authorization", authorization);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  delay(10000);
}
