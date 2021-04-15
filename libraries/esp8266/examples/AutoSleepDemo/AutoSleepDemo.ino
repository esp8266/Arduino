#include <Schedule.h>
#include <PolledTimeout.h>

void setup() {
  Serial.begin(74880);
  while (!Serial);
  delay(100);
  Serial.println("AutoLightSleep");
  Serial.flush();
}

void loop() {
  delay(10000); // this enters the idle task

  Serial.println("entering auto sleep section");
  Serial.flush();
  ESP.autoLightSleep();
  // ESP.autoModemSleep();

  delay(10000); // this enters the idle task with auto light sleep

  ESP.autoLightSleep(false);
  // ESP.autoModemSleep(false);
  Serial.println("left auto sleep section");
  Serial.flush();
}
