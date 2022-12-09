## Adding a new SDK library

- Create a directory for the new SDK.
- Copy .a files from SDK `lib` directory to the new directory
- Add the new SDK directory to those supported in `eval_fix_sdks.sh` and `fix_sdk_libs.sh`.
- To support WPA2 Enterprise connections, some patches are  reguired review `wpa2_eap_patch.cpp` and `eval_fix_sdks.sh` for details.
- Use `./eval_fix_sdks.sh --analyze` to aid in finding relevant differences.
  - Also, you can compare two SDKs with something like `./eval_fix_sdks.sh --analyze "NONOSDK305\nNONOSDK306"`
- Apply updates to `fix_sdk_libs.sh` and `wpa2_eap_patch.cpp`. You can run `./eval_fix_sdks.sh --patch` to do a batch run of `fix_sdk_libs.sh` against each SDK.
- If you used this section, you can skip _Updating SDK libraries_.

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
