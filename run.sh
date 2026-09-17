#!/usr/bin/env bash
qemu-system-x86_64 \
    -m 512M \
    -smp 1 \
    -device piix3-ide,id=ide \
    -drive file=image.iso,format=raw,if=none,id=bootcd,readonly=on \
    -device ide-cd,drive=bootcd,bus=ide.0,unit=0,bootindex=1 \
    -serial stdio \
    -no-reboot \
    -machine q35 -cpu qemu64 \
    -drive if=pflash,format=raw,unit=0,file="${OVMF_CODE:-/usr/share/edk2/x64/OVMF_CODE.4m.fd},readonly=on" \
    -drive if=pflash,format=raw,unit=1,file="${OVMF_VARS:-ovmf_vars.4m.fd}" \
    -net none
