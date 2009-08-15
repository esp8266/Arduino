
void setup() {
  pinMode(6, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(2);
  int ledFadeValue = map(sensorValue, 0, 1023, 0, 255);
  analogWrite(6, ledFadeValue);  
}



