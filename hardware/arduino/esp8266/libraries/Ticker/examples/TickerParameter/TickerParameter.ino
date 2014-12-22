/*
  Passing paramters to Ticker callbacks
  
  Apart from void(void) functions, the Ticker library supports 
  functions taking one argument. This argument's size has to be less or
  equal to 4 bytes (so char, short, int, float, void*, char* types will do).
  
  This sample runs two tickers that both call one callback function,
  but with different arguments.

  An LED connected to GPIO1 will be pulsing. Use a built-in LED on ESP-01
  or connect an external one to TXD on other boards.
*/

#include <Ticker.h>

Ticker tickerSetHigh;
Ticker tickerSetLow;

void setPin(int state) {
  digitalWrite(1, state);
}

void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  
  // every 25 ms, call setPin(0) 
  tickerSetLow.attach_ms(25, setPin, 0);
  
  // every 26 ms, call setPin(1)
  tickerSetHigh.attach_ms(26, setPin, 1);
}

void loop() {
}
