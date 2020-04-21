#include <ESP8266WiFi.h>

const int pin[] = { D0, D1, D2, D3, D4, D5, D6, D7, D8 };

void cputest() {
  volatile uint32_t x = 0;
  uint32_t start, end;
  start = ESP.getCycleCount();
  for (auto i=0; i<1000000; i++) x += x;
  end = ESP.getCycleCount();
  Serial.printf("%d\n", end - start);
}

void setup() {
  WiFi.disconnect(true);
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  Serial.printf("\nPWM tester\n");
  Serial.printf("z        = Reset ESP8266\n");
  Serial.printf("d # 0/1  = pinMode(D#, OUTPUT); digitalWrite(D#, h/l)\n");
  Serial.printf("t # freq = pinMode(D#, OUTPUT); tone(D#, freq)\n");
  Serial.printf("f freq   = analogWriteFreq(freq)\n");
  Serial.printf("r range  = analogWriteRange(range)\n");
  Serial.printf("a # val  = pinMode(D#, OUTPUT); analogWrite(D#, val)\n");
  Serial.printf("c        = Run CPU test, report clock cycles to count to 1 million\n\n");
}

void loop() {
  char buff[32];
  if (Serial.available() > 0) {
    memset(buff, 0, 32);
    Serial.readBytesUntil('\n', buff, 31);
    int a = 0, b = 0;
    sscanf(buff, "%*s %d %d", &a, &b);
    if (buff[0] == 'd') {
      pinMode(pin[a], OUTPUT);
      digitalWrite(pin[a], b ? HIGH : LOW);
    } else if (buff[0] == 't') {
      pinMode(pin[a], OUTPUT);
      tone(pin[a], b);
    } else if (buff[0] == 'f') {
      analogWriteFreq(a);
    } else if (buff[0] == 'r') {
      analogWriteRange(a);
    } else if (buff[0] == 'a') {
      pinMode(pin[a], OUTPUT);
      analogWrite(pin[a], b);
    } else if (buff[0] == 'z') {
      ESP.reset();
    } else if (buff[0] == 'c'){
      cputest();
    }
  }
}
