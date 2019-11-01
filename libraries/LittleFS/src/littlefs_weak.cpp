/*
 *  Called from LittleFS.begin() to avoid linking in httpUpdateServer FS when not needed
 */

#include <LittleFS.h>

void enable_real_littlefs_weak_end()
{
    /*
     * does nothing
     * allows overriding below
     */
}

/* the following code is linked only if a call to the above function is made somewhere */

extern void littlefs_weak_end_redefinable()
{
    LittleFS.end();
}
