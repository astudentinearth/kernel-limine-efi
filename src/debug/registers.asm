

section .data
cr3_msg: db "[DEBUG] CR3 value: %p",13,10,0 
cr4_msg: db "[DEBUG] CR4 value: %p",13,10,0

section .text 

global out_cr3
global out_cr4
extern debug_printf

out_cr3:
    mov rdi, cr3_msg
    mov rax, cr3
    mov rsi, rax 
    call debug_printf
    ret

out_cr4:
    mov rdi, cr4_msg
    mov rax, cr4
    mov rsi, rax
    call debug_printf
    ret


