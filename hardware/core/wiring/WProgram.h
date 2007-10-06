extern "C" {
  #include <inttypes.h>
  #include <math.h>
  #include <avr/io.h>

  #include "WConstants.h"
  
  // main program prototypes
  void setup(void);
  void loop(void);
  
  // timing prototypes
  void delay(long);
  void delayMicroseconds(unsigned int);
  long millis(void);
  
  // pin prototypes
  void pinMode(uint8_t, uint8_t);
  uint8_t digitalRead(uint8_t);
  void digitalWrite(int, uint8_t);
  void portMode(int, int);
  int portRead(int);
  void portWrite(int, int);
  int analogRead(int);
  void analogWrite(int, int);
  
  // pulse prototypes
  unsigned long pulseIn(int, int);
  
  // interrupt management prototypes
  void attachInterrupt(uint8_t, void (*)(void) );
  void detachInterrupt(uint8_t);
}

// random prototypes
float random(float);
float random(float, float);
void randomSeed(unsigned int);

