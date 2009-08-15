
void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  int switchValue = digitalRead(2);
  digitalWrite(13, switchValue);  
}



