#include "eboot_command.h"
#include "flash.h"

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

#if defined (EBOOT_ENABLE_FLASH_STORAGE)

uint32_t eboot_read_flash_index(eboot_index_t *eboot_index)
{
    if (SPIRead(0 + APP_START_OFFSET + sizeof(image_header_t) + sizeof(section_header_t), &eboot_index, sizeof(eboot_index))) {
        return 0;
    }

    if (eboot_index->magic != EBOOT_INDEX_MAGIC_V1) {
        return 0;
    }

    return 1;
}

eboot_flash_command_t *commandAddress(void) {
  eboot_index_t eboot_index;
  const uint32_t addr = 0 + APP_START_OFFSET + sizeof(image_header_t) + sizeof(section_header_t);
  if (SPIRead(addr, (uint32_t *)&eboot_index, sizeof(eboot_index))) {
    return NULL;
  }
  if (eboot_index.magic != EBOOT_INDEX_MAGIC_V1) {
    return NULL;
  } else {
    return eboot_index.commands;
  }
}

bool readBootCommand(int cmd, eboot_flash_command_t *dst) {
  eboot_flash_command_t *cmds = commandAddress();
  if (cmd >= EBOOT_COMMAND_MAX_COUNT) {
    return 0;
  }
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
    ets_putc('r');
    ets_putc('b');
    ets_putc('0' + cmd);
    ets_putc('\n');
#endif
  uint32_t uint = (uint32_t) cmds;
  uint32_t addr = (uint32_t)cmds - 0x40200000;
  addr += cmd * sizeof(*dst);
  if (SPIRead(addr, (uint32_t *)dst, sizeof(*dst))) {
    return 0;
  }
}

bool writeBootCommand(int cmd, eboot_flash_command_t *dst) {
  eboot_flash_command_t *cmds = commandAddress();
  if (cmd >= EBOOT_COMMAND_MAX_COUNT) {
    return 0;
  }
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
    ets_putc('w');
    ets_putc('b');
    ets_putc('0' + cmd);
    ets_putc('\n');
#endif
  uint32_t uint = (uint32_t) cmds;
  uint32_t addr = (uint32_t)cmds - 0x40200000;
  addr += cmd * sizeof(*dst);
  if (SPIWrite(addr, (uint32_t *)dst, sizeof(*dst))) {
    return 0;
  }
}

uint32_t eboot_command_read_from_flash(struct eboot_command *cmd)
{

    eboot_flash_command_t flash_command;
    uint32_t i, *src, *dst = (uint32_t *)cmd;
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    // ets_printf("Reading command from flash: %p\n", cmd);

    for (i = 0; i < EBOOT_COMMAND_MAX_COUNT; i++) {
        // ets_printf("Read bootCommand %d, flags: %x\n", i, flash_command.flags);
        readBootCommand(i, &flash_command);
        // ets_printf("Read bootCommand %d, flags: %x\n", i, flash_command.flags);
        if (((flash_command.flags & EBOOT_CMD_FLAG_SLOT_FREE) == 0) &&
            ((flash_command.flags & EBOOT_CMD_FLAG_PENDING) == EBOOT_CMD_FLAG_PENDING)) {
            // Not free (meaning there's some data) and pending (so it's yet to be completed)
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
        ets_putc('r');
        ets_putc('c');
        ets_putc('0' + i);
        ets_putc('\n');
#endif
            src = (uint32_t *)&flash_command.cmd;
            for (uint32_t j = 0; j < dw_count; ++j) {
                dst[j] = src[j];
            }
            // ets_printf("Returning bootCommand %d, flags: %x\n", i, flash_command.flags);
            return 1;
        }
    }
    return 0;
}

uint32_t eboot_command_write_to_flash(struct eboot_command *cmd)
{
    eboot_flash_command_t flash_command;
    uint32_t i, *dst, *src = (uint32_t *)cmd;
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);

    for (i = 0; i < EBOOT_COMMAND_MAX_COUNT; i++) {
        readBootCommand(i, &flash_command);
        if (((flash_command.flags & EBOOT_CMD_FLAG_SLOT_FREE) == EBOOT_CMD_FLAG_SLOT_FREE) &&
            ((flash_command.flags & EBOOT_CMD_FLAG_PENDING) == EBOOT_CMD_FLAG_PENDING)) {
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
            ets_putc('w');
            ets_putc('c');
            ets_putc('0' + i);
            ets_putc('\n');
#endif
            dst = (uint32_t *)&flash_command.cmd;
            for (uint32_t j = 0; j < dw_count; ++j) {
                dst[j] = src[j];
            }
            flash_command.flags &= ~EBOOT_CMD_FLAG_SLOT_FREE;
            writeBootCommand(i, &flash_command);
            return 1;
        }
    }
    return 0;
}

uint32_t eboot_command_clear_flash(void)
{
    eboot_flash_command_t flash_command;
    uint32_t i;

    for (i = 0; i < EBOOT_COMMAND_MAX_COUNT; i++) {
        readBootCommand(i, &flash_command);
        if (((flash_command.flags & EBOOT_CMD_FLAG_SLOT_FREE) == 0) &&
            ((flash_command.flags & EBOOT_CMD_FLAG_PENDING) == EBOOT_CMD_FLAG_PENDING)) {
            flash_command.flags &= ~EBOOT_CMD_FLAG_PENDING;
            // ets_printf("Clearing bootCommand %d, flags: %x\n", i, flash_command.flags);
        ets_wdt_disable();
            writeBootCommand(i, &flash_command);
        ets_wdt_enable();
            return 1;
        }
    }
    return 0;
}

#endif // EBOOT_ENABLE_FLASH_STORAGE

int eboot_command_read_from_rtc(struct eboot_command *cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    uint32_t* dst = (uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        dst[i] = RTC_MEM[i];
    }

    return 0;
}

int eboot_command_write_to_rtc(struct eboot_command *cmd)
{
    const uint32_t dw_count = sizeof(struct eboot_command) / sizeof(uint32_t);
    const uint32_t* src = (const uint32_t *) cmd;
    for (uint32_t i = 0; i < dw_count; ++i) {
        RTC_MEM[i] = src[i];
    }
}

void eboot_command_clear_rtc(void)
{
    RTC_MEM[offsetof(struct eboot_command, magic) / sizeof(uint32_t)] = 0;
    RTC_MEM[offsetof(struct eboot_command, crc32) / sizeof(uint32_t)] = 0;
}

int eboot_command_read(struct eboot_command* cmd)
{
    uint32_t have_command = 0;
    uint32_t count = 0;
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
    ets_putc('F');
    ets_putc(':');
#endif
    have_command = eboot_command_read_from_flash(cmd);
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
    ets_putc('0' + have_command);
    ets_putc('\n');
#endif // EBOOT_ENABLE_SERIAL_DEBUG
#endif // EBOOT_ENABLE_FLASH_STORAGE
    if (have_command == 0) {
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
        ets_putc('R');
        ets_putc('-');
#endif
        eboot_command_read_from_rtc(cmd);
        count = cmd->args[28];
        cmd->args[28] = 0;
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
        ets_putc('0' + count);
        ets_putc('\n');
#endif
    }

    uint32_t crc32 = eboot_command_calculate_crc32(cmd);
    if (cmd->magic & EBOOT_MAGIC_MASK != EBOOT_MAGIC || 
        cmd->crc32 != crc32) {
        return 1;
    }

    if (count >= 3) {
#if defined (EBOOT_ENABLE_SERIAL_DEBUG)
        ets_putc('R');
        ets_putc('>');
        ets_putc('!');
        ets_putc('\n');
#endif
        return 1;
    }

    cmd->args[28] = count + 1;
    eboot_command_write_to_rtc(cmd);
    return 0;
}

void eboot_command_write(struct eboot_command* cmd)
{
    cmd->magic = EBOOT_MAGIC;
    cmd->crc32 = eboot_command_calculate_crc32(cmd);
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
    eboot_command_write_to_flash(cmd);
#endif // EBOOT_ENABLE_FLASH_STORAGE
    eboot_command_write_to_rtc(cmd);
}

void eboot_command_clear()
{
    uint32_t cleared = 0;
#if defined (EBOOT_ENABLE_FLASH_STORAGE)
    cleared = eboot_command_clear_flash();
#endif // EBOOT_ENABLE_FLASH_STORAGE
    if (cleared > 0) {
        eboot_command_clear_rtc();
    }
}

