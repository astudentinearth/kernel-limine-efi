#!/bin/bash
qemu-system-x86_64 -m 512M -smp 1 -cdrom image.iso -boot d -serial stdio
