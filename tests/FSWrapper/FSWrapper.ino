#include <ESP8266WiFi.h>
#include "FS.h"

void fail(const char* msg) {
  Serial.println(msg);
  while(true) {
    yield();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("\n\nFS test\n");

  if (!mount(SPIFFS, "/")) {
    fail("mount failed");
  }

  String text = "write test";
  {
    File out = SPIFFS.open("/tmp.txt", "w");
    if (!out) {
      fail("failed to open tmp.txt for writing");
    }
    out.print(text);
  }

  {
    File in = SPIFFS.open("/tmp.txt", "r");
    if (!in) {
      fail("failed to open tmp.txt for reading");
    }
    Serial.printf("size=%d\r\n", in.size());
    if (in.size() != text.length()) {
      fail("invalid size of tmp.txt");
    }
    Serial.print("Reading data: ");
    in.setTimeout(0);
    String result = in.readString();
    Serial.println(result);
    if (result != text) {
      fail("invalid data in tmp.txt");
    }
  }

  {
    for (int i = 0; i < 10; ++i) {
      String name = "seq_";
      name += i;
      name += ".txt";

      File out = SPIFFS.open(name, "w");
      if (!out) {
        fail("can't open seq_ file");
      }

      out.println(i);
    }
  }
  {
    Dir root = SPIFFS.openDir("/");
    while (root.next()) {
      String fileName = root.fileName();
      File f = root.openFile("r");
      Serial.printf("%s: %d\r\n", fileName.c_str(), f.size());
    }
  }


  Serial.println("success");
}

void loop() {
}
