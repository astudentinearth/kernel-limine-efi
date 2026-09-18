
section .text

global ps2_kbd_scancode_cmd

wait_out_ready:
    in al, 0x64
    and al, 0x2
    cmp al, 0
    jne wait_out_ready
    ret

wait_in_ready:
    in al, 0x64
    and al, 0x1
    cmp al, 0
    je wait_in_ready
    ret

ps2_kbd_scancode_cmd:
    cmp rdi, 0
    je get_scancode_set
    jne set_scancode_set
   
set_scancode_set:
    call wait_out_ready
    mov al, 0xF0
    out 0x60, al ; scan code command
    
    call wait_in_ready
    in al, 0x60
    cmp al, 0xFE
    je return

    call wait_out_ready
    mov al, dil
    out 0x60, al ; sub command
    call wait_in_ready
    in al, 0x60
    ret

get_scancode_set:
    call wait_out_ready
    mov al, 0xF0
    out 0x60, al

    call wait_in_ready
    in al, 0x60
    cmp al, 0xFE
    je return

    call wait_out_ready
    mov al, 0
    out 0x60, al

    call wait_in_ready
    in al, 0x60
    cmp al, 0xFA
    je return_scancode_set
    ret ; return resend byte

return:
    ret

return_scancode_set:
    call wait_in_ready
    in al, 0x60 ; read scancode byte
    ret 

