#include <Schedule.h>
#include <PolledTimeout.h>

using oneShotYieldMs = esp8266::polledTimeout::timeoutTemplate<false, esp8266::polledTimeout::YieldPolicy::YieldOrSkip>;
oneShotYieldMs gotoSleep(2000);

void wakeupCallback() {
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
  schedule_function([]() {
    Serial.println("wakeup callback was performed");
    gotoSleep.reset(2000);
  });
}

void setup() {
  Serial.begin(74880);
  while (!Serial);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);  // activity and status indicator
  digitalWrite(LED_BUILTIN, LOW);  // turn on the LED
}

void loop() {
  if (gotoSleep && ESP.forcedModemSleep(10 * 1000 * 1000, wakeupCallback)) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off so they know the modem isn't running
    gotoSleep.resetToNeverExpires();
  }
}
