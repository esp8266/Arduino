/* Two Potentiometers
 * ------------------
 *
 * This program reads two potentiometers and
 * sends the data to the computer. It combines
 * both with PD or Processing
 *
 * (cleft) 2005 David Cuartielles for DojoCorp
 * @author: D. Cuartielles
 * @credit: Nima and Greg
 * @date: 2005-11-18
 * @location: SFU, Vancouver, Canada
 */
 
int potPin1 = 0;
int potPin2 = 1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print('A');
  Serial.print(analogRead(potPin1));
  Serial.println();
  delay(500);
  Serial.print('B');
  Serial.print(analogRead(potPin2));
  Serial.println();
  delay(500);
}
