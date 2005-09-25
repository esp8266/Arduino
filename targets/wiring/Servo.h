/*
  Servo.h - Servo library file for Wiring
  Part of the Wiring project - http://wiring.org.co

  Copyright (c) 2004-2005 Hernando Barragan

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Servo library by Nicholas Zambetti 
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

