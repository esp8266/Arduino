/* This sketch accepts strings and raw sysex messages and echos them back.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>

byte analogPin;

void stringCallback(char *myString)
{
    Firmata.sendString(myString);
}


void sysexCallback(byte command, byte argc, byte*argv)
{
    Serial.print(START_SYSEX, BYTE);
    Serial.print(command, BYTE);
    for(byte i=0; i<argc; i++) {
        Serial.print(argv[i], BYTE);
    }
    Serial.print(END_SYSEX, BYTE);
}

void setup()
{
    Firmata.setFirmwareVersion(0, 1);
    Firmata.attach(STRING_DATA, stringCallback);
    Firmata.attach(START_SYSEX, sysexCallback);
    Firmata.begin(57600);
}

void loop()
{
    while(Firmata.available()) {
        Firmata.processInput();
    }
}


