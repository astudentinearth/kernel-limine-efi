

section .text 

global get_cr3
global get_cr4
global get_rsp
global get_interrupt_flag

get_cr3:
    mov rax, cr3
    ret

get_cr4:
    mov rax, cr4
    ret

get_rsp:
    mov rax, rsp
    ret

get_interrupt_flag:
    pushfq
    pop rax
    and rax, 0x200
    ret
