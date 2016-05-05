/*
  ESP8266 mDNS-SD responder and query sample

  This is an example of announcing and finding services.
  
  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to two ESP8266 boards
  - The last one powered on should now find the other.
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// Put your own Wifi Router SSID/KEY here
// remember that password len should be >7 to get it working
// If you leave this wrong default values, ESP will try to connect
// to last SSID/PASS that worked on this device (if any of course)
const char* ssid = "******";
const char* password = "******";
char hostString[16] = {0};

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\r\nsetup()");

  sprintf(hostString, "ESP_%06X", ESP.getChipId());
  Serial.print("Hostname: ");
  Serial.println(hostString);
  WiFi.hostname(hostString);

 // If sketch as no default SSID and bad PSK (len <8)
  // it should try to connect to SDK saved one (if any)
  Serial.print(F("Connecting with SSID of "));
  if ( strlen(password)<8 ) {
    Serial.printf( "SDK '%s'\r\n", WiFi.SSID().c_str() );
    // If autoconnect is disabled force connection
    if (!WiFi.getAutoConnect()) 
      WiFi.begin();
  } else {
    Serial.printf( "Sketch '%s'\r\n", ssid ); 
    WiFi.begin(ssid, password);
  }
  // Will be blocked in this while loop until connected
  while (WiFi.waitForConnectResult() != WL_CONNECTED){
    delay(1000);
    Serial.printf("%4ld sec : ...\n", millis()/1000);
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

 if (!MDNS.begin(hostString)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  MDNS.addService("esp", "tcp", 8080); // Announce esp tcp service on port 8080

  Serial.println("Sending mDNS query");
  int n = MDNS.queryService("esp", "tcp"); // Send out query for esp tcp services
  Serial.println("mDNS query done");
  if (n == 0) {
    Serial.println("no services found");
  }
  else {
    Serial.print(n);
    Serial.println(" service(s) found");
    for (int i = 0; i < n; ++i) {
      // Print details for each service found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
    }
  }
  Serial.println();
  
  Serial.println("loop() next");
}

void loop() {
  // put your main code here, to run repeatedly:

}
