/**
 * simple demo to show sha1 calculation
 */
#include <Arduino.h>
#include <Hash.h>

void setup() {
    Serial.begin(921600);
}

void loop() {
    uint8_t hash[20];
    const uint8_t test[] = "test";

    sha1((uint8_t *)&test[0], sizeof(test)-1, &hash[0]);

    // SHA1: A94A8FE5CCB19BA61C4C0873D391E987982FBBD3
    Serial.print("SHA1:");
    for(uint16_t i = 0; i < 20; i++) {
        Serial.printf("%02X", hash[i]);
    }
    Serial.println();

    delay(1000);
}

