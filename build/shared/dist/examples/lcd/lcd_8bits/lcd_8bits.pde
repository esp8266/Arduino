/* LCD 8bits
 * ---------
 *
 * This is the first example in how to use an LCD screen
 * configured with data transfers over 8 bits. The example
 * uses all the digital pins on the Arduino board, but can
 * easily display data on the display
 *
 * There are the following pins to be considered:
 * 
 * - DI, RW, DB0..DB7, Enable (11 in total)
 *
 * the pinout for LCD displays is standard and there is plenty
 * of documentation to be found on the internet.
 *
 * Picture at:
 * http://arduino.berlios.de/index.php/Tutorial/LCD8Bits
 *
 * (cleft) 2005 DojoDave for K3
 *
 */
 
int DI = 12;
int RW = 11;
int DB[] = {3, 4, 5, 6, 7, 8, 9, 10};
int Enable = 2;

void LcdCommandWrite(int value) {
 // poll all the pins
 int i = 0;
 for (i=DB[0]; i <= DI; i++) {
   digitalWrite(i,value & 01);
   value >>= 1;
 }
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);
 // send a pulse to enable
 digitalWrite(Enable,HIGH);
 delayMicroseconds(1);  // pause 1 ms according to datasheet
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);  // pause 1 ms according to datasheet
}

void LcdDataWrite(int value) {
 // poll all the pins
 int i = 0;
 digitalWrite(DI, HIGH);
 digitalWrite(RW, LOW);
 for (i=DB[0]; i <= DB[7]; i++) {
   digitalWrite(i,value & 01);
   value >>= 1;
 }
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);
 // send a pulse to enable
 digitalWrite(Enable,HIGH);
 delayMicroseconds(1);
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);  // pause 1 ms according to datasheet
}

void setup (void) {
 int i = 0;
 for (i=Enable; i <= DI; i++) {
   pinMode(i,OUTPUT);
 }
 delay(100);
 // initiatize lcd after a short pause
 // needed by the LCDs controller
 LcdCommandWrite(0x30);  // function set:
                         // 8-bit interface, 1 display lines, 5x7 font
 delay(64);                      
 LcdCommandWrite(0x30);  // function set:
                         // 8-bit interface, 1 display lines, 5x7 font
 delay(50);                      
 LcdCommandWrite(0x30);  // function set:
                         // 8-bit interface, 1 display lines, 5x7 font
 delay(20);                      
 LcdCommandWrite(0x06);  // entry mode set:
                         // increment automatically, no display shift
 delay(20);                      
 LcdCommandWrite(0x0E);  // display control:
                         // turn display on, cursor on, no blinking
 delay(20);                      
 LcdCommandWrite(0x01);  // clear display, set cursor position to zero  
 delay(100);                      
 LcdCommandWrite(0x80);  // display control:
                         // turn display on, cursor on, no blinking
 delay(20);                      
}

void loop (void) {
  LcdCommandWrite(0x02);  // set cursor position to zero  
  delay(10);                     
  // Write the welcome message 
  LcdDataWrite('H');
  LcdDataWrite('o');
  LcdDataWrite('l');
  LcdDataWrite('a');
  LcdDataWrite(' ');
  LcdDataWrite('C');
  LcdDataWrite('a');
  LcdDataWrite('r');
  LcdDataWrite('a');
  LcdDataWrite('c');
  LcdDataWrite('o');
  LcdDataWrite('l');
  LcdDataWrite('a');
  delay(500);
}
