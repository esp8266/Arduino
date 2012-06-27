/*
  Arduino Programs Blink
 
 This sketch demonstrates the Keyboard library.
 
 When you connect pin 2 to ground, it creates a new
 window with a key combination (CTRL-N),
 then types in the Blink sketch, then auto-formats the text
 using another key combination (CTRL-T), then 
 uploads the sketch to the currently selected Arduino using 
 a final key combination (CTRL-U).
 
 Circuit:
 * Arduino Leonardo
 * wire to connect D2 to ground.
 
 created 5 Mar 2012
 modified 29 Mar 2012
 by Tom Igoe
 
 This example is in the public domain
 
 http://www.arduino.cc/en/Tutorial/KeyboardReprogram
 */

// use this option for OSX. 
// Comment it out if using Windows or Linux:
char ctrlKey = KEY_LEFT_GUI;
// use this option for Windows and Linux.
// leave commented out if using OSX:
//  char ctrlKey = KEY_LEFT_CTRL;    


void setup() {
  // make pin 2 an input and turn on the 
  // pullup resistor so it goes high unless
  // connected to ground:
  pinMode(2, INPUT_PULLUP);
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  while (digitalRead(2) == HIGH) {
    // do nothing until pin 2 goes low
    delay(500);
  }
  delay(1000);
  // new document:
  Keyboard.press(ctrlKey);
  Keyboard.press('n');
  delay(100);
  Keyboard.releaseAll();
  // wait for new window to open:
  delay(1000);

  // Type out "blink":
  Keyboard.println("void setup() {");
  Keyboard.println("pinMode(13, OUTPUT);");
  Keyboard.println("}");
  Keyboard.println();
  Keyboard.println("void loop() {");
  Keyboard.println("digitalWrite(13, HIGH);");
  Keyboard.print("delay(3000);");
  // 3000 ms is too long. Delete it:
  for (int keystrokes=0; keystrokes < 6; keystrokes++) {
    delay(500);
    Keyboard.write(KEY_BACKSPACE); 
  }
  // make it 1000 instead:
  Keyboard.println("1000);");
  Keyboard.println("digitalWrite(13, LOW);");
  Keyboard.println("delay(1000);");
  Keyboard.println("}");
  // tidy up:
  Keyboard.press(ctrlKey);
  Keyboard.press('t');
  delay(100);
  Keyboard.releaseAll();
  delay(3000);
  // upload code:
  Keyboard.press(ctrlKey);
  Keyboard.press('u');
  delay(100);
  Keyboard.releaseAll();

  // wait for the sweet oblivion of reprogramming:
  while(true);
}






