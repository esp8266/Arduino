// Wire Slave Receiver
// by devyte
// based on the example by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// This example code is in the public domain.

#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE  = 0x08;

void          setup()
{
  Serial.begin(115200);  // start serial for output

  Wire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE);  // new syntax: join i2c bus (address required for slave)
  Wire.onReceive(receiveEvent);             // register event
}

void loop()
{
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(size_t howMany)
{
  (void)howMany;
  while (1 < Wire.available())
  {                        // loop through all but the last
    char c = Wire.read();  // receive byte as a character
    Serial.print(c);       // print the character
  }
  int x = Wire.read();  // receive byte as an integer
  Serial.println(x);    // print the integer
}
