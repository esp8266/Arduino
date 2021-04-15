#include <Schedule.h>
#include <PolledTimeout.h>

#define WAKE_UP_PIN 0  // D3/GPIO0, can also force a serial flash upload with RESET
// you can use any GPIO for WAKE_UP_PIN except for D0/GPIO16 as it doesn't support interrupts

void IRAM_ATTR wakeupPinIsr() {
  schedule_function([]() {
    Serial.println("GPIO went from HI to LO");
  });
}

void IRAM_ATTR wakeupPinIsrWE() {
  schedule_function([]() {
    Serial.println("GPIO wakeup IRQ");
  });
  wakeupPinIsr();
  attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

//void wakeupCallback() {
//}

void setup() {
  Serial.begin(74880);
  while (!Serial);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);  // activity and status indicator
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);  // polled to advance tests, interrupt for Forced Light Sleep
  attachInterrupt(WAKE_UP_PIN, wakeupPinIsr, FALLING);
}

using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
oneShotYieldMs gotoSleep(2000);

void loop() {
  if (gotoSleep && ESP.forcedLightSleepBegin(10000000/*, wakeupCallback*/)) {
    // No new timers, no delay(), between forcedLightSleepBegin() and forcedLightSleepEnd().
    // Only ONLOW_WE or ONHIGH_WE interrupts work, no edge, that's an SDK or CPU limitation.
    // If the GPIO is in wakeup state while attaching the interrupt, it cannot trigger a wakeup,
    // but any sleep duration will be honored.
    bool wakeupPinIsHigh = digitalRead(WAKE_UP_PIN);
    // the GPIO might still bounce to LOW between both digital reads, disabling wakeup
    if (wakeupPinIsHigh) {
      attachInterrupt(WAKE_UP_PIN, wakeupPinIsrWE, ONLOW_WE);
    }
    wakeupPinIsHigh &= digitalRead(WAKE_UP_PIN);
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off so they know the CPU isn't running
    ESP.forcedLightSleepEnd(!wakeupPinIsHigh);
    digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
    if (wakeupPinIsHigh) {
      gotoSleep.reset();
    }
  }
}
