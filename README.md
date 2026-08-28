## x64 Kernel Project
"Kernel" I built purely for fun by following the [OSDev Wiki](https://wiki.osdev.org). This is mostly based on the [Limine Bare Bones tutorial.](https://wiki.osdev.org/Limine_Bare_Bones).

## Features
- Level 4 paging
- IOAPIC support
- PCI hardware enumeration
- PS/2 keyboard support (will output characters to COM1)
- A terribly drawn PC Screen Font
- In source tests that run automatically on boot (can be stripped with compiler directives)
- ACPI v1 and v2

## Disclaimers
This repository is mostly a proof of concept and some things are implemented in the worst way possible. This is not idiomatic C, and code isn't structured in  a way that could support multiple architectures or different types of hardware. Many things are built on assumptions, primarily running in QEMU, and hardware support is *below* bare minimum even for a VM. Because this is a fun project only, I don't really care.

## Resources that helped
- [OSDev Wiki](https://wiki.osdev.org)
- [Intel's developer manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html#combined)
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
