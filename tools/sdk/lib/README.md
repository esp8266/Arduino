## Updating SDK libraries

After updating SDK libraries to a new version, do the following changes.


1. Remove mem_manager.o from libmain.a to use custom heap implementation

    ```bash
    xtensa-lx106-elf-ar -d libmain.a mem_manager.o
    ```
