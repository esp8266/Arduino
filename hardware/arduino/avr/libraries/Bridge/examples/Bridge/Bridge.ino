
//#include <Bridge.h>
#include <Mailbox.h>

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  Serial.begin(9600);
}

void loop() {
  while (Mailbox.messageAvailable()) {
    String msg;
    Mailbox.readMessage(msg);
    process(msg);
  }
  delay(100); // Poll every 0.100s
}

void process(String command) {
  Serial.println(command);
  // "digital/13"     -> digitalRead(13)
  // "digital/13/1"   -> digitalWrite(13, HIGH)
  // "analog/2/123"   -> analogWrite(2, 123)
  // "analog/2"       -> analogRead(2)
  // "mode/13/input"  -> pinMode(13, INPUT)
  // "mode/13/output" -> pinMode(13, OUTPUT)

  // is digital command?
  if (command.startsWith("digital/")) {
    // extract subcommand (after the "/")
    command = command.substring(8);
    digitalCommand(command);

  } 
  // is analog command?
  else if (command.startsWith("analog/")) {
    // extract subcommand (after the "/")
    command = command.substring(7);
    analogCommand(command);

  } 
  // is mode command?
  else if (command.startsWith("mode/")) {
    // extract subcommand (after the "/")
    command = command.substring(5);
    modeCommand(command);
  }
}

void digitalCommand(String command) {
  int pin, value;

  // Find the position of the "/" inside the command
  int slashIndex = command.indexOf("/");

  // If there are no slashes
  if (slashIndex == -1) {
    // then we are in the following case:
    // "digital/13"     -> digitalRead(13)

    // so we can extract the pin number from the remainder of the command string
    pin = command.toInt();
  } 
  else {
    // else, we found a slash, so we are in the following case:
    // "digital/13/1"   -> digitalWrite(13, HIGH)
    
    // we must estract pin number before the "/"
    pin = command.substring(0, slashIndex).toInt();
    // and value after the "/"
    value = command.substring(slashIndex+1).toInt();
    digitalWrite(pin, value);
  }
  reportDigitalRead(pin, true);
}

void analogCommand(String command) {
  int pin, value;
  if (command.indexOf("/") != -1) {
    pin = command.substring(0, command.indexOf("/")).toInt();
    value = command.substring(command.indexOf("/") + 1, command.length()).toInt();
    analogWrite(pin, value);
  } 
  else {
    pin = command.toInt();
  }
  reportAnalogRead(pin, true);
}

void modeCommand(String command) {
  int pin;
  String strValue;
  pin = command.substring(0, command.indexOf("/")).toInt();
  strValue = command.substring(command.indexOf("/") + 1, command.length());
  if (strValue == "output") {
    pinMode(pin, OUTPUT);
    reportPinMode(pin, strValue);
  } 
  else if (strValue == "input") {
    pinMode(pin, INPUT);
    reportPinMode(pin, strValue);
  }
}

void reportPinMode(int pin, String mode) {
  String json = "{\"pin\":";
  json += pin;
  json += ", \"mode\": \"";
  json += mode;
  json += "\"}";
  Mailbox.writeJSON(json);
}

void reportDigitalRead(int pin, boolean dataset) {
  int value = digitalRead(pin);

  String json = "{\"pin\":";
  json += pin;
  json += ", \"value\": ";
  json += value;
  json += "}";
  Mailbox.writeJSON(json);

  if (dataset) {
    String key = "D";
    key += pin;
    Bridge.put(key.c_str(), String(value).c_str());
  }
}

void reportAnalogRead(int pin, boolean dataset) {
  int value = analogRead(pin);

  String json = "{\"pin\":";
  json += pin;
  json += ", \"value\": ";
  json += value;
  json += "}";
  Mailbox.writeJSON(json);

  if (dataset) {
    String key = "A";
    key += pin;
    Bridge.put(key.c_str(), String(value).c_str());
  }
}

