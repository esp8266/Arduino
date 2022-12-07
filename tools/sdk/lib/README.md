## Adding new SDKs

- Create a new directory for the new SDK
- Copy .a files from SDK `lib` directory to the new directory
- Run `./eval_fix_sdks.sh --analyze`.
- Use above results to update `fix_sdk_libs.sh` to handle new SDK
- Once `fix_sdk_libs.sh` has been updated. You can run `./eval_fix_sdks.sh --patch` to do a batch run of `fix_sdk_libs.sh` against each SDK.

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
