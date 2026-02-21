
section .data
gdtr DW 0 ; limit
     DQ 0 ; base


section .text
global set_gdt


set_gdt:
    cli
    mov [gdtr], di
    mov [gdtr+2], rsi
    lgdt [gdtr]

.reload_segments:
    push 0x08
    lea rax, [rel .reload_cs]
    push rax
    retfq

.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

    
    
    

