/*
 GSM Web Server
 
 A simple web server that shows the value of the analog input pins.
 using a GSM shield.

 Circuit:
 * GSM shield attached
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 8 Mar 2012
 by Tom Igoe
 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN" // replace your GPRS APN
#define GPRS_LOGIN     "login"    // replace with your GPRS login
#define GPRS_PASSWORD  "password" // replace with your GPRS password


// initialize the library instance
GPRS gprs;
GSM gsmAccess;     // include a 'true' parameter for debug enabled
GSMServer server(80); // port 80 (http default)

// timeout
const unsigned long __TIMEOUT__ = 10*1000;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // connection state
  boolean notConnected = true;
  
  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
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
  
  // start server
  server.begin();
  
  //Get IP.
  IPAddress LocalIP = gprs.getIPAddress();
  Serial.println("Server IP address=");
  Serial.println(LocalIP);
}

void loop() {


  // listen for incoming clients
  GSMClient client = server.available();



  if (client)
  {  
    while (client.connected())
    {
      if (client.available())
      {
        Serial.println("Receiving request!");
        bool sendResponse = false;
        while(char c=client.read()) {
          if (c == '\n') sendResponse = true;
        }

     // if you've gotten to the end of the line (received a newline
     // character) 
       if (sendResponse)
       {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(analogRead(analogChannel));
            client.println("<br />");       
          }
          client.println("</html>");
          //necessary delay
          delay(1000);
          client.stop();
        }
      }
    }
  }
}


