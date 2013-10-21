/*
  ControlBySMS

  Demonstrates using an SMS message to a Twilio account to turn an LED
  on the Yun board on and off using the Temboo Arduino Yun SDK.
  Sending a SMS with the text "LED ON" to your Twilio phone number
  will turn on the LED on the Yun.  Sending "LED OFF" will turn it off.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples.
  If you don't already have one, you can register for a free Temboo account at
  http://www.temboo.com

  Since this sketch uses Twilio to retrieve the SMS, you'll also need a valid
  Twilio account. You can create one for free at https://www.twilio.com.

  The sketch needs your Twilio Account SID and Auth Token you get when you
  register with Twilio.  Make sure to use the Account SID and Auth Token from
  your Twilio Dashboard (not your test credentials from the Dev Tools panel).

  Normally, Twilio expects to contact a web site you provide to get a response
  when an SMS message is received for your Twilio number.  In this case, we
  don't want to send any response (and we don't want to have to set up a web
  site just to receive SMS messages.)  You can use a URL that Twilio provides
  for this purpose.  When a message is received and sent to the Twilio "twimlets"
  URL, it returns a code meaning "no response required."  To set this up:

  1. Log in to your Twilio account and go to this URL:

      https://www.twilio.com/user/account/phone-numbers/incoming

  2. Select the Twilio number you want to receive SMS messages at.

  3. Put this URL in the "SMS Request URL" field:

     http://twimlets.com/echo?Twiml=%3CResponse%3E%3C%2FResponse%3E

     See this link to Twilio's FAQ for details:

     https://www.twilio.com/help/faq/sms/how-can-i-receive-sms-messages-without-responding

  4. Click the "Save Changes" button at the bottom of the page.

  Your account will now receive SMS messages, but won't send any responses.

  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet.

  Looking for another API? We've got over 100 in our Library!

  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
// as described in the footer comment below



/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

// the Account SID from your Twilio account
const String TWILIO_ACCOUNT_SID = "xxxxxxxxxxxxxxxxxx";

// the Auth Token from your Twilio account
const String TWILIO_AUTH_TOKEN = "xxxxxxxxxxxxxxxxx";

// only act on messages sent from this phone number. (e.g. 15415551212)
const String FROM_PHONE_NUMBER = "xxxxxxxxxxx";

// how often (in milliseconds) to check for new SMS messages.
const unsigned long SMS_CHECK_PERIOD = 60000;

// keep track of when we last checked for new messages
// (initialize it to SMS_CHECK_PERIOD seconds ago so
// we do the first check as soon as the sketch starts.)
unsigned long lastSMSCheckTime = -SMS_CHECK_PERIOD;

// keep track of the ID of the last SMS message we processed.
// (we only need to process newer messages)
String lastSid;

// we'll be turning the LED built in to the Yun on and off
// to simulate controlling some device.  That LED is on pin 13.
int LED_PIN = 13;

int numRuns = 1;   // execution count, so this doesn't run forever
int maxRuns = 10;   // the max number of times the Twitter HomeTimeline Choreo should run


void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while (!Serial);

  // tell the board to treat the LED pin as an output.
  pinMode(LED_PIN, OUTPUT);

  // start with the LED off
  digitalWrite(LED_PIN, LOW);

  // initialize the connection to the Linino processor.
  Bridge.begin();

  // Twilio will report old SMS messages.  We want to
  // ignore any existing control messages when we start.
  Serial.println("Ignoring any existing control messages...");
  checkForMessages(true);

}

void loop()
{

  // get the number of milliseconds the CPU has been running.
  unsigned long now = millis();

  // see if it's time to check for new SMS messages.
  if (now - lastSMSCheckTime >= SMS_CHECK_PERIOD) {

    // it's time to check for new messages
    // save this time so we know when to check next
    lastSMSCheckTime = now;

    if (numRuns <= maxRuns) {
      Serial.println("Checking for new SMS messages - Run #" + String(numRuns++));

      // execute the choreo and don't ignore control messages.
      checkForMessages(false);
    } else {
      Serial.println("Already ran " + String(maxRuns) + " times.");
    }
  }
}

/*
This function executes the Twilio > SMSMessages > ListMessages choreo
and processes the results.

If ignoreCommands is 'true', this function will read and process messages
updating 'lastSid', but will not actually take any action on any commands
found.  This is so we can ignore any old control messages when we start.

If ignoreCommands is 'false', control messages WILL be acted on.
*/
void checkForMessages(bool ignoreCommands) {

  // we need a TembooChoreo object to send a Choreo request to Temboo
  TembooChoreo ListMessagesChoreo;

  ListMessagesChoreo.begin();

  // set Temboo account credentials
  ListMessagesChoreo.setAccountName(TEMBOO_ACCOUNT);
  ListMessagesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  ListMessagesChoreo.setAppKey(TEMBOO_APP_KEY);

  // identify the Temboo Library choreo to run (Twilio > SMSMessages > ListMessages)
  ListMessagesChoreo.setChoreo("/Library/Twilio/SMSMessages/ListMessages");

  // set the choreo inputs
  // see https://www.temboo.com/library/Library/Twilio/SMSMessages/ListMessages/
  // for complete details about the inputs for this Choreo

  // the first input is a your Twilio AccountSID
  ListMessagesChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID);

  // next is your Twilio Auth Token
  ListMessagesChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);

  // we only want to know about messages sent from our designated phone number
  ListMessagesChoreo.addInput("From", FROM_PHONE_NUMBER);

  // Twilio can return information about up to 1000 messages at a time.
  // we're only interested in the 3 most recent ones.  Note that if
  // this account receives lots of messages in quick succession,
  // (more than 3 per minute in this case), we might miss some control
  // messages.  But if we request too many messages, we might run out of
  // memory on the Arduino side of the Yun.
  ListMessagesChoreo.addInput("PageSize", "3");

  // We want the response in XML format to process with our
  // XPath output filters.
  ListMessagesChoreo.addInput("ResponseFormat", "xml");

  // we don't want everything from the output, just the
  // message IDs (the Sids) and the message texts
  ListMessagesChoreo.addOutputFilter("sid", "Sid", "Response");
  ListMessagesChoreo.addOutputFilter("text", "Body", "Response");

  // tell the Choreo to run and wait for the results. The
  // return code (returnCode) will tell us whether the Temboo client
  // was able to send our request to the Temboo servers
  unsigned int returnCode = ListMessagesChoreo.run();

  // a return code of zero (0) means success
  if (returnCode == 0) {

    // Need a string to hold the list of message IDs.
    String messageSids;

    // Need a string to hold the texts of the messages.
    String messageTexts;

    // when the choreo results are available, process them.
    // the output filters we specified will return comma delimited
    // lists containing the Sids and texts of the messages
    // from our designated phone number.

    while (ListMessagesChoreo.available()) {

      // output names are terminated with '\x1F' characters.
      String name = ListMessagesChoreo.readStringUntil('\x1F');
      name.trim();
      //Serial.println(name);

      // output values are terminated with '\x1E' characters.
      String data = ListMessagesChoreo.readStringUntil('\x1E');
      data.trim();
      //Serial.println(data);

      // assign the data to the appropriate string based on the name
      if (name == "sid") {
        messageSids = data;
      } else if (name == "text") {
        messageTexts = data;
      }
    }

    // done reading output, close the Choreo to free up resources.
    ListMessagesChoreo.close();

    // parse the comma delimited lists of messages and Sids
    processMessages(messageTexts, messageSids, ignoreCommands);

  } else {
    // a non-zero return code means there was an error
    // read and print the error message
    while (ListMessagesChoreo.available()) {
      char c = ListMessagesChoreo.read();
      Serial.print(c);
    }
  }
}

/*
This function processes the lists of message texts and Sids.
If a message contains a comma as part of the
message text, that message will be enclosed in double quotes
(") in the list.  Example:

  A message,"Hey, now",Another message text

If the message contains double quotes, it will be enclosed in
double quotes AND the internal quotes will be doubled.
Example:

  "Hi ""Sam"" the man", Led on

NOTE! We are assuming that Twilio returns more recent messages
first.  This isn't officially documented by Twilio, but we've
not seen any other case.

'messageTexts' is a String containing a comma separated list of
message texts with commas and quotes escaped as described above.

'messageSids' is a String containing a comma separated list of
message Sids.  Sids should not contain embedded commas or quotes.

'ignoreCommands' is a boolean. 'true' means and control messages
will not be acted upon.  'false' means control messages will be
acted upon in the usual way.
*/
void processMessages(String messageTexts, String messageSids, bool ignoreCommands) {

  // proceed if we received at least one message
  if (messageSids.length() > 0) {
    int i = -1;
    int sidsStart = 0;
    int textsStart = 0;
    String sid;
    String text;
    bool ledUpdated = false;

    // go through the list until we run out of items
    // or otherwise know we can stop
    do {

      // Output filter list items are separated by commas
      // find the start of the next item in the list
      i = messageSids.indexOf(',', sidsStart);
      if (i >= 0) {

        //extract a single Sid from the list.
        sid = messageSids.substring(sidsStart, i);
        sidsStart = i + 1;

        // find the start of the next text in the list.
        // Note that we have to be prepared to handle embedded
        // quotes and commans in the message texts.
        // The standard Arduino String class doesn't handle
        // this, so we have to write our own function to do it.
        i = quotedIndexOf(messageTexts, ',', textsStart);
        if (i >= 0) {

          // extract a single message text from the list.
          text = messageTexts.substring(textsStart, i);
          textsStart = i + 1;

          // process the Sid and text to see if it's a
          // control message.
          ledUpdated = processMessage(sid, text, ignoreCommands);
        }
      } else {

        // the last item in the lists won't have a comma at the end,
        // so we have to handle them specially.
        // Since we know this is the last item, we can just
        // take the rest of the string for the Sid and text.
        sid = messageSids.substring(sidsStart);
        text = messageTexts.substring(textsStart);

        // process the last item.
        ledUpdated = processMessage(sid, text, ignoreCommands);
      }

      // keep going until either we run out of list items
      // or we run into a message we processed on a previous run.
    } while ((i >= 0) && (sid != lastSid));

    // print what we've found to the serial monitor,
    // just so we can see what's going on.
    if (sid == lastSid) {
      if (ledUpdated)
        Serial.println("Control message processed.");
      else
        Serial.println("No new control messages received.");
    } else {
      Serial.println("No control messages received.");
    }
  } else {
    Serial.println("No messages found");
  }
}

/*
This function knows how to tell if a message is a control message
or not.  It also knows know to control whatever it is we're controlling
(the state of the LED on pin 13 in this case.)

A message with the text "LED ON" turns the LED on.
A message with the text "LED OFF" turns the LED off.
(Case is ignored.)

If 'ignoreCommands' is true, the actions described above will NOT
take place.

It also updates the 'lastSid' global variable when
a control message is processed.

It returns 'true' if the message was a control message, and
'false' if it wasn't or if we've already processed this message.
*/
bool processMessage(String sid, String text, bool ignoreCommands) {

  // a flag to indicate whether this was a control message or not
  bool ledUpdated = false;

  // if we haven't already processed this message
  if (sid != lastSid) {

    if (text.equalsIgnoreCase("LED ON")) {

      if (!ignoreCommands) {
        //turn on the LED
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      }
      ledUpdated = true;
    } else if (text.equalsIgnoreCase("LED OFF")) {
      if (!ignoreCommands) {
        //turn off the LED
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }
      ledUpdated = true;
    }

    // If the LED state was updated, remember the Sid if this message.
    if (ledUpdated)
      lastSid = sid;
  }
  return ledUpdated;
}

/*
This function finds the index of a delimiter character in a String,
ignoring delimiters that appear inside double-quotes.
*/
int quotedIndexOf(String s, char delim, int start) {
  bool inQuotes = false;
  char c;
  int index = -1;
  const char QUOTE = '"';
  do {
    c = s[start++];
    if (c == QUOTE)
      inQuotes = !inQuotes;
    else if (c == delim && !inQuotes)
      index = --start;
  } while ((c != '\0') && (index < 0));
  return index;
}


/*
  IMPORTANT NOTE: TembooAccount.h:

  TembooAccount.h is a file referenced by this sketch that contains your Temboo account information.
  You'll need to edit the placeholder version of TembooAccount.h included with this example sketch,
  by inserting your own Temboo account name and app key information. The contents of the file should
  look like:

  #define TEMBOO_ACCOUNT "myTembooAccountName"  // your Temboo account name
  #define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
  #define TEMBOO_APP_KEY  "xxx-xxx-xxx-xx-xxx"  // your Temboo app key

  You can find your Temboo App Key information on the Temboo website,
  under My Account > Application Keys

  The same TembooAccount.h file settings can be used for all Temboo SDK sketches.

  Keeping your account information in a separate file means you can save it once,
  then just distribute the main .ino file without worrying that you forgot to delete your credentials.
*/

