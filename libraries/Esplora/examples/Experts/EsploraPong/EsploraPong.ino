#include <Esplora.h>

void setup() {
  Serial.begin(9600); 
}

void loop() {
  int slider = Esplora.readSlider();
  int resetButton = Esplora.readButton(1);
  int serveButton = Esplora.readButton(3);
  int switchPlayerButton = Esplora.readButton(4);

  Serial.print(slider);
  Serial.print(",");
  Serial.print(resetButton);
  Serial.print(",");
  Serial.print(serveButton);
  Serial.print(",");
  Serial.println(switchPlayerButton);
  delay(10);
}

