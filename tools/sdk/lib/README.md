## Updating SDK libraries

After updating SDK libraries to a new version, do the following changes.


1. Weaken symbols from mem_manager.o to use custom heap implementation

    ```bash
    xtensa-lx106-elf-ar -x libmain.a mem_manager.o
    xtensa-lx106-elf-objcopy --weaken mem_manager.o
    xtensa-lx106-elf-ar -r libmain.a mem_manager.o
    rm mem_manager.o
    ```
