#include "wiring_private.h"
#include "pins_arduino.h"

extern "C" int  __analogRead(uint8_t pin);

extern "C" int analogRead(uint8_t pin) {

  static uint8_t currentAin = 0;

  delay(0);
  if (pin >= ANALOG_INPUT_HARDWARE) {
    return 0;
  }
  uint8_t ain = analog_pin_to_mux_channel[pin];
  if (ain == (uint8_t)NOT_A_PIN) {
    return 0;
  } 
  if (currentAin > ain) {
    for (int i = 0; i < (8 - currentAin); ++i)
    {
      digitalWrite(ANALOG_INPUT_SELECTOR_PIN, HIGH);
      digitalWrite(ANALOG_INPUT_SELECTOR_PIN, LOW);
      digitalWrite(ANALOG_INPUT_SELECTOR_PIN, HIGH);
      digitalWrite(ANALOG_INPUT_SELECTOR_PIN, LOW);
    }
    currentAin = 0;
  }
  for (int i = currentAin; i < ain; ++i)
  {
    digitalWrite(ANALOG_INPUT_SELECTOR_PIN, HIGH);
    digitalWrite(ANALOG_INPUT_SELECTOR_PIN, LOW);
    digitalWrite(ANALOG_INPUT_SELECTOR_PIN, HIGH);
    digitalWrite(ANALOG_INPUT_SELECTOR_PIN, LOW);
  }
  currentAin = ain;
  return __analogRead(ANALOG_INPUT_HARDWARE);
}

void initVariant() {
  // we need to reset analog mux. When ANALOG_INPUT_SELECTOR_PIN is high
  // on ~0.4 ms, mux channel is becoming 0.
  // Mux channel is switching on back \_ front. But there is no switching
  // - rc reset is still high when ANALOG_INPUT_SELECTOR_PIN became low
  uint16_t resetDelay = 777;
  pinMode(ANALOG_INPUT_SELECTOR_PIN, OUTPUT);
  digitalWrite(ANALOG_INPUT_SELECTOR_PIN, HIGH);
  delayMicroseconds(resetDelay);
  digitalWrite(ANALOG_INPUT_SELECTOR_PIN, LOW);
  delayMicroseconds(resetDelay);
}
