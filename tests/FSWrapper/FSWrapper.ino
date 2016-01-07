#include <ESP8266WiFi.h>
#include "FS.h"


void fail(const char* msg) {
  Serial.println(msg);
  while (true) {
    yield();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("\n\nFS test\n");

  {
    if (!SPIFFS.format()) {
      fail("format failed");
    }
    Dir root = SPIFFS.openDir("/");
    int count = 0;
    while (root.next()) {
      ++count;
    }
    if (count > 0) {
      fail("some files left after format");
    }
  }


  if (!SPIFFS.begin()) {
    fail("SPIFFS init failed");
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

  {
    Dir root = SPIFFS.openDir("/");
    while (root.next()) {
      String fileName = root.fileName();
      Serial.print("deleting ");
      Serial.println(fileName);
      if (!SPIFFS.remove(fileName)) {
        fail("remove failed");
      }
    }
  }

  {
    File tmp = SPIFFS.open("/tmp1.txt", "w");
    tmp.println("rename test");
  }

  {
    if (!SPIFFS.rename("/tmp1.txt", "/tmp2.txt")) {
      fail("rename failed");
    }
    File tmp2 = SPIFFS.open("/tmp2.txt", "r");
    if (!tmp2) {
      fail("open tmp2 failed");
    }
  }

  {
    FSInfo info;
    if (!SPIFFS.info(info)) {
      fail("info failed");
    }
    Serial.printf("Total: %u\nUsed: %u\nBlock: %u\nPage: %u\nMax open files: %u\nMax path len: %u\n",
                  info.totalBytes,
                  info.usedBytes,
                  info.blockSize,
                  info.pageSize,
                  info.maxOpenFiles,
                  info.maxPathLength
                 );
  }

  {
    if (!SPIFFS.format()) {
      fail("format failed");
    }
    Dir root = SPIFFS.openDir("/");
    int count = 0;
    while (root.next()) {
      ++count;
    }
    if (count > 0) {
      fail("some files left after format");
    }
  }
  {
    File tmp = SPIFFS.open("/tmp.txt", "w");
  }
  {
    File tmp = SPIFFS.open("/tmp.txt", "w");
    if (!tmp) {
      fail("failed to re-open empty file");
    }
  }
  Serial.println("success");
}

void loop() {
}
