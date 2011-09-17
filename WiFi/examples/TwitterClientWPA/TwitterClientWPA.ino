/*
  Twitter Client with Strings
 
 This sketch connects to Twitter using using an Arduino WiFi shield. 
 It parses the XML returned, and looks for <text>this is a tweet</text>
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * WiFi shield attached to pins 10, 11, 12, 13
 
 created 15 Sep 2011
 by Tom Igoe
 
 This code is in the public domain.
 
 */
#include <SPI.h>
#include <WiFi.h>

char ssid[] = "yourNetwork";          //  your network SSID (name) 
char pass[] = "secretPassword";   // your network password
int status = WL_IDLE_STATUS;

// initialize the library instance:
WiFiClient client;

const int requestInterval = 30*1000;    // delay between requests; 30 seconds

IPAddress server(199,59,149,200);      // api.twitter.com

boolean requested;                     // whether you've made a request since connecting
long lastAttemptTime = 0;              // last time you connected to the server, in milliseconds

String currentLine = "";               // string to hold the text from server
String tweet = "";                     // string to hold the tweet
boolean readingTweet = false;          // if you're currently reading the tweet

void setup() {
  // reserve space for the strings:
  currentLine.reserve(256);
  tweet.reserve(150);
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  else {
    Serial.println("Connected to wifi");
    connectToServer();
  }
}



void loop()
{
  if (client.connected()) {
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();
      
      // add incoming byte to end of line:
      currentLine += inChar; 

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      } 
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<text>")) {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true; 
        tweet = "";
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<') {
          tweet += inChar;
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          Serial.println(tweet);   
          // close the connection to the server:
          client.stop(); 
        }
      }
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(server, 80)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    client.println("GET /1/statuses/user_timeline.xml?screen_name=arduinoteam HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}   

