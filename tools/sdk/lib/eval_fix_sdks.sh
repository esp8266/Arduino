#!/bin/bash
# set -e

single_sdk="${2}"
if [[ -n "$single_sdk" ]]; then
  if [[ "NONOSDK" != "${single_sdk:0:7}" ]]; then
    single_sdk=""
  fi
fi

add_path_ifexist() {
  if [[ -d $1 ]]; then
    export PATH=$( realpath $1 ):$PATH
    return 0
  fi
  return 1
}

if ! which xtensa-lx106-elf-ar | grep "tools/xtensa-lx106-elf/bin" >>/dev/null; then
  add_path_ifexist "../../../xtensa-lx106-elf/bin" || add_path_ifexist "../../xtensa-lx106-elf/bin"
fi

help_msg() {
  cat <<EOF
Try:
  eval_fix_sdks.sh --analyze
or
  eval_fix_sdks.sh --patch

EOF
}

list_sdks() {
  if [[ -n "$single_sdk" ]]; then
    echo -e "$single_sdk"
    return
  fi
  cat <<EOF
NONOSDK22x_190313
NONOSDK22x_190703
NONOSDK22x_191024
NONOSDK22x_191105
NONOSDK22x_191122
NONOSDK221
NONOSDK3V0
NONOSDK300
NONOSDK301
NONOSDK302
NONOSDK303
NONOSDK304
NONOSDK305
EOF
}

remove_ifexist() {
  [[ -f $1 ]] && rm $1
}

cleanup() {
  remove_ifexist old.txt
  remove_ifexist old2.txt
  remove_ifexist new.txt
  for sdk in `list_sdks`; do
    remove_ifexist $sdk/eap.o
  done
}

unasm() {
  xtensa-lx106-elf-objdump -d $*
}

analyze() {
  cleanup

  for sdk in `list_sdks`; do
    pushd $sdk
    xtensa-lx106-elf-ar x libwpa2.a eap.o
    popd
  done
  echo ""

  find . -name eap.o -exec md5sum {} \; | sort -k2
  echo ""

  unset prev_sdk
  for sdk in `list_sdks`; do
    unasm -j ".text.eap_peer_config_deinit" ${sdk}/eap.o >new.txt
    if [[ -f old.txt ]]; then
      echo "eap_peer_config_deinit: diff $prev_sdk $sdk"
      diff old.txt new.txt
      echo ""
    fi
    mv new.txt old.txt
    prev_sdk=${sdk}
  done

  unset prev_sdk
  for sdk in `list_sdks`; do
    unasm -j ".text.wpa2_sm_rx_eapol" ${sdk}/eap.o >new.txt
    if [[ -f old2.txt ]]; then
      echo "wpa2_sm_rx_eapol: diff $prev_sdk $sdk"
      diff old2.txt new.txt
      echo ""
    fi
    mv new.txt old2.txt
    prev_sdk=${sdk}
  done

  # Find offsets for patching vPortFree with z2EapFree
  for sdk in `list_sdks`; do
    echo -en "\n${sdk}/eap.o:\n  "
    grep --byte-offset --only-matching --text vPortFree ${sdk}/eap.o
  done

  cleanup
}


patch_all() {
  for sdk in `list_sdks`; do
    pushd $sdk
    ../fix_sdk_libs.sh
    popd
  done
}

if [[ "${1}" == "--analyze" ]]; then
  analyze
elif [[ "${1}" == "--patch" ]]; then
  patch_all
else
  help_msg
fi
exit 0
