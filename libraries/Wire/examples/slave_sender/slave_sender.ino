// Wire Slave Sender
// by devyte
// based on the example of the same name by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// This example code is in the public domain.

#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5
const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE  = 0x08;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE);  // join i2c bus with address #8
  Wire.onRequest(requestEvent);             // register event
}

void loop() {
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write("hello\n");  // respond with message of 6 bytes
  // as expected by master
}
