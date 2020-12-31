#include <Arduino.h>
#include <mmu_iram.h>

uint32_t IRAM_ATTR timed_byte_read(char *pc, uint32_t * o) {
  uint32_t start = esp_get_cycle_count();
  *o = *pc;
  // return clockCyclesToMicroseconds(esp_get_cycle_count() - start);
  return (esp_get_cycle_count() - start);
}

uint32_t IRAM_ATTR timed_byte_read2(char *pc, uint32_t * o) {
  uint32_t start = esp_get_cycle_count();
  *o = mmu_get_uint8(pc);
  // return clockCyclesToMicroseconds(esp_get_cycle_count() - start);
  return (esp_get_cycle_count() - start);
}
