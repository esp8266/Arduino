#include <esp8266_undocumented.h>

int* nullPointer = NULL;

void processKey(Print& out, int hotKey) {
  switch (hotKey) {
    case 'r':
      out.printf_P(PSTR("Reset, ESP.reset(); ...\r\n"));
      ESP.reset();
      break;
    case 't':
      out.printf_P(PSTR("Restart, ESP.restart(); ...\r\n"));
      ESP.restart();
      break;
    case 's': {
        uint32_t startTime = millis();
        out.printf_P(PSTR("Now crashing with Software WDT. This will take about 3 seconds.\r\n"));
        ets_install_putc1(ets_putc);
        while (true) {
          ets_printf("%9lu\r", (millis() - startTime));
          ets_delay_us(250000);
          // stay in an loop blocking other system activity.
        }
      }
      break;
    case 'h':
      out.printf_P(PSTR("Now crashing with Hardware WDT. This will take about 6 seconds.\r\n"));
      // ESP.wdtDisable();
      asm volatile("" ::: "memory");
      asm volatile("mov.n a2, %0\n"
                   "mov.n a3, %1\n"
                   "mov.n a4, %2\n"
                   "mov.n a5, %3\n"
                   "mov.n a6, %4\n"
                   : : "r"(0xaaaaaaaa), "r"(0xaaaaaaaa), "r"(0xaaaaaaaa), "r"(0xaaaaaaaa), "r"(0xaaaaaaaa));
      // Could not find these in the stack dump, unless interrupts were enabled.
      {
        uint32_t startTime = millis();
        // Avoid all the Core functions that play nice, so we can hog
        // the system and crash.
        ets_install_putc1(ets_putc);
        xt_rsil(15);
        while (true) {
          ets_printf("%9lu\r", (millis() - startTime));
          ets_delay_us(250000);
          // stay in an loop blocking other system activity.
          //
          // Note:
          // Hardware WDT kicks in if Software WDT is unable to perform.
          // With the Hardware WDT, nothing is saved on the stack, that I have seen.
        }
      }
      break;
    case '\r':
      out.println();
    case '\n':
      break;
    case '?':
      out.println();
      out.println(F("Press a key + <enter>"));
      out.println(F("  r    - Reset, ESP.reset();"));
      out.println(F("  t    - Restart, ESP.restart();"));
      out.println(F("  ?    - Print Help"));
      out.println();
      out.println(F("Crash with:"));
      out.println(F("  s    - Software WDT"));
      out.println(F("  h    - Hardware WDT"));
      printHelpAddOn(out);
      out.println();
      break;
    default:
      if (hotKeyHandlerAddOn(out, hotKey)) {
        return;
      }
      out.printf_P(PSTR("\"%c\" - Not an option?  / ? - help"), hotKey);
      out.println();
      processKey(out, '?');
      break;
  }
}
