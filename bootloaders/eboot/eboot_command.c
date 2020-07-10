#include "eboot_command.h"

uint32_t crc_update(uint32_t crc, const uint8_t *data, size_t length)
{
    uint32_t i;
    bool bit;
    uint8_t c;
    
    while (length--) {
        c = *data++;
        for (i = 0x80; i > 0; i >>= 1) {
            bit = crc & 0x80000000;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x04c11db7;
            }
        }
    }
    return crc;
}

uint32_t eboot_command_calculate_crc32(const struct eboot_command* cmd)
{
    return crc_update(0xffffffff, (const uint8_t*) cmd, 
                      offsetof(struct eboot_command, crc32));
}

int eboot_command_read(struct eboot_command* cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    uint32_t* dst = (uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        dst[i] = RTC_MEM[i];
    }

    uint32_t crc32 = eboot_command_calculate_crc32(cmd);
    if ((cmd->magic & EBOOT_MAGIC_MASK) != EBOOT_MAGIC ||
        cmd->crc32 != crc32) {
        return 1;
    }

    return 0;
}

void eboot_command_write(struct eboot_command* cmd)
{
    cmd->magic = EBOOT_MAGIC;
    cmd->crc32 = eboot_command_calculate_crc32(cmd);

    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    const uint32_t* src = (const uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        RTC_MEM[i] = src[i];
    }
}

void eboot_command_clear()
{
    RTC_MEM[offsetof(struct eboot_command, magic) / sizeof(uint32_t)] = 0;
    RTC_MEM[offsetof(struct eboot_command, crc32) / sizeof(uint32_t)] = 0;
}

