/*
  SCP1000 Barometric Pressure Sensor Display
 
 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/
 
 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip
 
 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11    MOSI, MISO, SCK pins are different depending on the 
 MISO: pin 12    board type. See your board's documentation for details
 SCK: pin 13

 created 31 July 2010
 by Tom Igoe
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

//Sensor's memory register addresses:
const int PRESSURE = 0x1F;      //3 most significant bits of pressure
const int PRESSURE_LSB = 0x20;  //16 least significant bits of pressure
const int TEMPERATURE = 0x21;   //16 bit temperature reading

// pins used for the connection with the sensor
// the other you need are controlled by the SPI library):
const int dataReadyPin = 6; 
const int chipSelectPin = 7;

void setup() {
  Serial.begin(9600);

  // start the SPI library:
  SPI.begin();

  // initalize the  data ready and chip select pins:
  pinMode(dataReadyPin, INPUT);
  pinMode(chipSelectPin, OUTPUT);

  //Configure SCP1000 for low noise configuration:
  writeRegister(0x02, 0x2D);
  writeRegister(0x01, 0x03);
  writeRegister(0x03, 0x02);
  // give the sensor time to set up:
  delay(100);
}

void loop() {
  //Select High Resolution Mode
  writeRegister(0x03, 0x0A);

  // don't do anything until the data ready pin is high:
  if (digitalRead(dataReadyPin) == HIGH) {
    //Read the temperature data
    int tempData = readRegister(0x21, 2);

    // convert the temperature to celsius and display it:
    float realTemp = (float)tempData / 20.0;
    Serial.print("Temp[C]=");
    Serial.print(realTemp);


    //Read the pressure data highest 3 bits:
    byte  pressure_data_high = readRegister(0x1F, 1);   
    pressure_data_high &= 0b00000111; //you only needs bits 2 to 0

    //Read the pressure data lower 16 bits:
    unsigned int pressure_data_low = readRegister(0x20, 2);    
    //combine the two parts into one 19-bit number:
    long pressure = ((pressure_data_high << 16) | pressure_data_low)/4;

    // display the temperature:
    Serial.println("\tPressure [Pa]=" + String(pressure));
  }
}

//Sends a write command to SCP1000

void writeRegister(byte registerName, byte registerValue) {
  // SCP1000 expects the reguster name in the upper 6 bits
  // of the byte:
  registerName <<= 2;
  // command goes in the lower two bits:
  registerName |= 0b00000010; //Write command

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW); 

  SPI.transfer(registerName); //Send register location
  SPI.transfer(registerValue); //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH); //Select SPI device
}


//Read register from the SCP1000:
unsigned int readRegister(byte registerName, int numBytes) {
  byte inByte = 0;           // incoming from  the SPI read
  unsigned int result = 0;   // result to return 

  // SCP1000 expects the reguster name in the upper 6 bits
  // of the byte:
  registerName <<= 2;
  // command goes in the lower two bits:
  registerName &= 0b11111100; //Read command

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW); 
  // send the device the register you want to read:
  int command = SPI.transfer(registerName); 
  // send a value of 0 to read the forst byte returned:
  inByte = SPI.transfer(0x00); 

  // if there's more than one byte returned, 
  // shift the first byte then get the second byte:
  switch(numBytes) {
  case 1:
    result = inByte;
    break;
  case 2: 
    result = inByte << 8;
    inByte = SPI.transfer(0x00); 
    result = result |inByte;
    break;
  }
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH); 
  // return the result:
  return(result);
}












