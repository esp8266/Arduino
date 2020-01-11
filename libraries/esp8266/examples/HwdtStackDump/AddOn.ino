extern struct rst_info resetInfo;


void printHwdtDebugInfo(Print& out) {
  extern struct rst_info resetInfo;
  out.println(String(F("RTC_SYS[0] = ")) +  (hwdt_info.rtc_sys_reason) + F(", resetInfo.reason = ") + (resetInfo.reason) + F(", ") + ESP.getResetReason());
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
  out.println(F("  d    - Print some HWDT Debug info.);"));
}


int hotKeyHandlerAddOn(Print& out, int hotKey) {
  switch (hotKey) {
    case 'd':
      printHwdtDebugInfo(out);
      break;
    default:
      return 0;
  }
  return 1;
}
