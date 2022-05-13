/*
 * To implement this patch, the SDK module `eap.o` from archive `libwpa2.a` must
 * be patched to call `z2EapFree` instead of `vPortFree`. This limits extending
 * the execution time of vPortFree to that module only. Not impacting other
 * modules.
 *
 */

#include <string.h>
#include <ets_sys.h>
#include <pgmspace.h>
#include "coredecls.h"
#if 0
#include "esp8266_undocumented.h"
#define DEBUG_PRINTF ets_uart_printf
#else
#define DEBUG_PRINTF(...)
#endif

extern "C" {

// extern "C" void z2EapFree(void *ptr, const char* file, int line) __attribute__((weak, alias("vPortFree")));

/*
 * Limited 2-part wrapper for vPortFree calls made in SDK module `eap.o` from
 * archive `libwpa2.a`.
 *
 * vPortFree calls from eap.o are monitored for calls from line 799. This is
 * the location of the memory leak. At entry register a12 contains the structure
 * address which has the addresses of the allocations that will be leaked.
 *
 * Part 1 of this wrapper, z2EapFree, appends the value of register a12 as a
 * 4th argument to part2 of this wrapper, patch_wpa2_eap_vPortFree_a12(). Which
 * in turn checks and frees the additional allocations, that would have been
 * lost.
 *
 * extern "C" z2EapFree(void*);
 */

/*
 * Part 1 of Limited vPortFree Wrapper
 */
asm(
    // ".section     .iram.text.z2EapFree,\"ax\",@progbits\n\t"
    // Since all the possible callers in eap.o are in sections starting with
    // .text and not .iram.text we should be safe putting these wrappers in .text.
    ".section     .text.z2EapFree,\"ax\",@progbits\n\t"
    ".literal_position\n\t"
    ".literal     .patch_wpa2_eap_vPortFree_a12, patch_wpa2_eap_vPortFree_a12\n\t"
    ".align       4\n\t"
    ".global      z2EapFree\n\t"
    ".type        z2EapFree, @function\n\t"
    "\n"
"z2EapFree:\n\t"
    "addi         a1,     a1,     -16\n\t"
    "s32i         a0,     a1,     0\n\t"
    "mov          a5,     a12\n\t"
    "l32r         a0,     .patch_wpa2_eap_vPortFree_a12\n\t"
    "callx0       a0\n\t"
    "l32i         a0,     a1,     0\n\t"
    "addi         a1,     a1,     16\n\t"
    "ret\n\t"
    ".size z2EapFree, .-z2EapFree\n\t"
);

/*
 * While some insight can be gained from the ESP32 repo for this structure.
 * It does not match exactly. This alternate structure focuses on correct offset
 * rather than trying to exactly reconstruct the original labels.
 */
struct StateMachine { // size 200 bytes
    void* beforeConfig[16];
    void* config[26];
    // 0  - mov a2, a12, 64  // username / Identity
    // 1  - mov a2, a12, 68
    // 2  - mov a2, a12, 72  // anonymous Identity
    // 3  - mov a2, a12, 76
    // 4  - mov a2, a12, 80  // password
    // 21 - mov a2, a12, 148 // ??
    void* afterConfig[8];
};

/*
 * Part 2 of Limited vPortFree Wrapper
 *
 * Presently, all SDKs have the same memory leaks in the same module at the
 * same line.
 */
void patch_wpa2_eap_vPortFree_a12(void *ptr, const char* file, int line, void* a12) {
    if (799 == line) {
        struct StateMachine* sm = (struct StateMachine*)a12;
        if (ptr == sm->config[0]) {
            // Fix leaky frunction - eap.o only frees one out of 4 config items
            // finish the other 3 first
            vPortFree(sm->config[2], file, line);
            vPortFree(sm->config[4], file, line);
            vPortFree(sm->config[21], file, line);
            // ptr is sm->config[0], let fall through handle it
        }
        DEBUG_PRINTF("\nz2EapFree/vPortFree patch working\n");
    }
    vPortFree(ptr, file, line);
}

};

/*
 * This will minimize code space for non-wifi enterprise sketches which do not
 * need the patch and disable_extra4k_at_link_time().
 */
void enable_wifi_enterprise_patch(void) {
    /*
     * Calling this from setup or anywhere ensures that the patch code is
     * included in the build.
     *
     * Also, WiFi Enterprise uses a lot of system stack space and may crash
     * unless we:
     */
    disable_extra4k_at_link_time();
}
