/*
   I2S stereo microphone (input) example
   Run using the Arduion Serial Plotter to see waveform.
   Released to the Public Domain by Earle F. Philhower, III

   For the Google AIY Voice Hat Microphone daughterboard, part
   of the Raspberry Pi AIY cardboard box, the I2S stereo pinout
   looking at the board top with the RPI logo on the left hand
   side:
            +--   ------------------------------------  --+
   left RPI | (1) GND (2) DIN (3) BCLK (4) LRCLK (5) 3.3V | AIY right
            +---------------------------------------------+

   The I2S pins are on different pins depending on your board.
   The *internal GPIO number* which is NOT NECESSARIALY the
   same as the pin numbers, are as follows:
   I2SI_DATA     = GPIO12
   IS2I_BCK      = GPIO13
   I2SI_WS/LRCLK = GPIO14

   On the D1 mini the I2SI pins map to the following D pins:
   I2SI_DATA     = GPIO12 = D6
   IS2I_BCK      = GPIO13 = D7
   I2SI_WS/LRCLK = GPIO14 = D5

   Expect different D pins on different ESP8266 boards, and of
   course be sure to wire up VCC(3.3V) and GND.
*/

#include <ESP8266WiFi.h>
#include <I2S.h>

void setup() {
  Serial.begin(115200);
  WiFi.forceSleepBegin();
  delay(500);

  i2s_rxtx_begin(true, false);  // Enable I2S RX
  i2s_set_rate(11025);

  delay(1000);

  while (1) {
    int16_t l, r;
    i2s_read_sample(&l, &r, true);
    char withScale[256];
    sprintf(withScale, "%d %d", l, r);
    Serial.println(withScale);
    yield();
  }
}

void loop() { /* Nothing here */
}
