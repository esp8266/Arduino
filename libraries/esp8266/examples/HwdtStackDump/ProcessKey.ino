#include <esp8266_undocumented.h>
void crashMeIfYouCan(void) __attribute__((weak));
int  divideA_B(int a, int b);

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
    } break;
    case 'h':
      out.printf_P(PSTR("Now crashing with Hardware WDT. This will take about 6 seconds.\r\n"));
      asm volatile("mov.n a2, %0\n\t"
                   "mov.n a3, %1\n\t"
                   "mov.n a4, %2\n\t"
                   "mov.n a5, %3\n\t"
                   "mov.n a6, %4\n\t"
                   :
                   : "r"(0xaaaaaaaa), "r"(0xaaaaaaaa), "r"(0xaaaaaaaa), "r"(0xaaaaaaaa),
                     "r"(0xaaaaaaaa)
                   : "memory");
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
    case 'p':
      out.println(F("Time to panic()!"));
      panic();
      break;
    case 'z':
      out.println(F("Crashing by dividing by zero. This should generate an exception(0)."));
      out.printf_P(PSTR("This should not print %d\n"), divideA_B(1, 0));
      break;
    case 'w':
      out.println(F("Now calling: void crashMeIfYouCan(void)__attribute__((weak));"));
      out.println(
          F("This function has a prototype but was missing when the sketch was linked. ..."));
      crashMeIfYouCan();
      break;
    case 'b':
      out.println(F("Executing a break instruction w/o GDB will cause a HWDT reset."));
      asm volatile("break 1, 15;");
      out.println(F("This line will not be printable w/o running GDB"));
      break;
    case '0':
      out.println(F("Crashing at an embedded 'break 1, 15' instruction that was generated"));
      out.println(F("by the compiler after detecting a divide by zero."));
      out.printf_P(PSTR("This should not print %d\n"), divideA_B_bp(1, 0));
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
      out.println(F("  h    - Hardware WDT - looping with interrupts disabled"));
      out.println(F("  w    - Hardware WDT - calling a missing (weak) function."));
      out.println(F("  0    - Hardware WDT - a hard coded compiler breakpoint from a compile time "
                    "detected divide by zero"));
      out.println(
          F("  b    - Hardware WDT - a forgotten hard coded 'break 1, 15;' and no GDB running."));
      out.println(F("  z    - Divide by zero, exception(0);"));
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

// With the current toolchain 10.1, using this to divide by zero will *not* be
// caught at compile time.
int __attribute__((noinline)) divideA_B(int a, int b) { return (a / b); }

// With the current toolchain 10.1, using this to divide by zero *will* be
// caught at compile time. And a hard coded breakpoint will be inserted.
int divideA_B_bp(int a, int b) { return (a / b); }
