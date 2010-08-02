/*
  Twitter  client
 
 This sketch connects to Twitter (http://www.twitter.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 Circuit:
 * Switch connected to digital pin 2
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 modified 23 July 2010
 by Tom Igoe
 
  http://www.tigoe.net/pcomp/code/category/arduinowiring/873
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet.h>

// pin that the pushButton is connected to:
const int buttonPin = 2;

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// assign an IP address for the controller:
byte ip[] = { 
  192,169,1,20 };
byte gateway[] = {
  192,168,1,1};	
byte subnet[] = { 
  255, 255, 255, 0 };

// The server you want to connect to (twitter.com)
byte server[] = { 
  168,143,162,68};

// initialize the library instance:
Client client(server, 80);

int lastButtonState = LOW;            // last state of the pushbutton
boolean connectedLastTime = false;    // state of the connection last time through the main loop

void setup() {
  // make the pushbutton's pin an input:
  pinMode(buttonPin, INPUT);
  // start the ethernet connection and serial port:
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && connectedLastTime) {
    Serial.println("disconnecting.");
    client.stop();
  }
  // read the pushbutton input pin:
  int buttonState = digitalRead(buttonPin);
  // make a connection only when the button goes from LOW to HIGH:
  if ((buttonState != lastButtonState) && (buttonState == HIGH)) {
    // if you're not connected, then connect:
    if(!client.connected()) {
      sendData();
    }
    // save the current button state as the last state, 
    //for next time through the loop

  }
  lastButtonState = buttonState;
  // store the state of the connection for next time through
  // the loop:
  connectedLastTime = client.connected();
}


// this method makes a HTTP connection to the server:
void sendData() {
  // if there's a successful connection:
  if (client.connect()) {
    Serial.println("connecting...");
    // send the HTTP POST request:
    client.print("POST http://twitter.com/statuses/update.json HTTP/1.1\n");
    client.print("Host: twitter.com\n");

    // fill in your twitter login here. It needs to be 
    // formatted like this:  username:password
    // then it needs to be  base64_encoded.  
    // you can do that online at many sites, including this one:
    // http://www.tools4noobs.com/online_php_functions/base64_encode/
    // once encoded, it'll look like a random string of characters  
    client.print("Authorization: Basic XXXXXXXXXXXXXXXXXXXXXXXX\n");
    client.print("Content-type: application/x-www-form-urlencoded\n");
    // content length of the status message that follows below:
    client.print("Content-Length: 26\n");
    client.println("Connection: Close\n");

    // generate a random number to add to the status message
    // to avoid duplicate status messages. This is a hack to get around
    // Twitter's requirement of unique status messages:
    int randomNumber = random(9);
    // here's the status message:
    client.print("status=Hello from Arduino");
    client.print(randomNumber);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}









