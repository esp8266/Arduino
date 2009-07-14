/* Supports as many digital inputs and outputs as possible.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>

byte previousPIN[2];  // PIN means PORT for input
byte previousPORT[2]; 

void outputPort(byte portNumber, byte portValue)
{
// only send the data when it changes, otherwise you get too many messages!
    if(previousPIN[portNumber] != portValue) {
        Firmata.sendDigitalPort(portNumber, portValue); 
        previousPIN[portNumber] = portValue;
        Firmata.sendDigitalPort(portNumber, portValue); 
    }
}

void setPinModeCallback(byte pin, int mode) {
    if(pin > 1) { // don't touch RxTx pins (0,1)
        pinMode(pin, mode);
    }
}

void digitalWriteCallback(byte port, int value)
{
    byte i;
    byte currentPinValue, previousPinValue;

    if(value != previousPORT[port]) {
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
    outputPort(0, PIND &~ B00000011); // pins 0-7, ignoring Rx/Tx pins (0/1)
    outputPort(1, PINB); // pins 8-13
    while(Firmata.available()) {
        Firmata.processInput();
    }
}
