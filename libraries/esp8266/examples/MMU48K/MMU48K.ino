#include <Arduino.h>
#include <ESP8266WiFi.h>

/*
  Notes,
  When accessing IRAM as data storage all access must be word aligned and
  full word length.

*/

#if (MMU_IRAM_SIZE > 32*1024)
uint32_t gobble[4 * 1024] IRAM_ATTR;
constexpr size_t gobble_sz = sizeof(gobble);
#endif

#ifdef MMU_SEC_HEAP
constexpr uint32_t *gobble = (uint32_t *)MMU_SEC_HEAP;
constexpr size_t gobble_sz = MMU_SEC_HEAP_SIZE;
#endif

bool  isValid(uint32_t *probe) {
  bool rc = true;
  ets_uart_printf("\nTesting for valid memory at %p ...\n", probe);
  uint32_t savePS = xt_rsil(15);
  uint32_t saveData = *probe;
  for (size_t i = 0; i < 32; i++) {
    *probe = BIT(i);
    asm volatile("" ::: "memory");
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


void dump_mem32(const void * addr, const size_t len) {
  uint32_t *addr32 = (uint32_t *)addr;
  ets_uart_printf("\n");
  if ((uintptr_t)addr32 & 3) {
    ets_uart_printf("non-32-bit access\n");
    ets_delay_us(12000);
  }
  for (size_t i = 0; i < len;) {
    ets_uart_printf("%p: ", &addr32[i]);
    do {
      ets_uart_printf(" 0x%08x", addr32[i]);
    } while (i++, (i & 3) && (i < len));
    ets_uart_printf("\n");
  }
  ets_uart_printf("\n");
}

extern "C" void _text_end(void);
// extern void *_text_end;
void print_mmu_status(Print& oStream) {
  oStream.println();
  oStream.printf_P(PSTR("MMU Configuration"));
  oStream.println();
  oStream.printf_P(PSTR("  mmu_status = {"
                        "v_cfg = %u, state = %d, enable/disable count = %u/%u, "
                        "map = 0x%02X, p = 0x%02X, v = 0x%02X}"),
                   mmu_status.v_cfg, mmu_status.state,
                   mmu_status.enable_count, mmu_status.disable_count,
                   mmu_status.map, mmu_status.p, mmu_status.v);
  oStream.println();
  uint32_t iram_bank_reg = ESP8266_DREG(0x24);
  if (0 == (iram_bank_reg & 0x10)) {  // if bit clear, is enabled
    oStream.printf_P(PSTR("  IRAM block mapped to:    0x40108000"));
    oStream.println();
  }
  if (0 == (iram_bank_reg & 0x08)) {
    oStream.printf_P(PSTR("  IRAM block mapped to:    0x4010C000"));
    oStream.println();
  }
#ifdef MMU_ICACHE_SIZE
  oStream.printf_P(PSTR("  ICACHE Size:             %u"), MMU_ICACHE_SIZE);
  oStream.println();
#endif
#ifdef MMU_IRAM_SIZE
  oStream.printf_P(PSTR("  IRAM Size:               %u"), MMU_IRAM_SIZE);
  oStream.println();
  const uint32_t iram_free = MMU_IRAM_SIZE - (uint32_t)((uintptr_t)_text_end - 0x40100000UL);
  oStream.printf_P(PSTR("  IRAM free:               %u"), iram_free);
  oStream.println();
#endif
  oStream.printf_P(PSTR("  IRAM _text_end:          %p"), _text_end);
  oStream.println();
#ifdef MMU_SEC_HEAP
  oStream.printf_P(PSTR("  Secondary Heap at:       %p"), MMU_SEC_HEAP);
  oStream.println();
  oStream.printf_P(PSTR("  Secondary Heap Size:     %u"), MMU_SEC_HEAP_SIZE);
  oStream.println();
#endif
}


void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  // Serial.begin(74880);
  Serial.begin(115200);
  delay(20);
  Serial.printf_P(PSTR("\n\nSetup ...\n"));

  print_mmu_status(Serial);

#if (MMU_IRAM_SIZE > 0x8000) || defined(MMU_SEC_HEAP)
  if (isValid(gobble)) {
    // Put something in our new memory
    for (size_t i = 0; i < (gobble_sz / 4); i++) {
      gobble[i] = (uint32_t)&gobble[i];
    }
  }

  // Now is it there?
  dump_mem32(gobble, 32);
  // dump_mem32(&gobble[gobble_sz / 4 / 2], 32);
  dump_mem32(&gobble[gobble_sz / 4 - 32], 32);
#endif

  // Lets peak over the edge
  dump_mem32((void *)(0x4010C000 - 16 * 4), 32);
}

int* nullPointer = NULL;
constexpr char *probe_b = (char *)0x40108000;
constexpr short *probe_s = (short *)0x40108000;
constexpr char *probe_c = (char *)0x40110000;
constexpr short *unaligned_probe_s = (short *)0x3FFF8001;

void processKey(Print& out, int hotKey) {
  switch (hotKey) {
    case '9':
      out.printf_P(PSTR("Unaligned exception by reading short"));
      out.println();
      out.flush();
      xt_rsil(3);
      out.printf_P(PSTR("Read short, 0x%02X at %p"), unaligned_probe_s[0], unaligned_probe_s);
      xt_rsil(0);
      out.println();
      break;
    case 'c':
      out.printf_P(PSTR("Load/Store exception by reading byte outside of handler range"));
      out.println();
      out.flush();
      xt_rsil(3);
      out.printf_P(PSTR("Read Byte, 0x%02X at %p"), probe_c[0], probe_c);
      xt_rsil(0);
      out.println();
      break;
    case 'b':
      out.printf_P(PSTR("Load/Store exception by reading byte from iRAM"));
      out.println();
      out.flush();
      out.printf_P(PSTR("Read Byte from iRAM, 0x%02X at %p"), probe_b[0], probe_b);
      out.println();
      break;
    case 'B': {
        out.printf_P(PSTR("Load/Store exception by writing byte to iRAM"));
        out.println();
        char val = 0x55;
        out.printf_P(PSTR("Write byte, 0x%02X, to iRAM at %p"), val, probe_b);
        out.println();
        out.flush();
        probe_b[0] = val;
        out.printf_P(PSTR("Read Byte back from iRAM, 0x%02X at %p"), probe_b[0], probe_b);
        out.println();
        break;
      }
    case 's':
      out.printf_P(PSTR("Load/Store exception by reading short from iRAM"));
      out.println();
      out.flush();
      out.printf_P(PSTR("Read short from iRAM, 0x%04X at %p"), probe_s[0], probe_s);
      out.println();
      break;
    case 'S': {
        out.printf_P(PSTR("Load/Store exception by writing short to iRAM"));
        out.println();
        short int val = 0x0AA0;
        out.printf_P(PSTR("Write short, 0x%04X, to iRAM at %p"), val, probe_s);
        out.println();
        out.flush();
        probe_s[0] = val;
        out.printf_P(PSTR("Read short back from iRAM, 0x%04X at %p"), probe_s[0], probe_s);
        out.println();
        break;
      }
    case 'R':
      out.printf_P(PSTR("Restart, ESP.restart(); ..."));
      out.println();
      ESP.restart();
      break;
    case 'p':
      out.println(F("Time to panic()!"));
      panic();
      break;
    case '0':
      out.println(F("Crashing by dividing by zero."));
      out.printf_P(PSTR("This should not print %d"), divideA_B(1, 0));
      out.println();
      break;
    case '\r':
      out.println();
    case '\n':
      break;
    case '?':
      out.println();
      out.println(F("Press a key + <enter>"));
      out.println(F("  R    - Restart, ESP.restart();"));
      out.println(F("  ?    - Print Help"));
      out.println();
      out.println(F("Crash with:"));
      out.println(F("  b    - read byte, Load/Store exception"));
      out.println(F("  B    - write byte, Load/Store exception"));
      out.println(F("  s    - read short, Load/Store exception"));
      out.println(F("  S    - write short, Load/Store exception"));
      out.println(F("  c    - read byte, Load/Store exception outside of handler range"));
      out.println(F("  9    - read short, Unaligned exception"));

      out.println(F("  0    - Divide by zero, exception(0);"));
      out.println(F("  p    - panic();"));
      out.println();
      break;
    default:
      out.printf_P(PSTR("\"%c\" - Not an option?  / ? - help"), hotKey);
      out.println();
      processKey(out, '?');
      break;
  }
}


void serialClientLoop(void) {
  if (Serial.available() > 0) {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}

void loop() {
  serialClientLoop();
}


int divideA_B(int a, int b) {
  return (a / b);
}
