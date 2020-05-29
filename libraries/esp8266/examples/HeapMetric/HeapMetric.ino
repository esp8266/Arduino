
// nothing else than showing heap metric usage
// released to public domain

#include <ESP8266WiFi.h>
#include <umm_malloc/umm_malloc.h>

// A macro to handle printing w/o using the heap.
#ifndef ETS_PRINTF
#define ETS_PRINTF(a, ...) ets_uart_printf(a, ##__VA_ARGS__)
#endif

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

  if (umm_get_oom_count()) {
    return;
  }
  /*
    heap-used ~= blocks*sizeof(void*) + blocks*blocksize

    This calculation gets deep into how umm_malloc divides up memory and
    understanding it is not important to this example.

    Internally the umm_malloc works with memory in 8-byte increments and aligns
    to 8 bytes. The creation of an allocation adds about 4-bytes of overhead
    plus alignment to the allocation size and more for debug builds. This
    complicates the calculation of `blocks` a little.

    With Core Version 2.7.2, ESP.getMaxFreeBlockSize() returns a value that has
    already been adjusted for umm_malloc overhead. It reports a value that could
    have been allocated at the time of the call. The overhead size is different
    for the debug build.

    umm_get_alloc_overhead() provides a value that can be used to adjust
    calculations when trying to dividing up memory as we are here. However, the
    calculation of multiple elements combined with the rounding up for the
    8-byte alignment of each allocation can make for some challenging
    calculations.
  */
  int maxFreeBlockSize = ESP.getMaxFreeBlockSize();
  int rawMemoryMaxFreeBlockSize = maxFreeBlockSize + umm_get_alloc_overhead();
  // Initial estimate to use all of the heap with multiples of 8 rounding up.
  blocks = maxFreeBlockSize /
           (((blocksize + umm_get_alloc_overhead() + 7) & ~7) + sizeof(void*));
  /*
    Note we did not include any adjustment for the overhead of the
    blocks*sizeof(void*) component. It has already been accounted for in the
    value returned by ESP.getMaxFreeBlockSize(). It was reduced by the amount of
    `umm_get_alloc_overhead()`. However, `blocks` could still be off by a count
    of 1, because of the 8-byte aligning on the blocks*sizeof(void*) allocation.
  */
  int rawMemoryEstimate =
    blocks * ((blocksize + umm_get_alloc_overhead() + 7) & ~7) +
    ((blocks * sizeof(void*) + umm_get_alloc_overhead() + 7) & ~7);
  if (rawMemoryMaxFreeBlockSize < rawMemoryEstimate) {
    --blocks;
  }
  Serial.printf("\nFilling memory with blocks of %d bytes each\n", blocksize);
  stats("before");

  p = (void**)malloc(sizeof(void*) * blocks);
  for (int i = 0; i < blocks; i++) {
    p[i] = malloc(blocksize);
    if (umm_get_oom_count()) {
      ETS_PRINTF("\n\nOOM at block index %d of possible %d.\n\n",  i, blocks);
      return;
    }
  }
  stats("array and blocks allocation");

  for (int i = 0; i < blocks; i += 2) {
    if (p[i]) {
      free(p[i]);
    }
    p[i] = nullptr;
  }
  stats("freeing every other blocks");

  for (int i = 0; i < (blocks - 1); i += 4) {
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
