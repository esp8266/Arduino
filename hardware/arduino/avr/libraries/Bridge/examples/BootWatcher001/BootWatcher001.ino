
/*
  Arduino Yun Boot watcher
 
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
 modified 26 May 2013
 by Tom Igoe
 
 This example code is in the public domain.
 */

long baud = 115200;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int ledState = HIGH;   // whether the LED is high or low      

String bootString = "";
int bootLineCount = 0;
boolean booting = true;

void setup() {
  Serial.begin(baud);        // open serial connection to Linino
  Serial1.begin(baud);       // open serial connection via USB-Serial

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);    
  digitalWrite(led, ledState);   // turn the LED on (HIGH is the voltage level)
  while(booting) {
    listenForBoot();
  }
  delay(500);
}


void loop() {
  // After booting, become a serial terminal:
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

void listenForBoot() {
  char c;
  if (Serial1.available()) {     // got anything from Linino?         
    c = (char)Serial1.read();    // read from Linino  
    
    if (c == '\n') {             // clear the bootString every newline
      bootLineCount++;                // increment the boot line counter
      Serial.println(bootLineCount);  // print the count
      bootString = "";           // clear the boot string
    } 
    else {                       // anything other than newline, add to string
      bootString += c;
    }
  }

  // look for the final boot string message:
  if (bootString.endsWith("entered forwarding state")) {
    Serial1.println(); 
  }

  // look for the command prompt:
  if (bootString.endsWith(":/#")) {
    Serial.println("Ready for action.");
    booting = false;
  }
}
