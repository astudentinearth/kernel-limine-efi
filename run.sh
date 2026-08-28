#!/bin/bash
qemu-system-x86_64 \
    -m 512M \
    -smp 1 \
    -cdrom image.iso -boot d \
    -serial stdio \
    -no-reboot \
    -machine q35 -cpu qemu64 \
    -drive if=pflash,format=raw,unit=0,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd,readonly=on \
    -drive if=pflash,format=raw,unit=1,file=ovmf_vars.4m.fd \
    -net none
