section .text

global trigger_gp

trigger_gp:
    mov rax, 0xDEADDEADDEADDEAD
    mov [rax], rbx

