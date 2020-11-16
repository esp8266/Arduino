/*
  Servo.h - Interrupt driven Servo library for Esp8266 using timers
  Original Copyright (c) 2015 Michael C. Miller. All right reserved.

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
//   The methods are:
//
//   Servo - Class for manipulating servo motors connected to Arduino pins.
//
//   attach(pin)  - Attaches a servo motor to an i/o pin.
//   attach(pin, min, max) - Attaches to a pin setting min and max values in microseconds
//   default min is 1000, max is 2000
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

// The following values are in us (microseconds).
// Since the defaults can be overwritten in the new attach() member function,
// they were modified from the Arduino AVR defaults to be in the safe range
// of publically available specifications. While this implies that many 180°
// servos do not operate the full 0° to 180° sweep using these, it also prevents
// unsuspecting damage. For Arduino AVR, the same change is being discussed.
#define DEFAULT_MIN_PULSE_WIDTH      1000 // uncalibrated default, the shortest duty cycle sent to a servo
#define DEFAULT_MAX_PULSE_WIDTH      2000 // uncalibrated default, the longest duty cycle sent to a servo 
#define DEFAULT_NEUTRAL_PULSE_WIDTH  1500 // default duty cycle when servo is attached
#define REFRESH_INTERVAL            20000 // classic default period to refresh servos in microseconds 
#define MAX_SERVOS                      9 // D0-D8

#if !defined(ESP8266)

#error "This library only supports esp8266 boards."

#endif

class Servo
{
public:
    Servo();
    ~Servo();
    // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure.
    // returns channel number or 0 if failure.
    uint8_t attach(int pin);
    // attach the given pin to the next free channel, sets pinMode, min, and max values for write().
    // returns channel number or 0 if failure.
    uint8_t attach(int pin, uint16_t min, uint16_t max);
    // attach the given pin to the next free channel, sets pinMode, min, and max values for write(),
    // and sets the initial value, the same as write().
    // returns channel number or 0 if failure.
    uint8_t attach(int pin, uint16_t min, uint16_t max, int value);
    void detach();
    void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
    void writeMicroseconds(int value); // Write pulse width in microseconds 
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds();            // returns current pulse width in microseconds for this servo (was read_us() in first release)
    bool attached();                   // return true if this servo is attached, otherwise false 
private:
    static uint32_t _servoMap;
    bool     _attached;
    uint8_t  _pin;
    uint16_t _minUs;                   
    uint16_t _maxUs;                   
    uint16_t _valueUs;
};

#endif
