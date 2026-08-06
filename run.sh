#!/bin/bash
qemu-system-x86_64 -m 64M -smp 1 -cdrom image.iso -boot d -serial stdio -no-reboot
