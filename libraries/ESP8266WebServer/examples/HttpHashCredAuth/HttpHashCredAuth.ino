/*
  HTTP Hashed Credential example
  Created April 27, 2019 by Tyler Moore.
  This example code is in the public domain.

  This is a simple Arduino example to demonstrate a few simple techniques:
  1. Creating a secure web server using ESP8266ESP8266WebServerSecure
  2. Use of HTTP authentication on this secure server
  3. A simple web interface to allow an authenticated user to change Credentials
  4. Persisting those credentials through a reboot of the ESP by saving them to LittleFS without storing them as plain text
*/

#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>

//Unfortunately it is not possible to have persistent WiFi credentials stored as anything but plain text. Obfuscation would be the only feasible barrier.
#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* wifi_pw = STAPSK;

const String file_credentials = R"(/credentials.txt)"; // LittleFS file name for the saved credentials
const String change_creds = "changecreds"; // Address for a credential change

//The ESP8266WebServerSecure requires an encryption certificate and matching key.
//These can generated with the bash script available in the ESP8266 Arduino repository.
//These values can be used for testing but are available publicly so should not be used in production.
static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSzCCAjMCCQD2ahcfZAwXxDANBgkqhkiG9w0BAQsFADCBiTELMAkGA1UEBhMC
VVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU9yYW5nZSBDb3VudHkx
EDAOBgNVBAoMB1ByaXZhZG8xGjAYBgNVBAMMEXNlcnZlci56bGFiZWwuY29tMR8w
HQYJKoZIhvcNAQkBFhBlYXJsZUB6bGFiZWwuY29tMB4XDTE4MDMwNjA1NDg0NFoX
DTE5MDMwNjA1NDg0NFowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3Rh
dGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAPVKBwbZ+KDSl40YCDkP6y8Sv4iNGvEOZg8Y
X7sGvf/xZH7UiCBWPFIRpNmDSaZ3yjsmFqm6sLiYSGSdrBCFqdt9NTp2r7hga6Sj
oASSZY4B9pf+GblDy5m10KDx90BFKXdPMCLT+o76Nx9PpCvw13A848wHNG3bpBgI
t+w/vJCX3bkRn8yEYAU6GdMbYe7v446hX3kY5UmgeJFr9xz1kq6AzYrMt/UHhNzO
S+QckJaY0OGWvmTNspY3xCbbFtIDkCdBS8CZAw+itnofvnWWKQEXlt6otPh5njwy
+O1t/Q+Z7OMDYQaH02IQx3188/kW3FzOY32knER1uzjmRO+jhA8CAwEAATANBgkq
hkiG9w0BAQsFAAOCAQEAnDrROGRETB0woIcI1+acY1yRq4yAcH2/hdq2MoM+DCyM
E8CJaOznGR9ND0ImWpTZqomHOUkOBpvu7u315blQZcLbL1LfHJGRTCHVhvVrcyEb
fWTnRtAQdlirUm/obwXIitoz64VSbIVzcqqfg9C6ZREB9JbEX98/9Wp2gVY+31oC
JfUvYadSYxh3nblvA4OL+iEZiW8NE3hbW6WPXxvS7Euge0uWMPc4uEcnsE0ZVG3m
+TGimzSdeWDvGBRWZHXczC2zD4aoE5vrl+GD2i++c6yjL/otHfYyUpzUfbI2hMAA
5tAF1D5vAAwA8nfPysumlLsIjohJZo4lgnhB++AlOg==
-----END CERTIFICATE-----
)EOF";
static const char serverKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA9UoHBtn4oNKXjRgIOQ/rLxK/iI0a8Q5mDxhfuwa9//FkftSI
IFY8UhGk2YNJpnfKOyYWqbqwuJhIZJ2sEIWp2301OnavuGBrpKOgBJJljgH2l/4Z
uUPLmbXQoPH3QEUpd08wItP6jvo3H0+kK/DXcDzjzAc0bdukGAi37D+8kJfduRGf
zIRgBToZ0xth7u/jjqFfeRjlSaB4kWv3HPWSroDNisy39QeE3M5L5ByQlpjQ4Za+
ZM2yljfEJtsW0gOQJ0FLwJkDD6K2eh++dZYpAReW3qi0+HmePDL47W39D5ns4wNh
BofTYhDHfXzz+RbcXM5jfaScRHW7OOZE76OEDwIDAQABAoIBAQDKov5NFbNFQNR8
djcM1O7Is6dRaqiwLeH4ZH1pZ3d9QnFwKanPdQ5eCj9yhfhJMrr5xEyCqT0nMn7T
yEIGYDXjontfsf8WxWkH2TjvrfWBrHOIOx4LJEvFzyLsYxiMmtZXvy6YByD+Dw2M
q2GH/24rRdI2klkozIOyazluTXU8yOsSGxHr/aOa9/sZISgLmaGOOuKI/3Zqjdhr
eHeSqoQFt3xXa8jw01YubQUDw/4cv9rk2ytTdAoQUimiKtgtjsggpP1LTq4xcuqN
d4jWhTcnorWpbD2cVLxrEbnSR3VuBCJEZv5axg5ZPxLEnlcId8vMtvTRb5nzzszn
geYUWDPhAoGBAPyKVNqqwQl44oIeiuRM2FYenMt4voVaz3ExJX2JysrG0jtCPv+Y
84R6Cv3nfITz3EZDWp5sW3OwoGr77lF7Tv9tD6BptEmgBeuca3SHIdhG2MR+tLyx
/tkIAarxQcTGsZaSqra3gXOJCMz9h2P5dxpdU+0yeMmOEnAqgQ8qtNBfAoGBAPim
RAtnrd0WSlCgqVGYFCvDh1kD5QTNbZc+1PcBHbVV45EmJ2fLXnlDeplIZJdYxmzu
DMOxZBYgfeLY9exje00eZJNSj/csjJQqiRftrbvYY7m5njX1kM5K8x4HlynQTDkg
rtKO0YZJxxmjRTbFGMegh1SLlFLRIMtehNhOgipRAoGBAPnEEpJGCS9GGLfaX0HW
YqwiEK8Il12q57mqgsq7ag7NPwWOymHesxHV5mMh/Dw+NyBi4xAGWRh9mtrUmeqK
iyICik773Gxo0RIqnPgd4jJWN3N3YWeynzulOIkJnSNx5BforOCTc3uCD2s2YB5X
jx1LKoNQxLeLRN8cmpIWicf/AoGBANjRSsZTKwV9WWIDJoHyxav/vPb+8WYFp8lZ
zaRxQbGM6nn4NiZI7OF62N3uhWB/1c7IqTK/bVHqFTuJCrCNcsgld3gLZ2QWYaMV
kCPgaj1BjHw4AmB0+EcajfKilcqtSroJ6MfMJ6IclVOizkjbByeTsE4lxDmPCDSt
/9MKanBxAoGAY9xo741Pn9WUxDyRplww606ccdNf/ksHWNc/Y2B5SPwxxSnIq8nO
j01SmsCUYVFAgZVOTiiycakjYLzxlc6p8BxSVqy6LlJqn95N8OXoQ+bkwUux/ekg
gz5JWYhbD6c38khSzJb0pNXCo3EuYAVa36kDM96k1BtWuhRS10Q1VXk=
-----END RSA PRIVATE KEY-----
)EOF";

ESP8266WebServerSecure server(443);

//These are temporary credentials that will only be used if none are found saved in LittleFS.
String login = "admin";
const String realm = "global";
String H1 = "";
String authentication_failed = "User authentication has failed.";

void setup() {
  Serial.begin(115200);

  //Initialize LittleFS to save credentials
  if (!LittleFS.begin()) {
    Serial.println("LittleFS initialization error, programmer flash configured?");
    ESP.restart();
  }

  //Attempt to load credentials. If the file does not yet exist, they will be set to the default values above
  loadcredentials();

  //Initialize wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_pw);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }

  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.on("/", showcredentialpage); //for this simple example, just show a simple page for changing credentials at the root
  server.on("/" + change_creds, handlecredentialchange); //handles submission of credentials from the client
  server.onNotFound(redirect);
  server.begin();

  Serial.print("Open https://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop() {
  yield();
  server.handleClient();
}

//This function redirects home
void redirect() {
  String url = "https://" + WiFi.localIP().toString();
  Serial.println("Redirect called. Redirecting to " + url);
  server.sendHeader("Location", url, true);
  Serial.println("Header sent.");
  server.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  Serial.println("Empty page sent.");
  server.client().stop(); // Stop is needed because we sent no content length
  Serial.println("Client stopped.");
}

//This function checks whether the current session has been authenticated. If not, a request for credentials is sent.
bool session_authenticated() {
  Serial.println("Checking authentication.");
  if (server.authenticateDigest(login, H1)) {
    Serial.println("Authentication confirmed.");
    return true;
  } else {
    Serial.println("Not authenticated. Requesting credentials.");
    server.requestAuthentication(DIGEST_AUTH, realm.c_str(), authentication_failed);
    redirect();
    return false;
  }
}

//This function sends a simple webpage for changing login credentials to the client
void showcredentialpage() {
  Serial.println("Show credential page called.");
  if (!session_authenticated()) {
    return;
  }

  Serial.println("Forming credential modification page.");

  String page;
  page = R"(<html>)";

  page +=
      R"(
  <h2>Login Credentials</h2><br>

  <form action=")"
      + change_creds + R"(" method="post">
  Login:<br>
  <input type="text" name="login"><br>
  Password:<br>
  <input type="password" name="password"><br>
  Confirm Password:<br>
  <input type="password" name="password_duplicate"><br>
  <p><button type="submit" name="newcredentials">Change Credentials</button></p>
  </form><br>
  )";

  page += R"(</html>)";

  Serial.println("Sending credential modification page.");

  server.send(200, "text/html", page);
}

//Saves credentials to LittleFS
void savecredentials(String new_login, String new_password) {
  //Set global variables to new values
  login = new_login;
  H1 = ESP8266WebServer::credentialHash(new_login, realm, new_password);

  //Save new values to LittleFS for loading on next reboot
  Serial.println("Saving credentials.");
  File f = LittleFS.open(file_credentials, "w"); //open as a brand new file, discard old contents
  if (f) {
    Serial.println("Modifying credentials in file system.");
    f.println(login);
    f.println(H1);
    Serial.println("Credentials written.");
    f.close();
    Serial.println("File closed.");
  }
  Serial.println("Credentials saved.");
}

//loads credentials from LittleFS
void loadcredentials() {
  Serial.println("Searching for credentials.");
  File f;
  f = LittleFS.open(file_credentials, "r");
  if (f) {
    Serial.println("Loading credentials from file system.");
    String mod = f.readString(); //read the file to a String
    int index_1 = mod.indexOf('\n', 0); //locate the first line break
    int index_2 = mod.indexOf('\n', index_1 + 1); //locate the second line break
    login = mod.substring(0, index_1 - 1); //get the first line (excluding the line break)
    H1 = mod.substring(index_1 + 1, index_2 - 1); //get the second line (excluding the line break)
    f.close();
  } else {
    String default_login = "admin";
    String default_password = "changeme";
    Serial.println("None found. Setting to default credentials.");
    Serial.println("user:" + default_login);
    Serial.println("password:" + default_password);
    login = default_login;
    H1 = ESP8266WebServer::credentialHash(default_login, realm, default_password);
  }
}

//This function handles a credential change from a client.
void handlecredentialchange() {
  Serial.println("Handle credential change called.");
  if (!session_authenticated()) {
    return;
  }

  Serial.println("Handling credential change request from client.");

  String login = server.arg("login");
  String pw1 = server.arg("password");
  String pw2 = server.arg("password_duplicate");

  if (login != "" && pw1 != "" && pw1 == pw2) {

    savecredentials(login, pw1);
    server.send(200, "text/plain", "Credentials updated");
    redirect();
  } else {
    server.send(200, "text/plain", "Malformed credentials");
    redirect();
  }
}
