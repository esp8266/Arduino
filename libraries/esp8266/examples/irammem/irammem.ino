/*
   This sketch assumes you have selected IRAM as a Second Heap from
   the Arduino IDE tools menu.
*/

#include <ESP8266WiFi.h>
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>

// #define USE_SET_IRAM_HEAP


#ifndef ETS_PRINTF
#define ETS_PRINTF ets_uart_printf
#endif

uint32_t cyclesToRead_nKx32(int n, unsigned int *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKx32(int n, unsigned int *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead_nKx16(int n, unsigned short *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKx16(int n, unsigned short *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead_nKxs16(int n, short *x, int32_t *res) {
  uint32_t b = ESP.getCycleCount();
  int32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKxs16(int n, short *x) {
  uint32_t b = ESP.getCycleCount();
  int32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead_nKx8(int n, unsigned char*x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKx8(int n, unsigned char*x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

// Compare with Inline

uint32_t cyclesToRead_nKx16_viaInline(int n, unsigned short *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += mmu_get_uint16(x++); //*(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKx16_viaInline(int n, unsigned short *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    // *(x++) = sum;
    mmu_set_uint16(x++, sum);
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead_nKxs16_viaInline(int n, short *x, int32_t *res) {
  uint32_t b = ESP.getCycleCount();
  int32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += mmu_get_int16(x++); //*(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKxs16_viaInline(int n, short *x) {
  uint32_t b = ESP.getCycleCount();
  int32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    // *(x++) = sum;
    mmu_set_int16(x++, sum);
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead_nKx8_viaInline(int n, unsigned char*x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += mmu_get_uint8(x++); //*(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite_nKx8_viaInline(int n, unsigned char*x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < n * 1024; i++) {
    sum += i;
    // *(x++) = sum;
    mmu_set_uint8(x++, sum);
  }
  return ESP.getCycleCount() - b;
}

bool perfTest_nK(int nK, uint32_t *mem, uint32_t *imem) {
  uint32_t res, verify_res;
  uint32_t t;
  bool success = true;
  int sres, verify_sres;

  Serial.printf("\r\nPerformance numbers for 16 bit access - using inline macros or exception handling for IRAM.\r\n");;
  t = cyclesToWrite_nKx16(nK, (uint16_t*)mem);
  Serial.printf("DRAM Memory Write:         %7d cycles for %dK by uint16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx16(nK, (uint16_t*)mem, &verify_res);
  Serial.printf("DRAM Memory Read:          %7d cycles for %dK by uint16, %3d AVG cycles/transfer (sum %08x)\r\n", t, nK, t / (nK * 1024), verify_res);
  t = cyclesToWrite_nKxs16(nK, (int16_t*)mem);
  Serial.printf("DRAM Memory Write:         %7d cycles for %dK by  int16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKxs16(nK, (int16_t*)mem, &verify_sres);
  Serial.printf("DRAM Memory Read:          %7d cycles for %dK by  int16, %3d AVG cycles/transfer (sum %08x)\r\n", t, nK, t / (nK * 1024), verify_sres);

  t = cyclesToWrite_nKx16_viaInline(nK, (uint16_t*)imem);
  Serial.printf("IRAM Memory Write Inline:  %7d cycles for %dK by uint16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx16_viaInline(nK, (uint16_t*)imem, &res);
  Serial.printf("IRAM Memory Read Inline:   %7d cycles for %dK by uint16, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), res);
  if (res == verify_res) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_res);
    success = false;
  }

  t = cyclesToWrite_nKxs16_viaInline(nK, (int16_t*)imem);
  Serial.printf("IRAM Memory Write Inline:  %7d cycles for %dK by  int16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKxs16_viaInline(nK, (int16_t*)imem, &sres);
  Serial.printf("IRAM Memory Read Inline:   %7d cycles for %dK by  int16, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), sres);
  if (sres == verify_sres) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_sres);
    success = false;
  }

  t = cyclesToWrite_nKx16(nK, (uint16_t*)imem);
  Serial.printf("IRAM Memory Write:         %7d cycles for %dK by uint16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx16(nK, (uint16_t*)imem, &res);
  Serial.printf("IRAM Memory Read:          %7d cycles for %dK by uint16, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), res);
  if (res == verify_res) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_res);
    success = false;
  }
  t = cyclesToWrite_nKxs16(nK, (int16_t*)imem);
  Serial.printf("IRAM Memory Write:         %7d cycles for %dK by  int16, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKxs16(nK, (int16_t*)imem, &sres);
  Serial.printf("IRAM Memory Read:          %7d cycles for %dK by  int16, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), sres);
  if (sres == verify_sres) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_sres);
    success = false;
  }

  Serial.printf("\r\nPerformance numbers for 8 bit access - using inline macros or exception handling for IRAM access.\r\n");;
  t = cyclesToWrite_nKx8(nK, (uint8_t*)mem);
  Serial.printf("DRAM Memory Write:         %7d cycles for %dK by  uint8, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx8(nK, (uint8_t*)mem, &verify_res);
  Serial.printf("DRAM Memory Read:          %7d cycles for %dK by  uint8, %3d AVG cycles/transfer (sum %08x)\r\n", t, nK, t / (nK * 1024), verify_res);

  t = cyclesToWrite_nKx8_viaInline(nK, (uint8_t*)imem);
  Serial.printf("IRAM Memory Write Inline:  %7d cycles for %dK by  uint8, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx8_viaInline(nK, (uint8_t*)imem, &res);
  Serial.printf("IRAM Memory Read Inline:   %7d cycles for %dK by  uint8, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), res);
  if (res == verify_res) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_res);
    success = false;
  }

  t = cyclesToWrite_nKx8(nK, (uint8_t*)imem);
  Serial.printf("IRAM Memory Write:         %7d cycles for %dK by  uint8, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx8(nK, (uint8_t*)imem, &res);
  Serial.printf("IRAM Memory Read:          %7d cycles for %dK by  uint8, %3d AVG cycles/transfer (sum %08x) ", t, nK, t / (nK * 1024), res);
  if (res == verify_res) {
    Serial.printf("- passed\r\n");
  } else {
    Serial.printf("!= (sum %08x ) - failed\r\n", verify_res);
    success = false;
  }
  Serial.println();

  return success;
}

void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  // Serial.begin(74880);
  Serial.begin(115200);
  delay(20);
  Serial.printf_P(PSTR("\n\nSetup ...\r\n"));
#ifndef UMM_HEAP_IRAM
  Serial.printf("\r\n"
                "This example needs IRAM Heap support enabled.\r\n"
                "  eg. Arduino IDE 'Tools->MMU:\"16KB cache + 48KB IRAM and 2nd Heap (shared)\"'\r\n"
                "This build has IRAM Heap support disabled.\r\n"
                "In this situation, all IRAM requests are satisfied with DRAM.\r\n\r\n");
#endif

  // Compiling with Secondary Heap option does not change malloc to use the
  // IRAM region.  It will continue to use the builtin DRAM until we request
  // otherwise.
  Serial.printf("DRAM free: %6d\r\n", ESP.getFreeHeap());
  uint32_t *mem = (uint32_t *)malloc(2 * 1024 * sizeof(uint32_t));
  Serial.printf("DRAM buffer: Address %p, free %d\r\n", mem, ESP.getFreeHeap());
  if (!mem) {
    return;
  }

  // Now request from the IRAM heap
#ifdef USE_SET_IRAM_HEAP
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  uint32_t *imem = (uint32_t *)malloc(2 * 1024 * sizeof(uint32_t));
  Serial.printf("IRAM buffer: Address %p, free %d\r\n", imem, ESP.getFreeHeap());
  // Make sure we go back to the DRAM heap for other allocations.  Don't forget to ESP.resetHeap()!
  ESP.resetHeap();
#else
  uint32_t *imem;
  {
    HeapSelectIram ephemeral;
    // This class effectively does this
    // size_t _heap_id = umm_get_current_heap_id();
    // umm_set_heap_by_id(UMM_HEAP_IRAM);
    //  ...
    // umm_set_heap_by_id(_heap_id);
    Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
    imem = (uint32_t *)malloc(2 * 1024 * sizeof(uint32_t));
    Serial.printf("IRAM buffer: Address %p, free %d\r\n", imem, ESP.getFreeHeap());
  }
#endif
  if (!imem) {
    return;
  }

  uint32_t res;
  uint32_t t;
  int nK = 1;
  Serial.printf("\r\nPerformance numbers for 32 bit access - no exception handler or inline macros needed.\r\n");;
  t = cyclesToWrite_nKx32(nK, mem);
  Serial.printf("DRAM Memory Write:         %7d cycles for %dK by uint32, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx32(nK, mem, &res);
  Serial.printf("DRAM Memory Read:          %7d cycles for %dK by uint32, %3d AVG cycles/transfer (sum %08x)\r\n", t, nK, t / (nK * 1024), res);

  t = cyclesToWrite_nKx32(nK, imem);
  Serial.printf("IRAM Memory Write:         %7d cycles for %dK by uint32, %3d AVG cycles/transfer\r\n", t, nK, t / (nK * 1024));
  t = cyclesToRead_nKx32(nK, imem, &res);
  Serial.printf("IRAM Memory Read:          %7d cycles for %dK by uint32, %3d AVG cycles/transfer (sum %08x)\r\n", t, nK, t / (nK * 1024), res);
  Serial.println();


  if (perfTest_nK(1, mem, imem)) {
    Serial.println();
  } else {
    Serial.println("\r\n*******************************");
    Serial.println("*******************************");
    Serial.println("**                           **");
    Serial.println("**  One or more test failed  **");
    Serial.println("**                           **");
    Serial.println("*******************************");
    Serial.println("*******************************\r\n");
    return;
  }

#ifdef USE_SET_IRAM_HEAP
  // Let's use IRAM heap to make a big ole' String
  ESP.setIramHeap();
  String s = "";
  for (int i = 0; i < 100; i++) {
    s += i;
    s += ' ';
  }
  ESP.resetHeap();
  Serial.printf("DRAM free: %6d\r\n", ESP.getFreeHeap());
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  ESP.resetHeap();
  Serial.printf("String: %s\r\n", s.c_str());
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  ESP.resetHeap();
#else
  {
    // Let's use IRAM heap to make a big ole' String
    HeapSelectIram ephemeral;
    String s = "";
    for (int i = 0; i < 100; i++) {
      s += i;
      s += ' ';
    }
    {
      HeapSelectDram ephemeral;
      Serial.printf("DRAM free: %6d\r\n", ESP.getFreeHeap());
    }
    // Back to IRAM
    Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
    Serial.printf("String: %s\r\n", s.c_str());
  }
  {
    HeapSelectIram ephemeral;
    Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  }
#endif

  // Note that free/realloc will use the heap specified when the pointer was created.
  // No need to change heaps to delete an object, only to create it.
  free(imem);
  free(mem);
  imem = NULL;
  mem = NULL;

  Serial.printf("DRAM free: %6d\r\n", ESP.getFreeHeap());
#ifdef USE_SET_IRAM_HEAP
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  ESP.resetHeap();
#else
  {
    HeapSelectIram ephemeral;
    Serial.printf("IRAM free: %6d\r\n", ESP.getFreeHeap());
  }
#endif
  {
    ETS_PRINTF("Try and allocate all of the heap in one chunk\n");
    HeapSelectIram ephemeral;
    size_t free_iram = ESP.getFreeHeap();
    ETS_PRINTF("IRAM free: %6d\n", free_iram);
    uint32_t hfree;
    uint16_t hmax;
    uint8_t hfrag;
    ESP.getHeapStats(&hfree, &hmax, &hfrag);
    ETS_PRINTF("ESP.getHeapStats(free: %u, max: %u, frag: %u)\n",
               hfree, hmax, hfrag);
    if (free_iram > UMM_OVERHEAD_ADJUST) {
      void *all = malloc(free_iram - UMM_OVERHEAD_ADJUST);
      ETS_PRINTF("%p = malloc(%u)\n", all, free_iram);
      umm_info(NULL, true);

      free_iram = ESP.getFreeHeap();
      ETS_PRINTF("IRAM free: %6d\n", free_iram);

      free(all);
      ETS_PRINTF("IRAM free: %6d\n", ESP.getFreeHeap());
    }
  }
}

void processKey(Print& out, int hotKey) {
  switch (hotKey) {
    case 'd': {
        HeapSelectDram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'i': {
        HeapSelectIram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'h': {
        {
          HeapSelectIram ephemeral;
          Serial.printf(PSTR("IRAM ESP.getFreeHeap:  %u\n"), ESP.getFreeHeap());
        }
        {
          HeapSelectDram ephemeral;
          Serial.printf(PSTR("DRAM ESP.getFreeHeap:  %u\r\n"), ESP.getFreeHeap());
        }
        break;
      }
    case 'R':
      out.printf_P(PSTR("Restart, ESP.restart(); ...\r\n"));
      ESP.restart();
      break;
    case '\r':
      out.println();
    case '\n':
      break;
    case '?':
      out.println();
      out.println(F("Press a key + <enter>"));
      out.println(F("  h    - Free Heap Report;"));
      out.println(F("  i    - iRAM umm_info(null, true);"));
      out.println(F("  d    - dRAM umm_info(null, true);"));
      out.println(F("  R    - Restart, ESP.restart();"));
      out.println(F("  ?    - Print Help"));
      out.println();
      break;
    default:
      out.printf_P(PSTR("\"%c\" - Not an option?  / ? - help"), hotKey);
      out.println();
      processKey(out, '?');
      break;
  }
}


void loop(void) {
  if (Serial.available() > 0) {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}
