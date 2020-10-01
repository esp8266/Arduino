#if !defined(HWDT_STACK_DUMP_H) || defined(HWDT_VERIFY_HWDT_INFO)
#define HWDT_STACK_DUMP_H

typedef struct hwdt_info_ {
    uint32_t rom;
    uint32_t sys;
    uint32_t cont;
    uint32_t bearssl;
    uint32_t rom_api_reason;
    uint32_t rtc_sys_reason;
    uint32_t reset_reason;
    uint32_t cont_integrity;
    bool g_pcont_valid;
} hwdt_info_t;

extern "C" void debug_hwdt_init(void);

extern uint32_t *g_rom_stack;
extern hwdt_info_t hwdt_info;

#endif
