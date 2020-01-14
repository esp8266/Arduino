#ifdef DEBUG_HWDT

extern struct rst_info resetInfo;

void printHwdtDebugInfo(Print& out) {
  extern struct rst_info resetInfo;
  out.println(String(F("RTC_SYS[0] = ")) + (hwdt_info.rtc_sys_reason) + F(", resetInfo.reason = ") + (resetInfo.reason) + F(", ") + ESP.getResetReason());
  out.println(String(F("ROM API reset reason = ")) + (hwdt_info.rom_api_reason));
  out.println(String(F("HWDT reset reason = ")) + (hwdt_info.reset_reason));
  if (REASON_EXT_SYS_RST < hwdt_info.reset_reason) {
    out.println(String(F("Reset reason confirmation failed!\n")));
  }

  out.println();
  if (hwdt_info.sys) {
    out.println(String(F("Stack Usages:")));
    out.printf_P(PSTR("  ctx: sys  %6u\r\n"), hwdt_info.sys);
    uint32 cont_flags = hwdt_info.cont_integrity;
    out.printf_P(PSTR("  ctx: cont %6u, Integrity Flags: %04X - %s\r\n"), hwdt_info.cont, cont_flags, (cont_flags) ? "fail" : "pass");
    if (hwdt_info.rom) {
      out.printf_P(PSTR("  ctx: ROM  %6u\r\n"), hwdt_info.rom);
    }
  } else {
    out.println(F("No stack usage information available at this time. Try restart."));
  }
  out.println();
}


void printHelpAddOn(Print& out) {
  out.println(F("  0    - Divide by zero, exception(0);"));
  out.println(F("  p    - panic();"));
  out.println(F("  c    - Hardware WDT reset by calling a missing function."));
  out.println();
  out.println(F("Additional options:"));
  out.println(F("  d    - Print some HWDT Debug info."));
}



int hotKeyHandlerAddOn(Print& out, int hotKey) {
  switch (hotKey) {
    case 'd':
      printHwdtDebugInfo(out);
      break;
    case 'p':
      out.println(F("Time to panic()!"));
      panic();
      break;
    case '0':
      out.println(F("Crashing by dividing by zero."));
      out.printf_P(PSTR("This should not print %d\n"), divideA_B(1, 0));
      break;
    case 'c':
      out.println(F("Now calling: void crashMeIfYouCan(void)__attribute__((weak));"));
      out.println(F("This function was missing when the sketch was linked. ..."));
      crashMeIfYouCan();
      break;
    default:
      return 0;
  }
  return 1;
}

int divideA_B(int a, int b) {
  return (a / b);
}

#else
void printHwdtDebugInfo(Print& out) {
  (void)out;
}

int hotKeyHandlerAddOn(Print& out, int hotKey) {
  (void)out;
  (void)hotKey;
  return 0;
}

void printHelpAddOn(Print& out) {
  (void)out;
}
#endif
