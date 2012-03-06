/*
  Arduino Programs Blink
 
 This sketch demonstrates the Keyboard library.
 
 When you connect pin 12 to ground, it creates a new
 window with a key combination (CTRL-N),
 then types in the Blink sketch, then auto-formats the text
 using another key combination (CTRL-T), then 
 uploads the sketch to the currently selected Arduino using 
 a final key combination (CTRL-U).
 
 Circuit:
 * Arduino Leonardo
 * wire to connect D12 to ground.
 
 created 5 Mar 2012
 by Tom Igoe
 
 This example is in the public domain
 
 http://www.arduino.cc/en/Tutorial/KeyboardReprogram
 */
void setup() {
  // make pin 12 an input and turn on the 
  // pullup resistor so it goes high unless
  // connected to ground:
  pinMode(12, INPUT_PULLUP);
}

void loop() {
  while (digitalRead(12) == HIGH) {
    // do nothing until pin 12 goes low
    delay(500);
  }
  // new document:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('N');
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
    Keyboard.type(KEY_BACKSPACE); 
  }
  // make it 1000 instead:
  Keyboard.println("1000);");
  Keyboard.println("digitalWrite(13, LOW);");
  Keyboard.println("delay(1000);");
  // tidy up:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('T');
  Keyboard.releaseAll();
  delay(3000);
  // upload code:
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('U');
  Keyboard.releaseAll();

  // wait for the sweet oblivion of reprogramming:
  while(true);
}



