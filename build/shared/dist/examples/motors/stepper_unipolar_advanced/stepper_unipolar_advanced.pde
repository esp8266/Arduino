/* Stepper Unipolar Advanced
 * -------------------------
 *
 * Program to drive a stepper motor coming from a 5'25 disk drive
 * according to the documentation I found, this stepper: "[...] motor 
 * made by Copal Electronics, with 1.8 degrees per step and 96 ohms 
 * per winding, with center taps brought out to separate leads [...]"
 * [http://www.cs.uiowa.edu/~jones/step/example.html]
 *
 * It is a bipolar stepper motor with 5 wires:
 * 
 * - red: power connector, I have it at 5V and works fine
 * - brown and black: coil 1
 * - orange and yellow: coil 2
 *
 * We use a potentiometer to control the speed and direction of the motor
 *
 * (cleft) 2005 DojoDave for K3
 * http://www.0j0.org | http://arduino.berlios.de
 *
 * @author: David Cuartielles
 * @date: 20 Oct. 2005
 */

int ledPin = 13;
int statusLed = LOW; 
int motorPins[] = {8, 9, 10, 11};
int count = 0;
int count2 = 0;
int delayTime = 500;
int val = 0;
  
void setup() {
  pinMode(ledPin, OUTPUT);
  for (count = 0; count < 4; count++) {
    pinMode(motorPins[count], OUTPUT);
  }
}

void moveForward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[count], count2>>count&0x01);
  }
  delay(delayTime);
}

void moveBackward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2>>=1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[3 - count], count2>>count&0x01);
  }
  delay(delayTime);
}

void loop() {
  val = analogRead(0);
  if (val > 540) {
    delayTime = 2048 - 1024 * val / 512 + 1; // move faster the higher the value from the potentiometer
    moveForward();
  } else if (val < 480) {
    delayTime = 1024 * val / 512 + 1; // move faster the lower the value from the potentiometer
    moveBackward();
  } else {
    delayTime = 1024;
  }
}
