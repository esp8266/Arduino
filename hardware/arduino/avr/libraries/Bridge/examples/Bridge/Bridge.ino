
#include <Bridge.h>

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH); 
}

void loop() {
  while (Bridge.messageAvailable()) {
    uint8_t buff[64];
    int l = Bridge.readMessage(buff, 64);
    process(buff, l);
  }
  delay(100); // Poll every 0.100s
}

void process(uint8_t buff[], int l) {
  // "DWppv"   -> digitalWrite(pp, v)
  // "DRpp"    -> digitalRead(pp)      -> "Dpp0" / "Dpp1"
  // "AWppvvv" -> analogWrite(pp, vvv)
  // "ARpp"    -> analogRead(pp)       -> "App0000" - "App1023"
  // "PIpp"    -> pinMode(pp, INPUT)
  // "POpp"    -> pinMode(pp, OUTPUT)

  // Sanity check
  if (l<4 || l>7)
    return;
  if (buff[2]<'0' || buff[2]>'9')
    return;
  if (buff[3]<'0' || buff[3]>'9')
    return;
  char cmd0 = buff[0];
  char cmd1 = buff[1];
  int pin = (buff[2]-'0')*10 + (buff[3]-'0');
  if (pin<0 || pin>13)
    return;

  // Command selection
  if (l==5 && cmd0=='D' && cmd1=='W') {
    char c = buff[4];
    if (c=='0' || c=='1')
      digitalWrite(pin, c-'0');
  } else if (l==4 && cmd0=='D' && cmd1=='R') {
    reportDigitalRead(pin, true, true);
  } else if (l==7 && cmd0=='A' && cmd1=='W') {
    analogWrite(pin, buff[4]);
  } else if (l==4 && cmd0=='A' && cmd1=='R') {
    reportAnalogRead(pin);
  } else if (l==4 && cmd0=='P' && cmd1=='I') {
    pinMode(pin, INPUT);
  } else if (l==4 && cmd0=='P' && cmd1=='O') {
    pinMode(pin, OUTPUT);
  }
}

void reportDigitalRead(int pin, boolean raw, boolean dataset) {
  // "Dpp0" - "Dpp1"
  //                  0    1    2    3
  uint8_t buff[] = { 'D', '0', '0', '0' };
  buff[1] += pin/10;
  buff[2] += pin%10;
  if (digitalRead(pin) == HIGH)
    buff[3] = '1';
  if (raw)
    Bridge.writeMessage(buff, 4);
  if (dataset) {
    char *val = "0";
    val[0] = buff[3];
    buff[3] = 0;
    Bridge.put((const char *)buff, val);
  }
}

void reportAnalogRead(int pin) {
  // "App0000" - "App1023"
  //                  0    1    2    3    4    5    6
  uint8_t buff[] = { 'A', '0', '0', '0', '0', '0', '0' };
  buff[1] += pin/10;
  buff[2] += pin%10;

  int v = analogRead(pin);
  buff[6] += v%10; v /= 10;
  buff[5] += v%10; v /= 10;
  buff[4] += v%10; v /= 10;
  buff[3] += v;
  Bridge.writeMessage(buff, 7);
}