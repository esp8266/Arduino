
// nothing else than showing heap metric usage
// released to public domain

#include <ESP8266WiFi.h>
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>

void stats(const char* what) {
  // we could use getFreeHeap() getMaxFreeBlockSize() and getHeapFragmentation()
  // or all at once:
  uint32_t free;
  uint32_t max;
  uint8_t  frag;
  ESP.getHeapStats(&free, &max, &frag);

  Serial.printf("free: %7u - max: %7u - frag: %3d%% <- ", free, max, frag);
  // %s requires a malloc that could fail, using println instead:
  Serial.println(what);
}

void tryit(int blocksize) {
  void** p;
  int    blocks;

  /*
    heap-used ~= blocks*sizeof(void*) + blocks*blocksize

    This calculation gets deep into how umm_malloc divides up memory and
    understanding it is not important for this example. However, some may find
    the details useful when creating memory restricted test cases and possibly
    other manufactured failures.

    Internally the umm_malloc works with memory in 8-byte increments and aligns
    to 8 bytes. The creation of an allocation adds about 4-bytes of overhead
    plus alignment to the allocation size and more for debug builds. This
    complicates the calculation of `blocks` a little.

    ESP.getMaxFreeBlockSize() does not indicate the amount of memory that is
    available for use in a single malloc call.  It indicates the size of a
    contiguous block of (raw) memory before the umm_malloc overhead is removed.

    It should also be pointed out that, if you allow for the needed overhead in
    your malloc call, it could still fail in the general case. An IRQ handler
    could have allocated memory between the time you call
    ESP.getMaxFreeBlockSize() and your malloc call, reducing the available
    memory. In this particular sketch, with "WiFi off" we are not expecting this
    to be an issue.

    The macro UMM_OVERHEAD_ADJUST provides a value that can be used to adjust
    calculations when trying to dividing up memory as we are here. However, the
    calculation of multiple elements combined with the rounding up for the
    8-byte alignment of each allocation can make for some tricky calculations.
  */
  int rawMemoryMaxFreeBlockSize = ESP.getMaxFreeBlockSize();
  // Remove the space for overhead component of the blocks*sizeof(void*) array.
  int maxFreeBlockSize = rawMemoryMaxFreeBlockSize - UMM_OVERHEAD_ADJUST;
  // Initial estimate to use all of the MaxFreeBlock with multiples of 8 rounding up.
  blocks = maxFreeBlockSize / (((blocksize + UMM_OVERHEAD_ADJUST + 7) & ~7) + sizeof(void*));
  /*
    While we allowed for the 8-byte alignment overhead for blocks*blocksize we
    were unable to compensate in advance for the later 8-byte aligning needed
    for the blocks*sizeof(void*) allocation. Thus blocks may be off by one count.
    We now validate the estimate and adjust as needed.
  */
  int rawMemoryEstimate = blocks * ((blocksize + UMM_OVERHEAD_ADJUST + 7) & ~7)
                          + ((blocks * sizeof(void*) + UMM_OVERHEAD_ADJUST + 7) & ~7);
  if (rawMemoryMaxFreeBlockSize < rawMemoryEstimate) {
    --blocks;
  }
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
  delay(50);

  Serial.printf("\r\nDemo Heap Metrics for DRAM\r\n");
  tryit(8000);
  tryit(4000);
  tryit(2000);
  tryit(1000);
  tryit(500);
  tryit(200);
  tryit(100);
  tryit(50);
  tryit(15);
#ifdef UMM_HEAP_IRAM
  {
    HeapSelectIram ephemeral;
    Serial.printf("\r\nDemo Heap Metrics for IRAM\r\n");
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
#endif
#ifdef MMU_EXTERNAL_HEAP
  {
    HeapSelect ephemeral = HeapSelect(UMM_HEAP_EXTERNAL);
    Serial.printf("\r\nDemo Heap Metrics for External RAM\r\n");
#if (MMU_EXTERNAL_HEAP > 64)
    tryit(64000);
    tryit(32000);
#endif
    tryit(16000);
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
#endif
}

void loop() { }
