/**
 * simple demo to show sha1 calculation
 */
#include <Arduino.h>
#include <Hash.h>

void setup() {
    Serial.begin(921600);
}

void loop() {

    // usage as String
    // SHA1:a94a8fe5ccb19ba61c4c0873d391e987982fbbd3

    Serial.print("SHA1:");
    Serial.println(sha1("abc"));

    // usage as ptr
    // SHA1:a94a8fe5ccb19ba61c4c0873d391e987982fbbd3
    uint8_t hash[20];
    sha1("abc", &hash[0]);

    Serial.print("SHA1:");
    for(uint16_t i = 0; i < 20; i++) {
        Serial.printf("%02x", hash[i]);
    }
    Serial.println();

    delay(1000);
}

