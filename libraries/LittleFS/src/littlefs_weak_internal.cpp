extern void littlefs_weak_end_redefinable() __attribute__((weak));
extern void littlefs_weak_end_redefinable(void)
{
    /* noop unless oveerridden by SPIFFS.begin() */
}

static void littlefs_weak_end_custom (void) __attribute__((weakref("littlefs_weak_end_redefinable")));

extern void littlefs_weak_end (void)
{
    littlefs_weak_end_custom();
}



