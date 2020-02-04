#include <Arduino.h>
#include <ESP8266WiFi.h>

/*
  Notes,
  When accessing IRAM as data storage all access must be word aligned and
  full word length.

*/

#if (IRAM_SIZE > 32*1024)
uint32_t gobble[4 * 1024] IRAM_ATTR;
constexpr size_t gobble_sz = sizeof(gobble);
#endif

#ifdef SEC_HEAP
constexpr uint32_t *gobble = (uint32_t *)SEC_HEAP;
constexpr size_t gobble_sz = SEC_HEAP_SIZE;
#endif

bool  isValid(uint32_t *probe) {
  bool rc = true;
  ets_uart_printf("\nTesting for valid memory at %p ...\n", probe);
  uint32_t savePS = xt_rsil(15);
  uint32_t saveData = *probe;
  for (size_t i = 0; i < 32; i++) {
    *probe = BIT(i);
    asm volatile ("" ::: "memory");
    uint32_t val = *probe;
    if (val != BIT(i)) {
      ets_uart_printf("  Read 0x%08X != Wrote 0x%08X\n", val, (uint32_t)BIT(i));
      rc = false;
    }
  }
  *probe = saveData;
  xt_wsr_ps(savePS);
  ets_uart_printf("  %s\n", (rc) ? "Pass" : "Fail!");
  return rc;
}


void dump_mem(void * addr, size_t len) {
  uint32_t *addr32 = (uint32_t *)((uintptr_t)addr & ~3);
  len = (len + 3) & ~3;

  ets_uart_printf("\n");
  for (size_t i = 0; i < len;) {
    ets_uart_printf("%p:  ", &addr32[i]);
    do {
      ets_uart_printf("0x%08x ", addr32[i]);
    } while (i++, (i & 3));
    ets_uart_printf("\n");
  }
  ets_uart_printf("\n");
}


void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(20);
  Serial.printf_P(PSTR("\n\nI am Alive!\n"));

  Serial.printf_P(PSTR("\nMMU Configuration\n"));
  Serial.printf_P(PSTR("  Cache_Read_Enable status %d\n"), Cache_Read_Enable_status);
#ifdef ICACHE_SIZE
  Serial.printf_P(PSTR("  ICACHE Size:             %u\n"), ICACHE_SIZE);
#endif
#ifdef IRAM_SIZE
  Serial.printf_P(PSTR("  IRAM Size:               %u\n"), IRAM_SIZE);
#endif
#ifdef SEC_HEAP
  Serial.printf_P(PSTR("  Secondary Heap at:       %p\n"), SEC_HEAP);
  Serial.printf_P(PSTR("  Secondary Heap Size:     %u\n"), SEC_HEAP_SIZE);
#endif

#if (IRAM_SIZE > 0x8000) || defined(SEC_HEAP)
  if (isValid(gobble)) {
    // Put something in our new memory
    for (size_t i = 0; i < (gobble_sz/4); i++) {
      gobble[i] = (uint32_t)&gobble[i];
    }
  }

  // Now is it there?
  dump_mem(gobble, 32);
  dump_mem(&gobble[gobble_sz/4/2], 32);
  dump_mem(&gobble[gobble_sz/4 - 32], 32);
#endif

  // Lets peak over the edge
  dump_mem((void *)(0x4010C000 - 16 * 4), 32);
}

void loop() {

}
