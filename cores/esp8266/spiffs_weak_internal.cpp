extern void spiffs_weak_end_redefinable() __attribute__((weak));
extern void spiffs_weak_end_redefinable(void)
{
    /* noop unless oveerridden by SPIFFS.begin() */
}

static void spiffs_weak_end_custom (void) __attribute__((weakref("spiffs_weak_end_redefinable")));

extern void spiffs_weak_end (void)
{
    spiffs_weak_end_custom();
}



