
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

WiFiUDP udp;
const char * ssid = "fyffest";
const char * pass = "wellcometrust";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  Serial.println("Booting...");
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str() );

  // Bind a callback function.  
  // warning: handler is called from tcp stack context
  // esp_yield and non-reentrant functions which depend on it will fail.
  udp.onRx( []() {
    size_t len = udp.available();
    std::unique_ptr<char[]> buf(new char[len + 1]);
    udp.read(buf.get(), len);
    buf[len] = '\0';
    Serial.printf("Packet Recieved: len = %u, data = %s\n", len, buf.get());
  });

  udp.begin(9000);

}

void loop() {

}
