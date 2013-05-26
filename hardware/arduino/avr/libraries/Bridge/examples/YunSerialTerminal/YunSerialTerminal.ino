
/*
  Arduino Yun USB-to-Serial
 
 Allows you to use the Yun's 32U4 processor as a
 serial terminal for the linino processor
 
 Upload this to an Arduino Yun via serial (not WiFi) 
 then open the serial monitor at 115200 to see the boot process
 of the linino processor. You can also use the serial monitor
 as a basic command line interface for the linino processor using 
 this sketch.
 
 The circuit:
 * Arduino Yun
 
 created March 2013
 by Massimo Banzi
 
 This example code is in the public domain.
 */

long baud = 115200;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int ledState = HIGH;   // whether the LED is high or low      


void setup() {
  Serial.begin(baud);        // open serial connection to Linino
  Serial1.begin(baud);       // open serial connection via USB-Serial
  Serial.println("Prova");   // Hello USB
  Serial1.println("Prova1"); // Hello Linino

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);    
  digitalWrite(led, ledState);   // turn the LED on (HIGH is the voltage level)
}


void loop() {

  // copy from virtual serial line to uart and vice versa
  if (Serial.available()) {           // got anything from USB-Serial?
    char c = (char)Serial.read();     // read from USB-serial
    Serial1.write(c);                 // write to Linino
    ledState=!ledState;               // invert LED state
    digitalWrite(led, ledState);      // toggle the LED
  }
  if (Serial1.available()) {          // got anything from Linino?         
    char c = (char)Serial1.read();    // read from Linino  
    Serial.write(c);                  // write to USB-serial
  }
}





