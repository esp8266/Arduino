/* ESP8266 I2C master-slave communication, requires i2c_slave by bjoham */

#include <Wire.h>

int scan(uint16_t) {
    byte error, address;
    int nDevices;
    int aDevice = 0;
    
    Serial.println("Scanning...");
    
    nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println(" !");            
            nDevices++;            
            aDevice = address;
            
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
    
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("Done.\n");
    }
    
    return aDevice;
}
