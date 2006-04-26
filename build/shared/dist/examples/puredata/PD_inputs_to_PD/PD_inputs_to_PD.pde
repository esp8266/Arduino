/* Inputs to PD 
 * ------------------- 
 * 
 * This program sends data from a bunch of inputs to PD 
 * over the serial port (works on PC: Windows and Linux, and MAC) 
 * The code reads 3 potentiometers and 6 buttons plugged to Arduino 
 * input and sends the data back to the computer. 
 * 
 * On the other side there will be a PureData sketch running 
 * comport2000 and will use the data to change a sound or video
 * file properties. 
 * 
 * The buttons will be characterized with '1' or '0' depending on
 * their state, while potentiometers will be characterized with a
 * 10 bits integer in the range 0..1024
 * 
 * The first sensor will be marked with 'A' (ascii 65), the second
 * with 'B', and so on. The end of sensor reading is marked with
 * the characters EOLN (ascii 10). 
 * 
 * (cleft) 2005 DojoDave for K3 
 * 
 * @author: David Cuartielles 
 * @context: ID3 - K3 - MAH - Sweden 
 */ 

int ledPin = 13;          // declare the pin with the LED
int potentioMeter = 0;    // declare the analog pin for the potentiometer
int pushButton = 0;       // declare the value
int writeChar = 65;       // declare the first reading as 'A'
int value = 0;            // value to read the different sensors
int ledStatus = LOW;      // status of the LED

void setup() { 
  pinMode(ledPin, OUTPUT); // declare the LED as output 
  Serial.begin(9600);       // intitialize the serial port
} 

void loop() {

  writeChar = 65;    //Sets the sensor idendifier 
                     // character back to 'A'.

  // start reading the potentiometers on the analog pins
  for(potentioMeter=0;potentioMeter<3;potentioMeter++){
    Serial.print(writeChar, BYTE);
    Serial.print(analogRead(potentioMeter));
    Serial.println();
    writeChar = writeChar + 1;
    delay(10);
  }

  // read the pushbuttons
  for(pushButton=2;pushButton<8;pushButton++){
    Serial.print(writeChar, BYTE);
    value = digitalRead(pushButton);   // reads the value at a digital input           
    if (value)
    {
 	Serial.print('0');
    } else {
	Serial.print('1');
    }
    Serial.println();
    writeChar = writeChar + 1;
    delay(10);
  }

  delay(100); 
  ledStatus = !ledStatus; 
  digitalWrite(ledPin, ledStatus); 
}
