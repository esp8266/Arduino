/*
 * Echo Server
 *
 * Echoes back the headers of the web request.  Good for
 * learning how the HTTP protocol works.
 */

#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 0, 0, 177 };

Server server(80);

void setup()
{
  Client client(255);
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  server.begin();
}

void loop()
{
  char buf[512];
  int i = 0;
  Client client = server.available();
  if (client) {
    boolean previous_is_newline = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && previous_is_newline) {
          buf[i] = 0;
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<pre>");
          client.println(buf);
          client.println("</pre>");
          break;
        }
        if (i < 511)
          buf[i++] = c;
        if (c == '\n')
          previous_is_newline = true;
        else if (c != '\r')
          previous_is_newline = false;
      }
    }
    client.stop();
  }
}
