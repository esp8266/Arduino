/*
  This example reads audio data from an Invensense's ICS43432 I2S microphone
  breakout board, and prints out the samples to the Serial console. The
  Serial Plotter built into the Arduino IDE can be used to plot the audio
  data (Tools -> Serial Plotter)
  created 17 November 2016
  by Sandeep Mistry
*/

#include <I2S.h>

void setup() {
  // Open serial communications and wait for port to open:
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // start I2S at 8 kHz with 24-bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 24)) {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ;  // do nothing
  }
}

void loop() {
  // read a sample
  int sample = I2S.read();

  if (sample) {
    // if it's non-zero print value to serial
    Serial.println(sample);
  }
}
