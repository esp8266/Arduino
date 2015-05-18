/*
  Esplora.h - Arduino Esplora board library
  Written by Enrico Gueli
  Copyright (c) 2012 Arduino LLC.  All right reserved.

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

#ifndef ESPLORA_H_
#define ESPLORA_H_

#include <Arduino.h>

/*
 * The following constants are used internally by the Esplora
 * library code.
 */

const byte JOYSTICK_BASE  = 16; // it's a "virtual" channel: its ID won't conflict with real ones

const byte MAX_CHANNELS   = 13;

const byte CH_SWITCH_1    = 0;
const byte CH_SWITCH_2    = 1;
const byte CH_SWITCH_3    = 2;
const byte CH_SWITCH_4    = 3;
const byte CH_SLIDER      = 4;
const byte CH_LIGHT       = 5;
const byte CH_TEMPERATURE = 6;
const byte CH_MIC         = 7;
const byte CH_TINKERKIT_A = 8;
const byte CH_TINKERKIT_B = 9;
const byte CH_JOYSTICK_SW = 10;
const byte CH_JOYSTICK_X  = 11;
const byte CH_JOYSTICK_Y  = 12;

/*
 * The following constants can be used with the readButton()
 * method.
 */

const byte SWITCH_1       = 1;
const byte SWITCH_2       = 2;
const byte SWITCH_3       = 3;
const byte SWITCH_4       = 4;

const byte SWITCH_DOWN  = SWITCH_1;
const byte SWITCH_LEFT  = SWITCH_2;
const byte SWITCH_UP    = SWITCH_3;
const byte SWITCH_RIGHT = SWITCH_4;

const byte JOYSTICK_DOWN  = JOYSTICK_BASE;
const byte JOYSTICK_LEFT  = JOYSTICK_BASE+1;
const byte JOYSTICK_UP    = JOYSTICK_BASE+2;
const byte JOYSTICK_RIGHT = JOYSTICK_BASE+3;

/*
 * These constants can be use for comparison with the value returned
 * by the readButton() method.
 */
const boolean PRESSED   = LOW;
const boolean RELEASED  = HIGH;

/*
 * The following constants can be used with the readTemperature()
 * method to specify the desired scale.
 */
const byte DEGREES_C = 0;
const byte DEGREES_F = 1;

/*
 * The following constants can be used with the readAccelerometer()
 * method to specify the desired axis to return.
 */
const byte X_AXIS = 0;
const byte Y_AXIS = 1;
const byte Z_AXIS = 2;


class _Esplora {
private:
  byte lastRed;
  byte lastGreen;
  byte lastBlue;

  unsigned int readChannel(byte channel);    
  
  boolean joyLowHalf(byte joyCh);
  boolean joyHighHalf(byte joyCh);
    
public:
  _Esplora();
  
  /*
   * Returns a number corresponding to the position of the
   * linear potentiometer. 0 means full right, 1023 means
   * full left.
   */
  inline unsigned int readSlider() { return readChannel(CH_SLIDER); }

  /*
   * Returns a number corresponding to the amount of ambient
   * light sensed by the light sensor.
   */
  inline unsigned int readLightSensor() { return readChannel(CH_LIGHT); }

  /*
   * Returns the current ambient temperature, expressed either in Celsius
   * or Fahreneit scale.
   */
  int readTemperature(const byte scale);

  /*
   * Returns a number corresponding to the amount of ambient noise.
   */
  inline unsigned int readMicrophone() { return readChannel(CH_MIC); }
  
  inline unsigned int readJoystickSwitch() { return readChannel(CH_JOYSTICK_SW); }

  inline int readJoystickX() { 
    return readChannel(CH_JOYSTICK_X) - 512;
  }
  inline int readJoystickY() {
    return readChannel(CH_JOYSTICK_Y) - 512;
  }

  int readAccelerometer(const byte axis);
  
  /*
   * Reads the current state of a button. It will return
   * LOW if the button is pressed, and HIGH otherwise.
   */
  boolean readButton(byte channel);

  boolean readJoystickButton();
  
  void writeRGB(byte red, byte green, byte blue);
  void writeRed(byte red);
  void writeGreen(byte green);
  void writeBlue(byte blue);

  byte readRed();
  byte readGreen();
  byte readBlue();
  
  void tone(unsigned int freq);
  void tone(unsigned int freq, unsigned long duration);
  void noTone();
  
  inline unsigned int readTinkerkitInput(byte whichInput) {
    return readChannel(whichInput + CH_TINKERKIT_A);
  }
  inline unsigned int readTinkerkitInputA() {
    return readChannel(CH_TINKERKIT_A);
  }
  inline unsigned int readTinkerkitInputB() {
    return readChannel(CH_TINKERKIT_B);
  }
};



extern _Esplora Esplora;

#endif // ESPLORA_H_
