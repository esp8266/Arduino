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
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
 */

/*
 * This is an old version of StandardFirmata (v2.0).  It is kept here because
 * its the last version that works on an ATMEGA8 chip.  Also, it can be used
 * for host software that has not been updated to a newer version of the
 * protocol.  It also uses the old baud rate of 115200 rather than 57600.
 */

#include <EEPROM.h>
#include <Firmata.h>

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* analog inputs */
int analogInputsToReport = 0; // bitwise array to store pin reporting
int analogPin = 0; // counter for reading analog pins

/* digital pins */
byte reportPINs[TOTAL_PORTS];   // PIN == input port
byte previousPINs[TOTAL_PORTS]; // PIN == input port
byte pinStatus[TOTAL_PINS]; // store pin status, default OUTPUT
byte portStatus[TOTAL_PORTS];

/* timer variables */
unsigned long currentMillis;     // store the current value from millis()
unsigned long previousMillis;    // for comparison with currentMillis


/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void outputPort(byte portNumber, byte portValue)
{
  portValue = portValue &~ portStatus[portNumber];
  if (previousPINs[portNumber] != portValue) {
    Firmata.sendDigitalPort(portNumber, portValue);
    previousPINs[portNumber] = portValue;
    Firmata.sendDigitalPort(portNumber, portValue);
  }
}

/* -----------------------------------------------------------------------------
 * check all the active digital inputs for change of state, then add any events
 * to the Serial output queue using Serial.print() */
void checkDigitalInputs(void)
{
  byte i, tmp;
  for (i = 0; i < TOTAL_PORTS; i++) {
    if (reportPINs[i]) {
      switch (i) {
        case 0: outputPort(0, PIND &~ B00000011); break; // ignore Rx/Tx 0/1
        case 1: outputPort(1, PINB); break;
        case 2: outputPort(2, PINC); break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
/* sets the pin mode to the correct state and sets the relevant bits in the
 * two bit-arrays that track Digital I/O and PWM status
 */
void setPinModeCallback(byte pin, int mode) {
  byte port = 0;
  byte offset = 0;

  if (pin < 8) {
    port = 0;
    offset = 0;
  } else if (pin < 14) {
    port = 1;
    offset = 8;
  } else if (pin < 22) {
    port = 2;
    offset = 14;
  }

  if (pin > 1) { // ignore RxTx (pins 0 and 1)
    pinStatus[pin] = mode;
    switch (mode) {
      case INPUT:
        pinMode(pin, INPUT);
        portStatus[port] = portStatus[port] &~ (1 << (pin - offset));
        break;
      case OUTPUT:
        digitalWrite(pin, LOW); // disable PWM
      case PWM:
        pinMode(pin, OUTPUT);
        portStatus[port] = portStatus[port] | (1 << (pin - offset));
        break;
        //case ANALOG: // TODO figure this out
      default:
        Firmata.sendString("");
    }
    // TODO: save status to EEPROM here, if changed
  }
}

void analogWriteCallback(byte pin, int value)
{
  setPinModeCallback(pin, PWM);
  analogWrite(pin, value);
}

void digitalWriteCallback(byte port, int value)
{
  switch (port) {
    case 0: // pins 2-7 (don't change Rx/Tx, pins 0 and 1)
      // 0xFF03 == B1111111100000011    0x03 == B00000011
      PORTD = (value &~ 0xFF03) | (PORTD & 0x03);
      break;
    case 1: // pins 8-13 (14,15 are disabled for the crystal)
      PORTB = (byte)value;
      break;
    case 2: // analog pins used as digital
      PORTC = (byte)value;
      break;
  }
}

// -----------------------------------------------------------------------------
/* sets bits in a bit array (int) to toggle the reporting of the analogIns
 */
//void FirmataClass::setAnalogPinReporting(byte pin, byte state) {
//}
void reportAnalogCallback(byte pin, int value)
{
  if (value == 0) {
    analogInputsToReport = analogInputsToReport &~ (1 << pin);
  }
  else { // everything but 0 enables reporting of that pin
    analogInputsToReport = analogInputsToReport | (1 << pin);
  }
  // TODO: save status to EEPROM here, if changed
}

void reportDigitalCallback(byte port, int value)
{
  reportPINs[port] = (byte)value;
  if (port == 2) // turn off analog reporting when used as digital
    analogInputsToReport = 0;
}

/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup()
{
  byte i;

  Firmata.setFirmwareVersion(2, 0);

  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
  Firmata.attach(REPORT_ANALOG, reportAnalogCallback);
  Firmata.attach(REPORT_DIGITAL, reportDigitalCallback);
  Firmata.attach(SET_PIN_MODE, setPinModeCallback);

  portStatus[0] = B00000011;  // ignore Tx/RX pins
  portStatus[1] = B11000000;  // ignore 14/15 pins
  portStatus[2] = B00000000;

  //    for(i=0; i<TOTAL_PINS; ++i) { // TODO make this work with analogs
  for (i = 0; i < 14; ++i) {
    setPinModeCallback(i, OUTPUT);
  }
  // set all outputs to 0 to make sure internal pull-up resistors are off
  PORTB = 0; // pins 8-15
  PORTC = 0; // analog port
  PORTD = 0; // pins 0-7

  // TODO rethink the init, perhaps it should report analog on default
  for (i = 0; i < TOTAL_PORTS; ++i) {
    reportPINs[i] = false;
  }
  // TODO: load state from EEPROM here

  /* send digital inputs here, if enabled, to set the initial state on the
   * host computer, since once in the loop(), this firmware will only send
   * digital data on change. */
  if (reportPINs[0]) outputPort(0, PIND &~ B00000011); // ignore Rx/Tx 0/1
  if (reportPINs[1]) outputPort(1, PINB);
  if (reportPINs[2]) outputPort(2, PINC);

  Firmata.begin(115200);
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  /* DIGITALREAD - as fast as possible, check for changes and output them to the
   * FTDI buffer using Serial.print()  */
  checkDigitalInputs();
  currentMillis = millis();
  if (currentMillis - previousMillis > 20) {
    previousMillis += 20;     // run this every 20ms
    /* SERIALREAD - Serial.read() uses a 128 byte circular buffer, so handle
     * all serialReads at once, i.e. empty the buffer */
    while (Firmata.available())
      Firmata.processInput();
    /* SEND FTDI WRITE BUFFER - make sure that the FTDI buffer doesn't go over
     * 60 bytes. use a timer to sending an event character every 4 ms to
     * trigger the buffer to dump. */

    /* ANALOGREAD - right after the event character, do all of the
     * analogReads().  These only need to be done every 4ms. */
    for (analogPin = 0; analogPin < TOTAL_ANALOG_PINS; analogPin++) {
      if ( analogInputsToReport & (1 << analogPin) ) {
        Firmata.sendAnalog(analogPin, analogRead(analogPin));
      }
    }
  }
}
