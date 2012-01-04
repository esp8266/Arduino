/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please clink on the following link
 * to open the download page in your default browser.
 *
 * http://firmata.org/wiki/Download
 */

/*
 Copyright (C) 2009 Jeff Hoefs.  All rights reserved.
 Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 See file LICENSE.txt for further informations on licensing terms.
 */

#include <Wire.h>
#include <Firmata.h>


#define I2C_WRITE B00000000
#define I2C_READ B00001000
#define I2C_READ_CONTINUOUSLY B00010000
#define I2C_STOP_READING B00011000
#define I2C_READ_WRITE_MODE_MASK B00011000

#define MAX_QUERIES 8

unsigned long currentMillis;     // store the current value from millis()
unsigned long previousMillis;    // for comparison with currentMillis
unsigned int samplingInterval = 32;  // default sampling interval is 33ms
unsigned int i2cReadDelayTime = 0;  // default delay time between i2c read request and Wire.requestFrom()
unsigned int powerPinsEnabled = 0;  // use as boolean to prevent enablePowerPins from being called more than once

#define MINIMUM_SAMPLING_INTERVAL 10

#define REGISTER_NOT_SPECIFIED -1

struct i2c_device_info {
  byte addr;
  byte reg;
  byte bytes;
};

i2c_device_info query[MAX_QUERIES];

byte i2cRxData[32];
boolean readingContinuously = false;
byte queryIndex = 0;

void readAndReportData(byte address, int theRegister, byte numBytes)
{
  if (theRegister != REGISTER_NOT_SPECIFIED) {
    Wire.beginTransmission(address);
    Wire.write((byte)theRegister);
    Wire.endTransmission();
    delayMicroseconds(i2cReadDelayTime);  // delay is necessary for some devices such as WiiNunchuck
  } 
  else {
    theRegister = 0;  // fill the register with a dummy value
  }

  Wire.requestFrom(address, numBytes);

  // check to be sure correct number of bytes were returned by slave
  if(numBytes == Wire.available()) {
    i2cRxData[0] = address;
    i2cRxData[1] = theRegister;
    for (int i = 0; i < numBytes; i++) {
      i2cRxData[2 + i] = Wire.read();
    }
    // send slave address, register and received bytes
    Firmata.sendSysex(I2C_REPLY, numBytes + 2, i2cRxData);
  }
  else {
    if(numBytes > Wire.available()) {
      Firmata.sendString("I2C Read Error: Too many bytes received");
    } else {
      Firmata.sendString("I2C Read Error: Too few bytes received"); 
    }
  }
  
}

void sysexCallback(byte command, byte argc, byte *argv)
{
  byte mode;
  byte slaveAddress;
  byte slaveRegister;
  byte data;
  int  delayTime;

  if (command == I2C_REQUEST) {
    mode = argv[1] & I2C_READ_WRITE_MODE_MASK;
    slaveAddress = argv[0];

    switch(mode) {
    case I2C_WRITE:
      Wire.beginTransmission(slaveAddress);
      for (byte i = 2; i < argc; i += 2) {
        data = argv[i] + (argv[i + 1] << 7);
        Wire.write(data);
      }
      Wire.endTransmission();
      delayMicroseconds(70); // TODO is this needed?
      break;
    case I2C_READ:
      if (argc == 6) {
        // a slave register is specified
        slaveRegister = argv[2] + (argv[3] << 7);
        data = argv[4] + (argv[5] << 7);  // bytes to read
        readAndReportData(slaveAddress, (int)slaveRegister, data);
      } 
      else {
        // a slave register is NOT specified
        data = argv[2] + (argv[3] << 7);  // bytes to read
        readAndReportData(slaveAddress, (int)REGISTER_NOT_SPECIFIED, data);
      }
      break;
    case I2C_READ_CONTINUOUSLY:
      if ((queryIndex + 1) >= MAX_QUERIES) {
        // too many queries, just ignore
        Firmata.sendString("too many queries");
        break;
      }
      query[queryIndex].addr = slaveAddress;
      query[queryIndex].reg = argv[2] + (argv[3] << 7);
      query[queryIndex].bytes = argv[4] + (argv[5] << 7);
      readingContinuously = true;
      queryIndex++;
      break;
    case I2C_STOP_READING:
      readingContinuously = false;
      queryIndex = 0;
      break;
    default:
      break;
    }
  }
  else if (command == SAMPLING_INTERVAL) {
    samplingInterval = argv[0] + (argv[1] << 7);

    if (samplingInterval < MINIMUM_SAMPLING_INTERVAL) {
      samplingInterval = MINIMUM_SAMPLING_INTERVAL;
    }

    samplingInterval -= 1;
    Firmata.sendString("sampling interval");
  }

  else if (command == I2C_CONFIG) {
    delayTime = (argv[4] + (argv[5] << 7));                        // MSB
    delayTime = (delayTime << 8) + (argv[2] + (argv[3] << 7));     // add LSB

    if((argv[0] + (argv[1] << 7)) > 0) {
      enablePowerPins(PORTC3, PORTC2);
    }

    if(delayTime > 0) {
      i2cReadDelayTime = delayTime;
    }

    if(argc > 6) {
      // If you extend I2C_Config, handle your data here
    }

  }
}

void systemResetCallback()
{
  readingContinuously = false;
  queryIndex = 0;
}

/* reference: BlinkM_funcs.h by Tod E. Kurt, ThingM, http://thingm.com/ */
// Enables Pins A2 and A3 to be used as GND and Power
// so that I2C devices can be plugged directly
// into Arduino header (pins A2 - A5)
static void enablePowerPins(byte pwrpin, byte gndpin)
{
  if(powerPinsEnabled == 0) {
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    powerPinsEnabled = 1;
    Firmata.sendString("Power pins enabled");
    delay(100);
  }
}

void setup()
{
  Firmata.setFirmwareVersion(2, 0);

  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  for (int i = 0; i < TOTAL_PINS; ++i) {
    pinMode(i, OUTPUT);
  }

  Firmata.begin(57600);  
  Wire.begin();
}

void loop()
{
  while (Firmata.available()) {
    Firmata.processInput();
  }

  currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    previousMillis += samplingInterval;

    for (byte i = 0; i < queryIndex; i++) {
      readAndReportData(query[i].addr, query[i].reg, query[i].bytes);
    }
  }
}
