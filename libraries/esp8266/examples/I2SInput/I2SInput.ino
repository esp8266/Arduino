/*
   I2S stereo microphone (input) example
   Run using the Arduion Serial Plotter to see waveform.
   Released to the Public Domain by Earle F. Philhower, III
*/

#include <ESP8266WiFi.h>
#include <i2s.h>

void dump() {
  Serial.printf("I2SC:    %08x\n", I2SC);
  Serial.printf("I2SFC:   %08x\n", I2SFC);
  Serial.printf("I2SCC:   %08x\n", I2SCC);
  Serial.printf("I2SRXEN: %08x\n", I2SRXEN);
  Serial.printf("SLCC0:   %08x\n", SLCC0);
  Serial.printf("SLCRXDC: %08x\n", SLCRXDC);
  Serial.printf("SLCTXL:  %08x\n", SLCTXL);
  Serial.printf("SLCIE:   %08x\n", SLCIE);
}

void setup() {
  Serial.begin(115200);
  WiFi.forceSleepBegin();
  delay(500);

  i2s_rxtx_begin(true, false); // Enable I2S RX
  i2s_set_rate(11025);
  dump();

  delay(1000);

  while (1) {
    uint32_t l, r;
    i2s_read_sample(&l, &r, true);
    int16_t lh = l >> 16;
    int16_t rh = r >> 16;
    char withScale[256];
    sprintf(withScale, "%d %d", lh, rh);
    Serial.println(withScale);
    yield();
  }
}

void loop() {
  /* Nothing here */
}
