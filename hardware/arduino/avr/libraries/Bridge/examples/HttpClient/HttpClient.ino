
#include <HttpClient.h>

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
}

void loop() {
  HttpClient client;
  client.get("http://my.server.address/file.php");
  
  char c = client.read();
  if (c=='1')
    digitalWrite(13, HIGH);
  if (c=='0')
    digitalWrite(13, LOW);
  
  delay(5000);
}


