/*
  Esplora.cpp - Arduino Esplora board library
  Written by Enrico Gueli
  Copyright (c) 2012 Arduino(TM)  All right reserved.

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


#include "Esplora.h"

_Esplora Esplora;

/*
 * The following constants tell, for each accelerometer
 * axis, which values are returned when the axis measures
 * zero acceleration.
 */
const int ACCEL_ZERO_X = 320;
const int ACCEL_ZERO_Y = 330;
const int ACCEL_ZERO_Z = 310;

const byte MUX_ADDR_PINS[] = { A0, A1, A2, A3 };
const byte MUX_COM_PIN = A4;

const int JOYSTICK_DEAD_ZONE = 100;

const byte RED_PIN    = 5;
const byte BLUE_PIN   = 9;
const byte GREEN_PIN  = 10;

const byte BUZZER_PIN = 6;

// non-multiplexer Esplora pins: 
// Accelerometer: x-A5, y-A7, z-A6
// External outputs: D3, D11
// Buzzer: A8
// RGB Led: red-D5, green-D10/A11, blue-D9/A10
// Led 13: D13

const byte ACCEL_X_PIN = A5;
const byte ACCEL_Y_PIN = A11;
const byte ACCEL_Z_PIN = A6;

const byte LED_PIN     = 13;

_Esplora::_Esplora() {
  for (byte p=0; p<4; p++) {
    pinMode(MUX_ADDR_PINS[p], OUTPUT);
  }
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

unsigned int _Esplora::readChannel(byte channel) {
  digitalWrite(MUX_ADDR_PINS[0], (channel & 1) ? HIGH : LOW);
  digitalWrite(MUX_ADDR_PINS[1], (channel & 2) ? HIGH : LOW);
  digitalWrite(MUX_ADDR_PINS[2], (channel & 4) ? HIGH : LOW);
  digitalWrite(MUX_ADDR_PINS[3], (channel & 8) ? HIGH : LOW);
  // workaround to cope with lack of pullup resistor on joystick switch
  if (channel == CH_JOYSTICK_SW) {
    pinMode(MUX_COM_PIN, INPUT_PULLUP); 
    unsigned int joystickSwitchState = (digitalRead(MUX_COM_PIN) == HIGH) ? 1023 : 0;
    digitalWrite(MUX_COM_PIN, LOW);
    return joystickSwitchState;
  }
  else
    return analogRead(MUX_COM_PIN);
}

boolean _Esplora::joyLowHalf(byte joyCh) {
  return (readChannel(joyCh) < 512 - JOYSTICK_DEAD_ZONE)
    ? LOW : HIGH;
}

boolean _Esplora::joyHighHalf(byte joyCh) {
  return (readChannel(joyCh) > 512 + JOYSTICK_DEAD_ZONE)
    ? LOW : HIGH;
}

boolean _Esplora::readButton(byte ch) {
  if (ch >= SWITCH_1 && ch <= SWITCH_4) {
    ch--;
  }
  
  switch(ch) {
  case JOYSTICK_RIGHT:
    return joyLowHalf(CH_JOYSTICK_X);
  case JOYSTICK_LEFT:
    return joyHighHalf(CH_JOYSTICK_X);
  case JOYSTICK_UP:
    return joyLowHalf(CH_JOYSTICK_Y);
  case JOYSTICK_DOWN:
    return joyHighHalf(CH_JOYSTICK_Y);
  }
    
  unsigned int val = readChannel(ch);
  return (val > 512) ? HIGH : LOW;
}

boolean _Esplora::readJoystickButton() {
 if (readChannel(CH_JOYSTICK_SW) == 1023) {
  return HIGH;
 } else if (readChannel(CH_JOYSTICK_SW) == 0) {
  return LOW;
 }
}


void _Esplora::writeRGB(byte r, byte g, byte b) {
  writeRed(r);
  writeGreen(g);
  writeBlue(b);
}

#define RGB_FUNC(name, pin, lastVar) \
void _Esplora::write##name(byte val) { \
  if (val == lastVar) \
    return; \
  analogWrite(pin, val);  \
  lastVar = val; \
  delay(5); \
} \
\
byte _Esplora::read##name() { \
  return lastVar; \
}
  
RGB_FUNC(Red,   RED_PIN,   lastRed)
RGB_FUNC(Green, GREEN_PIN, lastGreen)
RGB_FUNC(Blue,  BLUE_PIN,  lastBlue)

void _Esplora::tone(unsigned int freq) {
  if (freq > 0)
    ::tone(BUZZER_PIN, freq);
  else
    ::noTone(BUZZER_PIN);
}

void _Esplora::tone(unsigned int freq, unsigned long duration) {
  if (freq > 0)
    ::tone(BUZZER_PIN, freq, duration);
  else
    ::noTone(BUZZER_PIN);
}

void _Esplora::noTone() {
  ::noTone(BUZZER_PIN);
}

int _Esplora::readTemperature(const byte scale) {
  long rawT = readChannel(CH_TEMPERATURE);
  if (scale == DEGREES_C) {
    return (int)((rawT * 500 / 1024) - 50);
  }
  else if (scale == DEGREES_F) {
    return (int)((rawT * 450 / 512 ) - 58);
  }
  else {
    return readTemperature(DEGREES_C);
  }
}

int _Esplora::readAccelerometer(const byte axis) {
  switch (axis) {
    case X_AXIS: return analogRead(ACCEL_X_PIN) - ACCEL_ZERO_X;
    case Y_AXIS: return analogRead(ACCEL_Y_PIN) - ACCEL_ZERO_Y;
    case Z_AXIS: return analogRead(ACCEL_Z_PIN) - ACCEL_ZERO_Z;
    default: return 0;
  }
}
