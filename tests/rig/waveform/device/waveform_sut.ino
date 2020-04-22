/*
  waveform_sut.ino - ESP8266 waveform/analogWrite/Tone/Servo test
  Copyright (c) 2020 Earle F. Philhower, III. All rights reserved.

  Intended to be uploaded to the ESP8266 and used to generate a
  specific test pattern by an automates regression suite.  Minimal
  error checking has been implemented.

  Test commands are 1-line followed by a \n or \r\n and consist of
  a single character command code followed up by to 2 different
  parameters for the command.

  On successful completion of a command the ESP8266 sends a
  success line of the format "+XXX\n" where the XXX portion is
  optional.

  On error, the ESP8266 will return a failure line of the format
  "-XXXX" where the XXX is an optional description of the error.

  This can be run on any ESP8266 variant with at least 9 D pins
  available in the Arduino IDE.

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

#include <Arduino.h>
#include <ESP8266WiFi.h>

const int Dpin[] = { D0, D1, D2, D3, D4, D5, D6, D7, D8 };

void cputest() {
  volatile uint32_t x = 0;
  uint32_t start, end;
  start = ESP.getCycleCount();
  for (auto i=0; i<1000000; i++) x += x;
  end = ESP.getCycleCount();
  Serial.printf("+%d\n", end - start);
}

void success() {
  Serial.printf("+\n");
}

void info() {
  Serial.printf("+%d %s\n", ESP.getCpuFreqMHz(), ESP.getFullVersion().c_str());
}

void help() {
  Serial.printf("\nPWM tester\n");
  Serial.printf("h        = Display this help\n");
  Serial.printf("z        = Reset ESP8266\n");
  Serial.printf("d # 0/1  = pinMode(D#, OUTPUT); digitalWrite(D#, h/l)\n");
  Serial.printf("t # freq = pinMode(D#, OUTPUT); tone(D#, freq)\n");
  Serial.printf("f freq   = analogWriteFreq(freq)\n");
  Serial.printf("r range  = analogWriteRange(range)\n");
  Serial.printf("a # val  = pinMode(D#, OUTPUT); analogWrite(D#, val)\n");
  Serial.printf("c        = Run CPU test, report clock cycles to count to 1 million\n");
  Serial.printf("i        = Return SUT info\n");
  Serial.printf("\n");
}

void setup() {
  WiFi.disconnect(true);
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  help();
}

void loop() {
  char buff[32];
  if (Serial.available() > 0) {
    memset(buff, 0, 32);
    Serial.readBytesUntil('\n', buff, 31);
    int a = 0, b = 0;
    sscanf(buff, "%*s %d %d", &a, &b);
    switch (buff[0]) {
      case 'd':
        pinMode(Dpin[a], OUTPUT);
        digitalWrite(Dpin[a], b ? HIGH : LOW);
        success();
        break;
      case 't':
        pinMode(Dpin[a], OUTPUT);
        tone(Dpin[a], b);
        success();
        break;
      case 'f':
        analogWriteFreq(a);
        success();
        break;
      case 'r':
        analogWriteRange(a);
        success();
        break;
      case 'a':
        pinMode(Dpin[a], OUTPUT);
        analogWrite(Dpin[a], b);
        success();
        break;
      case 'z':
        ESP.reset(); // Can't really send a code after this succeeds :)
        break;
      case 'c':
        cputest(); // Sends code inside function
        break;
      case 'h':
        help(); // No success, this is only ever used in interactive
        break;
      case 'i':
        info(); // Sends code inside function
        break;
      default:
        Serial.printf("-Unknown command %s\n", buff);
        break;
    }
  }
}
