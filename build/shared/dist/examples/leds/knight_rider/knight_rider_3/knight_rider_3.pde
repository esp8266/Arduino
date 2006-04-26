/* Knight Rider 3
 * --------------
 *
 * Knight Rider is an AI car that showed up in the TV series from
 * the 80's with David Hasselhoff. The car had plenty of LED effects.
 *
 * Basically this is an extension of Blink_LED. This program handles 
 * 6 LEDs connected to pins 2 to 7. This is the third of a series of
 * three examples useful to understand the for(;;) loop
 *
 * In this example we declare an array with the pin numbers we use as inputs,
 * then we browse the array up and down to turn the different pins on/off
 *
 * This example concentrates in making the visuals fluid.
 *
 * Picture at:
 * http://arduino.berlios.de/index.php/Tutorial/KnightRider
 *
 * (cleft) 2005 K3, Malmo University
 * @author: David Cuartielles
 * @hardware: David Cuartielles, Aaron Hallborg
 */

int pinArray[] = {2, 3, 4, 5, 6, 7};
int count = 0;
int timer = 30;

void setup(){
  for (count=0;count<6;count++) {
    pinMode(pinArray[count], OUTPUT);
  }
}

void loop() {
  for (count=0;count<5;count++) {
   digitalWrite(pinArray[count], HIGH);
   delay(timer);
   digitalWrite(pinArray[count + 1], HIGH);
   delay(timer);
   digitalWrite(pinArray[count], LOW);
   delay(timer*2);
  }
  for (count=5;count>0;count--) {
   digitalWrite(pinArray[count], HIGH);
   delay(timer);
   digitalWrite(pinArray[count - 1], HIGH);
   delay(timer);
   digitalWrite(pinArray[count], LOW);
   delay(timer*2);
  }
}
