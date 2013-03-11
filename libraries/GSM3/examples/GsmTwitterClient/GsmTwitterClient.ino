/*
  GSM Twitter Client with Strings
 
 This sketch connects to Twitter using an Arduino GSM shield. 
 It parses the XML returned, and looks for the string <text>this is a tweet</text>
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * GSM shield attached to an Arduino
 * SIM card with a data plan
 
 created 8 Mar 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/GSMExamplesTwitterClient
 
 This code is in the public domain.
 
 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "APN" // replace your GPRS APN
#define GPRS_LOGIN     "LOGIN" // replace with your GPRS login
#define GPRS_PASSWORD  "PASSWORD" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

const unsigned long requestInterval = 30*1000;    // delay between requests: 30 seconds

// API Twitter URL
char server[] = "api.twitter.com";

boolean requested;                     // whether you've made a request since connecting
unsigned long lastAttemptTime = 0;   // last time you connected to the server, in milliseconds

String currentLine = "";               // string to hold the text from server
String tweet = "";                     // string to hold the tweet
boolean readingTweet = false;          // if you're currently reading the tweet

void setup()
{
  // reserve space for the strings:
  currentLine.reserve(256);
  tweet.reserve(150);
  
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
    
  // connection state
  boolean notConnected = true;
  
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
  Serial.println("Connected to GPRS network");
  
  Serial.println("connecting...");
  connectToServer();
}



void loop()
{
  char c;
  if (client.connected())
  {
    if (client.available())
    {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar; 

      // if you get a newline, clear the line:
      if (inChar == '\n')
      {
        currentLine = "";
      } 
      
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if (currentLine.endsWith("<text>"))
      {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true; 
        tweet = "";
      }
      
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet)
      {
        if (inChar != '<')
        {
          tweet += inChar;
        } 
        else
        {
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
  else if (millis() - lastAttemptTime > requestInterval)
  {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

/*
  Connect to API Twitter server and do a request for timeline
*/
void connectToServer()
{
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(server, 80))
  {
    Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    client.println("GET /1/statuses/user_timeline.xml?screen_name=arduino&count=1 HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
