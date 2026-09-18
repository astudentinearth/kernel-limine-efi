section .data
idtr dw 0 ; size
     dq 0 ; offset


section .text

extern handle_interrupt
extern handle_interrupt_with_error_code
extern keyboard_interrupt

global enable_hardware_interrupts
global disable_hardware_interrupts

enable_hardware_interrupts:
    sti
    ret

disable_hardware_interrupts:
    cli
    ret

%macro save_registers 0
push rax
push rbx
push rcx
push rdx
push rbp
push rsi
push rdi
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro restore_registers 0
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rdi
pop rsi
pop rbp
pop rdx
pop rcx
pop rbx
pop rax
%endmacro


%macro isr_err_stub 1
isr_stub_%+%1:
    save_registers
    mov rbp, rsp

    lea rdi, [rbp + 128] ; skip saved registers + error code

    mov esi, %1
    mov rdx, [rbp + 120] ; grab error code
    and rsp, -16

    cld
    call handle_interrupt_with_error_code

    mov rsp, rbp
    restore_registers

    add rsp, 8
    iretq
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    
    save_registers
    mov rbp, rsp
    lea rdi, [rbp + 120] ; skip saved registers
    mov esi, %1
    and rsp, -16

    cld
    call handle_interrupt

    mov rsp, rbp
    restore_registers
    iretq
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
isr_no_err_stub 32

isr_stub_33:
    save_registers

    mov rbp, rsp
    and rsp, -16
    cld

    in al, 0x60
    movzx edi, al
    call keyboard_interrupt
    mov rsp, rbp
    
    restore_registers
    iretq

%assign i 34
%rep 222
    isr_no_err_stub i
%assign i i+1
%endrep
    
global isr_stub_table
isr_stub_table:
%assign i 0
%rep    256
    dq isr_stub_%+i
%assign i i+1
%endrep

global load_idt
load_idt:
    mov [idtr], di
    mov [idtr + 2], rsi
    lidt [idtr]
    ret

