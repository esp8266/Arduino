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
 '~' followed by '2' -> Set the UART speed to 250000 baud
 '~' followed by '3' -> Set the UART speed to 500000 baud
 '~' followeb by '~' -> Sends the bridge's shutdown command to
                        obtain the console.

 The circuit:
 * Arduino Yun

 created March 2013
 by Massimo Banzi
 modified by Cristian Maglie

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/YunSerialTerminal

 */


long lininoBaud = 250000;

void setup() {
  Serial.begin(115200);      // open serial connection via USB-Serial
  Serial1.begin(lininoBaud); // open serial connection to Linino
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
      } else if (c == '2') {          //     '2' key pressed?
        Serial1.begin(250000);        //        set speed to 250000
        Serial.println("Speed set to 250000");
      } else if (c == '3') {          //     '3' key pressed?
        Serial1.begin(500000);        //        set speed to 500000
        Serial.println("Speed set to 500000");
      } else if (c == '~') {
        Serial1.write((uint8_t *)"\xff\0\0\x05XXXXX\x0d\xaf", 11);
        Serial.println("Sending bridge's shutdown command");
      } else {                        //     any other key pressed?
        Serial1.write('~');           //        write '~' to Linino
        Serial1.write(c);             //        write char to Linino
      }
      commandMode = false;            //     in all cases exit from command mode
    }
  }
  if (Serial1.available()) {          // got anything from Linino?
    char c = (char)Serial1.read();    // read from Linino
    Serial.write(c);                  // write to USB-serial
  }
}
