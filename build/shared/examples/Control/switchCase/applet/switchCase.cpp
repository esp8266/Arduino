
#include "WProgram.h"
void setup();
void loop();
void setup() {
  Serial.begin(9600);  
}
void loop() {
  int distance = analogRead(0);

  int range = map(distance, 0, 600, 0, 3);
 
  switch (range) {
  case 0:
  Serial.println("dark");
    break;
  case 1:
    Serial.println("dim");
    break;
  case 2:
    Serial.println("medium");
    break;
  case 3:
    Serial.println("bright");
    break;
  } 
 
}





int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

