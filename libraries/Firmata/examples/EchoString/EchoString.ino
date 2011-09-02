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
    Firmata.sendSysex(command, argc, argv);
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


