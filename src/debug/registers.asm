

section .text 

global get_cr3
global get_cr4
global get_rsp

get_cr3:
    mov rax, cr3
    ret

get_cr4:
    mov rax, cr4
    ret

get_rsp:
    mov rax, rsp
    ret


