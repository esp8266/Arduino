extern "C" {
  #include <inttypes.h>
  #include <math.h>
  #include <stdio.h>
  #include <avr/io.h>
  #include <avr/delay.h>

  #include "WConstants.h"
  
  // main program prototypes
  void setup(void);
  void loop(void);
  
  // timing prototypes
  void delay(int);
  int millis(void);
  
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
  void beginPulse(void);
  void endPulse(void);
  void pulseFrequency(int, int);
  void pulseRun(int, int);
  void pulseStop(int);
  int pulseRunning(int);
  uint16_t pulseIn(uint8_t, uint8_t);
  
  // interrupt management prototypes
  void attachInterrupt(uint8_t, void *(void) );
  void detachInterrupt(uint8_t);
  
  // serial prototypes
  void beginSerial(int);
  void endSerial(void);
  int serialRead(void);
  void serialWrite(int);
  void serialWriteBytes(char[]);
  uint8_t serialAvailable(void);
  void beginSerial1(int);
  void endSerial1(void);
  int serial1Read(void);
  void serial1Write(int);
  void serial1WriteBytes(char[]);
  uint8_t serial1Available(void);
  void printMode(int);
  void print(const char *, ...);
  
  /*
  // servo prototypes
  void beginServo(void);
  void endServo(void);
  void attachServo(int, int);
  uint8_t servoRead(int);
  void servoWrite(int, int);
  */
  
  // lcd prototypes
  void beginDisplay(void);
  void endDisplay(void);
  void displayWrite(int);
  void displayClear(void);
  void cursorHome(void);
  void cursorMoveTo(uint8_t, uint8_t);
  
  // encoder prototypes
  void beginEncoder(void);
  void endEncoder(void);
  void attachEncoder(int, int, int);
  int32_t encoderRead(uint8_t);
  void encoderWrite(uint8_t, uint32_t);
}

// library includes
#include "Servo.h"
//#include "Wire.h"
//#include "Matrix.h"

