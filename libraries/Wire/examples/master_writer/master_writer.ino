// Wire Master Writer
// by devyte
// based on the example of the same name by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// This example code is in the public domain.

#include <Wire.h>
#include <PolledTimeout.h>

#define SDA_PIN 4
#define SCL_PIN 5
const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER); // join i2c bus (address optional for master)
}

byte x = 0;

void loop() {
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(1000);

  if (nextPing) {
    Wire.beginTransmission(I2C_SLAVE); // transmit to device #8
    Wire.write("x is "); // sends five bytes
    Wire.write(x); // sends one byte
    Wire.endTransmission(); // stop transmitting

    x++;
  }
}
