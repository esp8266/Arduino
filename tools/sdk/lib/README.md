## Updating SDK libraries

- Copy .a files from SDK `lib` directory to this directory
- Run `fix_sdk_libs.sh`

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

