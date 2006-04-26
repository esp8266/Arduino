/* Double Counter
 * --------------
 *
 * This program creates a double counter
 * and sends the information over the port
 * back to the computer.
 * It can be used to test the connection of two different
 * sensors to the board at the same time.
 *
 * (cleft) 2005 David Cuartielles for DojoCorp
 * @author: D. Cuartielles
 * @credits: Greg and Nima from SFU
 */

int count = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // counter A will go forwards
  Serial.print('A');
  Serial.print(count);
  Serial.println(); 
  delay(1000);

  // counter B will go backwards
  Serial.print('B');
  Serial.print(1024 - count);
  Serial.println();
  delay(1000);
  
  // increase and reset the counter (if needed)
  count++;
  if (count == 1024) count = 0;
}
