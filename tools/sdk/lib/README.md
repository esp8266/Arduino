## Updating SDK libraries

After updating SDK libraries to a new version, do the following changes.


Remove mem_manager.o from libmain.a to use custom heap implementation, and time.o to fix redefinition of time-related functions:

```bash
xtensa-lx106-elf-ar -d libmain.a mem_manager.o
xtensa-lx106-elf-ar -d libmain.a time.o
```

## Updating libstdc++

After building gcc using crosstool-NG, get compiled libstdc++ and remove some objects:

```bash
xtensa-lx106-elf-ar d libstdc++.a pure.o
xtensa-lx106-elf-ar d libstdc++.a vterminate.o
xtensa-lx106-elf-ar d libstdc++.a guard.o
xtensa-lx106-elf-ar d libstdc++.a functexcept.o
xtensa-lx106-elf-ar d libstdc++.a del_op.o
xtensa-lx106-elf-ar d libstdc++.a del_opv.o
xtensa-lx106-elf-ar d libstdc++.a new_op.o
xtensa-lx106-elf-ar d libstdc++.a new_opv.o
```

