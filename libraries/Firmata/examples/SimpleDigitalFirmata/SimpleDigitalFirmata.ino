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

/* Supports as many digital inputs and outputs as possible.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>

byte previousPIN[TOTAL_PORTS];  // PIN means PORT for input
byte previousPORT[TOTAL_PORTS]; 

void outputPort(byte portNumber, byte portValue)
{
    // only send the data when it changes, otherwise you get too many messages!
    if (previousPIN[portNumber] != portValue) {
        Firmata.sendDigitalPort(portNumber, portValue); 
        previousPIN[portNumber] = portValue;
    }
}

void setPinModeCallback(byte pin, int mode) {
    if (IS_PIN_DIGITAL(pin)) {
        pinMode(PIN_TO_DIGITAL(pin), mode);
    }
}

void digitalWriteCallback(byte port, int value)
{
    byte i;
    byte currentPinValue, previousPinValue;

    if (port < TOTAL_PORTS && value != previousPORT[port]) {
        for(i=0; i<8; i++) {
            currentPinValue = (byte) value & (1 << i);
            previousPinValue = previousPORT[port] & (1 << i);
            if(currentPinValue != previousPinValue) {
                digitalWrite(i + (port*8), currentPinValue);
            }
        }
        previousPORT[port] = value;
    }
}

void setup()
{
    Firmata.setFirmwareVersion(0, 1);
    Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
    Firmata.attach(SET_PIN_MODE, setPinModeCallback);
    Firmata.begin(57600);
}

void loop()
{
    byte i;

    for (i=0; i<TOTAL_PORTS; i++) {
        outputPort(i, readPort(i, 0xff));
    }

    while(Firmata.available()) {
        Firmata.processInput();
    }
}
