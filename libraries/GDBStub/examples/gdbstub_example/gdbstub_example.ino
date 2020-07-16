#include <GDBStub.h>

void setup() {
  Serial.begin(115200);
  gdbstub_init();
  Serial.printf("Starting...\n");
}

void loop() {
  static uint32_t cnt = 0;
  Serial.printf("%d\n", cnt++);
  delay(100);
}
