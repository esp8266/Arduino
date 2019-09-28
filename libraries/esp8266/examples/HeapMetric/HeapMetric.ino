
// nothing else than showing heap metric usage
// released to public domain

#include <ESP8266WiFi.h>

void stats(const char* what) {
  // we could use getFreeHeap() getMaxFreeBlockSize() and getHeapFragmentation()
  // or all at once:
  uint32_t free;
  uint16_t max;
  uint8_t frag;
  ESP.getHeapStats(&free, &max, &frag);

  Serial.printf("free: %5d - max: %5d - frag: %3d%% <- ", free, max, frag);
  // %s requires a malloc that could fail, using println instead:
  Serial.println(what);
}

void tryit(int blocksize) {
  void** p;
  int blocks;

  // heap-used ~= blocks*sizeof(void*) + blocks*blocksize
  blocks = ((ESP.getMaxFreeBlockSize() / (blocksize + sizeof(void*))) + 3) & ~3; // rounded up, multiple of 4

  Serial.printf("\nFilling memory with blocks of %d bytes each\n", blocksize);
  stats("before");

  p = (void**)malloc(sizeof(void*) * blocks);
  for (int i = 0; i < blocks; i++) {
    p[i] = malloc(blocksize);
  }
  stats("array and blocks allocation");

  for (int i = 0; i < blocks; i += 2) {
    if (p[i]) {
      free(p[i]);
    }
    p[i] = nullptr;
  }
  stats("freeing every other blocks");

  for (int i = 0; i < blocks; i += 4) {
    if (p[i + 1]) {
      free(p[i + 1]);
    }
    p[i + 1] = nullptr;
  }
  stats("freeing every other remaining blocks");

  for (int i = 0; i < blocks; i++) {
    if (p[i]) {
      free(p[i]);
    }
  }
  stats("freeing array");

  free(p);
  stats("after");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  tryit(8000);
  tryit(4000);
  tryit(2000);
  tryit(1000);
  tryit(500);
  tryit(200);
  tryit(100);
  tryit(50);
  tryit(15);
}

void loop() {
}
