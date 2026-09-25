#!/usr/bin/env bash

set -euo pipefail

ovmf_code=${OVMF_CODE:-/usr/share/edk2/x64/OVMF_CODE.4m.fd}
ovmf_vars=${OVMF_VARS:-ovmf_vars.4m.fd}
ovmf_flags="-drive if=pflash,format=raw,unit=0,file=$ovmf_code,readonly=on -drive if=pflash,format=raw,unit=1,file=$ovmf_vars"

default_qemu_flags="-m 512M -smp 1 -device piix3-ide,id=ide -drive file=image.iso,format=raw,if=none,id=bootcd,readonly=on -device ide-cd,drive=bootcd,bus=ide.0,unit=0,bootindex=1 -serial stdio -no-reboot -net none"

if [[ $OSTYPE == linux-gnu* ]]; then
    default_qemu_flags="$default_qemu_flags -machine q35 -accel kvm -cpu host"
elif [[ $OSTYPE == darwin* ]]; then
    default_qemu_flags="$default_qemu_flags -machine q35 -cpu qemu64 -display cocoa,zoom-to-fit=on"
fi

qemu_flags="${QEMU_FLAGS:-$default_qemu_flags} $ovmf_flags"
qemu-system-x86_64 $qemu_flags
