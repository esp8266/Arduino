/*
 *  Called from SPIFFS.begin() to avoid linking in httpUpdateServer FS when not needed
 */

#include <FS.h>

void enable_real_spiffs_weak_end()
{
    /*
     * does nothing
     * allows overriding below
     */
}

/* the following code is linked only if a call to the above function is made somewhere */

extern void spiffs_weak_end_redefinable()
{
    SPIFFS.end();
}
