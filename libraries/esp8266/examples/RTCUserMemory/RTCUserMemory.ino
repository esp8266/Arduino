// Example: Storing struct data in RTC user memory
//
// Struct data with the maximum size of 512 bytes can be stored in the RTC user memory using the ESP-specifc APIs.
// The stored data can be retained between deep sleep cycles. However, the data might be lost after power cycling the ESP8266.
//
// Created Mar 30, 2016 by Macro Yau.
//
// This example code is in the public domain.

typedef struct {
  byte data[512];
} rtcUserMemory;

rtcUserMemory mem;

void printMemory(bool readFromRtc) {
  char buf[3];
  Serial.print(readFromRtc ? "Read:  " : "Write: ");
  for (int i = 0; i < sizeof(mem); i++) {
    sprintf(buf, "%02X", mem.data[i]);
    Serial.print(buf);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Read struct from RTC user memory
  if (ESP.rtcUserMemoryRead((uint32_t*) &mem, sizeof(mem))) {
    printMemory(true);
  }

  // Generate new data set for the struct
  for (int i = 0; i < sizeof(mem); i++) {
    mem.data[i] = random(0, 128);
  }

  // Write struct to RTC user memory
  if (ESP.rtcUserMemoryWrite((uint32_t*) &mem, sizeof(mem))) {
    printMemory(false);
  }
  
  // Enter deep sleep mode for 10 seconds
  ESP.deepSleep(10e6);
}

void loop() {
  
}
