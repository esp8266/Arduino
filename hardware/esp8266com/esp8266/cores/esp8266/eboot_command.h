#ifndef EBOOT_COMMAND_H
#define EBOOT_COMMAND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_MEM ((volatile uint32_t*)0x60001200)

enum action_t {
    ACTION_COPY_RAW = 0x00000001,
    ACTION_LOAD_APP = 0xffffffff
};

#define EBOOT_MAGIC 	 0xeb001000
#define EBOOT_MAGIC_MASK 0xfffff000

struct eboot_command {
    uint32_t magic;
    enum action_t action;
    uint32_t args[29];
    uint32_t crc32;
};


int eboot_command_read(struct eboot_command* cmd);
void eboot_command_write(struct eboot_command* cmd);
void eboot_command_clear();

#ifdef __cplusplus
}
#endif

#endif //EBOOT_COMMAND_H
