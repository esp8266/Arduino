// Simple speed test for filesystem objects
// Released to the public domain by Earle F. Philhower, III

#include <FS.h>
#include <LittleFS.h>

// Choose the filesystem to test
// WARNING:  The filesystem will be formatted at the start of the test!

#define TESTFS LittleFS
//#define TESTFS SPIFFS
//#define TESTFS SDFS

// How large of a file to test
#define TESTSIZEKB 512

void DoTest(FS *fs) {
  if (!fs->format()) {
    Serial.printf("Unable to format(), aborting\n");
    return;
  }
  if (!fs->begin()) {
    Serial.printf("Unable to begin(), aborting\n");
    return;
  }

  uint8_t data[256];
  for (int i = 0; i < 256; i++) {
    data[i] = (uint8_t) i;
  }

  Serial.printf("Creating %dKB file, may take a while...\n", TESTSIZEKB);
  long start = millis();
  File f = fs->open("/testwrite.bin", "w");
  if (!f) {
    Serial.printf("Unable to open file for writing, aborting\n");
    return;
  }
  for (int i = 0; i < TESTSIZEKB; i++) {
    for (int j = 0; j < 4; j++) {
      f.write(data, 256);
    }
  }
  f.close();
  long stop = millis();
  Serial.printf("==> Time to write %dKB in 256b chunks = %ld milliseconds\n", TESTSIZEKB, stop - start);

  f = fs->open("/testwrite.bin", "r");
  Serial.printf("==> Created file size = %d\n", f.size());
  f.close();

  Serial.printf("Reading %dKB file sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("/testwrite.bin", "r");
  for (int i = 0; i < TESTSIZEKB; i++) {
    for (int j = 0; j < 4; j++) {
      f.read(data, 256);
    }
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB * 1024 / (stop - start) * 1000);

  Serial.printf("Reading %dKB file MISALIGNED in flash and RAM sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("/testwrite.bin", "r");
  f.read();
  for (int i = 0; i < TESTSIZEKB; i++) {
    for (int j = 0; j < 4; j++) {
      f.read(data + 1, 256);
    }
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially MISALIGNED in flash and RAM in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB * 1024 / (stop - start) * 1000);


  Serial.printf("Reading %dKB file in reverse by 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("/testwrite.bin", "r");
  for (int i = 0; i < TESTSIZEKB; i++) {
    for (int j = 0; j < 4; j++) {
      if (!f.seek(256 + 256 * j * i, SeekEnd)) {
        Serial.printf("Unable to seek to %d, aborting\n", -256 - 256 * j * i);
        return;
      }
      if (256 != f.read(data, 256)) {
        Serial.printf("Unable to read 256 bytes, aborting\n");
        return;
      }
    }
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB in reverse in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB * 1024 / (stop - start) * 1000);


  Serial.printf("Writing 64K file in 1-byte chunks\n");
  start = millis();
  f = fs->open("/test1b.bin", "w");
  for (int i = 0; i < 65536; i++) {
    f.write((uint8_t*)&i, 1);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to write 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536 / (stop - start) * 1000);

  Serial.printf("Reading 64K file in 1-byte chunks\n");
  start = millis();
  f = fs->open("/test1b.bin", "r");
  for (int i = 0; i < 65536; i++) {
    char c;
    f.read((uint8_t*)&c, 1);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536 / (stop - start) * 1000);


}

void setup() {
  Serial.begin(115200);
  Serial.printf("Beginning test\n");
  Serial.flush();
  DoTest(&TESTFS);
}

void loop() {
  delay(10000);
}
