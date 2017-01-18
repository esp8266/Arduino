eboot Bootloader
===================


A simple bootloader which can copy an OTA update to main application address in flash and load the application code to IRAM.

# Contents
- [Startup sequence](#startup-sequence)


### Startup sequence ###

On startup the ESP ROM will [load and start the firmware image](#load-and-start) from flash location 0x0 - eboot.

##### eboot will #####
1. print a version string from the current firmware image
2. print a list of the flags for the first 8 flash-command slots
3. [attempt to read a command](#read-eboot-command)
	1. execute command (if found)
	2. on completion of the command, mark it complete
4. [load and start the application](#load-and-start)



### Read eboot command ###

Historically eboot commands were stored in RTC RAM. In an effort to survive a power-outage mid-update (which will claer the RTC RAM) there is now provision for storing the commands in Flash. If no valid command is found in the flash storage (or flash storage is disabled) eboot will try RTC RAM.

Commands are considered valid if the checksum is correct.

```C
typedef struct eboot_command {
    uint32_t magic;
    enum action_t action;
    uint32_t args[29];
    uint32_t crc32;
} eboot_command_t;
```


### Load and start ###

The firmware images are expected to have ELF headers indicating what location each section should be loaded into - which is relevant for the RAM locations.

Both the ESP bootloader and eboot will work through the image sections copying the contents of the appropriate ones into their address.

Finally execution jumps to the entry-point specified in the image header.

