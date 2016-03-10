/*
  Servo.h - Interrupt driven Servo library for Esp8266 using timers
  Copyright (c) 2015 Michael C. Miller. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  */


//   A servo is activated by creating an instance of the Servo class passing
//   the desired pin to the attach() method.
//   The servos are pulsed in the background using the value most recently
//   written using the write() method.
//
//   This library uses timer0 and timer1.
//   Note that timer0 may be repurposed when the first servo is attached.
//
//   Timers are seized as needed in groups of 12 servos - 24 servos use two
//   timers, there are only two timers for the esp8266 so the support stops here
//   The sequence used to sieze timers is defined in timers.h
//
//   The methods are:
//
//   Servo - Class for manipulating servo motors connected to Arduino pins.
//
//   attach(pin )  - Attaches a servo motor to an i/o pin.
//   attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
//   default min is 544, max is 2400
//
//   write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
//   writeMicroseconds() - Sets the servo pulse width in microseconds
//   read()      - Gets the last written servo pulse width as an angle between 0 and 180.
//   readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
//   attached()  - Returns true if there is a servo attached.
//   detach()    - Stops an attached servos from pulsing its i/o pin.
  

#ifndef Servo_h
#define Servo_h

#include <Arduino.h>

// the following are in us (microseconds)
//
#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000     // minumim time to refresh servos in microseconds 

// NOTE: to maintain a strict refresh interval the user needs to not exceede 8 servos 
#define SERVOS_PER_TIMER       12     // the maximum number of servos controlled by one timer 
#define MAX_SERVOS   (ServoTimerSequence_COUNT  * SERVOS_PER_TIMER)

#if defined(ESP8266)

#include "esp8266/ServoTimers.h"

#else

#error "This library only supports esp8266 boards."

#endif

class Servo
{
public:
    Servo();
    uint8_t attach(int pin);           // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
    uint8_t attach(int pin, int min, int max); // as above but also sets min and max values for writes. 
    void detach();
    void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
    void writeMicroseconds(int value); // Write pulse width in microseconds 
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds();            // returns current pulse width in microseconds for this servo (was read_us() in first release)
    bool attached();                   // return true if this servo is attached, otherwise false 
private:
    uint8_t _servoIndex;               // index into the channel data for this servo
    uint16_t _minUs;                   
    uint16_t _maxUs;                   
};

#endif
