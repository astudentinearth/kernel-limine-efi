
section .text
global invalidate_page
global set_pml4_addr

invalidate_page:
    invlpg [rdi]
    ret

set_pml4_addr:
    mov rax, rdi
    mov cr3, rax
    ret

