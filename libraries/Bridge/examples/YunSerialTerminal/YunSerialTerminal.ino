/*
  Arduino Yún USB-to-Serial

 Allows you to use the Yún's 32U4 processor as a
 serial terminal for the Linux side on the Yún.

 Upload this to an Arduino Yún via serial (not WiFi) then open
 the serial monitor at 115200 to see the boot process of Linux.
 You can also use the serial monitor as a basic command line
 interface for Linux using this sketch.

 From the serial monitor the following commands can be issued:

 '~' followed by '0' -> Set the UART speed to 57600 baud
 '~' followed by '1' -> Set the UART speed to 115200 baud
 '~' followed by '2' -> Set the UART speed to 250000 baud
 '~' followed by '3' -> Set the UART speed to 500000 baud
 '~' followed by '~' -> Sends the bridge's shutdown command to
                        obtain the console.

 The circuit:
 * Arduino Yún

 created March 2013
 by Massimo Banzi
 modified by Cristian Maglie

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/YunSerialTerminal

 */


long linuxBaud = 250000;

void setup() {
  Serial.begin(115200);      // open serial connection via USB-Serial
  Serial1.begin(linuxBaud);  // open serial connection to Linux
}

boolean commandMode = false;

void loop() {
  // copy from USB-CDC to UART
  int c = Serial.read();              // read from USB-CDC
  if (c != -1) {                      // got anything?
    if (commandMode == false) {       // if we aren't in command mode...
      if (c == '~') {                 //    Tilde '~' key pressed?
        commandMode = true;           //       enter in command mode
      } else {
        Serial1.write(c);             //    otherwise write char to UART
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
      } else if (c == '~') {          //     '~` key pressed?
                                      //        send "bridge shutdown" command
        Serial1.write((uint8_t *)"\xff\0\0\x05XXXXX\x7f\xf9", 11);
        Serial.println("Sending bridge's shutdown command");
      } else {                        //     any other key pressed?
        Serial1.write('~');           //        write '~' to UART
        Serial1.write(c);             //        write char to UART
      }
      commandMode = false;            //     in all cases exit from command mode
    }
  }

  // copy from UART to USB-CDC
  c = Serial1.read();                 // read from UART
  if (c != -1) {                      // got anything?
    Serial.write(c);                  //    write to USB-CDC
  }
}
