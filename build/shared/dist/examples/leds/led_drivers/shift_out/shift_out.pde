/* Shift Out
 * ---------
 * 
 * This example uses the LED driver 4794 from Philips to drive
 * 8 LEDs at once. The 4794 is a chip that can be chained so that
 * we will be able of adding 8 outputs each time you add a new chip
 *
 * We use four pins to connect to the chip:
 * 
 * - data: this one sends the data out to the chip
 *
 * - strob: controls the load of data to the chip's output
 * 
 * - clock: synchronizes the load of data
 *
 * - oe: turns the output on/off, we use it to control the luminosity of the LEDs
 *
 * Pictures at:
 * http://arduino.berlios.de/index.php/Tutorial/LEDDriver
 * 
 * (copyleft) 2005 K3, Malmo University 
 * @author: David Cuartielles, Marcus Hannerstig
 * @hardware: David Cuartielles, Marcos Yarza
 * @project: SMEE - Experiential Vehicles
 *
 */


int data = 9;
int strob = 8;
int clock = 10;
int oe = 11;
int count = 0;
int dato = 0;  // dato is a varible we use to send data to the LEDs

void setup()
{
  //Serial.begin(9600);    // uncomment the serial-related lines to monitor the program's progress
  pinMode(data, OUTPUT);  // declare all the control pins as outputs
  pinMode(clock, OUTPUT);
  pinMode(strob, OUTPUT);
  pinMode(oe, OUTPUT);
}


// sends a pulse to the 4794 indicating that
// it is time to load data
void PulseClock(void) {
  digitalWrite(clock, LOW);
  delayMicroseconds(20);
  digitalWrite(clock, HIGH);
  delayMicroseconds(50);
  digitalWrite(clock, LOW);
}

void loop()
{
  dato = 129;  // if dato equals 129 the first and last LED will be on
   
  // go through the "dato" variable and send it bit by bit over the data pin
  for (count = 0; count < 8; count++) {
    digitalWrite(data, dato & 01);
    //Serial.print((dato & 01) + 48, BYTE);
    dato>>=1;
    if (count == 7){
      digitalWrite(oe, LOW);
      digitalWrite(strob, HIGH);
    }
    PulseClock();
    digitalWrite(oe, HIGH);
  }
   
  delayMicroseconds(20);
  digitalWrite(strob, LOW);
  delay(100);
   
  //Serial.println();
  delay(100);                  // waits for a second
}
