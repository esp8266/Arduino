/*
 Wifi Signal Strength Indicator
 
 This example demonstrates the use of the bridge and process libraries 
 to communicate between the Arduino side and the linux side of the Arduino Yun. 
 
 The Linux script returns the strength of the wifi signal.
 
 The Arduino sketch uses LEDs to indicate whether the current value of 
 the signal strength is above, below, or the same as the last value 
 
 The circuit: 
 * LEDs on pins 8, 9, and 10
 * Built-in LED on pin 13
 
 The script:
 The following one line script must exist in the /root directory of the 
 linux file system, in a file named "wifiStrength.sh", and it must be executable:
 
 tail -1 /proc/net/wireless | cut  -c22-23
 
 created 06 June 2013
 by Michael Shiloh
 modified 08 June 2013
 by Tom Igoe
 
 This example code is in the public domain
 
 */


#include <Process.h>

// global variable to store the last value of the signal strength
int lastValue;

void setup() {
  // set up LED pins as outputs:
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(13,OUTPUT);

  // Indicate that you're ready by flashing pin 13 LED twice
  for (int flash = 0; flash < 3; flash++) {
    digitalWrite(13,HIGH);
    delay(200);
    digitalWrite(13,LOW);
    delay(800);
  }
  // initialize Serial and Bridge:
  Serial.begin(9600);
  Bridge.begin();

  // Indicate that setup is finished
  digitalWrite(13,HIGH);
}

void loop() {
  int value = 0;      // the signal strength as an integer
  String result;      // the result of the process as a String
  Process wifiCheck;  // the process itself


  // Run the script on the linux side. Note that any word 
  //or text separated by a tab or space is considered 
  //an additional parameter:
  wifiCheck.begin("ash");
  wifiCheck.addParameter("/root/wifiStrength.sh");
  wifiCheck.run();

  // If the process has sent any characters:
  while (wifiCheck.available()>0) {
    result = wifiCheck.readString(); // read the result into a string
    value = result.toInt();  // parse the string as an int
  }

  //  for debugging
  Serial.print("previous strength:");  
  Serial.print(lastValue);
  Serial.print("\tcurrent strength:");  
  Serial.println(value);

  // indicate the relative string by lighting the appropriate LED
  allOff();      // turn off all the LEDS

  if (value > lastValue) {     // if the signal's getting stronger
    digitalWrite(10, HIGH);
  }
  else if (value < lastValue){ // if the signal's getting weaker
    digitalWrite(8, HIGH);
  }
  else {                       // if the signal's stayed steady
    digitalWrite(9, HIGH);
  }

  lastValue = value; // record this value for next time
  delay(10);      // small delay before next time through the loop
}


void allOff() {
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW); 
}






