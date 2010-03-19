
void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(0);
  Serial.println(sensorValue, DEC);
}



