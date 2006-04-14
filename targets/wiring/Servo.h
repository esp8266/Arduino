/*
  Servo.h - Servo library for Arduino & Wiring
  Based on Hernando Barragan's original C implementation
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

#ifndef Servo_h
#define Servo_h

#include <inttypes.h>

class Servo
{
  private:
    uint8_t _index;
    uint8_t _pin;
    uint16_t _duty;
    static uint8_t _count;
    static Servo* _servos[];
    static int8_t _current;
    static uint16_t _positionTicks;
    static void start();
    static void end();
    static void service();
  public:
    Servo();
    uint8_t attach(int);
    void detach();
    void write(int);
    uint8_t read();
    uint8_t attached();
};

#endif

