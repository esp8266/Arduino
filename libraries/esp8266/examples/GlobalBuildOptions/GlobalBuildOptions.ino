/*
 * Showcase the use of embedded build options and global defines through a specially named .h file.
 * Sketch file name followed by ".globals.h", "GlobalBuildOptions.ino.globals.h"
 *
 * Example from https://arduino-esp8266.readthedocs.io/en/latest/faq/a06-global-build-options.html
 *
 * It is not necessary to have `#include "GlobalBuildOptions.ino.globals.h"`.
 * However, any code inside of the special header file would not be available,
 * unless there is also a matching `create-file:...` directive inside of it.
 *
 * Note that when building in debug mode, `build.opt:debug` should be used instead.
 *
 * Remember to update the global .h file name when saving this sketch to a new location!
 */

#include <umm_malloc/umm_malloc.h>  // has prototype for umm_free_heap_size_min()

void setup() {
  Serial.begin(115200);
  delay(200);

#ifdef MYTITLE1
  Serial.printf("\r\n" MYTITLE1 MYTITLE2 "\r\n");
#else
  Serial.println("ERROR:  MYTITLE1 not present");
#endif

#if defined(UMM_STATS_FULL)
  Serial.printf("Heap Low Watermark %u\r\n", umm_free_heap_size_min());
#endif
}

void loop() {}
