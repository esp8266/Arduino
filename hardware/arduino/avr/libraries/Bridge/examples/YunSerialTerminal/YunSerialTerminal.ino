/*
  Arduino Yun USB-to-Serial
 
 Allows you to use the Yun's 32U4 processor as a
 serial terminal for the linino processor.
 
 Upload this to an Arduino Yun via serial (not WiFi) 
 then open the serial monitor at 115200 to see the boot process
 of the linino processor. You can also use the serial monitor
 as a basic command line interface for the linino processor using 
 this sketch.
 
 From the serial monitor the following commands can be issued:
 
 '~' followed by '0' -> Set the UART speed to 57600 baud
 '~' followed by '1' -> Set the UART speed to 115200 baud
 
 The circuit:
 * Arduino Yun
 
 created March 2013
 by Massimo Banzi
 modified by Cristian Maglie
 
 This example code is in the public domain.
 */

long lininoBaud = 57600;

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;
int ledState = HIGH;   // whether the LED is high or low      


void setup() {
  Serial.begin(115200);      // open serial connection via USB-Serial
  Serial1.begin(lininoBaud); // open serial connection to Linino

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);    
  digitalWrite(led, ledState);   // turn the LED on (HIGH is the voltage level)
}


boolean commandMode = false;

void loop() {
  // copy from virtual serial line to uart and vice versa
  if (Serial.available()) {           // got anything from USB-Serial?
    char c = (char)Serial.read();     // read from USB-serial
    if (commandMode == false) {       // if we aren't in command mode...
      if (c == '~') {                 //    Tilde '~' key pressed?
        commandMode = true;           //       enter in command mode
      } else {
        Serial1.write(c);             //    otherwise write char to Linino
      }
    } else {                          // if we are in command mode...
      if (c == '0') {                 //     '0' key pressed?
        Serial1.begin(57600);         //        set speed to 57600
        Serial.println("Speed set to 57600");
      } else if (c == '1') {          //     '1' key pressed?
        Serial1.begin(115200);        //        set speed to 115200
        Serial.println("Speed set to 115200");
      } else {                        //     any other key pressed?
        Serial1.write('~');           //        write '~' to Linino
        Serial1.write(c);             //        write char to Linino
      }
      commandMode = false;            //     in all cases exit from command mode
    }
    ledState=!ledState;               // invert LED state
    digitalWrite(led, ledState);      // toggle the LED
  }
  if (Serial1.available()) {          // got anything from Linino?         
    char c = (char)Serial1.read();    // read from Linino  
    Serial.write(c);                  // write to USB-serial
  }
}





