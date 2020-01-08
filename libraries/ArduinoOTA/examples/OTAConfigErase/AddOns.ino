
#ifdef ADDON_DEMO
void printHelpAddOn(Print& oStream) {
    oStream.println(F("  x - use this space to add on extra hot key options"));
}

int hotKeyHandlerAddOn(Print& oStream, char hotKey) {
  switch (hotKey) {
    case 'x':
      oStream.printf_P(PSTR("This could be an extra option"));
      break;
    default:
      oStream.println();
      return 0;
  }
  oStream.println();
  return 1;
}
#else
void printHelpAddOn(Print& oStream) {
  (void)oStream;
}
int hotKeyHandlerAddOn(Print& oStream, char inChar) {
  (void)oStream;
  (void)inChar;
  return 0;
}
#endif
