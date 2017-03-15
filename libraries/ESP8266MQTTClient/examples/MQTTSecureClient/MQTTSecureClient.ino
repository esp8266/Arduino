#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>
MQTTClient mqtt;
String fingerprint = "7E 36 22 01 F9 7E 99 2F C5 DB 3D BE AC 48 67 5B 5D 47 94 D2";

void setup() {
    Serial.begin(115200);
    //  WiFi.begin("ssid", "pass");

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    mqtt.onSecure([](WiFiClientSecure & client, String host) {
        Serial.printf("Verify: %s\r\n", host.c_str());
        return client.verify(fingerprint.c_str(), host.c_str());
    });
    
    //topic, data, data is continuing
    mqtt.onData([](String topic, String data, bool cont) {
        Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
        mqtt.unSubscribe("/qos0");
    });

    mqtt.onSubscribe([](int sub_id) {
        Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
        mqtt.publish("/qos0", "qos0", 0, 0);
    });
    mqtt.onConnect([]() {
        Serial.printf("MQTT: Connected\r\n");
        mqtt.subscribe("/qos0", 0);
    });

    mqtt.begin("mqtts://test.mosquitto.org:8883");
    //mqtt.begin("mqtts://test.mosquitto.org:8883", {.lwtTopic = "hello", .lwtMsg = "offline", .lwtQos = 0, .lwtRetain = 0});
    //mqtt.begin("mqtts://user:pass@mosquito.org:8883");
    //mqtt.begin("mqtts://user:pass@mosquito.org:8883#clientId");

}

void loop() {
    mqtt.handle();
}
