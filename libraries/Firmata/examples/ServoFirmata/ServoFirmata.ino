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

/* This firmware supports as many servos as possible using the Servo library 
 * included in Arduino 0017
 *
 * TODO add message to configure minPulse/maxPulse/degrees
 *
 * This example code is in the public domain.
 */
 
#include <Servo.h>
#include <Firmata.h>

Servo servos[MAX_SERVOS];

void analogWriteCallback(byte pin, int value)
{
    if (IS_PIN_SERVO(pin)) {
        servos[PIN_TO_SERVO(pin)].write(value);
    }
}

void setup() 
{
    byte pin;

    Firmata.setFirmwareVersion(0, 2);
    Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);

    for (pin=0; pin < TOTAL_PINS; pin++) {
        if (IS_PIN_SERVO(pin)) {
	    servos[PIN_TO_SERVO(pin)].attach(PIN_TO_DIGITAL(pin));
        }
    }
   
    Firmata.begin(57600);
}

void loop() 
{
    while(Firmata.available())
        Firmata.processInput();
}

