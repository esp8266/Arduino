/*
   This sketch assumes you have selected IRAM as a Second Heap from
   the Arduino IDE tools menu.
*/

#include <ESP8266WiFi.h>
#include <umm_malloc/umm_malloc.h>

// #define USE_SET_IRAM_HEAP


#ifndef ETS_PRINTF
#define ETS_PRINTF ets_uart_printf
#endif

uint32_t cyclesToRead1Kx32(unsigned int *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx32(unsigned int *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}


uint32_t cyclesToRead1Kx16(unsigned short *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx16(unsigned short *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead1Kx8(unsigned char*x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx8(unsigned char*x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

// Compare with Inline

uint32_t cyclesToRead1Kx16_viaInline(unsigned short *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += get_uint16_iram(x++); //*(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx16_viaInline(unsigned short *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    // *(x++) = sum;
    set_uint16_iram(x++, sum);
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead1Kx8_viaInline(unsigned char*x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += get_uint8_iram(x++); //*(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx8_viaInline(unsigned char*x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    // *(x++) = sum;
    set_uint8_iram(x++, sum);
  }
  return ESP.getCycleCount() - b;
}

void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  // Serial.begin(74880);
  Serial.begin(115200);
  delay(20);
  Serial.printf_P(PSTR("\n\nSetup ...\n"));

  // IRAM Heap Inialized moed to user_init() in core_esp8266_main.cpp
  // umm_init_iram();

  // Compiling with Secondary Heap option VM does not change malloc to use the
  // IRAM region.  It will continue to use the builtin DRAM until we request
  // otherwise.
  Serial.printf("DRAM free: %6d\n", ESP.getFreeHeap());
  uint32_t *mem = (uint32_t *)malloc(1024 * sizeof(uint32_t));
  Serial.printf("DRAM buffer: Address %p, free %d\n", mem, ESP.getFreeHeap());

  // Now request from the IRAM heap
#ifdef USE_SET_IRAM_HEAP
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
  uint32_t *imem = (uint32_t *)malloc(1024 * sizeof(uint32_t));
  Serial.printf("IRAM buffer: Address %p, free %d\n", imem, ESP.getFreeHeap());
  // Make sure we go back to the DRAM heap for other allocations.  Don't forget to ESP.resetHeap()!
  ESP.resetHeap();
#else
  uint32_t *imem;
  {
    HeapSelectIram ephemeral;
    // size_t _heap_id = umm_get_current_heap_id();
    // umm_set_heap_by_id(UMM_HEAP_IRAM);
    Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
    imem = (uint32_t *)malloc(1024 * sizeof(uint32_t));
    Serial.printf("IRAM buffer: Address %p, free %d\n", imem, ESP.getFreeHeap());
    // umm_set_heap_by_id(_heap_id);
  }
#endif

  uint32_t res;
  uint32_t t;
  t = cyclesToWrite1Kx32(imem);
  Serial.printf("IRAM Memory Write:         %6d cycles for 4K\n", t);
  t = cyclesToWrite1Kx32(mem);
  Serial.printf("DRAM Memory Write:         %6d cycles for 4K\n", t);

  t = cyclesToRead1Kx32(imem, &res);
  Serial.printf_P(PSTR("IRAM Memory Read:          %6d cycles for 4K (sum %08x)\n"), t, res);
  t = cyclesToRead1Kx32(mem, &res);
  Serial.printf("DRAM Memory Read:          %6d cycles for 4K (sum %08x)\n", t, res);

  t = cyclesToWrite1Kx16((uint16_t*)imem);
  Serial.printf("IRAM Memory Write:         %6d cycles for 2K by 16\n", t);
  t = cyclesToWrite1Kx16_viaInline((uint16_t*)imem);
  Serial.printf("IRAM Memory Write Inline:  %6d cycles for 2K by 16\n", t);
  t = cyclesToWrite1Kx16((uint16_t*)mem);
  Serial.printf("DRAM Memory Write:         %6d cycles for 2K by 16\n", t);

  t = cyclesToRead1Kx16((uint16_t*)imem, &res);
  Serial.printf("IRAM Memory Read:          %6d cycles for 2K by 16 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx16_viaInline((uint16_t*)imem, &res);
  Serial.printf("IRAM Memory Read Inline:   %6d cycles for 2K by 16 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx16((uint16_t*)mem, &res);
  Serial.printf("DRAM Memory Read:          %6d cycles for 2K by 16 (sum %08x)\n", t, res);

  t = cyclesToWrite1Kx8((uint8_t*)imem);
  Serial.printf("IRAM Memory Write:         %6d cycles for 1K by  8\n", t);
  t = cyclesToWrite1Kx8_viaInline((uint8_t*)imem);
  Serial.printf("IRAM Memory Write Inline:  %6d cycles for 1K by  8\n", t);
  t = cyclesToWrite1Kx8((uint8_t*)mem);
  Serial.printf("DRAM Memory Write:         %6d cycles for 1K by  8\n", t);

  t = cyclesToRead1Kx8((uint8_t*)imem, &res);
  Serial.printf("IRAM Memory Read:          %6d cycles for 1K by  8 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx8_viaInline((uint8_t*)imem, &res);
  Serial.printf("IRAM Memory Read Inline:   %6d cycles for 1K by  8 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx8((uint8_t*)mem, &res);
  Serial.printf("DRAM Memory Read:          %6d cycles for 1K by  8 (sum %08x)\n", t, res);

#ifdef USE_SET_IRAM_HEAP
  // Let's use IRAM heap to make a big ole' String
  ESP.setIramHeap();
  String s = "";
  for (int i = 0; i < 100; i++) {
    s += i;
    s += ' ';
  }
  ESP.resetHeap();
  Serial.printf("DRAM free: %6d\n", ESP.getFreeHeap());
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
  ESP.resetHeap();
  Serial.printf("String: %s\n", s.c_str());
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
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
      Serial.printf("DRAM free: %6d\n", ESP.getFreeHeap());
    }
    // Back to IRAM
    Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
    Serial.printf("String: %s\n", s.c_str());
  }
  {
    HeapSelectIram ephemeral;
    Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
  }
#endif

  // Note that free/realloc will all use the heap specified when the pointer was created.
  // No need to change heaps to delete an object, only to create it.
  free(imem);
  free(mem);

  Serial.printf("DRAM free: %6d\n", ESP.getFreeHeap());
#ifdef USE_SET_IRAM_HEAP
  ESP.setIramHeap();
  Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
  ESP.resetHeap();
#else
  {
    HeapSelectIram ephemeral;
    Serial.printf("IRAM free: %6d\n", ESP.getFreeHeap());
  }
#endif
}

void loop() {

}
