
// used by ../../test.sh

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>

#define FILE "/randfile"
#define URL "http://127.0.0.1:9080/edit"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

WiFiClient client;

void setup() {
    Serial.begin(115200);
    Serial.println("starting...");
    WiFi.begin(STASSID, STAPSK);
    while (!WiFi.isConnected()) {
        Serial.println(".");
        delay(250);
    }

    if (!SPIFFS.begin())
        Serial.println("CLIENT: failed to start SPIFFS!");

    auto f = SPIFFS.open(FILE, "r");
    const auto f_size = f.size();
    Serial.printf("CLIENT: size to upload: %d\n", (int)f_size);

    Serial.println("CLIENT: connecting to server...");
    if (!client.connect("127.0.0.1", 9080)) {
        Serial.println("\nfailed to connect\n");
        return;
    }
    
    client.println("POST /edit HTTP/1.1");
    client.println("Host: 127.0.0.1");
    client.println("Connection: close");
    client.println("Content-Type: multipart/form-data; boundary=glark");
    client.printf("Content-Length: %d\r\n", f_size);
    client.println();
    client.println("--glark");
    client.printf("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n", FILE "copy", FILE "copy");
    client.println("Content-Type: application/octet-stream");
    client.println("");


    Serial.printf("\r\n\r\n");
    Serial.printf("\r\n\r\n##########################\r\n");


#define TESTSTREAM 0

printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

    auto A = millis();
    Serial.printf("\r\n\r\nCLIENT: ----> upload duration: %lums  sent: %d\r\n\r\n",
        millis() - A,
#if TESTSTREAM
        (int)f.streamTo(client, 0)
#else
        (int)client.write(f)
#endif
        );
    A = millis() - A;

printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

    Serial.println("CLIENT waiting for server ack...");
    client.println("\r\n--glark--");
    while (!client.available())
         yield();

    Serial.println("@@@@@@@@@@@@@ CLIENT: server response:");
#if TESTSTREAM
    client.setTimeout(10000);
    client.streamTo(Serial, 0);
#else
    while (client.available())
        Serial.write(client.read());
#endif

    Serial.println("@@@@@@@@@@@@@ CLIENT: end");
    client.stop();
    f.close();
}

void loop ()
{
}
