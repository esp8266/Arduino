// Wire Master Reader
// by devyte
// based on the example of the same name by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// This example code is in the public domain.


#include <Wire.h>
#include <PolledTimeout.h>

#define SDA_PIN 4
#define SCL_PIN 5
const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;

void setup() {
  Serial.begin(115200);                      // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER);  // join i2c bus (address optional for master)
}

void loop() {
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(1000);

  if (nextPing) {
    Wire.requestFrom(I2C_SLAVE, 6);  // request 6 bytes from slave device #8

    while (Wire.available()) {  // slave may send less than requested
      char c = Wire.read();     // receive a byte as character
      Serial.print(c);          // print the character
    }
  }
}
