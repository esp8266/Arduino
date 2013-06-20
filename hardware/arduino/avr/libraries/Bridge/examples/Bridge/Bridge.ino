
#include <Bridge.h>

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH); 
}

void loop() {
  while (Bridge.messageAvailable()) {
    uint8_t buff[64];
    int l = Bridge.readMessage(buff, 64);
    process(buff, l);
  }
  delay(100); // Poll every 0.100s
}

void process(uint8_t buff[], int length) {
  // "digital/13/1"   -> digitalWrite(13, HIGH)
  // "digital/13"     -> digitalRead(13)
  // "analog/2/123"   -> analogWrite(2, 123)
  // "analog/2"       -> analogRead(2)
  // "mode/13/input"  -> pinMode(13, INPUT)
  // "mode/13/output" -> pinMode(13, OUTPUT)

  // Sanity check
  if (length < 9 || length > 14)
    return;

  // string terminator    
  buff[length] = '\0';
  
  String command = String((char*)buff);
  
  // digital command
  if (command.indexOf("digital/") == 0) {
    command = command.substring(8);
    digitalCommand(command);

  // analog command  
  } else if (command.indexOf("analog/") == 0) {
    command = command.substring(7);
    analogCommand(command);
  
  // mode command
  } else if (command.indexOf("mode/") == 0) {
    command = command.substring(5);
    modeCommand(command);
  }
}

void digitalCommand(String command) {
  int pin, value;
  if (command.indexOf("/") != -1) {
    pin = command.substring(0, command.indexOf("/")).toInt();
    value = command.substring(command.indexOf("/") + 1, command.length()).toInt();
    digitalWrite(pin, value);
  } else {
    pin = command.toInt();
  }
  reportDigitalRead(pin, true);
}

void analogCommand(String command) {
  int pin, value;
  if (command.indexOf("/") != -1) {
    pin = command.substring(0, command.indexOf("/")).toInt();
    value = command.substring(command.indexOf("/") + 1, command.length()).toInt();
    analogWrite(pin, value);
  } else {
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
  } else if (strValue == "input") {
    pinMode(pin, INPUT);
    reportPinMode(pin, strValue);
  }
}

void reportPinMode(int pin, String mode) {
  String message = "{\"pin\":";
  message += pin;
  message += ", \"mode\": \"";
  message += mode;
  message += "\"}";
  Bridge.writeMessage(message);
}

void reportDigitalRead(int pin, boolean dataset) {
  int value = digitalRead(pin);
  
  String message = "{\"pin\":";
  message += pin;
  message += ", \"value\": ";
  message += value;
  message += "}";
  Bridge.writeMessage(message);

  if (dataset) {
    String key = "D";
    key += pin;
    Bridge.put(key.c_str(), String(value).c_str());
  }
}

void reportAnalogRead(int pin, boolean dataset) {
  int value = analogRead(pin);
  
  String message = "{\"pin\":";
  message += pin;
  message += ", \"value\": ";
  message += value;
  message += "}";
  Bridge.writeMessage(message);

  if (dataset) {
    String key = "A";
    key += pin;
    Bridge.put(key.c_str(), String(value).c_str());
  }
}
